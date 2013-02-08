
MIRROR=http://archive.ubuntu.com/ubuntu/pool/main/l/linux

KVER_SHORT=2.6.24
KVER_EXTRA=-19.41
KVER_UBUNTU=-19-generic


KVER=$(KVER_SHORT)$(KVER_UBUNTU)
KVER_LONG=$(KVER_SHORT)$(KVER_EXTRA)



all: default vmlinuz-${KVER} initrd.img-${KVER}

clean:
	-rm -f default vmlinuz-${KVER} initrd.img-${KVER}

distclean: clean
	rm -rf root
	rm -f linux-image-${KVER}_${KVER_LONG}_i386.deb

install: all
	mkdir -pv $(DESTDIR)/tftpboot/default $(DESTDIR)/tftpboot/pxelinux.cfg
	cp -av default $(DESTDIR)/tftpboot/pxelinux.cfg/
	cp -av vmlinuz-${KVER} initrd.img-${KVER} $(DESTDIR)/tftpboot/default/


default:
	echo > $@
	echo "DEFAULT NFS_Bootstrap" >> $@
	echo "LABEL NFS_Bootstrap" >> $@
	echo "KERNEL default/vmlinuz-${KVER}" >> $@
	echo "APPEND root=/dev/nfs acpi=off vga=normal initrd=default/initrd.img-${KVER} ramdisk_size=10240 rw ip=all apicpmtimer" >> $@
	echo "" >> $@


linux-image-${KVER}_${KVER_LONG}_i386.deb:
	wget ${MIRROR}/$@


vmlinuz-${KVER}: linux-image-${KVER}_${KVER_LONG}_i386.deb
	mkdir -pv root
	dpkg -x linux-image-${KVER}_${KVER_LONG}_i386.deb root
	mv root/boot/vmlinuz-${KVER} ./
	rm -rf root


initrd.img-${KVER}:
	mkinitramfs -d /etc/initramfs-tools-diskless/ -o $@ ${KVER}




