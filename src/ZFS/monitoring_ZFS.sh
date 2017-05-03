#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Param_Event=$1
Param_Raid=$2
Param_Disk=$3
DD_BLOCK_DEVICE=152
NEW_ADD_ID=204
STATE_ID=200
DISK_SIZE_ID=201
SPARE_ID=202
HARD_DRIVE_DEVICE_TEMPLATE=1850
ZFS_POOL_DEVICE_TEMPLATE=2351
ZFS_MIRROR_DEVICE_TEMPLATE=2352
ZFS_RAIDZ1_DEVICE_TEMPLATE=2353
ZFS_RAIDZ2_DEVICE_TEMPLATE=2354
ZFS_RAIDZ3_DEVICE_TEMPLATE=2355

date=$(date)

if [[ -z $Param_Disk ]]; then
	echo "($date) Catched event $Param_Event for $Param_Raid" >> /var/log/pluto/ZFS_monitoring.log
else	
	echo "($date) Catched event $Param_Event for $Param_Raid from $Param_Disk" >> /var/log/pluto/ZFS_monitoring.log
fi


function Raid_SetStatus {
	local DeviceID="$1"
	local StatusMessage="$2"

	local Q="UPDATE Device_DeviceData SET IK_DeviceData= '$StatusMessage' WHERE FK_Device='$DeviceID' AND FK_DeviceData='$STATE_ID'"
	RunSQL "$Q"
}

function Raid_SetSize {
	local DeviceID="$1"
	local Size="$2"

	local Q="UPDATE Device_DeviceData SET IK_DeviceData = '$Size' WHERE FK_Device='$DeviceID' AND FK_DeviceData='$DISK_SIZE_ID'"
	RunSQL "$Q"
}

Raid_DeviceID="$(RunSQL "SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE")"
if [[ "$Raid_DeviceID" == "" ]] ;then
	exit 0
fi

RAID_STATUS=$(zpool status $Param_Raid | grep "state" | cut -d ' ' -f3)

case "$RAID_STATUS" in
    "ONLINE")   Raid_SetStatus "$Raid_DeviceID" 'OK'      ;;
    "DEGRADED") Raid_SetStatus "$Raid_DeviceID" 'DAMAGED' ;;
    "FAULTED")  Raid_SetStatus "$Raid_DeviceID" 'FAILED'  ;;
    "OFFLINE")  Raid_SetStatus "$Raid_DeviceID" 'FAILED'  ;;
    "UNAVAIL")  Raid_SetStatus "$Raid_DeviceID" 'FAILED'  ;;
    "REMOVED")  Raid_SetStatus "$Raid_DeviceID" 'FAILED'  ;;
esac

RAID_SIZE=$(zpool get -H size $Param_Raid | cut -f3)

Raid_SetSize "$Raid_DeviceID" "$RAID_SIZE"

Q="
	SELECT 
		PK_Device,
		IK_DeviceData
	FROM
		Device
		INNER JOIN Device_DeviceData on FK_Device = PK_Device
	WHERE
		FK_Device_ControlledVia = $Raid_DeviceID
		AND
		FK_DeviceData = $DD_BLOCK_DEVICE
"

R=$(RunSQL "$Q")
Disk=""

for Disk in $R; do
    Disk_DeviceID=$(Field "1" "$Disk")
    Disk_Block=$(Field "2" "$Disk")
    Disk_Block=${Disk_Block#/dev/} # glob remove /dev/ from string
    Disk_Type=""
    disk_info_line=$(zpool status "$Param_Raid" | grep "$Disk_Block")
    
    if [[ "$disk_info_line" != "" ]]; 
    then
	Disk_Type=$(echo $disk_info_line | cut -d ' ' -f2)
    else
	Disk_Type="UNAVAIL"
    fi
    
    case "$Disk_Type" in
	"ONLINE")   Raid_SetStatus "$Disk_DeviceID" 'OK'      ;;
	"DEGRADED") Raid_SetStatus "$Disk_DeviceID" 'DAMAGED' ;;
	"FAULTED")  Raid_SetStatus "$Disk_DeviceID" 'FAILED'  ;;
	"OFFLINE")  Raid_SetStatus "$Disk_DeviceID" 'FAILED'  ;;
	"UNAVAIL")  Raid_SetStatus "$Disk_DeviceID" 'FAILED'  ;;
	"REMOVED")  Raid_SetStatus "$Disk_DeviceID" 'FAILED'  ;;
    esac
    
done
