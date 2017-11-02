#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

RaidDevice=$1
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $BLOCK_DEVICE_ID"
array=$(RunSQL "$Q")

zpool destroy $array

Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $RaidDevice"
HardDriveList=$(RunSQL "$Q")
for Drive in $HardDriveList; do
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData = $BLOCK_DEVICE_ID AND FK_Device=$Drive"
	R=$(RunSQL "$Q")
    	parted -a optimal -s $R mklabel gpt
done
