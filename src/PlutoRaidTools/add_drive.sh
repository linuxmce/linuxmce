#!/bin/bash

#used to add a kicked drive back to a RAID array.
#Jon Decker - jondecker76@gmail.com
#params:
#$1 raid device, such as "/dev/md0"
#$2 drive to add, such as "/dev/sdb"

RaidDevice=$1
DriveToAdd=$2

#Lets ensure that the user has passed in parameters for the script to work with
if [[ "$RaidDevice" == "" || "$DriveToAdd" == "" ]] ; then
	echo "Usage: add_drive.sh [block device path] [drive path]"
	echo "Example: grow_drive.sh /dev/md0 /dev/sdb"
	echo "         Will add /dev/sdb to the /dev/md0 Raid device."
	echo ""
	echo "Add drive Failed: Please try again with the correct parameters."
	exit 0
fi

echo "Adding drive $DriveToAdd to RAID device $RaidDevice"
mdadm --add $RaidDevice $DriveToAdd
exit 0

