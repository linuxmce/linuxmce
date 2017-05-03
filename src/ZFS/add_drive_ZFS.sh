#!/bin/bash

#
# Add Drive to ZFS pool.
#
#params:
#$1 raid device, such as "/dev/md0"
#$2 drive to add, such as "/dev/sdb"

ZFSPool=$1
DriveToAdd=$2

#Lets ensure that the user has passed in parameters for the script to work with
if [[ "$ZFSPool" == "" || "$DriveToAdd" == "" ]] ; then
	echo "Usage: add_drive.sh [ZFS pool] [drive path]"
	echo "Example: add_drive_ZFS.sh lmce /dev/sdb"
	echo "         Will add /dev/sdb to the lmce storage pool."
	echo ""
	echo "Add drive Failed: Please try again with the correct parameters."
	exit 0
fi

echo "Adding drive $DriveToAdd to RAID device $ZFSPool"
zpool add $ZFSPool $DriveToAdd
exit 0

