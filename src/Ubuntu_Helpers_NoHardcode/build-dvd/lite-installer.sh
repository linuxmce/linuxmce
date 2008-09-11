#!/bin/bash

set -e

Debug=0
FromHdd=0
Upgrade=0
KeepMedia=0
LinuxMCE_Password=
grep -q "install_from_hdd" /proc/cmdline && FromHdd="1"

if grep -q "noinst" /proc/cmdline; then
	clear >/dev/tty1
	echo "Installer bypassed" >/dev/tty1
	chvt 2
	exit 0
fi

TargetHdd=
RootUUID=
SwapUUID=
RecoveryUUID=

Error()
{
	local Message="$*"
	echo
	echo -e "\033[1;31mERROR: $*"
	echo
	tput sgr0
	exit 1
}

NukeFS()
{
	local List="$*" # "/,home,var /var,lib BasePath,Exception1,Exception2,..."
	local Item Path
	local BasePath ExceptionList
	local OldIFS
	local -a Exceptions

	for Item in $List; do
		Exceptions=(-false)
		BasePath="${Item%%,*}"
		ExceptionList="${Item#*,}"
		OldIFS="$IFS"
		IFS=","
		for Path in $ExceptionList; do
			Exceptions=("${Exceptions[@]}" -or -name "$Path")
		done
		IFS="$OldIFS"
		find "$BasePath" -mindepth 1 -maxdepth 1 '(' "${Exceptions[@]}" ')' -or -exec rm -rf '{}' ';'
	done
}

GetConsole()
{
	exec &>/dev/tty1 </dev/tty1
	dmesg -n1
	chvt 1
}

GetHddToUse()
{
	local HddList Done Drive i Choice DiskSize DiskDev
	local -a Hdd

	clear
	HddList=$(fdisk -l|grep '^Disk /dev' |cut -d' ' -f2-4|sed -r 's,[:,],,g; s, ,:,g')
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
			Error "Failed to determine current hard drive."
		fi
	elif [[ -z "$HddList" ]]; then # No hard drives
		Error "No hard drives found."
	else
		Done=0
		mkdir -p /media/target
		for Drive in $HddList; do
				DiskDev="${Drive%%:*}"
				set +e
				DiskSize="${Drive#*:}"
				DiskDescription="$(hdparm -I $DiskDev | grep "Model Number:" | cut -d ':' -f2 | sed 's/ *//g')"
				DiskSerial="$(hdparm -I $DiskDev | grep "Serial Number:" | cut -d ':' -f2 | sed 's/ *//g')"
				set -e
				if mount "$DiskDev"1 /media/target; then
					if [[ -f /media/target/etc/pluto.conf ]]; then
						Choice=
						until [[ "$Choice" == [YyNn] ]] ;do
							clear	
							echo "Detected Previous Installation"
							echo "=============================="
							echo
							echo "I've found an old installation on drive :"
							echo "    $DiskDev ($DiskDescription $DiskSerial ${DiskSize//:/ })"
							echo
							echo -n "Do you want to use this drive (Y/N) ? "
							read Choice
						done

						if [[ "$Choice" == [Yy] ]] ;then
							Choice=
							until [[ "$Choice" == [123] ]]; do
								clear
								echo "Previous Installation Options"
								echo "============================="
								echo 
								echo "Choose :"
								echo "    1. Preserve my setting and /home directory."
								echo "    2. Preserve my /home directory only."
								echo "    3. Do a clean install without keeping anything."
								echo 
								echo -n "Answer :  "
								read Choice

								if [[ "$Choice" != [123] ]]; then
									echo "Invalid selection. Try again"
									sleep 1; clear
								fi
							done
							if [[ "$Choice" == "1" ]]; then
								Done=1
								Upgrade=1
								TargetHdd="$DiskDev"
							fi
							if [[ "$Choice" == "2" ]]; then
								Done=1
								Upgrade=1
								KeepMedia=1
								TargetHdd="$DiskDev"
							fi
							if [[ "$Choice" == "3" ]] ;then
								Done=1
								TargetHdd="$DiskDev"
							fi
					
							umount /media/target || :
							break
						fi
					fi
					umount /media/target || :
				fi
		done
		clear


		while [[ "$Done" == 0 ]]; do
		echo "Hard Drive Selection"
		echo "===================="
		echo 
		echo "WARNING :"
		echo "This will delete everything on all partitions of your hard drive"
		echo "the data is not recoverable"
		echo 

		echo "Hard drives in the system:"
			i=1
			for Drive in $HddList; do
				DiskDev="${Drive%%:*}"
				DiskSize="${Drive#*:}"
				set +e
				DiskDescription="$(hdparm -I $DiskDev | grep "Model Number:" | cut -d ':' -f2 | sed 's/ *//g')"
				DiskSerial="$(hdparm -I $DiskDev | grep "Serial Number:" | cut -d ':' -f2 | sed 's/ *//g')"
				set -e
				echo "    $i. $DiskDev ($DiskDescription $DiskSerial ${DiskSize//:/ })"
				Hdd[$i]="$DiskDev"
				((i++))
			done
			((i--))
			echo 
			echo -n "Choose which drive to install the system on: "
			read Choice
			if [[ "$Choice" == *[^0-9]* || "$Choice" -lt 1 || "$Choice" -gt "$i" ]]; then
				echo "Invalid selection. Try again"
				sleep 1; clear
			else
				TargetHdd="${Hdd[$Choice]}"
				Done=1
			fi
		done	
	fi
	
	if [[ "$Upgrade" != "1" ]] || [[ "$Upgrade" == "1" && "$KeepMedia" == "1" ]] ;then
		clear
		echo "System User Password"
		echo "===================="
		echo
		echo "A system user named 'linuxmce' will be created by default."
		echo "Please type a password for this user."
		echo 
		while [[ "$LinuxMCE_Password" == "" ]] ;do
			echo -n 'Password : '
			read Password
			if [[ "$Password" != "" ]] ;then
				LinuxMCE_Password=$(echo 'printf "%s\n", crypt("'$Password'", "\$1\$6-8-letter-salt\$")' | perl)
#				LinuxMCE_Password=$(mkpasswd -H md5 "$Password")
			fi
		done
	fi

	clear
	echo "Installing System"
	echo "================="
	echo 
	echo "Started installing on $TargetHdd"
	if [[ -z "$TargetHdd" ]]; then
		echo "Got out of loop with empty selection. This shouldn't be possible."
		exit 1
	fi
}

