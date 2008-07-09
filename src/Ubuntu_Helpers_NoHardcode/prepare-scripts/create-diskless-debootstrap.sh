#!/bin/bash

. /usr/local/lmce-build/common/env.sh

function Create_Diskless_Debootstrap_Archive {
	# Create a temp dir to store a diskless debootstrap image into
	local temp_dir="$(mktemp -d)"

	# Run debootstrap in that dir
	debootstrap "$(lsb_release -c -s)" "$temp_dir" http://ro.archive.ubuntu.com/ubuntu/

	# Clean it's apt cache so we don't end up with it in the archive
	rm -f $temp_dir/var/cache/apt/archives/*.deb

	# Pack the diskless image in the right dir
	mkdir -p "$diskless_dir"
	pushd "$temp_dir"
		tar -jcf "$diskless_dir/PlutoMD_Debootstraped.tar.bz2" *
	popd
	
	# Remove the temp dir
	rm -rf "$temp_dir"	
}

Create_Diskless_Debootstrap_Archive

if [ x = y ] ; then

tmp_dir=/tmp/mk_tftpboot
mkdir $tmp_dir
pushd $tmp_dir
	wget http://archive.ubuntu.com/ubuntu/pool/main/l/linux/linux-image-${KVER}_${KVER_SHORT}-${KVER_VERY_SHORT}_i386.deb
	mkdir root
	dpkg -x linux-image-${KVER}_${KVER_SHORT}-${KVER_VERY_SHORT}_i386.deb root
        mkdir -p tftpboot/default/
        mkdir -p tftpboot/pxelinux.cfg/
	cat <<EOF > tftpboot/pxelinux.cfg/default

DEFAULT NFS_BOOT
LABEL NFS_BOOT
KERNEL default/vmlinuz-${KVER}
APPEND root=/dev/nfs acpi=off vga=normal initrd=default/initrd.img-${KVER} ramdisk_size=10240 rw ip=all apicpmtimer

EOF
	mkinitramfs -d /etc/initramfs-tools-diskless/ -o `pwd`/tftpboot/default/initrd.img-$KDEV $KVER
	mv root/* tftpboot/default/
popd

fi