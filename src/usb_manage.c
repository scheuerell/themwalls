#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>
#include "usb_manage.h"

/*
int main (void)
{
	
	int retval;
	char dev_path[100];

	retval = usb_removable_find(dev_path);
	if (retval != 0) {
		printf("Unable to find removable usb storage device. Return Value: %d\n", retval);
	}	
	retval = usb_removable_unmount(dev_path);
	if (retval != 0) {
		printf("Unable to unmount removable usb storage device. Return Value: %d\n", retval);
	}	
	retval = usb_removable_mount(dev_path);
	if (retval != 0) {
		printf("Unable to mount removable usb storage device. Return Value: %d\n", retval);
	}	
	
	return 0;       
}
*/

int usb_removable_prep(char *dev_path)
{
	int retval;
	
	/* find the first USB flash drive */
	retval = usb_removable_find(dev_path);
	if (retval != 0) {
		//printf("Unable to find removable usb storage device. Return Value: %d\n", retval);
	}
	/* unmount the drive */	
	retval = usb_removable_unmount(dev_path);
	if (retval != 0) {
		//printf("Unable to unmount removable usb storage device. Return Value: %d\n", retval);
	}	
	/* re-mount the drive to /media/pi/USB_DISK */
	retval = usb_removable_mount(dev_path);
	if (retval != 0) {
		//printf("Unable to mount removable usb storage device. Return Value: %d\n", retval);
	}	
	return retval;
}

int usb_removable_mount(char *dev_path)
{
	char sys_cmd[100];
	int retval;
	
	/* create the preferred directory */
	strcpy(sys_cmd, "sudo mkdir -p /media/pi/USB_DISK");
	//printf("%s\n", sys_cmd);
	system(sys_cmd);
	/* mount the device to the preferred directory */
	strcpy(sys_cmd, "sudo mount -o uid=1000 ");
	//strcpy(sys_cmd, "sudo mount -o uid=$UID ");
	strcat(sys_cmd, dev_path);
	strcat(sys_cmd, " /media/pi/USB_DISK");
	//printf("%s\n", sys_cmd);
	retval = system(sys_cmd);
	return retval; 
}

int usb_removable_unmount(char *dev_path)
{
	char sys_cmd[100];
	int retval;
	
	/* unmount the device using system() */
	strcpy(sys_cmd, "sudo umount ");
	strcat(sys_cmd, dev_path);
	//printf("%s\n", sys_cmd);
	retval = system(sys_cmd);
	return retval; 
}

int usb_removable_find(char *dev_path)
{
	struct udev_device *dev;
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices; 
	const char *path;
	
	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		return 2;
	}
	
	/* find the first removable device in the 'block' subsystem,
	 * with non-zero size */
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "block");
	udev_enumerate_add_match_sysattr(enumerate, "removable", "1");
	udev_enumerate_add_nomatch_sysattr(enumerate, "size", "0");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* Get the filename of the /sys entry for the device
	   and create a udev_device object (dev) representing it */
	path = udev_list_entry_get_name(devices);
	dev = udev_device_new_from_syspath(udev, path);
	if (!dev) {
		return 1;
	}
	/* copy the device path */
	strcpy(dev_path, udev_device_get_devnode(dev));
	/* FIXME - assumes partition = 1 */
	strcat(dev_path, "1");

	/* Free the dev, udev, and enumerator objects */
	udev_device_unref(dev);
	udev_enumerate_unref(enumerate);
	udev_unref(udev);
	
	return 0;
}
