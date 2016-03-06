#!/bin/bash

. /etc/lmce-build/builder.conf

if [ "$KVER" ]
then
	Moon_KernelVersion=$KVER
else
	Moon_KernelVersion=$(uname -r)
fi
#Moon_KernelArch="i386"
#Moon_KernelArch=$(apt-config dump | grep 'APT::Architecture ' | sed 's/APT::Architecture "\(.*\)".*/\1/g')

# We want to build the package for 'all' so this is the default diskless image regardless of core architecture.
Moon_KernelArch="all"
Moon_RootLocation='package/'
# Remove old kernel images from package dir
rm -fr $Moon_RootLocation/{boot,lib/modules}
mkdir -p $Moon_RootLocation/{boot,lib/modules}

# Copy kernel image and sysmap
if [ ! -f /boot/vmlinuz-${Moon_KernelVersion} ]; then
	echo Need to install linux-image-${Moon_KernelVersion}
	apt-get install linux-image-${Moon_KernelVersion}
fi
cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot
cp /boot/System.map-${Moon_KernelVersion} ${Moon_RootLocation}/boot/


# Generate NFS bootable initrd
AddModules()
{
	ModFile=/etc/initramfs-tools-diskless/modules
	for Mod in "$@"; do
		if ! grep -q "^$Mod\$" "$ModFile"; then
			echo "$Mod" >>"$ModFile"
		fi
	done
}

if [[ ! -d /etc/initramfs-tools-diskless ]]; then
	cp -a /etc/initramfs-tools{,-diskless}
fi
sed -i 's/^.*BOOT=.*/BOOT=nfs/g' /etc/initramfs-tools-diskless/initramfs.conf

# NOTE: when you add a module to this list,
# NOTE: please be sure to update and rebuild pluto-default-tftpboot too
# NOTE: reference: UbuntuDiskless/Diskless_BuildDefaultImage.sh
AddModules sky2 atl1

mkinitramfs -d /etc/initramfs-tools-diskless/ -o ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion} ${Moon_KernelVersion}

# Setup the symlinks
pushd ${Moon_RootLocation}/boot
ln -s vmlinuz-${Moon_KernelVersion} vmlinuz
ln -s initrd.img-${Moon_KernelVersion} initrd.img
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

cp ${Moon_RootLocation}/DEBIAN/control{.in,}
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/control
sed -i "s/{Kernel_Arch}/${Moon_KernelArch}/g"       ${Moon_RootLocation}/DEBIAN/control

# Create .deb file
rm -rf pkgrootdir
cp -a ${Moon_RootLocation} pkgrootdir
find pkgrootdir -type d -name .svn -exec rm -rf '{}' ';' -prune
cd pkgrootdir
dpkg-deb -b . ..
cd ..
rm -rf pkgrootdir

## Create .changes file
Package_Filename="./linux-image-diskless_${Moon_KernelVersion}-lmce1_all.deb"
changes_file="./linux-image-diskless_${Moon_KernelVersion}-lmce1_all.changes"
cp ./linux-image-diskless.changes.in "${changes_file}"
Date=$(date +"%a, %d %b %Y %X %z")
Package_sha1sum=$(sha1sum "${Package_Filename}" | cut -d' ' -f1 )
Package_sha256sum=$(sha256sum "${Package_Filename}" | cut -d' ' -f1 )
Package_md5sum=$(md5sum "${Package_Filename}" | cut -d' ' -f1 )
Package_Size=$(du -b "${Package_Filename}" | cut -f1 )
Moon_Distribution=$(lsb_release -cs)
sed -i "s/{Date}/${Date}/g" "${changes_file}"
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" "${changes_file}"
sed -i "s/{Kernel_Arch}/${Moon_KernelArch}/g" "${changes_file}"
sed -i "s/{Distribution}/${Moon_Distribution}/g" "${changes_file}"
sed -i "s,{Package_Filename},${Package_Filename},g" "${changes_file}"
sed -i "s/{Package_sha1sum}/${Package_sha1sum}/g" "${changes_file}"
sed -i "s/{Package_sha256sum}/${Package_sha256sum}/g" "${changes_file}"
sed -i "s/{Package_md5sum}/${Package_md5sum}/g" "${changes_file}"
sed -i "s/{Package_Size}/${Package_Size}/g" "${changes_file}"

exit 0
