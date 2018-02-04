#ifndef USB_MANAGE_H
#define USB_MANAGE_H

int usb_removable_find(char *dev_path);
int usb_removable_unmount(char *dev_path);
int usb_removable_mount(char *dev_path);
int usb_removable_prep(char *dev_path);

#endif
