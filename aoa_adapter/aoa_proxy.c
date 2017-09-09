#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <libusb.h>
#include "aoa_proxy.h"
#include "accessory.h"
#include "log.h"
#include "tcp.h"
#include "u2spipe.h"

static void shutdown_everything(void);
static void disconnect_device(libusb_device *dev);
static void cleanup_dead_devices(void);
static void destroy_usb_xfer_thread(usbXferThread *t);
static void stop_usb_pipe(struct listentry *device);
static int init_usb_xfer_thread(usbXferThread *t);
static int start_usb_pipe(struct listentry *device);
static int connect_device(libusb_device *device);
static int update_usb_inventory(void);
static int init_usb(void);
static void tickle_usb_inventory_thread(void);
static void sig_handler(int signum);
static void init_sig_handler(void);

//A libusb session
static libusb_context *ctx = NULL;
//Pinter to pointer of device, used to retrieve a list of devices 
static libusb_device **g_devs = NULL;
//Control main cycle
static int do_exit = 0;
//Maintain all current usb and socket linked lists
static struct listentry *connectedDevices = NULL;

static int autoscan = 1;
static int do_update_usb_inventory = 0;
static t_socketfd g_socketfd;

static void shutdown_everything(void)
{
	logDebug("Shutdown everything\n");
    
	do_exit = 1;

    if(g_devs != NULL)
    {
        libusb_free_device_list(g_devs, 1);
    }
    
    if(autoscan)
    {
	    struct itimerval timer;
	    memset (&timer, 0, sizeof(timer));
	    setitimer (ITIMER_REAL, &timer, NULL);
    }

	while(connectedDevices != NULL)
    {
		disconnect_device(connectedDevices->usbDevice);
    }

	if(ctx != NULL)
    {
        //close the session
		libusb_exit(ctx); 
    }

	logDebug("Shutdown complete\n");
}

static void disconnect_device(libusb_device *dev)
{
    logDebug("Disconnect device\n");
        
	struct listentry *device = connectedDevices;

	while(device != NULL)
    {
		if (device->usbDevice == dev)
        {
			if (device->prev == NULL)
            {
				connectedDevices = device->next;
			}
            else
            {
				device->prev->next = device->next;
			}
			if (device->next != NULL)
            {
				device->next->prev = device->prev;
			}

			stop_usb_pipe(device);
			shutdownUSBDroid(device->usbDevice, &device->droid);
			free(device);
            
			logDebug("Android disconnected");       
			return;
		}
        
		device = device->next;
	}
}

static void cleanup_dead_devices(void)
{   
	struct listentry *device = connectedDevices;

	while(device != NULL)
    {
        if (device->usbDead)
		{
			logDebug("Found device with dead USB\n");
		} 
        else
        {
			device = device->next;
			continue;
		}
        
		disconnect_device(device->usbDevice);
		cleanup_dead_devices();
        
		break;
	}
}


static void destroy_usb_xfer_thread(usbXferThread *t)
{
    logDebug("destroy_usb_xfer_thread\n");
    
	pthread_mutex_destroy(&t->mutex);
	pthread_cond_destroy(&t->condition);
    
	libusb_free_transfer(t->xfr);
}

static void stop_usb_pipe(struct listentry *device)
{
    logDebug("stop_usb_pipe\n");

    #if 0
	device->usbRxThread.stop = 1;
    device->sockfd->cmd_socketDead = 1;
    device->sockfd->video_socketDead = 1;
    device->sockfd->media_socketDead = 1;
    device->sockfd->tts_socketDead = 1;
    device->sockfd->vr_socketDead = 1;
    device->sockfd->touch_socketDead = 1;
    #endif
    
	if(device->usbRxThread.usbActive)
    {
		libusb_cancel_transfer(device->usbRxThread.xfr);
	}

	tickle_usb_xfer_thread(&device->usbRxThread);
    
	pthread_kill(device->usbRxThread.thread, SIGUSR1);
    
	logDebug("Waiting for usb rx thread...\n");
	if(0 != pthread_join(device->usbRxThread.thread, NULL))
    {
		logError("Failed to join usb rx thread\n");
	}

	device->socketRxThread.stop = 1;
    
	if(device->socketRxThread.usbActive)
    {
		libusb_cancel_transfer(device->socketRxThread.xfr);
	}

	tickle_usb_xfer_thread(&device->socketRxThread);

	pthread_kill(device->socketRxThread.thread, SIGUSR1);
    
	logDebug("Waiting for socket rx thread...\n");
	if(0 != pthread_join(device->socketRxThread.thread, NULL))
    {
		logError("Failed to join socket rx thread\n");
	}

	destroy_usb_xfer_thread(&device->usbRxThread);
	destroy_usb_xfer_thread(&device->socketRxThread);

	logDebug("Threads stopped\n");
}
    
