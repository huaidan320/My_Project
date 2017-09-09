#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include "accessory.h"
#include "log.h"

const char *vendor = "Baidu";
const char *model = "CarLife";
const char *description = "Baidu CarLife";
const char *version = "1.0.0";
const char *uri = "http://carlife.baidu.com/";
const char *serial = "0720SerialNo.";

int setupDroid(libusb_device *usbDevice, accessory_droid *device)
{
	bzero(device, sizeof(accessory_droid));

	struct libusb_device_descriptor desc;
    
	//Get the usb descriptor
	int r = libusb_get_device_descriptor(usbDevice, &desc);
	if (r < 0)
    {
		logError("Failed to get device descriptor\n");
		return r;
	}

	struct libusb_config_descriptor *config;
    
	//Gets the usb device configuration descriptor
	r = libusb_get_config_descriptor(usbDevice, 0, &config);
	if (r < 0)
    {
		logError("Failed t oget config descriptor\n");
		return r;
	}

	const struct libusb_interface *inter;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_endpoint_descriptor *epdesc;
	int i,j,k;

	for(i=0; i<(int)config->bNumInterfaces; i++)
    {
		logDebug("Checking interface #%d\n", i);
        
		inter = &config->interface[i];
		if (inter == NULL)
        {
			logDebug("interface is null\n");
			continue;
		}
        
		logDebug("Interface has %d alternate settings\n", inter->num_altsetting);
        
		for(j=0; j<inter->num_altsetting; j++)
        {
			interdesc = &inter->altsetting[j];
			if (interdesc->bNumEndpoints == 2 
                && interdesc->bInterfaceClass == 0xff
				&& interdesc->bInterfaceSubClass == 0xff
				&& (device->inendp <= 0 || device->outendp <= 0))
		    {
				//Find the current interface for the accessory
				logDebug( "Interface %d is accessory candidate\n", i);
                
				for(k=0; k < (int)interdesc->bNumEndpoints; k++)
                {
					epdesc = &interdesc->endpoint[k];
					if (epdesc->bmAttributes != 0x02)
                    {
						break;
					}

					//Gets the usb input output endpoint
					if ((epdesc->bEndpointAddress & LIBUSB_ENDPOINT_IN) && device->inendp <= 0)
                    {
						device->inendp = epdesc->bEndpointAddress;
						device->inpacketsize = epdesc->wMaxPacketSize;
                        
						logDebug( "Using EP 0x%02x as bulk-in EP\n", (int)device->inendp);
					} 
                    else if ((!(epdesc->bEndpointAddress & LIBUSB_ENDPOINT_IN)) && device->outendp <= 0)
                    {
						device->outendp = epdesc->bEndpointAddress;
						device->outpacketsize = epdesc->wMaxPacketSize;
                        
						logDebug( "Using EP 0x%02x as bulk-out EP\n", (int)device->outendp);
					} else
					{
						break;
					}
				}
                
				if (device->inendp && device->outendp)
                {
					device->bulkInterface = interdesc->bInterfaceNumber;
				}
			} 
            else if (interdesc->bInterfaceClass == 0x01
					&& interdesc->bInterfaceSubClass == 0x02
					&& interdesc->bNumEndpoints > 0
					&& device->audioendp <= 0)
		    {

				logDebug( "Interface %d is audio candidate\n", i);

				device->audioInterface = interdesc->bInterfaceNumber;
				device->audioAlternateSetting = interdesc->bAlternateSetting;

				for(k=0; k < (int)interdesc->bNumEndpoints; k++)
                {
					epdesc = &interdesc->endpoint[k];
					if (epdesc->bmAttributes != ((3 << 2) | (1 << 0)))
                    {
						logDebug("Skipping non-iso ep\n");
						break;
					}
					device->audioendp = epdesc->bEndpointAddress;
					device->audiopacketsize = epdesc->wMaxPacketSize;
                    
					logDebug( "Using EP 0x%02x as audio EP\n", (int)device->audioendp);
					break;
				}
			}
		}
	}

	if (!(device->inendp && device->outendp))
    {
		logError("Device has no accessory endpoints\n");
		return -2;
	}

	//Open usb device
	r = libusb_open(usbDevice, &device->usbHandle);
	if(r < 0)
    {
		logError("Failed to open usb handle\n");
		return r;
	}

	//Batch transmission
	r = libusb_claim_interface(device->usbHandle, device->bulkInterface);
	if (r < 0)
    {
		logError("Failed to claim bulk interface\n");
		libusb_close(device->usbHandle);
		return r;
	}

	if (device->audioendp) 
    {
		r = libusb_claim_interface(device->usbHandle, device->audioInterface);
		if (r < 0)
        {
			logError("Failed to claim audio interface\n");
			libusb_release_interface(device->usbHandle, device->bulkInterface);
			libusb_close(device->usbHandle);
			return r;
		}

		r = libusb_set_interface_alt_setting(device->usbHandle, device->audioInterface, device->audioAlternateSetting);
		if (r < 0)
        {
			logError("Failed to set alternate setting\n");
			libusb_release_interface(device->usbHandle, device->bulkInterface);
			libusb_release_interface(device->usbHandle, device->audioInterface);
			libusb_close(device->usbHandle);
			return r;
		}
    }

	return 0;
}

