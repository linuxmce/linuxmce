#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
LogFile="/var/log/pluto/CheckForRaids.log"
Log "$LogFile" "$(date -R) Starting"

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

function arrayNotExists {
	local array="$1"
	local listOfArrays="$2"
	for arr in $listOfArrays ;do
		arr="$arr "
		if [[ "$arr" == "$array" ]] ;then
			return 1
		fi
	done
	
	return 0
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
echo ">>>>$availPart<<<<<"

raid_table=
echo "" > /tmp/raidarrays
for device in $availPart; do
	result=$(mdadm --examine --brief --scan /dev/$device | cut -d' ' -f2) 
	if [[ -n $result ]] && mdadm --detail --brief "$result" >/dev/null; then 
		if arrayNotExists "$raid_table" "$result" ;then
			raid_table="$raid_table""$result "
		fi
		echo "$result|$device" >> /tmp/raidarrays
	fi
done
for el in $raid_table ;do
	devs=$(cat /tmp/raidarrays|grep "$el"|cut -d'|' -f2)
	no_disks=$(cat /tmp/raidarrays|grep "$el"|wc -l)
	one_dev=$(echo "$devs" | tail -1)

	Q="
	SELECT
		PK_Device 
	FROM 
		Device
		JOIN Device_DeviceData ON FK_Device = PK_Device
	WHERE
		FK_DeviceTemplate IN (1849, 1851, 1854)
		AND
		FK_DeviceData = 152
		AND
		IK_DeviceData = '$el'
	"
	if [[ "$(RunSQL "$Q")" != "" ]] ;then
		continue
	fi

	raid_level=$(mdadm --examine --brief --scan /dev/$one_dev | cut -d' ' -f3 | cut -d'=' -f2)
	device=
	case "$raid_level" in
		"raid5")
			device=$(/usr/pluto/bin/MessageSend "$DCERouter" -targetType template -o 0 27 1 718 44 1849 156 1|tail -1|cut -d':' -f2)
			Log "$LogFile" "Recreating Raid 5 device $device $el"
			;;
		"raid1")
			device=$(/usr/pluto/bin/MessageSend "$DCERouter" -targetType template -o 0 27 1 718 44 1851 156 1|tail -1|cut -d':' -f2)
			Log "$LogFile" "Recreating Raid 1 device $device $el"
			;;
		"raid0")
			device=$(/usr/pluto/bin/MessageSend "$DCERouter" -targetType template -o 0 27 1 718 44 1854 156 1|tail -1|cut -d':' -f2)
			Log "$LogFile" "Recreating Raid 0 device $device $el"
			;;
	esac
	
	raidSize=$(mdadm --query $el | head -1 |cut -d' ' -f2) 
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='$el' WHERE FK_Device=$device and FK_DeviceData=152"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='$raidSize' WHERE FK_Device=$device and FK_DeviceData=201"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='$no_disks' WHERE FK_Device=$device and FK_DeviceData=199"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='3' WHERE FK_Device=$device AND FK_DeviceData=204"

	for dev in $devs ;do
		disk_device=$(/usr/pluto/bin/MessageSend "$DCERouter" -targetType template -o 0 27 1 718 44 1850 156 $device|tail -1|cut -d':' -f2)
		Log "$LogFile" "Added drive $dev device $device $el"
		RunSQL "UPDATE Device SET Description='/dev/$dev' WHERE PK_Device=$disk_device"
		RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='/dev/$dev' WHERE FK_Device=$disk_device AND FK_DeviceData=152"

		size=$(fdisk -l /dev/$dev | grep Disk | cut -d' ' -f3,4)
                length=${#size}
                if [[ -n $size ]]; then
	                size=${size:0:$(($length-1))}
			Q="UPDATE Device_DeviceData SET IK_DeviceData='$size GB' WHERE FK_Device=$disk_device and FK_DeviceData=201"
			RunSQL "$Q"
		fi	
	done

	## Triger a refresh of status
	/usr/pluto/bin/monitoring_RAID.sh 'REFRESH WEB' $el
done

