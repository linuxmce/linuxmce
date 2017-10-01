#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

function isInList 
{
	local disk="$1"
	for dev in $availPart; do
		if [[ ${#dev} > '3' ]]; then	
			len=$((${#dev}-1))
		  	dev=${dev:0:$len}

			if [[ "$disk" == "$dev" ]]; then 
				echo '0'
			fi
		fi
 	done
	echo '1'
}

function substractParts {
    local availPart=$1
    local unavailPart=$2
    local retPart=""

    for aPart in $availPart ;do
        available=1
        for uPart in $unavailPart ;do
	    if [[ ${#aPart} == '3' ]]; then 
		len=$((${#uPart}-1))
                uPart=${uPart:0:$len}	
	    fi
			
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
														    

## Available partitions
availPart=$(cat /proc/partitions | awk '/(sd|hd)/ { print $4 }')

## Remove Mounted partitions
mountedPart=$(mount | awk '/dev\/(sd|hd)/ {print $1}' | sed 's/\/dev\///g')
availPart=$(substractParts "$availPart" "$mountedPart")

## Remove partitions if they're alias is mounted
mounted_aliases=$(mount | awk '/dev\/disk\// {print $1}' | sed 's/\/dev\///g')
for part in $availPart ;do
	for part_alias in $(udevadm info --query=symlink --name="/dev/$part") ;do
		for alias_mounted in $mounted_aliases ;do
			if [[ "$part_alias" == "$alias_mounted" ]] ;then
				availPart=$(substractParts "$availPart" "$part")
			fi
		done
	done
done


## Remove Internal Drives that are in the database
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData = 152")
for Drive in $R ;do
	block_dev=$(Field "1" "$Drive")
	block_dev=$(basename $block_dev)
	availPart=$(substractParts "$availPart" "$block_dev")
done


## Remove Unmountable partitions (swap / extended)
auxPart=""
for part in $availPart ;do
     ## If is extended partition
     if file -s /dev/$part | grep -q "extended partition table$"  ;then
        continue
     fi

    ## If is swap partition
    if file -s /dev/$part | grep -q " Linux/i386 swap file"  ;then
    	continue
    fi
    auxPart="$auxPart $part"
done
availPart=$auxPart

for device in $availPart; do
	if [[ ${#device} > '3' ]]; then
		new_list="$new_list""$device "
	else
		#len=$((${#device}-1))
		#device=${device:0:$len}
		if [[ "$(isInList "$device")" == '1' ]]; then
			new_list="$new_list""$device "
		fi
	fi
done
availPart=$new_list

for device in $availPart; do
	raid_block=$(mdadm --examine --brief --scan /dev/$device | cut -d' ' -f2)
	# If it belongs to a raid device
	if [[ "$raid_block" != "" ]] ;then
		# And the raid device is active
		if mdadm --detail --brief $raid_block &>/dev/null ;then
			continue
		fi
	fi

	result=$(mdadm --query /dev/$device | grep 'is not an md array')
	if [[ -n $result ]]; then
		size=$(fdisk -l /dev/$device | grep Disk | cut -d' ' -f3,4)
		length=${#size}
		 if [[ -n $size ]]; then
			size=${size:0:$(($length-1))}
			free_disks="$free_disks/dev/$device,$size;"
		 fi
	fi
done

echo $free_disks