static int init_usb_xfer_thread(usbXferThread *t)
{
	t->xfr = libusb_alloc_transfer(0);
	if (t->xfr == NULL)
    {
		return -1;
	}
	t->stop = 0;
	t->stopped = 0;
	t->usbActive = 0;
	t->tickle = 0;
    
	pthread_mutex_init(&t->mutex, NULL);
	pthread_cond_init(&t->condition, NULL);
    
	return 0;
}

static int start_usb_pipe(struct listentry *device) 
{
    int r;
    
	if(init_usb_xfer_thread(&device->usbRxThread) < 0)
    {
		logError("Failed to allocate usb rx transfer\n");
		return -1;
	}
    
	if(init_usb_xfer_thread(&device->socketRxThread) < 0)
    {
		logError("Failed to allocate usb tx transfer\n");
		destroy_usb_xfer_thread(&device->usbRxThread);
        
		return -1;
	}

	//Write to the usb task
	r = pthread_create(&device->usbRxThread.thread, NULL, (void*)&u2s_usb_rx_thread, (void*)device);
	if (r < 0)
    {
		logError("Failed to start usb rx thread\n");
		return -1;
	}

	//Read the socket task
	r = pthread_create(&device->socketRxThread.thread, NULL, (void*)&u2s_socket_rx_thread, (void*)device);
	if (r < 0)
    {
		//other thread is stopped in disconnectDevice method
		logError("Failed to start socket rx thread\n");
		return -1;
	}

	return 0;
}

static int connect_device(libusb_device *device)
{
    logDebug("Prepare to connect device\n");

    struct libusb_device_descriptor desc;
    //Get the usb device description information
    int r = libusb_get_device_descriptor(device, &desc);
	if (r < 0)
    {
		logError("Failed to get device descriptor: %d", r);
		return -1;
	}

    logDebug("VID = %04x, PID = %04x (bus : %d, device : %d)\n", 
        desc.idVendor, desc.idProduct,
        libusb_get_bus_number(device), libusb_get_device_address(device));

    if (desc.bDeviceClass == 0x09)
    {
	    logError("Device 0x%04X:%04X has wrong deviceClass: 0x%02x", 
            desc.idVendor, desc.idProduct, desc.bDeviceClass);
		    return -1; 
	}
    
    //Check whether the current device is in accessory mode
    if (!isDroidInAcc(device))
    {
        logDebug("Attempting AOA on device VID = %04x: PID = %04x\n", 
            desc.idVendor, desc.idProduct);
        
        //Write the information for the application to start the android accessory mode
        switchDroidToAcc(device, 1);
        
        return -1;
    }

    //Entry management socket and usb
    struct listentry *entry = malloc(sizeof(struct listentry));
    if (entry == NULL)
    {
		logError("Not enough RAM\n");
		return -2;
	}

    bzero(entry, sizeof(struct listentry));

    //Entry gets the usb handle device
	entry->usbDevice = device;

    entry->sockfd = &g_socketfd;
    
    //If android devices are already aoa mode, open usb
    logDebug("Start setup droid\n");
    //Find the accessory interface and initialize the entry->droid with the interface information
    r = setupDroid(device, &entry->droid);
	if (r < 0)
    {
		logError("Failed to setup droid: %d\n", r);
		free(entry);
		return -3;
	}

    //Add the entry to the list
	entry->next = NULL;
	if (connectedDevices == NULL)
    {
		entry->prev = NULL;
		connectedDevices = entry;
	} 
    else
    {
		struct listentry *last = connectedDevices;
		while(last->next != NULL)
        {
			last = last->next;
        }
        
		entry->prev = last;
		last->next = entry;
	}

	//Build usb and socket communication tasks
	r = start_usb_pipe(entry);
	if (r < 0) {
		logError("Failed to start pipe: %d\n", r);
		disconnect_device(device);
		return -4;
	}

	logDebug("New Android connected\n");
       
    return 0;
}

