#!/bin/bash

########################################################################
# rpi2-build-image
# Copyright (C) 2015 Ryan Finnie <ryan@finnie.org>
# Copyright (C) 2015 Jason Murduck <phenigma@hotmail.com> lmce extensions
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
########################################################################

set -e
set -x

DATE=$(date -I)
RELEASE=trusty
BASEDIR=~/rpi2/${RELEASE}
BUILDDIR=${BASEDIR}/build
PROXY="http://10.10.42.99:3142/"

# Don't clobber an old build
if [ -e "$BUILDDIR" ]; then
	echo "$BUILDDIR exists, not proceeding"
	exit 1
fi

# Set up environment
export TZ=UTC
R=${BUILDDIR}/chroot
mkdir -p $R

# Base debootstrap
apt-get -y install ubuntu-keyring
if [ -n "$LOCAL_MIRROR" ]; then
	debootstrap $RELEASE $R $LOCAL_MIRROR
else
	http_proxy=$PROXY debootstrap $RELEASE $R http://ports.ubuntu.com/
fi

# Mount required filesystems
mount -t proc none $R/proc
mount -t sysfs none $R/sys


cat <<-EOM >$R/etc/apt/sources.list
	#deb http://10.10.42.99/trusty-armhf/ ./
	deb http://deb.linuxmce.org/ubuntu/ ${RELEASE} main
	deb http://deb.linuxmce.org/ ${RELEASE} main
	deb http://ports.ubuntu.com/ ${RELEASE} main restricted universe multiverse
	deb http://ports.ubuntu.com/ ${RELEASE}-updates main restricted universe multiverse
	deb http://ports.ubuntu.com/ ${RELEASE}-security main restricted universe multiverse
	deb http://ports.ubuntu.com/ ${RELEASE}-backports main restricted universe multiverse
	EOM

chroot $R apt-get update
chroot $R apt-get -y install sysv-rc upstart screen || :

################################################
################################################
[ -f /etc/apt/apt.conf.d/02proxy ] && cp /etc/apt/apt.conf.d/02proxy $R/etc/apt/apt.conf.d/02proxy
echo 'force-unsafe-io' > $R/etc/dpkg/dpkg.cfg.d/03apt-speedup

echo "Disable all service invocation"
# disable service invocation and tools
mv -f $R/sbin/start{,.orig} || :
mv -f $R/usr/sbin/invoke-rc.d{,.orig} || :
mv -f $R/sbin/restart{,.orig} || :
mv -f $R/sbin/initctl{,.orig} || :
mv -f $R/sbin/start-stop-daemon{,.orig} || :

cat <<-EOF > $R/sbin/start
		#!/bin/bash
		exit 0
		EOF
chmod +x $R/sbin/start
cp $R/sbin/start $R/usr/sbin/invoke-rc.d
cp $R/sbin/start $R/sbin/restart
cp $R/sbin/start $R/sbin/initctl
cp $R/sbin/start $R/sbin/start-stop-daemon

### Disable invoke-rc.d scripts
echo -en '#!/bin/sh\necho "WARNING: we dont want invoke-rc.d to run right now"\nexit 0' >$R/usr/sbin/invoke-rc.d
echo -en '#!/bin/sh\necho "WARNING: fake start called"\nexit 0' >$R/sbin/start
echo -en '#!/bin/sh\necho "WARNING: fake start-stop-daemon called"\nexit 0' >$R/sbin/start-stop-daemon
echo -en '#!/bin/sh\necho "WARNING: fake initctl called"\nexit 0' >$R/sbin/initctl
echo -en '#!/bin/sh\necho "WARNING: fake restart called"\nexit 0' >$R/sbin/restart
chmod +x $R/usr/sbin/invoke-rc.d
chmod +x $R/sbin/start-stop-daemon
chmod +x $R/sbin/start
chmod +x $R/sbin/restart
chmod +x $R/sbin/initctl
################################################
################################################

chroot $R apt-get -y dist-upgrade

# Install the RPi PPA
chroot $R apt-get -y --no-install-recommends install software-properties-common ubuntu-keyring
chroot $R apt-add-repository -y ppa:fo0bar/rpi2
chroot $R apt-get update

# Standard packages
chroot $R apt-get -y install ubuntu-standard initramfs-tools raspberrypi-bootloader-nokernel language-pack-en

# Kernel installation
# Install flash-kernel last so it doesn't try (and fail) to detect the
# platform in the chroot.
FLASH_KERNEL_SKIP="true" chroot $R apt-get -y --no-install-recommends install linux-image-rpi2 linux-headers-rpi2

FLASH_KERNEL_SKIP="true" chroot $R apt-get -y --no-install-recommends install wpasupplicant linux-firmware openssh-server nano cifs-utils pastebinit

FLASH_KERNEL_SKIP="true" chroot $R apt-get -y --allow-unauthenticated --no-install-recommends install lmce-disked-md-rpi2

FLASH_KERNEL_SKIP="true" chroot $R apt-get -y --allow-unauthenticated --no-install-recommends install lmce-md-meta

chroot $R apt-get -y --no-install-recommends install flash-kernel

VMLINUZ="$(ls -1 $R/boot/vmlinuz-* | sort | tail -n 1)"
[ -z "$VMLINUZ" ] && exit 1
cp $VMLINUZ $R/boot/firmware/kernel7.img

