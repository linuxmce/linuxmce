#!/bin/bash

shopt -s nullglob

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

## Notes for future reference
## ttyAMA is one of several things you can find on ARM
## Link: http://yarchive.net/comp/linux/serial.html
## if you're from the future and reading this, you probably encountered one of the others mentioned in that link:
## "ttyAM, ttySA, ttyPXA, and so the list goes on" :)

if [[ -f /proc/tty/driver/ttyAMA ]]; then
	Ports=$(cat /proc/tty/driver/ttyAMA | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyAMA$i"
	done
fi

if [[ -f /proc/tty/driver/starex-serial ]]; then
	Ports=$(cat /proc/tty/driver/starex-serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyD$i"
	done
fi

if [[ -d /sys/class/tty ]]; then
	pushd /sys/class/tty &>/dev/null
	for dev in ttyUSB*/device ttyACM*/device; do
		##id=$(readlink "$dev" | sed 's,^.*/\(usb.*\)/tty.*$,\1,g')
		id=$(readlink -f "$dev" | sed -r 's,^.*(pci.*|platform.*|^usb.*)/usb[0-9]*/[0-9./-]*/[0-9]*-([0-9.]*):[0-9.]*(/ttyUSB[0-9]*)?$,\1+\2,g')
		echo "$id"
	done
	popd &>/dev/null
fi

for Port in /dev/ttyS_*; do
	echo "$Port"
done

for Port in /dev/ttySNX*; do
	echo "$Port"
done
