#!/bin/bash
#!/bin/bash
## Put a kernel image in there
mkdir -p /tftpboot/default
LATEST_KERNEL=`ls /lib/modules --sort time --group-directories-first | head -1`
if [ ! -f /tftpboot/default/vmlinuz ]; then
        cp /boot/vmlinuz-$LATEST_KERNEL /tftpboot/default/vmlinuz
fi
if [ ! -f /tftpboot/default/initrd ]; then
	mkinitramfs -d /etc/initramfs-tools-interactor/ -o /tftpboot/default/initrd $LATEST_KERNEL
fi
                
## Setup the debfault boot file
mkdir -p /tftpboot/pxelinux.cfg
if [ ! -f /tftpboot/pxelinux.cfg/default ] ; then
	echo "
DEFAULT Pluto
LABEL Pluto
KERNEL default/vmlinuz
APPEND root=/dev/nfs acpi=off vga=normal initrd=default/initrd ramdisk_size=10240 rw ip=all apicpmtimer
" > /tftpboot/pxelinux.cfg/default
fi                    
