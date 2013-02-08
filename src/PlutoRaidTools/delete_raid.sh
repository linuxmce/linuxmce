#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

RaidDevice=$1
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $BLOCK_DEVICE_ID"
array=$(RunSQL "$Q")


Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $RaidDevice"
HardDriveList=$(RunSQL "$Q")
for Drive in $HardDriveList; do
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData = $BLOCK_DEVICE_ID AND FK_Device=$Drive"
	R=$(RunSQL "$Q")
    	/usr/pluto/bin/delete_drive.sh '1' $array $R
done

mdadm -S "$array"
#rm -f $array
#rm -f "$array:" "/dev/.static$array" "/dev/.tmp.$array"

# Save RAID configuration
mdadm --detail --scan >> /etc/mdadm/mdadm.conf
echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf
