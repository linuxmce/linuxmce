#!/bin/bash

## Put a kernel image in there
mkdir -p /tftpboot/default
cd /lib/modules
KERNEL=$(find * -maxdepth 0 -type d |sort -r |head -1)
cp /boot/vmlinuz-$KERNEL /tftpboot/default/vmlinuz
chmod +r /tftpboot/default/vmlinuz
mkinitramfs -d /etc/initramfs-tools-interactor/ -o /tftpboot/default/initrd $KERNEL

## Setup the debfault boot file
mkdir -p /tftpboot/pxelinux.cfg
echo "
DEFAULT Pluto
LABEL Pluto
KERNEL default/vmlinuz
APPEND root=/dev/nfs acpi=off vga=normal initrd=default/initrd ramdisk_size=10240 rw ip=all apicpmtimer net.ifnames=0 biosdevname=0
" > /tftpboot/pxelinux.cfg/default
