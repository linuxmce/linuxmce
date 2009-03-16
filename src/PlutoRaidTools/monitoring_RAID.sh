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
RAID5_DEVICE_TEMPLATE=1849
RAID1_DEVICE_TEMPLATE=1851
RAID0_DEVICE_TEMPLATE=1854

date=$(date)

if [[ -z $Param_Disk ]]; then
	echo "($date) Catched event $Param_Event for $Param_Raid" >> /var/log/pluto/RAID_monitoring.log
else	
	echo "($date) Catched event $Param_Event for $Param_Raid from $Param_Disk" >> /var/log/pluto/RAID_monitoring.log
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

function Raid_UpdateStatusFromMdadm {
	local Raid_DeviceID="$1"
	local Raid_Block=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Raid_DeviceID AND FK_DeviceData='$DD_BLOCK_DEVICE'")

	mdadm --detail --brief "$Raid_Block"
	mdadm_err=$?
	if [[ "$mdadm_err" != "0" ]] ;then
		Raid_SetStatus "$Raid_DeviceID" "FAILED"
		exit 0
	fi

	mdadm_state=$(mdadm --detail "$Raid_Block" | grep "State :" |cut -d":" -f2)
	case "$mdadm_state" in
		" clean"|" active")
			Raid_SetStatus "$Raid_DeviceID" "OK"
		;;
		" clean, degraded, recovering"|" active, degraded, recovering")
			Raid_SetStatus "$Raid_DeviceID" "DAMAGED, REBUILDING"
		;;
		" clean, degraded"|" active, degraded"|*)
#			local working_no=$(mdadm --detail $Raid_Block  | grep "Working Devices :" | cut -d':' -f2)
#			local failed_no=$(mdadm --detail $Raid_Block  | grep "Failed Devices :" | cut -d':' -f2)
#			local spare_no=$(mdadm --detail $Raid_Block  | grep "Spare Devices :" | cut -d':' -f2)

			local active_no=$(mdadm --detail $Raid_Block | grep "Active Devices :" | cut -d':' -f2)
			local needed_no=$(mdadm --detail $Raid_Block | grep "Raid Devices :" | cut -d':' -f2)

			Raid_DeviceTemplate=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $Raid_DeviceID")
			if [[ "$RaidTemplate" != "1854" ]] ; then # Than means if is not RAID 0
				needed_no=$(( needed_no - 1 ))
			fi

			if (( active_no >= needed_no )) ;then
				Raid_SetStatus "$Raid_DeviceID" "DAMAGED"
			else
				Raid_SetStatus "$Raid_DeviceID" "FAILED"
			fi
		;;
	esac
}


function Raid_UpdateDisksType {
	local Raid_DeviceID="$1"
	local Raid_Block=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Raid_DeviceID AND FK_DeviceData='$DD_BLOCK_DEVICE'")

	local Q="
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
	local R=$(RunSQL "$Q")

	local Disk
	for Disk in $R ;do
		local Disk_DeviceID=$(Field "1" "$Disk")
		local Disk_Block=$(Field "2" "$Disk")
		local Disk_Type=""
		
		local partition_info_line=$(mdadm --detail "$Raid_Block" | grep "${Disk_Block}$")
		
		if [[ "$partition_info_line" != "" ]] ;then
			partition_info_line=$(echo "$partition_info_line" | sed 's/active sync/AS/g')
			partition_info_line=$(echo "$partition_info_line" | sed 's/spare rebuilding/SR/g')
			partition_info_line=$(echo "$partition_info_line" | sed 's/faulty spare/FS/g')
			partition_info_line=$(echo "$partition_info_line" | sed 's/spare/S/g')
		
			Disk_Type="$(echo $partition_info_line | cut -d' ' -f5)"
		else
			Disk_Type="ABS"
		fi


		
		case "$Disk_Type" in
		"ABS") #disk can't be find in raid (absent)
			RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $Disk_DeviceID AND FK_DeviceData = $SPARE_ID"
			Raid_SetStatus "$Disk_DeviceID" "REMOVED"	
			;;
		"SR") #spare rebuilding
			RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $Disk_DeviceID AND FK_DeviceData = $SPARE_ID"
			Raid_SetStatus "$Disk_DeviceID" "ACTIVATING"
			;;
		"FS") #faulty spare
			RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $Disk_DeviceID AND FK_DeviceData = $SPARE_ID"
			Raid_SetStatus "$Disk_DeviceID" "FAULTY"
			;;
		"S")
			RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $Disk_DeviceID AND FK_DeviceData = $SPARE_ID"
			Raid_SetStatus "$Disk_DeviceID" "OK"
			;;
		"AS") #active sync
			RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 0 WHERE FK_Device = $Disk_DeviceID AND FK_DeviceData = $SPARE_ID"
			Raid_SetStatus "$Disk_DeviceID" "OK"
			;;
		esac
		
	done	
		
}

Raid_DeviceID="$(RunSQL "SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE")"
if [[ "$Raid_DeviceID" == "" ]] ;then
	exit 0
