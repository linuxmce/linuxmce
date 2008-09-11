#!/bin/bash

set -e

Debug=0

TargetHdd=
BootUUID=
SwapUUID=
RootUbuntuUUID=
RootDebianUUID=

GetConsole()
{
	exec &>/dev/tty1 </dev/tty1
	dmesg -n1
	clear
	chvt 1
}

GetHddToUse()
{
	local HddList Done Drive i Choice DiskSize DiskDev
	local -a Hdd
	local BootParm

	HddList=$(fdisk -l|grep ^Disk |cut -d' ' -f2-4|sed -r 's,[:,],,g; s, ,:,g')
	if [[ "$Debug" == 1 ]]; then
		HddList="/dev/loop0:0:GB"
	fi
	if [[ "$FromHdd" == 1 ]]; then
		for BootParm in $(</proc/cmdline); do
			if [[ "$BootParm" == "root="* ]]; then
				TargetHdd="${BootParm#root=}"
				TargetHdd="${TargetHdd//[0-9]}"
				break
			fi
		done
		if [[ -z "$TargetHdd" ]]; then
			echo "Error: Failed to determine current hard drive"
			exit 1
		fi
	elif [[ -z "$HddList" ]]; then # No hard drives
		echo "Error: No hard drives found"
		exit 1
	else
		echo "Hard drives in the system:"
		Done=0
		while [[ "$Done" == 0 ]]; do
			i=1
			for Drive in $HddList; do
				DiskDev="${Drive%%:*}"
				DiskSize="${Drive#*:}"
				set +e
				DiskDescription="$(hdparm -I $DiskDev | grep "Model Number:" | cut -d ':' -f2 | sed 's/^ *//g')"
				DiskSerial="$(hdparm -I $DiskDev | grep "Serial Number:" | cut -d ':' -f2 | sed 's/^ *//g')"
				set -e
				echo "$i. $DiskDev ($DiskDescription $DiskSerial ${DiskSize//:/ })"
				Hdd[$i]="$DiskDev"
				((i++))
			done
			((i--))
			echo -n "Choose which drive to install the system on: "
			read Choice
			if [[ "$Choice" == *[^0-9]* || "$Choice" -lt 1 || "$Choice" -gt "$i" ]]; then
				echo "Invalid selection. Try again"
			else
				TargetHdd="${Hdd[$Choice]}"
				Done=1
			fi
		done
	fi

	echo "Chosen hdd: $TargetHdd"
	if [[ -z "$TargetHdd" ]]; then
		echo "Got out of loop with empty selection. This shouldn't be possible."
		exit 1
	fi
}

PartitionHdd()
{
	if [[ "$FromHdd" == "1" ]] ;then
		return
	fi
set +e
	swapoff -a
	parted -s "$TargetHdd" -- mklabel msdos
	parted -s "$TargetHdd" -- mkpart primary ext2 0 500MB # /boot
	parted -s "$TargetHdd" -- mkpart extended 500MB -1s
	parted -s "$TargetHdd" -- mkpart logical linux-swap 500MB 1500MB # swap
	parted -s "$TargetHdd" -- mkpart logical ext2 1500MB 6500MB # / for Debian/Pluto
	parted -s "$TargetHdd" -- mkpart logical ext2 6500MB 16500MB # recovery system
	parted -s "$TargetHdd" -- mkpart logical ext2 16500MB -1s # / for Kubuntu
	sleep 1
	blockdev --rereadpt "$TargetHdd"
	sleep 5
set -e
}

FormatPartitions()
{
	if [[ "$Debug" == 1 ]]; then
		return 0
	fi
	echo y|mkfs.ext3 "$TargetHdd"6 # / for Debian/Pluto
	if [[ "$FromHdd" != "1" ]] ;then
		echo y|mkfs.ext3 "$TargetHdd"7 # recovery system
		mkswap "$TargetHdd"5
		echo y|mkfs.ext3 "$TargetHdd"1
	fi
	echo y|mkfs.ext3 "$TargetHdd"8 # / for Kubuntu

	blkid -w /etc/blkid.tab || :
	BootUUID=$(vol_id -u "$TargetHdd"1)
	SwapUUID=$(vol_id -u "$TargetHdd"5)
	RootDebianUUID=$(vol_id -u "$TargetHdd"6)
	RootUbuntuUUID=$(vol_id -u "$TargetHdd"8)
}

MountPartitions()
{
	mkdir -p /media/target/{boot,debian,ubuntu}
	mkdir -p /media/recovery
	if [[ "$FromHdd" != "1" ]] ;then
		mount "$TargetHdd"7 /media/recovery
	else
		mount -o bind /     /media/recovery
	fi

	mount "$TargetHdd"1 /media/target/boot
	mount "$TargetHdd"6 /media/target/debian
	mount "$TargetHdd"8 /media/target/ubuntu
}

CopyDVD()
{
	if [[ "$FromHdd" == "1" ]] ;then
		return
	fi
	local DVDdir=$(mktemp -d)
	mount -t squashfs -o loop,ro /cdrom/casper/filesystem.squashfs "$DVDdir"
	cp -a "$DVDdir"/. /media/recovery/
	cp /cdrom/lmce-image-via/firstboot-via /media/recovery
	cp /cdrom/lmce-image-via/LinuxMCE.desktop /media/recovery
	umount "$DVDdir"
	
	mkdir -p /media/recovery/archives/
	cp -a /cdrom/lmce-image-via/. /media/recovery/archives/
}

