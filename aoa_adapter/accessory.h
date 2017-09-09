#ifndef __ACCESSORY_H__
#define __ACCESSORY_H__

#include <libusb.h>

#define AOA_PROTOCOL_MIN	1
#define AOA_PROTOCOL_MAX	2

#define VID_GOOGLE			0x18D1  
#define	PID_AOA_ACC			0x2D00  //Supports Accessory Mode
#define	PID_AOA_ACC_ADB		0x2D01  //Supports Audio
#define	PID_AOA_AU			0x2D02  //Supports Audio
#define	PID_AOA_AU_ADB		0x2D03  //Supports Audio + ADB
#define	PID_AOA_ACC_AU		0x2D04  //Supports AOA 1.0 + Audio
#define	PID_AOA_ACC_AU_ADB	0x2D05  //Supports AOA 1.0 + Audio + ADB

typedef void (*fnusb_iso_cb)(unsigned char *buf, int len);

typedef struct
{
	struct libusb_transfer **xfers;
	unsigned char *buffer;
	fnusb_iso_cb cb;
	int num_xfers;
	int pkts;
	int len;
	int dead;
	int dead_xfers;
} fnusb_isoc_stream;

typedef struct t_accessory_droid
{
	libusb_device_handle *usbHandle;
	unsigned char inendp;
	unsigned char outendp;
	unsigned char audioendp;

	int inpacketsize;
	int outpacketsize;
	int audiopacketsize;

	unsigned char bulkInterface;
	unsigned char audioInterface;
	unsigned char audioAlternateSetting;

	fnusb_isoc_stream isocStream;
} accessory_droid;

extern int isDroidInAcc(libusb_device *dev);
extern void switchDroidToAcc(libusb_device *dev, int force);
extern int setupDroid(libusb_device *usbDevice, accessory_droid *device);
extern int shutdownUSBDroid(libusb_device *usbDevice, accessory_droid *device);

#endif /* __ACCESSORY_H__ */
