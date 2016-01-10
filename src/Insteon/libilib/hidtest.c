#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/hiddev.h>

main (int argc, char **argv)
{
	int fd;
	int version;
	struct hiddev_devinfo info;
	char dev_name[32];
	struct hiddev_event event;
	struct hiddev_usage_ref out;

	if (argc > 1) {
		strcpy(dev_name, argv[1]);
	} else {
		strcpy(dev_name, "/dev/usb/hiddev0");
	}

	fd = open (dev_name, O_RDWR);

	if (fd < 0) {
		switch (errno) {
			case EAGAIN: printf("EAGAIN - Try again\n"); break;
			case ENOENT: printf("ENOENT - No such file or directory\n"); break;
			case EBADF: printf("EBADF - Bad file number\n"); break;
			case EFAULT: printf("EFAULT - Bad address\n"); break;
			case EFBIG: printf("EFBIG - File too large\n"); break;
			case EINTR: printf("EINTR - Interrupted system call\n"); break;
			case EINVAL: printf("EINVAL - Invalid argument\n"); break;
			case EIO: printf("EIO - I/O error\n"); break;
			case ENOSPC: printf("ENOSPC - No space on device\n"); break;
			case EPIPE: printf("EPIPE - Broken pipe\n"); break;
			case ENODEV: printf("ENODEV - No such device\n"); break;
			case EBUSY: printf("EBUSY - Device or resource busy\n"); break;
			default: printf("Unknown %d\n", errno); break;
		}

		return 0;
	}

	ioctl(fd, HIDIOCGVERSION, &version);
	printf("returned version %d\n", version);

	ioctl(fd, HIDIOCGDEVINFO, &info);
	printf("device info:\n");
	printf("   bus type:     %d\n", info.bustype);
	printf("   bus num:      %d\n", info.busnum);
	printf("   dev num:      %d\n", info.devnum);
	printf("   if num:       %d\n", info.ifnum);
	printf("   vendor:       %x\n", info.vendor);
	printf("   product:      %x\n", info.product);
	printf("   version:      %x\n", info.version);
	printf("   num apps:     %d\n", info.num_applications);

	/*
	 * HID byte reading:
	 *   Reads from the device should return hiddev_envent structures so
	 *   a read(dev, (struct hiddev_event *)h_ev, sizeof(struct hiddev_event)
	 *   should get anything that is available. The question is, what's in
	 *   this structure:
	 *     hid - & with 0x0000000f to byte index.
	 *     value  - byte read?
	 *
	 *   The PLC only sends small packets (4 bytes max). So if the hid
	 *   index is one, then the value is the number of bytes in the packet.
	 *   In all cases, it looks like all 8 bytes are sent, unused bytes
	 *   are set to zero.
	 *
	 *   ev 0:  hid = 1,  value = 3
	 *   ev 1:  hid = 2,  value - 0x46
	 *   ev 2:  hid = 3,  value = 0x02
	 *   ev 3:  hid = 4,  value = 0x20
	 *   ev 4:  hid = 5,  value = 0x00 (padding)
	 *   ev 5:  hid = 6,  value = 0x00 (padding)
	 *   ev 6:  hid = 7,  value = 0x00 (padding)
	 *   ev 7:  hid = 8,  value = 0x00 (padding)
	 */
	while (1) {
		read(fd, &event, sizeof(struct hiddev_event));
		printf("hid = 0x%x  (%d), value = 0x%02x\n", event.hid, ((event.hid & 0xff000000) >> 24), event.value);
	}

	/*
	 * HID byte writing:
	 *   Looks like you have to build up an hiddev_usage_ref structure for
	 *   each byte and send them, then build a report info struct and send it.
	 */

	/*
	 * Put this in a loop to fill the packet buffer with bytes
	 */
	out.report_type = HID_REPORT_TYPE_OUTPUT;
	out.report_id = HID_REPORT_ID_FIRST;
	out.field_index = 0;
	out.usage_index = buffer_index; /* index in packet */
	out.usage_code= 0;
	out.value = byte_to_send;
	ioctl(fd, HIDIOCSUAGE, &out);

	/*
	 * Once the packet is filled, send a report control to
	 * send the data to the device.
	 */
	report.report_type = HID_REPORT_TYPE_OUTPUT;
	report.report_id = HID_REPORT_ID_FIRST;
	ioctl(fd, HIDIOCSREORT, &report);


	close(fd);
}
