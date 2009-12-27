#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

export PATH=$PATH:/sbin:/usr/sbin:/usr/local/sbin

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_UUID=267

function substractParts {
	local availPart=$1
	local unavailPart=$2
	local retPart=""

	for aPart in $availPart ;do
		available=1

		for uPart in $unavailPart ;do
			if [[ "$aPart" == "$uPart" ]]; then
				available=0
			fi
		done

		if [[ $available == 1 ]]; then
			retPart="$retPart $aPart"
		fi
	done

	echo $retPart
}

function Detect {
	## Available partitions
	availPart=$(cat /proc/partitions | awk '/(sd|hd).[0-9]/ { print $4 }')

	## Remove Mounted partitions
	mountedPart=$(mount | awk '/dev\/(sd|hd).[0-9]/ {print $1}' | sed 's/\/dev\///g')
	availPart=$(substractParts "$availPart" "$mountedPart")
	
	## Remove partitions if they're alias is mounted
	for part in $availPart ;do
		for part_alias in $(udevinfo --query=symlink --name="/dev/$part") ;do
			for alias_mounted in $(mount | awk '/dev\/disk\// {print $1}' | sed 's/\/dev\///g') ;do
				if [[ "$part_alias" == "$alias_mounted" ]] ;then
					availPart=$(substractParts "$availPart" "$part")
				fi
			done
		done
	done

	## Remove Unmountable partitions (swap / extended)
	auxPart=""
	for part in $availPart ;do
		## If is a boot partition
		if file -s /dev/$part | grep -q "x86 boot sector" ;then
			continue
		fi

		## If is extended partition
		if file -s /dev/$part | grep -q "extended partition table"  ;then
			continue
		fi

		## If is swap partition
		if file -s /dev/$part | grep -q " Linux/i386 swap file"  ;then
                        if ! grep -q "^/dev/$part" /proc/swaps ;then
				echo "Enabling swap partition /dev/$part"
				/sbin/swapon /dev/$part
			fi

			continue
		fi

		auxPart="$auxPart $part"
	done
	availPart=$auxPart

	## Remove partitions that belong to a raid
	if [[ -x /sbin/mdadm ]] ;then
		auxPart=""
		for part in $availPart ;do
			if [[ "$(mdadm --examine /dev/$part 2>&1)" == *"No md superblock"* ]] ;then
				auxPart="$auxPart $part"
			fi
		done
		availPart="$auxPart"
	fi
	

	## Test if we found any available partitions
	if [[ "$availPart" != "" ]] ;then
		
		for partition in $availPart ;do
			#/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 228 109 "/dev/$partition" 156 $PK_Device 163 "$InfoMessage"

			## Get info about this partition
			partition_diskname=$(udevinfo --query=all --name="/dev/$partition" | grep 'ID_MODEL' | cut -d'=' -f2)
			partition_haludi=$(hal-find-by-property --key 'block.device' --string "/dev/$partition")
			partition_uuid=$(hal-get-property --udi "$partition_haludi" --key 'volume.uuid')

			if [[ "$partition_uuid" == "" ]] ;then
				continue
			fi

			## Mount the partition to get it's size
			mountpoint_temp=$(mktemp -d)
			mount -o ro "/dev/$partition" "$mountpoint_temp"
			partition_size=$(df -h "/dev/$partition" | tail -n +2 | awk '{ print $2 }')
			umount -lf "$mountpoint_temp" && rmdir "$mountpoint_temp"
			
			thisHost=$(hostname)
			Sent="false"
			Count=0
			while [[ "$Sent" == "false" ]] ;do
				/usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 55 "$DD_UUID|$partition_uuid|277|$partition_size" 54 "$partition_uuid" 52 8 49 1790 13 "$partition_diskname [$partition] on $thisHost"
				err=$?

				if [[ "$err" == "0" ]] ;then
					Sent="true"
				else
					Count=$(( Count + 1 ))
					sleep 2
				fi


				if [[ "$Count" == "10" ]] ;then
					Sent="true"
				fi
			done
		done
	fi
}

Detect
