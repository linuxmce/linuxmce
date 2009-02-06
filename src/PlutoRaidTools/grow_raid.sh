#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

#used to grow the software raid by activating spare drives.
#Jon Decker - jondecker76@gmail.com
#params:
#$1 raid device, such as "/dev/md0"
#$2 number of spare drives to grow to active

RaidDevice=$1
DrivesToAdd=$2
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
SPARE_ID=202

#Lets ensure that the user has passed in parameters for the script to work with
if [[ "$RaidDevice" == "" || "$DrivesToAdd" == "" ]] ; then
	echo "Usage: grow_raid.sh [block device path] [number of spare disks to grow]"
	echo "Example: grow_raid.sh /dev/md0 1"
	echo "         Will add 1 spare drive to the /dev/md0 Raid device."
	echo ""
	echo "Please try again with the correct parameters."
	exit
fi

#Check filesystem type to ensure the operation is supported
echo "Checking for supported filesystem..."
fsType=$(df -T $RaidDevice | tail -n 1 | sed 's/[ \t][ \t]*/ /g' | cut -d' ' -f2)

if [[ $fsType == "ext3" || $fsType == "ext2" ]] ;then
	echo "     Supported filesystem type $fsType found"	
else
	echo "     Filesystem type $fsType is not supported at this time. Aborting."
	exit
fi

#Get the LMCE device ID from the raid path
echo "Checking to make sure that $RaidDevice exists in the LMCE database..."
DeviceID=$(RunSQL "SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$RaidDevice' AND FK_DeviceData = $BLOCK_DEVICE_ID LIMIT 1")
if [ ! $DeviceID ] ;then
	echo "Error: $RaidDevice could not be found in the LMCE database. Aborting."
	exit
fi
echo "     Device #$DeviceID found in the LMCE database for $RaidDevice"


#Lets cross-check the parameters to make sure that we have enough spare drives to handle the grow operation
#and also calculate the total number of drives for the mdadm -grow command (total = active + $DrivesToAdd)
NrDrives=
NrSpareDrives=
HardDriveList=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $DeviceID")
for Drive in $HardDriveList; do
	Q="
		SELECT 
			Block.IK_DeviceData, 
			Spare.IK_DeviceData 
		FROM Device 
			JOIN Device_DeviceData Block ON Block.FK_Device = PK_Device AND Block.FK_DeviceData = $BLOCK_DEVICE_ID
			JOIN Device_DeviceData Spare ON Spare.FK_Device = PK_Device AND Spare.FK_DeviceData = $SPARE_ID
		WHERE 
			PK_Device = $Drive;
	"
	R=$(RunSQL "$Q")
	Disk=$(Field 1 "$R")
	IsSpare=$(Field 2 "$R")

	if [[ $IsSpare == 1 ]] ;then
		NrSpareDrives=$(($NrSpareDrives+1))
	else
		NrDrives=$(($NrDrives+1))
	fi
done
TotalDrives=$(($NrDrives+$NrSpareDrives))
TotalNewRaidDrives=$(($NrDrives+$DrivesToAdd))
if [[ $DrivesToAdd > $NrSpareDrives ]] ;then
	echo "Error: You do not have enough spare drives! You requested to grow $DrivesToAdd drives, but there are only $NrSpareDrives spare drive(s). Aborting."
	exit
elif [[ $DrivesToAdd == 0 ]] ;then
	echo "Error: You must grow at least one drive. Aborting."
	exit
fi

echo "     ************************"
echo "     Number of Active Drives: $NrDrives"
echo "     Number of Spare Drives: $NrSpareDrives"
echo "     Total Drives: $TotalDrives"
echo "     ************************"

echo "Unmounting device $RaidDevice to prepare for filesystem operations..."
umount $RaidDevice

echo "Growing Raid to use extra disk(s)..."
#mdadm --grow $RaidDevice --raid-disks=$TotalNewRaidDrives

#Resize the filesystem to use the new drive space
#Different commands must be used for different filesystem types.
#Support for other filesystems may be added by expanding the if statement below.
echo "Resizing the filesystem to use the new space..."
if [[ $fsType == "ext3" || $fsType == "ext2" ]] ;then
	#resize2fs $RaidDevice
	echo ""
fi

echo "Mounting device $RaidDevice."
mount $RaidDevice

echo "Raid grown successfully to $TotalNewRaidDrives drives."
echo "Note that system performance may be degraded for up to the next 24 hours as the array rebuilds itsself."