ExtractArchives()
{
	echo "Extracting archives (this will take a while)"

	# Boot
	echo "Boot archive"
	tar -C /media/target/boot --checkpoint=10000 -zxf /media/recovery/archives/boot.tgz

	# Debian
	echo "Debian archive"
	tar -C /media/target/debian --checkpoint=10000 -zxf /media/recovery/archives/pluto44.tgz

	# Kubuntu
	echo "Kubuntu archive"
	tar -C /media/target/ubuntu --checkpoint=10000 -zxf /media/recovery/archives/kubuntu.tgz

	# Update the UUIDs
	rm /media/target/ubuntu/etc/blkid.tab || :
	blkid -w /media/target/ubuntu/etc/blkid.tab

	#Bind boot into debian and ubuntu filesystems
	mount --bind /media/target/boot /media/target/debian/boot
	mount --bind /media/target/boot /media/target/ubuntu/boot
}

PrepareFirstBoot()
{
	#Copy the fist run script
	cp /media/recovery/firstboot-via /media/target/debian/etc/init.d/firstboot-via
	#ln -s /etc/init.d/firstboot-via /media/target/debian/etc/rc2.d/S90firstboot-via
	sed '/^1:2345:/ s,^.*$,1:2345:once:/etc/init.d/firstboot-via,g' /media/target/debian/etc/inittab >/media/target/debian/etc/inittab.new
	mv /media/target/debian/etc/inittab{.new,}

	mkdir -p /media/target/ubuntu/usr/share/apps/kdesktop/Desktop/
	cp /media/recovery/LinuxMCE.desktop /media/target/ubuntu/usr/share/apps/kdesktop/Desktop/

	rm -f /media/target/debian/etc/rc2.d/*Pluto*
	>/media/target/debian/usr/pluto/install/.notdone
	sed '/^FirstBoot/d' /media/target/debian/etc/pluto.conf >/media/target/debian/etc/pluto.conf.new
	mv /media/target/debian/etc/pluto.conf{.new,}
}

SetupFstab()
{
	local fstab_text

	fstab_text="# /etc/fstab: static file system information.
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>                                                                                      proc            /proc           proc    defaults        0       0
${TargetHdd}6   /               ext3    defaults,errors=remount-ro,user_xattr 0       1
${TargetHdd}1   /boot           ext3    defaults,user_xattr                   0       2
${TargetHdd}5   none            swap    sw                                    0       0
${TargetHdd}8   /media/ubuntu   ext3    defaults,errors=remount-ro              0       1
/dev/cdrom      /media/cdrom0   udf,iso9660 user,noauto                       0       0
"
	echo "$fstab_text" >/media/target/debian/etc/fstab

	fstab_text="# /etc/fstab: static file system information.
#
# <file system> <mount point>    <type>  <options>                 <dump>  <pass>
proc              /proc           proc    defaults                   0       0
${TargetHdd}8     /               ext3    defaults,errors=remount-ro 0       1
${TargetHdd}1     /boot           ext3    defaults                   0       2
${TargetHdd}5     none            swap    sw                         0       0
${TargetHdd}6     /media/debian   ext3    defaults,errors=remount-ro 0       0
/dev/cdrom        /media/cdrom0   udf,iso9660 user,noauto            0       0
"
	echo "$fstab_text" >/media/target/ubuntu/etc/fstab
}

InstallGrub()
{
	mkdir -p /media/target/ubuntu/{proc,sys,dev}
	mount --bind /dev/ /media/target/ubuntu/dev/
	mount -t proc -o bind /proc /media/target/ubuntu/proc
	grub-install --recheck --root-directory=/media/target/ubuntu "$TargetHdd"
	umount /media/target/ubuntu/{proc,dev}

	## XXX: "sed -i" doesn't seem to work
	sed -r "s,root=UUID=[^ ]*,root=${TargetHdd}8,g; s,root=/dev/hdc,root=${TargetHdd},g" /media/target/boot/grub/menu.lst >/tmp/menu1.lst
	sed -r 's/^default[\t ]+.*$/default saved/g' /tmp/menu1.lst >/media/target/boot/grub/menu.lst
	echo "
	title           System Recovery
	root            (hd0,6)
	kernel          /boot/vmlinuz-2.6.20-15-generic root=${TargetHdd}7 from_hdd_recovery
	initrd          /boot/initrd.img-2.6.20-15-generic
	" >> /media/target/boot/grub/menu.lst
	/usr/sbin/grub-set-default --root-directory=/media/target/boot 2 # boot Debian by default
}

TargetCleanup()
{
	local NR ifnr ifname link ifmac rest line

	NR=0
	echo "# This file assigns persistent names to network interfaces.
# See iftab(5) for syntax.
" >/media/target/ubuntu/etc/iftab

	while read line; do
		if ((NR < 2)); then
			((NR++))
			continue
		fi
		if ((NR % 2 == 0)); then
			read ifnr ifname rest < <(echo "$line")
		else
			read link ifmac rest < <(echo "$line")
			echo "${ifname%:} mac $ifmac arp 1"
		fi
		((NR++))
	done < <(/sbin/ip l) >>/media/target/ubuntu/etc/iftab
	#chroot /media/target/debian/ update-initramfs -u
	chroot /media/target/ubuntu/ update-initramfs -u

	#chroot /media/target update-grub

	# Create mount point for eachother's filesystems
	mkdir -p /media/target/ubuntu/media/debian
	mkdir -p /media/target/debian/media/ubuntu
}

UnmountPartitions()
{
	set +e
	umount /media/target/{debian,ubuntu}/boot
	umount /media/target/*
	umount /media/recovery
	set -e
}

Reboot()
{
	clear
	if [[ "$FromHdd" != 1 ]]; then
		exec &>/dev/tty8 </dev/tty8
		clear
		chvt 8
	fi
	echo "**********************"
	echo "Installation complete."
	echo "**********************"
	reboot
}