fi
Raid_UpdateDisksType "$Raid_DeviceID"

case "$Param_Event" in
	"DeviceDisappeared" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")a

		Q="UPDATE Device_DeviceData SET IK_DeviceData = 3 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
		$(RunSQL "$Q")

		Raid_SetStatus "$DeviceID" 'FAILED'
	;;

	"RebuildStarted" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
		RunSQL "$Q"

		Raid_SetStatus "$DeviceID" "DAMAGED, REBUILDING (0/100)"

		raidSize=$(mdadm --query $Param_Raid | head -1 |cut -d' ' -f2)
		Raid_SetSize "$DeviceID" "$raidSize"
	;; 

	"RebuildFinished" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		if [[ "$(mdadm --detail "$Param_Raid"  | grep "State :" | grep "degraded")" != "" ]] ;then
			Raid_SetStatus "$DeviceID" "DAMAGED"
		else

			Q="UPDATE Device_DeviceData SET IK_DeviceData = 4 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
			RunSQL "$Q"

			Raid_SetStatus "$DeviceID" "OK"

			raidSize=$(mdadm --query $Param_Raid | head -1 |cut -d' ' -f2)
			Raid_SetSize "$DeviceID" "$raidSize"

			
		fi
	;;

	"Rebuild"* )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
		RunSQL "$Q"

		len=$((${#Param_Event}))
		progress=${Param_Event:$len-2:$len}
		Raid_SetStatus "$DeviceID" "DAMAGED, REBUILDING ($progress/100)"

		raidSize=$(mdadm --query $Param_Raid | head -1 |cut -d' ' -f2)
		Raid_SetSize "$DeviceID" "$raidSize"
	;;

	"Fail" )
		#TODO: Fix the logic for this event 
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Disk' AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")
		Raid_SetStatus "$DeviceID" "FAULTY"

		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Q="SELECT COUNT(PK_Device) FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $DeviceID"
		HardDriveNr=$(RunSQL "$Q")

		Q="SELECT COUNT(IK_DeviceData)
		   FROM Device_DeviceData 
		   INNER JOIN Device ON Device_DeviceData.FK_Device = Device.PK_Device 
		   WHERE Device.FK_Device_ControlledVia = $DeviceID AND
			 Device_DeviceData.FK_DeviceData = $STATE_ID AND 
			 Device_DeviceData.IK_DeviceData = 'FAULTY'"
		FailedDriveNr=$(RunSQL "$Q")

		Q="SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $DeviceID"
		RaidTemplate=$(RunSQL "$Q")
		case "$RaidTemplate" in 
			"$RAID5_DEVICE_TEMPLATE" | "$RAID1_DEVICE_TEMPLATE" )
				if (( $HardDriveNr == 2 && $FailedDriveNr == 1 )) ||  (( $FailedDriveNr > 1 )) ;then
					Raid_SetStatus "$DeviceID" "FAILED"
				else
					Raid_SetStatus "$DeviceID" "DAMAGED"
				fi
			;;

			"$RAID0_DEVICE_TEMPLATE" )
				if (( $FailedDriveNr >= 1 )) ;then
					Raid_SetStatus "$DeviceID" "FAILED"
				fi
			;;
		esac
	;;

	"FailSpare" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Disk' AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Raid_SetStatus "$DeviceID" "FAULTY"
	;;

	"SpareActive" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Disk' AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Raid_SetStatus "$DeviceID" "OK"

		Q="UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device = $DeviceID and FK_DeviceData = $SPARE_ID"
		RunSQL "$Q"
	;;

	"DegradedArray" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")

		Q="UPDATE Device_DeviceData SET IK_DeviceData = 5 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
		RunSQL "$Q"

		Q="
		SELECT 
			COUNT(PK_Device)
		FROM
			Device
			JOIN Device_DeviceData Spare ON PK_Device=Spare.FK_Device
			JOIN Device_DeviceData State ON PK_Device=State.FK_Device
		WHERE
			PK_Device = $DeviceID
			AND
			Spare.FK_DeviceData = $SPARE_ID AND Spare.IK_DeviceData = 1
			AND
			State.FK_DeviceData = $STATE_ID AND State.IK_DeviceData = 'OK'

		"
		SpareNr=$(RunSQL "$Q")
		if (( $SpareNr >= 1 )) ;then
			Raid_SetStatus "$DeviceID" "DAMAGED, REBUILDING"
		else
			Raid_SetStatus "$DeviceID" "DAMAGED"
		fi
	;;

	*)
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$Param_Raid'  AND FK_DeviceData = $DD_BLOCK_DEVICE"
		DeviceID=$(RunSQL "$Q")
		
		x=$(mdadm --detail $Param_Raid  | grep "Array Size" | cut -d ':' -f2 | awk '{print $2 $3}' | sed 's/(//')
		Raid_SetSize "$DeviceID" "$x"	

		Raid_UpdateStatusFromMdadm "$DeviceID"
	;;
esac
	
