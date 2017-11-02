#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Device=$1
RaidDevice=$2
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
NO_DISK_ID=199

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Device and FK_DeviceData = $BLOCK_DEVICE_ID"
Disk=$(RunSQL "$Q")

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice AND FK_DeviceData = $BLOCK_DEVICE_ID"
Raid=$(RunSQL "$Q")

zpool add $Raid spare $Disk

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $NO_DISK_ID"
noDisks=$(RunSQL "$Q")
noDisks=$(($noDisks+1))
Q="UPDATE Device_DeviceData SET IK_DeviceData = $noDisks WHERE FK_Device = $RaidDevice and FK_DeviceData = $NO_DISK_ID"
RunSQL "$Q"

