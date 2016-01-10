#ifndef _USB_PROBE_H
#define _USB_PROBE_H

int usb_get_string(int fd, char *buf, size_t size, u_int8_t id, u_int16_t lang);
int usb_get_procfs_driver (int fd, int interface, char *module);

#endif //_USB_PROBE_H
