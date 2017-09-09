#ifndef __U2SPIPE_H__
#define __U2SPIPE_H__

extern void tickle_usb_xfer_thread(usbXferThread *t);
extern void *u2s_usb_rx_thread(void *d);
extern void *u2s_socket_rx_thread(void *d);

#endif /* __U2SPIPE_H__ */