static int update_usb_inventory(void) 
{
    //Holding number of devices in list
    static ssize_t cnt = 0;
    static ssize_t last_cnt = 0;
    static libusb_device **last_devs = NULL;
    ssize_t i, j;
    int found_before;
    
    //Get the list of devices 
	cnt = libusb_get_device_list(ctx, &g_devs);
	if(cnt < 0)
    {
		logError("Failed to list devices\n");
		return -1;
	}

    for(i = 0; i < cnt; i++)
    {
        found_before = 0;
        if(last_devs != NULL)
        {
            for(j = 0; j < last_cnt; j++)
            {
                if(g_devs[i] == last_devs[j])
                {
                    found_before = 1;
                    break;
                }     
            }
        }

        if(!found_before)
        {
            logDebug("Start connect device\n");
    		//Connecting device
    		if(connect_device(g_devs[i]) == 0)
            {
    			libusb_ref_device(g_devs[i]);
            }
        }
	}

    if(last_devs != NULL)
    {
        for(i = 0; i < last_cnt; i++)
        {
            found_before = 0;
            for(j = 0; j < cnt; j++)
            {
                if(g_devs[j] == last_devs[i])
                {
                    found_before = 1;
                    break;
                }
            }

            if(!found_before)
            {
                struct listentry *hit = connectedDevices;

                while(hit != NULL)
                {
                    if(hit->usbDevice == last_devs[i])
                    {
                        disconnect_device(last_devs[i]);
                        libusb_unref_device(last_devs[i]);
                        break;
                    }
                    hit= hit->next;
                }
            }         
        }
        
        libusb_free_device_list(last_devs, 1);
    }

    last_devs = g_devs;
    last_cnt = cnt;
    
    return 0;
}

static int init_usb(void)
{
    //For return values
	int r;

    //Initialize a library session
	r = libusb_init(&ctx);
	if(r < 0)
    {
		return r;
	}

    //set verbosity level to 3, as suggested in the documentation
	libusb_set_debug(ctx, 3);
    
	return 0;
}

static void tickle_usb_inventory_thread(void)
{
	do_update_usb_inventory = 1;
}

static void sig_handler(int signum)
{
	switch (signum)
    {
    	case SIGINT:
    		logDebug("Received SIGINT\n");
    		do_exit = 1;
    		exit(EXIT_SUCCESS);
    		break;
    	case SIGUSR1:
    		/* USR1 is used to stop usb/socket rx threads.
    		when this signal arrives here, the thread was
    		already dead when the signal was sent. */
    		logDebug("Received SIGUSR1\n");
    		break;
    	case SIGALRM:    
    		tickle_usb_inventory_thread();
    		break;
    	default:
    		break;
	}
}

static void init_sig_handler(void)
{
	struct sigaction sigact;
    
	sigact.sa_handler = sig_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGUSR1, &sigact, NULL);
	sigaction(SIGALRM, &sigact, NULL);
}

int main(int argc, char **argv)
{
    CL_UNUSED(argc);
    CL_UNUSED(argv);

    //For return values
    int r;

    g_socketfd.cmd_socketDead = 1;
    g_socketfd.video_socketDead = 1;
    g_socketfd.media_socketDead = 1;
    g_socketfd.tts_socketDead = 1;
    g_socketfd.vr_socketDead = 1;
    g_socketfd.touch_socketDead = 1;
        
    create_start_server(&g_socketfd);
    
    init_sig_handler();

	if(0 > init_usb())
    {
		logError("Failed to initialize USB\n");    
		return -1;
	}
  
    //Regular task polling
    if(autoscan)
    {
		struct itimerval timer;
		timer.it_value.tv_sec = 1;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_sec = 1;
		timer.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &timer, NULL);
	}

    while(!do_exit)
    {
		if(do_update_usb_inventory == 1)
        {
			do_update_usb_inventory = 0;
            
			cleanup_dead_devices();
			
			//Try link device  
			update_usb_inventory();
		}

        //Blocking waits for usb events
		r = libusb_handle_events(ctx);
		if(r)
        {
			if(r == LIBUSB_ERROR_INTERRUPTED)
            {
				//ignore		
			} 
            else
            {
				if(!do_exit)
                {
					logDebug("libusb_handle_events_timeout: %d\n", r);
                }
                
				break;
			}
		}
	}
    
    shutdown_everything();

    return 0;
}
