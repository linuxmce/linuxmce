#!/bin/bash

call_from=$1
NO_DISK_ID=199

if [[ $call_from == '1' ]]; then
	array=$2
	disk=$3
else
	. /usr/pluto/bin/SQL_Ops.sh

	RaidDevice=$2
	Device=$3
	BLOCK_DEVICE_ID=152

	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $BLOCK_DEVICE_ID"
	array=$(RunSQL "$Q")
	
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Device and FK_DeviceData = $BLOCK_DEVICE_ID"
	disk=$(RunSQL "$Q")
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $NO_DISK_ID"
	noDisks=$(RunSQL "$Q")
	noDisks=$(($noDisks-1))
	Q="UPDATE Device_DeviceData SET IK_DeviceData = $noDisks WHERE FK_Device = $RaidDevice and FK_DeviceData = $NO_DISK_ID"
	RunSQL "$Q"
fi

mdadm --manage --set-faulty $array $disk
mdadm --manage --remove $array $disk
mdadm --zero-superblock $disk

# Save RAID configuration
mdadm --detail --scan >> /etc/mdadm/mdadm.conf
echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf
