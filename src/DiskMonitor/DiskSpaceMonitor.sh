#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

module=$(basename $0)
TPL_BUFFALO_HDHG300LAN=1794
TPL_GENERIC_INTERNAL_DRIVE=1790
TPL_GENERIC_NFS_SHARE=1769
TPL_GENERIC_SAMBA_SHARE=1768
TPL_RAID_0=1854
TPL_RAID_1=1851
TPL_RAID_5=1849


DD_FREE_SPACE=160
DD_FILESYSTEM=159
DD_READONLY=194
DD_ONLINE=195


## Lock
mkdir -p /usr/pluto/locks
lock=/usr/pluto/locks/DiskSpaceMonitor.lock

if [ -f $lock ]; then
        Logging $TYPE $SEVERITY_WARNING $module "Lock file ($lock) already exists"
        exit
fi

touch $lock
trap "rm -f $lock" EXIT


## Clean / filesystem
function cleanRootFS { 
	## Clean the apt cache 
	# info : could fail if apt is running
	apt-get autoclean 2&>1 >/dev/null

	return 0
}

## Clean /home filesystem
function cleanHomeFS {
	## Remove coredups if any
	rm -rf /usr/pluto/coredump/* 

	## If we are on core, the diskless path is also in /home
	for DisklessRoot in /home/pluto/diskless/* ;do
		## Clean the coredumps
		rm -rf $DisklessRoot/usr/pluto/coredump/*
		
		## Clean the apt cache
		chroot $DisklessrRoot apt-get autoclean 2&>1 >/dev/null
	done

	return 0
}


## Cleanup only on core
if [[ ! -f /etc/diskless.conf ]]; then
	SystemFilesystemFull="false"

	## Check system root partitions for free space (/)
	dfOutput=$(df / | grep -v "^Filesystem")
	rootUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
	rootFree=$(echo $dfOutput | awk '{ print $4 }')
	if [[ $rootUsed -gt 95 || $rootFree -lt 204800 ]] ;then
		Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( / ) is getting full, trying to clean quietly!"
		cleanRootFS

		dfOutput=$(df / | grep -v "^Filesystem")
		rootUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
		rootFree=$(echo $dfOutput | awk '{ print $4 }')

		if [[ $rootUsed -gt 95 || $rootFree -lt 204800 ]] ;then
			Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( / ) was auto cleaned but there is still not much space left"
			SystemFilesystemFull="true"
		else
			Logging $TYPE $SEVERITY_NORMAL $module "Filesystem ( / ) was auto cleaned and now looks ok"
		fi
	fi

	## Check system /home partition for free space (/home)
	dfOutput=$(df '/home' | grep -v '^Filesystem')
	homeUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
	homeFree=$(echo $dfOutput | awk '{ print $4 }')
	if [[ $homeUsed -gt 95 || $homeFree -lt 204800 ]] ;then
		Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( /home ) is getting full, trying to clean quietly"
		cleanHomeFS

		dfOutput=$(df '/home' | grep -v '^Filesystem')
		homeUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
		homeFree=$(echo $dfOutput | awk '{ print $4 }')

		if [[ $homeUsed -gt 95 || $homeFree -lt 204800 ]] ;then
			Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( /home ) wan auto cleaned but there is still not much space left"
			SystemFilesystemFull="true"
		else
			Logging $TYPE $SEVERITY_NORMAL $module "Filesystem ( /home ) was auto cleaned and looks ok now"
		fi
	fi

	## Update Free Space device data for Core device
	Core_HomeFree=$(( $homeFree / 1024 ))
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$Core_HomeFree' WHERE FK_DeviceData = '$DD_FREE_SPACE' AND FK_Device = '$PK_Device'"
	RunSQL "$Q"

	if [[ $SystemFilesystemFull == "true" ]]; then
		# Trigger the Low System Disk Space (64) event
		touch /var/DiskSpaceMonitor.stamp
		LastEventTrigered=$(cat /var/DiskSpaceMonitor.stamp)
		Now=$(date +%s)
		if [[ $LastEventTrigered == "" || $(( $LastEventTrigered + 60 * 60 )) -lt $Now ]]; then
			/usr/pluto/bin/MessageSend $DCERouter 0 -1001 2 64
			echo $Now > /var/DiskSpaceMonitor.stamp
		fi
	fi
fi

# Check Pluto Storage device for free space

Q="SELECT PK_Device, Description  FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE, $TPL_GENERIC_NFS_SHARE, $TPL_GENERIC_SAMBA_SHARE, $TPL_RAID_0, $TPL_RAID_1, $TPL_RAID_5)"
StorageDevices=$(RunSQL "$Q")

for Device in $StorageDevices; do
        Device_ID=$(Field 1 "$Device")
        Device_Description=$(Field 2 "$Device")
        Device_MountPoint="/mnt/device/$Device_ID"

	## Check filesystem and force autmount to mount the device
	## and also to check if is readonly or not
	touch $Device_MountPoint/.check_mounted 2>/dev/null
	if [[ "$?" == 0 ]] ;then
		Device_isReadonly=0
		rm -f $Device_MountPoint/.check_mounted
	else
		Device_isReadonly=1
	fi

	## Check filesystem is mounted
	mount | grep -q " $Device_MountPoint " 1>/dev/null 2>/dev/null
	if [[ "$?" != 0 ]]; then
		Device_isMounted=0
	else
		Device_isMounted=1
	fi

	## Update the "Online" device data	
	if [[ $Device_isMounted == "0" ]] ;then
		Logging $TYPE $SEVERITY_NORMAL $module "Filesystem ( $Device_MountPoint ) is Offline"
	fi

	[[ $Device_isMounted == "0" ]] && continue
	
	if [[ $Device_isReadonly == "1" ]] ;then
		Logging $TYPE $SEVERITY_NORMAL $module "Filesystem ( $Device_MountPoint ) is ReadOnly"
	fi
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$Device_isReadonly' WHERE FK_DeviceData = '$DD_READONLY' AND FK_Device = '$Device_ID'"
	RunSQL "$Q"

	## Check the space on the drive
	dfOutput=$(df $Device_MountPoint 2>/dev/null | grep -v '^Filesystem')
	[[ $dfOutput == "" ]] && continue
		
	Device_DiskFree=$(echo $dfOutput | awk '{ print $4 }') ; Device_DiskFree=$(( $Device_DiskFree / 1024 ))
	Device_FileSystem=$(mount | grep "on $Device_MountPoint" | awk '{ print $5 }')

	## Update Free Space device data
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$Device_DiskFree' WHERE FK_DeviceData = '$DD_FREE_SPACE' AND FK_Device = '$Device_ID'"
	RunSQL "$Q"	

	## Update Filesystem device data
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$Device_FileSystem' WHERE FK_DeviceData = '$DD_FILESYSTEM' AND FK_Device = '$Device_ID'"
	RunSQL "$Q"	

	Logging $TYPE $SEVERITY_NORMAL  $module "Filesystem ( $Device_MountPoint ) has $(($Device_DiskFree)) MB free"
	
done
