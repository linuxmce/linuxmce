#!/bin/bash

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

# Pattern matches paths like these:
# Ubuntu 7.10: ../../../devices/pci0000:00/0000:00:06.0/usb4/4-2/4-2:1.0/ttyUSB0
# Ubuntu 7.10: ../../../devices/pci0000:00/0000:00:04.0/usb1/1-1/1-1:1.0
# Ubuntu 8.10:    ../../devices/pci0000:00/0000:00:1d.0/usb2/2-2/2-2:1.0/ttyUSB0/tty/ttyUSB0
# Ubuntu 8.10:    ../../devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1:1.0/tty/ttyACM0
SedPattern_UsbId='s,.*(pci.*)/usb[0-9]*/[0-9./-]*/[0-9]*-([0-9.:]*)[0-9.]*(/tty(USB[0-9]*(/tty/.*)?|/ttyACM[0-9]*))?$,\1+\2,g' 

# Look for ttyUSB and ttyACM devices
Ports=
while read Dev; do
	Path=
	if [[ -L "$Dev" ]]; then
		# Ubuntu 8.10 has symlinks to directories here
		Path=$(readlink "$Dev")
	elif [[ -d "$Dev" ]]; then
		# Ubuntu 7.10 has real directories here (symlink test failed above, so this is a real directory)
		# And there's a "device" symlink in each directory that contains the info we want
		Path=$(readlink "$Dev"/device)
	fi
	# TODO: see what 8.04 has in here (I think there's no difference between 8.04 and 8.10 in this respect)

	if [[ -z "$Path" ]]; then
		# Something didn't go right, so don't add any extra blanks to the port list
		# Normally, this branch shouldn't be hit at all
		continue
	fi
	Id=$(echo "$Path" | sed -r "$SedPattern_UsbId")
	Ports="$Ports $Id"
done < <(find /sys/class/tty -mindepth 1 -maxdepth 1 -name 'ttyUSB*' -or -name 'ttyACM*')
echo "${Ports# }" | tr ' ' '\n'

shopt -s nullglob
for Port in /dev/ttyS_*; do
	echo "$Port"
done
