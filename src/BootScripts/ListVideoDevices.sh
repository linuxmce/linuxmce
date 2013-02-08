#!/bin/bash

# PLEASE, DO NOT DELETE THIS SCRIPT

# Note:
#   This script is not currently used, but when we'll have time to update
#   lmce-admin, we'll use this script for a drop down box similar to the
#   one used for serial ports.
#
#   This would involve changes in the web pages, a Device Data called
#   "Available Video Devices" would probably need to be added to the
#   computer devices, just like "Available Serial Ports", motion would
#   need to be updated to use the output of this script, and a UniqID to
#   /dev-entry translation script was written: TranslateVideoDevice.sh
#
#                                    -- Radu Cristescu

if [[ ! -d /sys/class/video4linux ]]; then
	exit 1
fi

shopt -s nullglob

for Dev in /sys/class/video4linux/*; do
	devV4L=/dev/"${Dev#/sys/class/video4linux/}"
	if [[ "$devV4L" != /dev/video* ]]; then
		continue
	fi
	Model=$(cat "$Dev"/model 2>/dev/null)
	Name=$(cat "$Dev"/name 2>/dev/null)
	#UniqID=$(readlink "$Dev"/device | sed 's,^.*/\(usb.*\).*$,\1,g') # initial form, used for USB webcams
	UniqID=$(readlink "$Dev"/device | sed 's,^.*/\(pci.*\).*$,\1,g') # generic form, which also works for TV Tuners
	echo "$UniqID $devV4L $Model:$Name"
done
