#!/bin/bash

. /etc/lmce-build/builder.conf

#if [ "$KVER" ]
#then
#	Moon_KernelVersion=$KVER
#else
	Moon_KernelVersion=$(basename $(find /lib/modules/* -maxdepth 0 -type d | sort -r | head -1))
#fi

#Moon_KernelArch=$(apt-config dump | grep 'APT::Architecture ' | sed 's/APT::Architecture "\(.*\)".*/\1/g')
Moon_KernelArch="all"
Moon_RootLocation="package"
initramfs_tools_dir="initramfs-tools-default-$flavor-$build_name"
default_name="rpi"
kernel_dir="tftpboot/$default_name"
pxe_config_dir="$Moon_RootLocation/tftpboot/pxelinux.cfg"
pxe_config_file="$pxe_config_dir/$default_name"

# Remove old kernel images from package dir
rm -fr $Moon_RootLocation/$kernel_dir
mkdir -p $Moon_RootLocation/$kernel_dir

#old
## Copy kernel image and sysmap
#if [ ! -f /boot/vmlinuz-${Moon_KernelVersion} ]; then
#	echo Need to install linux-image-${Moon_KernelVersion}
#	apt-get install linux-image-${Moon_KernelVersion}
#fi

#copy current raspbian boot dir to default kernel dir
cp -aR /boot/* ${Moon_RootLocation}/${kernel_dir}/
chmod a+r -R ${Moon_RootLocation}/${kernel_dir}/

# Generate NFS bootable initrd
AddModules()
{
	ModFile="$initramfs_tools_dir/modules"
	for Mod in "$@"; do
		if ! grep -q "^$Mod\$" "$ModFile"; then
			echo "$Mod" >>"$ModFile"
		fi
	done
}

# TODO: verify this is in the file?
sed -i 's/^.*BOOT=.*/BOOT=nfs/g' "$initramfs_tools_dir/initramfs.conf"

#mkinitramfs -d "$initramfs_tools_dir/" -o ${Moon_RootLocation}/${kernel_dir}/initrd.img-${Moon_KernelVersion} ${Moon_KernelVersion}
mkinitramfs -d "$initramfs_tools_dir/" -o ${Moon_RootLocation}/${kernel_dir}/initramfs.img ${Moon_KernelVersion}

cp ${Moon_RootLocation}/DEBIAN/changelog{.in,}
cp ${Moon_RootLocation}/DEBIAN/control{.in,}
sed -i "s/{Flavor}/${flavor}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Build_Name}/${build_name}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/control
sed -i "s/{Kernel_Arch}/${Moon_KernelArch}/g"       ${Moon_RootLocation}/DEBIAN/control
sed -i "s/^.*Package:.*/Package: linux-image-default-$flavor-$build_name/g" $Moon_RootLocation/DEBIAN/control

# Create PXE configuration for default boot
echo "dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=/dev/nfs rw ip=dhcp rootwait elevator=deadline" > ${Moon_RootLocation}/${kernel_dir}/cmdline.txt

cat <<-EEOF >${Moon_RootLocation}/${kernel_dir}/config.txt
	# For more options and information see
	# http://www.raspberrypi.org/documentation/configuration/config-txt.md
	# Some settings may impact device functionality. See link above for details

	# uncomment if you get no picture on HDMI for a default "safe" mode
	#hdmi_safe=1
	#disable_overscan=1
	gpu_mem=128
	#kernel=kernel7.img
	initramfs initramfs.img
EEOF

rm -rf pkgrootdir
cp -a ${Moon_RootLocation} pkgrootdir
find pkgrootdir -type d -name .svn -exec rm -rf '{}' ';' -prune
cd pkgrootdir
dpkg-deb -b . ..
cd ..
rm -rf pkgrootdir
