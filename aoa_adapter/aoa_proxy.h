#ifndef __AOA_PROXY_H__
#define __AOA_PROXY_H__

#include <libusb.h>
#include <pthread.h>
#include "accessory.h"

typedef struct t_audioXfer
{
	int stop;
} audioXfer;

typedef struct t_usbXferThread
{
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	struct libusb_transfer *xfr;
	int usbActive;
	int stop;
	int stopped;
	int tickle;
} usbXferThread;

typedef struct
{
    int cmd_socketfd;
    int cmd_socketDead;

    int video_socketfd;
    int video_socketDead;

    int media_socketfd;
    int media_socketDead;

    int tts_socketfd;
    int tts_socketDead;

    int vr_socketfd;
    int vr_socketDead;

    int touch_socketfd;
    int touch_socketDead;
} t_socketfd;

typedef struct listentry
{
	libusb_device *usbDevice; //usb device
	t_socketfd *sockfd;
	int usbDead;
	accessory_droid droid;  //accessory information
	usbXferThread usbRxThread; //Write to a usb thread
	usbXferThread socketRxThread; //Read the usb thread
	audioXfer audio;

	struct listentry *prev;
	struct listentry *next;
} t_listentry;

#endif /* __AOA_PROXY_H__ */
