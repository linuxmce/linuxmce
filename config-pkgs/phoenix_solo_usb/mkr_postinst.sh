#!/bin/sh

cat <<-EEOF >/etc/udev/rules.d/10-pheonix-duet.rules
	SUBSYSTEM=="usb", ATTRS{idVendor}=="1de7", ATTRS{idProduct}=="0114", MODE="0660", GROUP="plugdev"
	ATTRS{idVendor}=="1de7", ATTRS{idProduct}=="0114", DRIVER=="usbhid", RUN="/bin/sh -c 'echo -n $kernel >/sys/bus/usb/drivers/usbhid/unbind'"
	EEOF

# Kill SimplePhone so it will get the new configuration
killall -9 SimplePhone