# Set up fstab
cat <<-EOM >$R/etc/fstab
	proc            /proc           proc    defaults          0       0
	/dev/mmcblk0p2  /               ext4    defaults,noatime  0       1
	/dev/mmcblk0p1  /boot/firmware  vfat    defaults          0       2
	EOM

# Set up hosts
echo ubuntu >$R/etc/hostname
cat <<-EOM >$R/etc/hosts
	127.0.0.1       localhost
	::1             localhost ip6-localhost ip6-loopback
	ff02::1         ip6-allnodes
	ff02::2         ip6-allrouters

	127.0.1.1       ubuntu
	EOM

# Set up default user
chroot $R adduser --gecos "Ubuntu user" --add_extra_groups --disabled-password ubuntu
chroot $R usermod -a -G sudo,adm -p '$6$iTPEdlv4$HSmYhiw2FmvQfueq32X30NqsYKpGDoTAUV2mzmHEgP/1B7rV3vfsjZKnAWn6M2d.V2UsPuZ2nWHg1iqzIu/nF/' ubuntu

# Clean cached downloads
chroot $R apt-get clean

# Set up interfaces
cat <<-EOM >$R/etc/network/interfaces
	# interfaces(5) file used by ifup(8) and ifdown(8)
	# Include files from /etc/network/interfaces.d:
	source-directory /etc/network/interfaces.d

	# The loopback network interface
	auto lo
	iface lo inet loopback

	# The primary network interface
	allow-hotplug eth0
	iface eth0 inet dhcp
	EOM

# Set up firmware config
cat <<-EOM >$R/boot/firmware/config.txt
	disable_overscan=1
	config_hdmi_boost=4
	EOM
ln -sf firmware/config.txt $R/boot/config.txt

echo 'dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootwait' > $R/boot/firmware/cmdline.txt
ln -sf firmware/cmdline.txt $R/boot/cmdline.txt

# Load sound module on boot
cat <<-EOM >$R/lib/modules-load.d/rpi2.conf
	snd_bcm2835
	bcm2708_rng
	EOM

# Blacklist platform modules not applicable to the RPi2
cat <<-EOM >$R/etc/modprobe.d/rpi2.conf
	blacklist snd_soc_pcm512x_i2c
	blacklist snd_soc_pcm512x
	blacklist snd_soc_tas5713
	blacklist snd_soc_wm8804
	EOM

################################################
################################################
mv -f $R/usr/sbin/invoke-rc.d{.orig,} || :
mv -f $R/sbin/start-stop-daemon{.orig,} || :
mv -f $R/sbin/start{.orig,} || :
mv -f $R/sbin/restart{.orig,} || :
mv -f $R/sbin/initctl{.orig,} || :

[ -f $R/etc/apt/apt.conf.d/02proxy ] && rm $R/etc/apt/apt.conf.d/02proxy
[ -f $R/etc/apt/apt.conf.d/03apt-speedup ] && rm $R/etc/apt/apt.conf.d/03apt-speedup
################################################
################################################


# Unmount mounted filesystems
umount $R/proc
umount $R/sys

# Clean up files
rm -f $R/etc/apt/sources.list.save
rm -f $R/etc/resolvconf/resolv.conf.d/original
rm -rf $R/run
mkdir -p $R/run
rm -f $R/etc/*-
rm -f $R/root/.bash_history
rm -rf $R/tmp/*
rm -f $R/var/lib/urandom/random-seed
[ -L $R/var/lib/dbus/machine-id ] || rm -f $R/var/lib/dbus/machine-id
rm -f $R/etc/machine-id

# Build the image file
# Currently hardcoded to a 1.75GiB image
DATE="$(date +%Y-%m-%d)"
dd if=/dev/zero of="$BASEDIR/${DATE}-ubuntu-${RELEASE}.img" bs=1M count=1
dd if=/dev/zero of="$BASEDIR/${DATE}-ubuntu-${RELEASE}.img" bs=1M count=0 seek=1792

sfdisk -f "$BASEDIR/${DATE}-ubuntu-${RELEASE}.img" <<EOM
unit: sectors

1 : start=     2048, size=   131072, Id= c, bootable
2 : start=   133120, size=  3536896, Id=83
3 : start=        0, size=        0, Id= 0
4 : start=        0, size=        0, Id= 0
EOM

VFAT_LOOP="$(losetup -o 1M --sizelimit 64M -f --show $BASEDIR/${DATE}-ubuntu-${RELEASE}.img)"
EXT4_LOOP="$(losetup -o 65M --sizelimit 1727M -f --show $BASEDIR/${DATE}-ubuntu-${RELEASE}.img)"
mkfs.vfat "$VFAT_LOOP"
mkfs.ext4 "$EXT4_LOOP"
MOUNTDIR="$BUILDDIR/mount"
mkdir -p "$MOUNTDIR"
mount "$EXT4_LOOP" "$MOUNTDIR"
mkdir -p "$MOUNTDIR/boot/firmware"
mount "$VFAT_LOOP" "$MOUNTDIR/boot/firmware"
rsync -a "$R/" "$MOUNTDIR/"
umount "$MOUNTDIR/boot/firmware"
umount "$MOUNTDIR"
losetup -d "$EXT4_LOOP"
losetup -d "$VFAT_LOOP"
if which bmaptool ; then
	bmaptool create -o "$BASEDIR/${DATE}-ubuntu-${RELEASE}.bmap" "$BASEDIR/${DATE}-ubuntu-${RELEASE}.img"
fi

echo "DONE!"
# Done!

