#!/bin/bash

. /etc/lmce-build/builder.conf

if [ "$KVER" ]
then
	Moon_KernelVersion=$KVER
else
	Moon_KernelVersion=$(uname -r)
fi

#Moon_KernelArch=$(apt-config dump | grep 'APT::Architecture ' | sed 's/APT::Architecture "\(.*\)".*/\1/g')
Moon_KernelArch="all"
Moon_RootLocation='package/'
initramfs_tools_dir="initramfs-tools-default-$flavor-$build_name/"
default_name="default-arm-bcm2835"
kernel_dir="tftpboot/$default_name/"
pxe_config_file="tftpboot/pxelinux.cfg/$default_name"

# Remove old kernel images from package dir
rm -fr $Moon_RootLocation/{$kernel_dir,lib/modules}
mkdir -p $Moon_RootLocation/{$kernel_dir,lib/modules}

# Copy kernel image and sysmap
if [ ! -f /boot/vmlinuz-${Moon_KernelVersion} ]; then
	echo Need to install linux-image-${Moon_KernelVersion}
	apt-get install linux-image-${Moon_KernelVersion}
fi
cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/${kernel_dir}
cp /boot/System.map-${Moon_KernelVersion} ${Moon_RootLocation}/${kernel_dir}

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

sed -i 's/^.*BOOT=.*/BOOT=nfs/g' "$initramfs_tools_dir/initramfs.conf"

mkinitramfs -d "$initramfs_tools_dir" -o ${Moon_RootLocation}/${kernel_dir}/initrd.img-${Moon_KernelVersion} ${Moon_KernelVersion}

# Setup the symlinks
pushd ${Moon_RootLocation}/${kernel_dir}
ln -fs vmlinuz-${Moon_KernelVersion} vmlinuz
ln -fs initrd.img-${Moon_KernelVersion} initrd.img
popd

# Copy from /lib/modules only whare belongs to linux-image-`uname -r`
dpkg -L linux-image-${Moon_KernelVersion}  | grep '^/lib/modules/'  | sed 's|^/lib/modules/||g' | while read line ;do
	if [[ -f "/lib/modules/$line" ]] ;then
		cp "/lib/modules/$line" "${Moon_RootLocation}/lib/modules/$line"
	elif [[ -d "/lib/modules/$line" ]] ;then
		mkdir -p "${Moon_RootLocation}/lib/modules/$line"
	else
		echo "Skiped $line"
	fi
done

# Create PXE configuration for default boot
echo "DEFAULT Pluto
LABEL Pluto
KERNEL default-arm-bcm2835/vmlinuz
APPEND root=/dev/nfs initrd=default-arm-bcm2835/initrd ramdisk_size=10240 rw
" > ${pxe_config_file}

cp ${Moon_RootLocation}/DEBIAN/changelog{.in,}
cp ${Moon_RootLocation}/DEBIAN/control{.in,}
sed -i "s/{Flavor}/${flavor}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Build_Name}/${build_name}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/changelog
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/control
sed -i "s/{Kernel_Arch}/${Moon_KernelArch}/g"       ${Moon_RootLocation}/DEBIAN/control
sed -i "s/^.*Package:.*/Package: linux-image-default-$flavor-$build_name/g" $Moon_RootLocation/DEBIAN/control

rm -rf pkgrootdir
cp -a ${Moon_RootLocation} pkgrootdir
find pkgrootdir -type d -name .svn -exec rm -rf '{}' ';' -prune
cd pkgrootdir
dpkg-deb -b . ..
cd ..
rm -rf pkgrootdir
