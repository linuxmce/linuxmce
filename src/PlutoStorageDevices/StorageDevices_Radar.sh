#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

export PATH=$PATH:/sbin:/usr/sbin:/usr/local/sbin

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_UUID=267

## Function to remove undesired paths
function substractPaths {
	local availPath=$1
	local unavailPath=$2
	local retPath=""

	for aPath in $availPath; do
		available=1

		for uPath in $unavailPath; do
			if [[ "$aPath" == "$uPath" ]]; then
				available=0
			fi
		done

		if [[ $available == 1 ]]; then
			retPath="$retPath $aPath"
		fi
	done

	echo $retPath
}

function Detect {
	## Available Paths
	availPath=$(ls /dev/disk/by-path)

	## Removes drive paths that aren't storage drives
	Hdds=""
	for Drive in $availPath; do
		drivePathDevName=$(udevadm info --query=all --name="/dev/disk/by-path/$Drive" | grep 'N:' | awk '{ print $2 }') 
		confirmDrive=$(echo $drivePathDevName | grep -E 'sd|hd|xd|md' | awk '{ print $1 }')
		if [[ "$confirmDrive" == "" ]]; then continue; fi

		Hdds="$Hdds $Drive"
	done
	availPath=$Hdds

	## Looks for standard device names mounted in mtab and learns path. Note some mounted drives show only UUID
	mountedDevName=$(cat /etc/mtab | awk '/dev\/(sd|hd|md|xd)./ {print $1}')
	mountedDevPath=""
	for Drive in $mountedDevName; do
		mountedDevPath="$mountedDevPath $(udevadm info --query=all --name="$Drive" | grep 'S: disk/by-path/' | sed 's,disk/by-path/,,g' | awk '{ print $2 }')"
	done
	
	## Grabs UUID of mounted devices in mtab and learns path. Note some mounted drives only show device names.
	mountedUuid=$(cat /etc/mtab | awk '/uuid/ { print $1 }' | sed 's,/dev/disk/by-uuid/,,g' | sort -u)
	mountedPath=""
	for Drive in $mountedUuid; do
		mountedPath="$mountedPath $(udevadm info --query=all --name="/dev/disk/by-uuid/$Drive" | grep 'S: disk/by-path/' | sed 's,disk/by-path/,,g' | awk '{ print $2 }')"
	done

	## Remove the mounted paths
	availPath=$(substractPaths "$availPath" "$mountedPath")
	availPath=$(substractPaths "$availPath" "$mountedDevPath")

	## Remove partitions if they're alias is mounted
	for Path in $availPath; do
		for path_alias in $(udevadm info --query=symlink --name="/dev/disk/by-path/$Path" | awk '{ print $4 }' | sed 's,disk/by-uuid/,,g'); do
			for alias_mounted in $(mount | awk '/dev\/disk\// {print $1}' | sed 's/\/dev\///g'); do
				if [[ "$path_alias" == "$alias_mounted" ]]; then
					availPath=$(substractPaths "$availPath" "$Path")
				fi
			done
		done
	done

	## Remove unmountable path additions (swap/extended/boot sector)
	auxPath=""
	for Path in $availPath; do
		## If is extended partition
		if file -sL /dev/disk/by-path/$Path | grep -q "extended partition table" ; then
			continue
		fi

		## If is swap partition
		if file -sL /dev/disk/by-path/$Path | grep -q "swap file" ; then
			continue
		fi
		## If is boot sector
		if file -sL /dev/disk/by-path/$Path | grep -q "boot sector;" ; then
			continue
		fi

		auxPath="$auxPath $Path"
	done
	availPath=$auxPath

	## Remove paths that belong to a mounted RAID
	if [[ -x /sbin/mdadm ]]; then
		auxPath=""
		for Path in $availPath; do
			if [[ "$(mdadm --examine /dev/disk/by-path/$Path 2>&1)" == *"No md superblock"* ]]; then
				auxPath="$auxPath $Path"
			fi
		done
		availPath="$auxPath"
	fi

	## Test to see if we found any available paths
	if [[ "$availPath" != "" ]]; then
		
		for pathPosition in $availPath; do
			## I assume someone left this non working MessageSend line in for reference, and have done the same.
			#/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 228 109 "[$pathPosition]" 156 $PK_Device 163 "$InfoMessage"

			## Get info about this volume
			partition_diskname=$(udevadm info --query=all --name="/dev/disk/by-path/$pathPosition" | grep 'ID_MODEL=' | cut -d'=' -f2)
			partition_serial=$(udevadm info --query=all --name="/dev/disk/by-path/$pathPosition" | grep 'ID_SERIAL_SHORT=' | cut -d'=' -f2)
			partition_label=$(udevadm info --query=all --name="/dev/disk/by-path/$pathPosition" | grep 'ID_FS_LABEL=' | cut -d'=' -f2)
			partition_uuid=$(udevadm info --query=all --name="/dev/disk/by-path/$pathPosition" | grep 'ID_FS_UUID=' | cut -d'=' -f2)
			partition=$(udevadm info --query=all --name="/dev/disk/by-path/$pathPosition" | grep 'N:' | awk '{ print $2 }')
                        
			## fallback to serial number if no UUID available (for instance FAT devices)
			if [[ "$partition_uuid" == "" && "$partition_serial" != "" ]]; then
			        partition_uuid="$partition_serial"
			fi

			## fallback to volume label if no UUID or serial number available
			if [[ "$partition_uuid" == "" && "$partition_label" != "" ]]; then
			        partition_uuid="$partition_label"
			fi

			if [[ "$partition_uuid" == "" ]]; then
				continue
			fi

			## Mount the partition to get it's size
			mountpoint_temp=$(mktemp -d)
			mount -o ro "/dev/$partition" "$mountpoint_temp"
			partition_size=$(df -h "/dev/$partition" | tail -n +2 | awk '{ print $2 }')
			umount -lf "$mountpoint_temp";  rmdir "$mountpoint_temp"

			## Sends data to MessageSend
			thisHost=$(hostname)
			Sent="false"
			Count=0
			while [[ "$Sent" == "false" ]]; do
				/usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 55 "$DD_UUID|$partition_uuid|277|$partition_size" 54 "$partition_uuid" 52 8 49 1790 13 "$partition_size $partition_diskname [$partition] on $thisHost"
				err=$?

				if [[ "$err" == "0" ]]; then
					Sent="true"
				else
					Count=$(( Count + 1 ))
					sleep 2
				fi

				if [[ "$Count" == "10" ]]; then
					Sent="true"
				fi
			done
		done
	fi
}

Detect
