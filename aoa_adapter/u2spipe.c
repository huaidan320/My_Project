#include <libusb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include "aoa_proxy.h"
#include "u2spipe.h"
#include "log.h"

#define MAX(a, b)   ((a) > (b) ? (a) : (b))

static void u2s_usbrx_cb(struct libusb_transfer *transfer)
{
	usbXferThread *t = (usbXferThread*)transfer->user_data;
    
	tickle_usb_xfer_thread(t);
}

void tickle_usb_xfer_thread(usbXferThread *t)
{
	pthread_mutex_lock(&t->mutex);
	if(t->usbActive)
    {
		t->usbActive = 0;
		pthread_cond_signal(&t->condition);
	}
    
	pthread_mutex_unlock(&t->mutex);
}

//Usb writes the socket task
void *u2s_usb_rx_thread(void *d)
{
    logDebug("u2s_usb_rx_thread started\n");

	struct listentry *device = (struct listentry*)d;

    logDebug("droid.inpacketsize = %d\n", device->droid.inpacketsize);

    //unsigned char tmp_buffer[device->droid.inpacketsize];
    //unsigned char tmp_buffer[100*1024];
    unsigned char *tmp_buffer = NULL;
	unsigned char buffer[device->droid.inpacketsize];
	int rxBytes = 0;
	int txBytes;
	int r;
    
    unsigned int aoa_channel_id = 0;
    int aoa_msg_len = 0;
    int head_flag = 0;

	/*
	  Initialize usbrxthread.xfr, 
	  and the associated data buffer,
	  after transmission, the callback u2s_usbrx_cb unlocked device->usbrxthread.condition
	*/
	libusb_fill_bulk_transfer(device->usbRxThread.xfr, device->droid.usbHandle, 
	    device->droid.inendp, buffer, sizeof(buffer),
	    (libusb_transfer_cb_fn)&u2s_usbrx_cb, (void*)&device->usbRxThread, 0);

	while(!device->usbRxThread.stop && !device->usbDead)
    {
        memset(buffer, 0, device->droid.inpacketsize);

        while((device->sockfd->cmd_socketDead == 1) || (device->sockfd->video_socketDead == 1) ||
            (device->sockfd->media_socketDead == 1) || (device->sockfd->tts_socketDead == 1) ||
            (device->sockfd->vr_socketDead == 1))
        {
            sleep(1);
        }
        
		pthread_mutex_lock(&device->usbRxThread.mutex);
        
		device->usbRxThread.usbActive = 1;

		//The request data
		r = libusb_submit_transfer(device->usbRxThread.xfr);
		if(r < 0)
        {
			logError("u2s usbrx submit transfer failed\n");
			device->usbDead = 1;
			device->usbRxThread.usbActive = 0;
            
			pthread_mutex_unlock( &device->usbRxThread.mutex );
            
			break;
		}
           
		//Waiting to receive data
		pthread_cond_wait(&device->usbRxThread.condition, &device->usbRxThread.mutex);

		if(device->usbRxThread.usbActive)
        {
			logError("Wait, unlock but usbActive!\n");
		}
        
		pthread_mutex_unlock(&device->usbRxThread.mutex );

		if (device->usbRxThread.stop || device->usbDead)
        {
			break;
        }

		//Check the status of usb received data
		switch(device->usbRxThread.xfr->status)
        {
    		case LIBUSB_TRANSFER_COMPLETED:
                //logDebug("USB receive len %d\n", device->usbRxThread.xfr->actual_length);
           
                if(head_flag == 0)
                {
                    aoa_channel_id = (buffer[0] << 24) | (buffer[1] << 16) |
                        (buffer[2] << 8) | (buffer[3]);
                    aoa_msg_len = (buffer[4] << 24) | (buffer[5] << 16) |
                        (buffer[6] << 8) | (buffer[7]);

                    logDebug("USB receive aoa_msg_len = %d\n", aoa_msg_len);
                    logDebug("USB receive aoa_channel_id = %d\n", aoa_channel_id);

                    tmp_buffer = (unsigned char *)malloc(aoa_msg_len);
                    if(NULL == tmp_buffer)
                    {
                        logDebug("malloc not enough memory\n");
                        break;
                    }
                    //memset(tmp_buffer, 0, aoa_msg_len);
                    
                    head_flag = 1;
                        
                    break;
                }

                
                memcpy(tmp_buffer + rxBytes, buffer, device->usbRxThread.xfr->actual_length);
                
                rxBytes += device->usbRxThread.xfr->actual_length;    

                //All data received from usb is written to socket
    			if(head_flag == 1 && aoa_msg_len == rxBytes)
                { 
                    switch(aoa_channel_id)
                    {
                        case 1:
                            txBytes = send(device->sockfd->cmd_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send cmd failed\n");
                                device->sockfd->cmd_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send cmd success len : %d\n", txBytes);
                            break;
                        case 2:
                            txBytes = send(device->sockfd->video_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send video failed\n");
                                device->sockfd->video_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send video success len : %d\n", txBytes);
                            break;
                        case 3:
                            txBytes = send(device->sockfd->media_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send media failed\n");
                                device->sockfd->media_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send media success len : %d\n", txBytes);
                            break;
                        case 4:
                            txBytes = send(device->sockfd->tts_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send tts failed\n");
                                device->sockfd->tts_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send tts success len : %d\n", txBytes);
                            break;
                        case 5:
                            txBytes = send(device->sockfd->vr_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send vr failed\n");
                                device->sockfd->vr_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send vr success len : %d\n", txBytes);
                            break;
                        case 6:
                            txBytes = send(device->sockfd->touch_socketfd, tmp_buffer, aoa_msg_len, 0);
                            if(txBytes <= 0)
                            {
                                logError("USB rx send touch failed\n");
                                device->sockfd->touch_socketDead = 1;
                                break;
                            }
                            logDebug("USB rx send touch success len : %d\n", txBytes);
                            break;
                        default:
                            logDebug("USB rx unknow channel id.\n");
                            break;
                    }
   
                    rxBytes = 0;
                    head_flag =0;
                    free(tmp_buffer);
                    tmp_buffer = NULL;
    			}
    			break;
    		case LIBUSB_TRANSFER_NO_DEVICE:
    			device->usbDead = 1;
    			device->usbRxThread.stop = 1;
    			break;
    		default:
    			break;
		}
	}

	device->usbRxThread.stopped = 1;
    
	logDebug("u2s_usb_rx_thread finished\n");
    
	pthread_exit(0);
    
	return NULL;
}

//The socket writes to the usb task
void *u2s_socket_rx_thread(void *d)
{
	logDebug("u2s_socket_rx_thread started\n");
    
	struct listentry *device = (struct listentry*)d;

    logDebug("droid.inpacketsize = %d\n", device->droid.outpacketsize);

    unsigned char rx_buffer[device->droid.outpacketsize];
	unsigned char header_buffer[8]; 
	int rx_bytes;
	int tx_ret;
    int tx_actual_length;
    int sent;
    int rx_buffer_len;
    int rx_flag;

    fd_set inset;
    int maxfd;
    int res;

	while(!device->socketRxThread.stop && !device->usbDead)
    {   
        memset(rx_buffer, 0, device->droid.outpacketsize);
        memset(header_buffer, 0, sizeof(header_buffer));

        while(1)
        {
            if((device->sockfd->cmd_socketDead == 1) || (device->sockfd->video_socketDead == 1) ||
            (device->sockfd->media_socketDead == 1) || (device->sockfd->tts_socketDead == 1) ||
            (device->sockfd->vr_socketDead == 1))
            {
                sleep(1);
                continue;
            }
            else
            {
                maxfd = MAX(device->sockfd->cmd_socketfd, device->sockfd->video_socketfd);
                maxfd = MAX(maxfd, device->sockfd->media_socketfd);
                maxfd = MAX(maxfd, device->sockfd->tts_socketfd);
                maxfd = MAX(maxfd, device->sockfd->vr_socketfd);
                maxfd = MAX(maxfd, device->sockfd->touch_socketfd);

                FD_ZERO(&inset);

                FD_SET(device->sockfd->cmd_socketfd, &inset);
                FD_SET(device->sockfd->video_socketfd, &inset);
                FD_SET(device->sockfd->media_socketfd, &inset);
                FD_SET(device->sockfd->tts_socketfd, &inset);
                FD_SET(device->sockfd->vr_socketfd, &inset);
                FD_SET(device->sockfd->touch_socketfd, &inset);

                break;
            }         
        }
  
        logDebug("Wait ... select...\n");
        res = select(maxfd + 1, &inset, NULL, NULL, NULL);
        switch(res)
        {
            case -1:
                {
                    logError("Select error\n");
                    return NULL;             
                }
                break;
            case 0: /* Timeout */
                {
                    logError("Time out\n");
                    return NULL;
                }
                break;
            default:
                {
                    if(FD_ISSET(device->sockfd->cmd_socketfd, &inset))
                    {
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->cmd_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve cmd data failed.\n");
                                device->sockfd->cmd_socketDead = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife cmd msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve cmd data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->cmd_socketDead != 1)
                        {
                            header_buffer[3] = 1;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }
                        break;
                    }
                    else if(FD_ISSET(device->sockfd->video_socketfd, &inset))
                    {          
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->video_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve video data failed.\n");
                                device->sockfd->video_socketDead = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife video msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve video data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->video_socketDead != 1)
                        {
                            header_buffer[3] = 2;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);
                            
                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }
                        break;                 
                    }
                    else if(FD_ISSET(device->sockfd->media_socketfd, &inset))
                    {
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->media_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve media data failed.\n");
                                device->sockfd->media_socketDead = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife media msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve media data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->media_socketDead != 1)
                        {
                            header_buffer[3] = 3;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);
                            
                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }
                        break;
                    } 
                    else if(FD_ISSET(device->sockfd->tts_socketfd, &inset))
                    {
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->tts_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve tts data failed.\n");
                                device->sockfd->tts_socketfd = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife tts msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve tts data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->tts_socketDead != 1)
                        {
                            header_buffer[3] = 4;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);
                            
                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }

                        break;
                    }
                    else if(FD_ISSET(device->sockfd->vr_socketfd, &inset))
                    {
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->vr_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve vr data failed.\n");
                                device->sockfd->vr_socketfd = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife vr msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve vr data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->vr_socketDead != 1)
                        {
                            header_buffer[3] = 5;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);
                            
                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }
                        break;
                    }
                    else if(FD_ISSET(device->sockfd->touch_socketfd, &inset))
                    {
                        sent = 0;
                        rx_buffer_len = 8;
                        rx_flag = 1;
                        
                        while(sent <= 8)
                        {
                            rx_bytes = recv(device->sockfd->touch_socketfd, rx_buffer + sent, rx_buffer_len, 0);
                            if(rx_bytes <= 0)
                            {
                                logError("Server recieve touch data failed.\n");
                                device->sockfd->touch_socketfd = 1;
                                break;
                            }
                            else
                            {
                                sent += rx_bytes;
                                
                                if(rx_flag == 1)
                                {
                                    rx_buffer_len = (rx_buffer[0] << 8) | (rx_buffer[1]);
                                    if(rx_buffer_len == 0)
                                    {
                                        logDebug("Carlife touch msg len is %d.\n", rx_buffer_len);
                                        break;
                                    }
                                }

                                rx_flag = 0;
                                
                                logDebug("Server recieve touch data success : %d.\n", rx_bytes);
                            }
                        }

                        if(device->sockfd->touch_socketDead != 1)
                        {
                            header_buffer[3] = 6;
                            header_buffer[7] = *((char *)(&sent));
                            header_buffer[6] = *((char *)(&sent) + 1);
                            header_buffer[5] = *((char *)(&sent) + 2);
                            header_buffer[4] = *((char *)(&sent) + 3);
                            
                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, header_buffer, 
                                sizeof(header_buffer), &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sizeof(header_buffer))
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            } 
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }

                            tx_ret = libusb_bulk_transfer(device->droid.usbHandle, device->droid.outendp, rx_buffer, 
                                sent, &tx_actual_length , 0);
                            if(tx_ret == 0 && tx_actual_length == sent)
                            {
                                logDebug("r = %d, actual_length = %d\n", tx_ret, tx_actual_length);
                            }
                            else
                            {
    	                        logError("libusb_bulk_transfer error\n");
                            }
                        }
                        break;
                    }
                    else
                    {
                        logDebug("Socket fd not exist inset\n");
                        device->sockfd->cmd_socketDead = 1;
                        device->sockfd->video_socketDead = 1;                
                        device->sockfd->media_socketDead = 1;
                        device->sockfd->tts_socketDead = 1;
                        device->sockfd->vr_socketDead = 1;
                        break;
                    }
                break;  
                }
        }
    }

	device->socketRxThread.stopped = 1;
    
	logDebug("u2s_socket_rx_thread finished\n");
    
	pthread_exit(0);

	return NULL;
}