PartitionHdd()
{
	if [[ "$FromHdd" == 1 ]]; then
		return
	fi

	swapoff -a
	if [[ "$Upgrade" == 1 ]]; then
		return
	fi

set +e
	parted -s "$TargetHdd" -- mklabel msdos
	parted -s "$TargetHdd" -- mkpart primary ext2 0 -12GB # root filesystem
	parted -s "$TargetHdd" -- set 1 boot on
	parted -s "$TargetHdd" -- mkpart extended -12GB -1s
	parted -s "$TargetHdd" -- mkpart logical linux-swap -12GB -10GB # swap
	parted -s "$TargetHdd" -- mkpart logical ext2 -10GB -1s # recovery system
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
	mkdir -p /media/target
	if [[ "$FromHdd" == 1 || "$Upgrade" == 1 ]] && mount "$TargetHdd"1 /media/target; then
		pushd /media/target &>/dev/null

		if [[ "$KeepMedia" == "1" ]] ;then
			NukeFS .,home
		else
			if [[ -d ./usr/pluto/diskless ]]; then
				pushd ./usr/pluto/diskless &>/dev/null
					while read MD; do
						NukeFS "./$MD",etc "./$MD/etc",pluto.conf
					done < <(find . -mindepth 1 -maxdepth 1 -type d)
				popd &>/dev/null
	
				mkdir .upgrade-diskless
				mv ./usr/pluto/diskless/* .upgrade-diskless/
			fi
	
			NukeFS .,home,var,etc,usr,.upgrade-diskless ./var,lib ./var/lib,mysql ./etc,pluto.conf,ssh,passwd,shadow,group,mysql ./usr,pluto ./usr/pluto,orbiter ./usr/pluto/orbiter,rooms,scenarios,users
			mkdir .upgrade-save
		fi
			
		find -mindepth 1 -maxdepth 1 -not -name '.upgrade-*' -exec mv '{}' .upgrade-save/ ';'
		popd &>/dev/null
		umount "$TargetHdd"1
	else
		echo y|mkfs.ext3 "$TargetHdd"1 # root filesystem
	fi
	if [[ "$FromHdd" != 1 && "$Upgrade" != 1 ]]; then
		mkswap "$TargetHdd"5 # swap
		echo y|mkfs.ext3 "$TargetHdd"6 # recovery system
	fi

	blkid -w /etc/blkid.tab || :
	RootUUID=$(vol_id --skip-raid -u "$TargetHdd"1 2>/dev/null)
	SwapUUID=$(vol_id --skip-raid -u "$TargetHdd"5 2>/dev/null)
	RecoveryUUID=$(vol_id --skip-raid -u "$TargetHdd"6 2>/dev/null || :)
}

MountPartitions()
{
	mkdir -p /media/target /media/recovery
	mount -t ext3 "$TargetHdd"1 /media/target

	if [[ "$Upgrade" != "1" ]] ;then
		if [[ "$FromHdd" != 1 ]]; then
			mount -t ext3 "$TargetHdd"6 /media/recovery || :
		else
			mount -o bind / /media/recovery
		fi
	fi
}

CopyDVD()
{
	if [[ "$FromHdd" == "1" || "$Upgrade" == "1" ]] ;then
		return
	fi
	echo "Copying DVD to hard drive"
	local DVDdir=$(mktemp -d)
	mount -t squashfs -o loop,ro /cdrom/casper/filesystem.squashfs "$DVDdir"
	cp -a "$DVDdir"/. /media/recovery/ || Error "I/O failure while copying from the DVD."
	umount "$DVDdir"

	#Copy archives
	mkdir -p /media/recovery/archives/lmce-image/
	cp -a /cdrom/lmce-image/. /media/recovery/archives/lmce-image/ || Error "I/O failure while copying from the DVD."

	#Copy demo videos, if any
	if [[ -d /cdrom/lmce-videos ]]; then
		mkdir -p /media/recovery/archives/lmce-videos
		cp -a /cdrom/lmce-videos/. /media/recovery/archives/lmce-videos/ || Error "I/O failure while copying from the DVD."
	fi

	#Copy VIA archives
	if [[ -d /cdrom/via-archives ]]; then
		mkdir -p /media/recovery/archives/via-archives
		cp -a /cdrom/via-archives/. /media/recovery/archives/via-archives/ || Error "I/O failure while copying from the DVD."

	fi
	
	#Copy Diskless images
	if [[ -d /cdrom/diskless-images ]]; then
		mkdir -p /media/recovery/archives/diskless-images
		cp -a /cdrom/diskless-images/. /media/recovery/archives/diskless-images/ || Error "I/O failure while copying from the DVD."

	fi

	if [[ -d /cdrom/deb-cache ]] ;then
		mkdir -p /media/recovery/archives/deb-cache
		cp -a /cdrom/deb-cache/. /media/recovery/archives/deb-cache/ || Error "I/O failure while copying from the DVD."

	fi

	local interfaces="
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp

auto eth1
iface eth1 inet dhcp
"
	echo "$interfaces" >/media/recovery/etc/network/interfaces
}

ExtractArchive()
{
	archives_path="/media/recovery/archives"
	if [[ "$Upgrade" == "1" ]] ;then
		archives_path="/cdrom"
	fi

	echo "Extracting archive (this will take about 10 minutes)"
	cat "$archives_path"/lmce-image/linux-mce.tar.gz* | tar -C /media/target -zx --checkpoint=10000
	mkdir -p /media/target/etc/pluto

	if [[ "$Upgrade" == 1 && "$KeepMedia" != "1" ]]; then
		touch /media/target/etc/pluto/install_cleandb
	fi

	# Update the UUIDs
	rm /media/target/etc/blkid.tab || :
	blkid -w /media/target/etc/blkid.tab

	#Copy the fist run script
	cp "$archives_path"/lmce-image/firstboot /media/target/etc/rc2.d/S90firstboot

	#Copy demo videos, if any
	if [[ -d "$archives_path"/lmce-videos ]]; then
		mkdir -p /media/target/home/public/data/videos/
		cp -a "$archives_path"/lmce-videos/. /media/target/home/public/data/videos/
	fi

	#Copy VIA archives
	if [[ -d "$archives_path/via-archives" ]]; then
		mkdir -p /media/target/usr/pluto/install/via/
		cp -a "$archives_path"/via-archives/. /media/target/usr/pluto/install/via/
	fi
	
	#Copy Diskless images
	if [[ -d "$archives_path"/diskless-images ]]; then
		mkdir -p /media/target/usr/pluto/install
		cp -a "$archives_path"/diskless-images/. /media/target/usr/pluto/install/
	fi

	#Copy deb-cache
	if [[ -d "$archives_path"/deb-cache ]] ;then
		mkdir -p /media/target/usr/pluto/deb-cache
		cp -a "$archives_path"/deb-cache/. /media/target/usr/pluto/deb-cache/
	fi
}

NetworkSetup()
{
	clear
	# detect cards
	NCards=$(ip addr | grep -cF 'link/ether')
	if [[ "$NCards" -eq 1 ]]; then
		ExtIf="eth0"
		IntIf="eth0:0"
	else
		chown root.dhcp /lib/dhcp3-client/call-dhclient-script
		chmod 4754 /lib/dhcp3-client/call-dhclient-script

		/etc/init.d/networking restart &>/dev/null
		#echo "DEBUG: Press CTRL+D after finish debuging to continue..."; bash

		if host -W 1 www.google.com && ping -qc1 -I eth0 www.google.com &>/dev/null; then
			ExtIf="eth0"
			IntIf="eth1"
		else
			ExtIf="eth1"
			IntIf="eth0"
		fi
	fi
	

	echo "
auto lo
iface lo inet loopback

auto $ExtIf
iface $ExtIf inet dhcp

auto $IntIf
iface $IntIf inet static
	address 192.168.80.1
	netmask 255.255.255.0
" >/media/target/etc/network/interfaces
}

SetupFstab()
{
	mkdir -p /media/target/mnt/recovery

	local fstab_text="
# /etc/fstab: static file system information.
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
proc            /proc           proc    defaults        0       0
UUID=$RootUUID	/               ext3    defaults,errors=remount-ro 0       1
UUID=$SwapUUID  none            swap    sw              0       0
/dev/cdrom      /media/cdrom0   udf,iso9660 user,noauto     0       0
"
	if  [[ "$RecoveryUUID" != "" ]] ;then
		fstab_text="$fstab_text
UUID=$RecoveryUUID /mnt/recovery   ext3    ro              0       0
"
	fi
	echo "$fstab_text" > /media/target/etc/fstab
}

InstallGrub()
{
	mkdir -p /media/target/proc
	mkdir -p /media/target/dev
	mkdir -p /media/target/sys
	mount --bind /dev/ /media/target/dev/
	mount -t proc -o bind /proc /media/target/proc
	grub-install --recheck --root-directory=/media/target "$TargetHdd"
	umount /media/target/dev/
	cp -r /dev/.static/dev/* /media/target/dev/

	sed -ir "s,^\(# kopt=root=[^ ]*\) \(.*\),# kopt=root=UUID=${RootUUID} \2,g" /media/target/boot/grub/menu.lst
	sed -ir "s,root=.* ro quiet splash,root=UUID=${RootUUID} ro quiet splash,g" /media/target/boot/grub/menu.lst
	sed -ir "s,root=.* ro single,root=UUID=${RootUUID} ro single,g" /media/target/boot/grub/menu.lst

	if [[ "$RecoveryUUID" != "" ]] ;then
		echo "
		title		System Recovery
		root		(hd0,5)
		kernel		/boot/vmlinuz-2.6.20-15-generic root=UUID=${RecoveryUUID} quiet install_from_hdd
		initrd		/boot/initrd.img-2.6.20-15-generic
		" >> /media/target/boot/grub/menu.lst
	fi
}

TargetCleanup()
{
	local NR ifnr ifname link ifmac rest line

	NR=0
	echo "# This file assigns persistent names to network interfaces.
# See iftab(5) for syntax.
" >/media/target/etc/iftab
	rm -f /media/target/etc/udev/rules.d/70-persistent-net.rules

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
	done < <(/sbin/ip l) >>/media/target/etc/iftab

	chroot /media/target update-initramfs -u

	ifconfig -a | grep ^eth | awk '{print "SUBSYSTEM==\"net\", DRIVERS==\"?*\", ATTRS{address}==\"" tolower($5) "\", NAME=\"" $1 "\""}' > /media/target/etc/udev/rules.d/70-persistent-net.rules || :
	#chroot /media/target update-grub

	if [[ "$LinuxMCE_Password" != "" ]] ;then
		sed -i "s,\(linuxmce:\)[^:]*\(.*\),\1$LinuxMCE_Password\2,g" /media/target/etc/shadow
	fi

	if [[ -d /media/target/.upgrade-save ]]; then
		pushd /media/target/.upgrade-save &>/dev/null
		while read filepath; do
			filepath="${filepath#+}"
			filepath="${filepath%+}"

			directory=$(dirname "$filepath")
			dir_owner=$(stat -c '%u:%g' "$directory")
			dir_rights=$(stat -c '%a' "$directory")
			directory="../$directory"

			mkdir -p "$directory"
			mv "$filepath" "$directory"

			# preserve directory access rights and ownership; files don't need this since they're moved, not created
			chmod "$dir_rights" "$directory"
			chown "$dir_owner" "$directory"
		done < <(find -not -type d -printf "+%p+\n")
		popd &>/dev/null
		rm -rf /media/target/.upgrade-save
		sed -ir 's/^PK_Distro.*$/PK_Distro=15/g' /media/target/etc/pluto.conf
		chroot /media/target chown -R mysql.mysql /var/lib/mysql
	fi
}

UnmountPartitions()
{
set +e
	umount /media/target
	if [[ "$Upgrade" != "1" ]] ;then
		umount /media/recovery
	fi
set -e
}

Reboot()
{
	clear
	exec &>/dev/tty8 </dev/tty8
	clear
	chvt 8
	echo "**********************"
	echo "Installation complete."
	echo "**********************"
	reboot
}

GetConsole
GetHddToUse
PartitionHdd
FormatPartitions
MountPartitions
CopyDVD
ExtractArchive
SetupFstab
InstallGrub
NetworkSetup
TargetCleanup
UnmountPartitions
Reboot
