#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

function Log {
        echo "$(date -R) $$ $*" >&2
	echo "$(date -R) $$ $*" >> /var/log/pluto/CreateRaid5.log
}

Device=$1
RAID1_DEVICE_TEMPLATE=1851
NEW_ADD_ID=204
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
SPARE_ID=202
DISK_SIZE_ID=201

if [[ "$3" != "demonized" ]] ;then
	name=$(/usr/pluto/bin/get_RAID_name.sh)
	name="/dev/$name"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '$name' WHERE FK_Device = $Device and FK_DeviceData = $BLOCK_DEVICE_ID"

	#if is new added get the list of active drives and spares
	NewAdd=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Device and FK_DeviceData = $NEW_ADD_ID")
	if [[ $NewAdd == 0 ]] ;then
		Log "Createing new RAID 1 on /dev/$name"

		## Counting the number of spares and active diskes
                ## Creating a list with spares and active disks
		ActiveDrives=
		SpareDrives=
		NrDrives=
		NrSpareDrives=
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

			if [[ $IsSpare == 1 ]] ;then
				Log "Adding Spare drive $Disk to $name"
				SpareDrives="$SpareDrives "$Disk
				NrSpareDrives=$(($NrSpareDrives+1))
			else
				Log "Adding Active drive $Disk to $name"
				ActiveDrives="$ActiveDrives "$Disk
				NrDrives=$(($NrDrives+1))
			fi
		done

		## Create the array with mdadm
		Log "Using mdadm to actually create the raid"
		rm -f "/dev/.static$name"
		if [[ $NrSpareDrives > 0 ]] ;then
			echo "y" | mdadm --create $name --auto=yes --level=1 --spare-devices=$NrSpareDrives $SpareDrives --raid-devices=$NrDrives $ActiveDrives
			mdadm_err=$?
		else	
			echo "y" | mdadm --create $name --auto=yes --level=1 --raid-devices=$NrDrives $ActiveDrives
			mdadm_err=$?
		fi
		Log "Process 'mdadm --create' exited with error $mdadm_err"

		sleep 3

		# Save RAID configuration
		mdadm --detail --scan >> /etc/mdadm/mdadm.conf
		echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf

		# Update Raid Information
		Log "Updating raid information in the database"
                /usr/pluto/bin/monitor_Raid.sh "REFRESH_FROM_CREATE_RAID1" "/dev/$name"

		## Create a ext3 fielsystem on the new raid
                Log "Start creating a filesystem on the new raid"
		screen -S FormatingRaid1 -d -m /bin/bash $0 $Device $name demonized

                Log "Mkfs is started in the background"
                screen -S FormatingRaid1-Mkfs -d -m bash -c "echo \"y\" | mkfs.ext3 $name > /var/log/mkfs_${Device} 2>/var/log/mkfs_${Device}_err"		
		exit 0
	fi
else
	## Format the new raid
	LogFile="/usr/pluto/var/${Device}_Raid.log"
	echo "FORMAT,1" > $LogFile

	percent=0
	while [[ "$percent" != 100 ]] ;do
		mkfsOutput=$(cat /var/log/mkfs_${Device}  | grep "Writing inode tables: " | sed -r 's/.*[^0-9]([0-9]+\/[0-9]+|done)[^0-9]*/\1/')
		sleep 4

		if [[ "$mkfsOutput" == "" ]] ;then
			percent=1

		elif [[ "$mkfsOutput" == "done" ]] ;then
			percent=100

		else
			done=$(echo $mkfsOutput | cut -d'/' -f1)
			total=$(echo $mkfsOutput | cut -d'/' -f2)
			percent=$(((done * 100) / total ))
		fi

		echo "FORMAT,$percent" > $LogFile
	done
	Log "Mkfs finshed"

	## Create a pluto directory structure (depending on UsePlutoDirStructure device data)a
	Log "Activating raid to be used as a storage device"
	touch /usr/pluto/var/DCERouter.running
	/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device
	/usr/pluto/bin/StorageDevices_Setup.sh
	
	service mdadm reload

	rm $LogFile
fi			
	