int shutdownUSBDroid(libusb_device *usbDevice, accessory_droid *device)
{
    CL_UNUSED(usbDevice);

	if (device->audioendp)
		libusb_release_interface(device->usbHandle, device->audioInterface);

	if ((device->inendp && device->outendp))
		libusb_release_interface(device->usbHandle, device->bulkInterface);

	if(device->usbHandle != NULL)
		libusb_close(device->usbHandle);

	return 0;
}

int isDroidInAcc(libusb_device *dev)
{
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0)
    {
		logError("Failed to get device descriptor\n");
		return 0;
	}

	if (desc.idVendor == VID_GOOGLE)
    {
		switch(desc.idProduct)
        {
    		case PID_AOA_ACC:
    		case PID_AOA_ACC_ADB:
    		case PID_AOA_ACC_AU:
    		case PID_AOA_ACC_AU_ADB:
    			return 1;
    		case PID_AOA_AU:
    		case PID_AOA_AU_ADB:
    			logDebug("device is audio-only\n");
    			break;
    		default:
    			break;
		}
	}

	return 0;
}

//Used to switch android devices to acc mode
void switchDroidToAcc(libusb_device *dev, int force)
{
	struct libusb_device_handle* handle;
	unsigned char ioBuffer[2];
	int r;
	int deviceProtocol;

	if(0 > libusb_open(dev, &handle))
    {
		logError("Failed to connect to device\n");
		return;
	}

	if(libusb_kernel_driver_active(handle, 0) > 0)
    {
		if(!force)
        {
			logError("Kernel driver active, ignoring device");
			libusb_close(handle);
			return;
		}
		if(libusb_detach_kernel_driver(handle, 0) !=0)
        {
			logError("Failed to detach kernel driver, ignoring device");
			libusb_close(handle);
			return;
		}
	}
    
	if(0> (r = libusb_control_transfer(handle, 
        0xC0, //bmRequestType
		51,   //Get Protocol
		0,
		0,
		ioBuffer,
		2,
		2000)))
    {
		logError("Get protocol call failed %d \n", r);
		libusb_close(handle);
		return;
	}

	deviceProtocol = ioBuffer[1] << 8 | ioBuffer[0];
    
	if (deviceProtocol < AOA_PROTOCOL_MIN || deviceProtocol > AOA_PROTOCOL_MAX)
    {
		logDebug("Unsupported AOA protocol %d\n", deviceProtocol);
		libusb_close(handle);
		return;
	}

	const char *setupStrings[6];
    
	setupStrings[0] = vendor;
	setupStrings[1] = model;
	setupStrings[2] = description;
	setupStrings[3] = version;
	setupStrings[4] = uri;
	setupStrings[5] = serial;

	int i;
	for(i = 0; i < 6; i++)
    {
		if(0 > (r = libusb_control_transfer(handle,
            0x40, //Manufacturer's request
            52,
			0,
			(uint16_t)i,
			(unsigned char*)setupStrings[i],
			strlen(setupStrings[i]),
			2000)))
	    {
			logDebug("Send string %d call failed\n", i);
			libusb_close(handle);
			return;
		}
	}

	if(0 > (r = libusb_control_transfer(handle, 0x40, 53, 0, 0, NULL, 0, 2000)))
    {
		logDebug("Start accessory call failed\n");
		libusb_close(handle);
		return;
	}

	libusb_close(handle);
}
