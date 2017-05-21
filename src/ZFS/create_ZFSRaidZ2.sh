#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

function Log {
	echo "$(date -R) $$ $*" >&2
	echo "$(date -R) $$ $*" >> /var/log/pluto/CreateZFSRaidZ2.log
}


Device=$1
NEW_ADD_ID=204
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
SPARE_ID=202
DISK_SIZE_ID=201

if [[ "$3" != "demonized" ]] ;then
	name=$(/usr/pluto/bin/get_ZFS_name.sh "$1")
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '$name' WHERE FK_Device = $Device and FK_DeviceData = $BLOCK_DEVICE_ID"

	#if is new added get the list of active drives and spares
	NewAdd=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Device and FK_DeviceData = $NEW_ADD_ID")
	if [[ $NewAdd == 0 ]] ;then
		Log "Creating new RAID 0 on /dev/$name"

		## Counting the number of spares and active diskes
		## Creating a list with spares and active disks
		ActiveDrives=
		NrDrives=
		AddSpares=0
		SpareDrives=
		HardDriveList=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $Device")
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
			if [[ $IsSpare == 1 ]]; then
			    Log "Adding Spare Disk $Disk to $name"
			    AddSpares=1
			    SpareDrives="$SpareDrives "$Disk
			else
			    Log "Adding Active drive $Disk to $name"
			    ActiveDrives="$ActiveDrives "$Disk
			fi
		done

		Log "Using parted to create GPT disklabels"

		if [[ $AddSpares == 1 ]]; then
 		    for d in $SpareDrives; do parted -a optimal -s $d mklabel gpt; done
		else
		    for d in $ActiveDrives; do parted -a optimal -s $d mklabel gpt; done
		fi
		
		Log "Using zpool to actually create the pool"
		if [[ $AddSpares == 1 ]]; then
		    zpool_err="$?"
		    Log "Process 'zpool create $name raidz2 $ActiveDrives' spare '$SpareDrives' exited with error $zpool_err"
		    zpool create -m legacy "$name" raidz2 $ActiveDrives spare $SpareDrives
		else
		    zpool create -m legacy "$name" raidz2 $ActiveDrives
		    zpool_err="$?"
		    Log "Process 'zpool create $name raidz2 $ActiveDrives' exited with error $zpool_err"
		fi

		sleep 3

		# Update Raid Information
		Log "Updating raid information in the database"

		## Create a pluto directory structure (depending on UsePlutoDirStructure device data)
		Log "Activating raid to be used as a storage device"
		touch /usr/pluto/var/DCERouter.running
		/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device
		/usr/pluto/bin/StorageDevices_Setup.sh

		/usr/pluto/bin/monitor_ZFS.sh "REFRESH_FROM_CREATE_ZFS_POOL" "$name"

		exit 0
	fi
	## Format the new raid
	LogFile="/usr/pluto/var/${Device}_Raid.log"
	echo "FORMAT,100" > $LogFile

	sleep 5
		
	rm $LogFile
fi
