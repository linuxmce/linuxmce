/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <hid.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	HIDInterface* hid;
	hid_return ret;

	HIDInterfaceMatcher matcher = { 0x0c16, 0x0002, NULL, NULL, 0 };

	/* see include/debug.h for possible values */
	//hid_set_debug(HID_DEBUG_ALL);
	hid_set_debug(0);
	hid_set_debug_stream(stderr);
	/* passed directly to libusb */
	hid_set_usb_debug(0);
	
	ret = hid_init();
	if (ret != HID_RET_SUCCESS)
	{
		fprintf(stderr, "hid_init failed with return code %d\n", ret);
		return 1;
	}

	hid = hid_new_HIDInterface();
	if (hid == 0)
	{
		fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
		return 1;
	}

	ret = hid_force_open(hid, 0, &matcher, 3);
	if (ret != HID_RET_SUCCESS)
	{
		fprintf(stderr, "hid_force_open failed with return code %d\n", ret);
		return 1;
	}

	ret = hid_write_identification(stdout, hid);
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_write_identification failed with return code %d\n", ret);
		return 1;
	}

	const unsigned char RECV_PACKET_LEN = 6;
	unsigned char packet[RECV_PACKET_LEN];
	do
	{
		ret = hid_interrupt_read(hid, 0x81, packet, RECV_PACKET_LEN, 250);
		int i;
		if (ret != HID_RET_SUCCESS && ret != HID_RET_FAIL_INT_READ)
		{
			printf("\n");
			fprintf(stderr, "hid_get_input_report failed with return code %d\n", ret);
		}
		else if (ret != HID_RET_FAIL_INT_READ)
		{
			printf("ret: %d; ", ret);
			for (i = 0; i < RECV_PACKET_LEN; i++)
				printf("%02x ", (unsigned char) packet[i]); // packet that is to be interpreted according to specs
			printf(".\n");
		}
	} while (1);

	ret = hid_close(hid);
	if (ret != HID_RET_SUCCESS)
	{
		fprintf(stderr, "hid_close failed with return code %d\n", ret);
		return 1;
	}

	hid_delete_HIDInterface(&hid);

	ret = hid_cleanup();
	if (ret != HID_RET_SUCCESS)
	{
		fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
		return 1;
	}
	
	return 0;
}
