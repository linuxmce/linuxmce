#!/bin/bash

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

Ports=
Ports=`find /sys/devices -name '*tty*' | grep '/tty:' | grep usb | sed 's,^.*\(pci.*\)/usb[0-9]*/[0-9./-]*/[0-9]*-\([0-9.]*\):[0-9.]*/tty.*$,\1+\2,g'`
echo "${Ports# }" | tr ' ' '\n'

shopt -s nullglob
for Port in /dev/ttyS_*; do
	echo "$Port"
done
