#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DD_MODULES=106
DT_GENERIC_PC_AS_MD=28
DT_RPI_B_AS_MD=2216

R=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $PK_Device")
ThisDeviceTemplate=$(Field "1" "$R")

# FIXME: should check for Device Category: Media Director instead
if [[ "$ThisDeviceTemplate" == "$DT_GENERIC_PC_AS_MD" ]] || [[ "$ThisDeviceTemplate" == "$DT_RPI_B_AS_MD" ]]; then
	R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=${PK_Device} AND FK_DeviceData=$DD_MODULES")
else
	Q="
		SELECT
			IK_DeviceData
		FROM
			Device_DeviceData
			INNER JOIN Device ON Device.PK_Device = Device_DeviceData.FK_Device
		WHERE
			FK_DeviceData=$DD_MODULES
			AND
			Device.FK_Device_ControlledVia = $PK_Device
			AND
			(Device.FK_DeviceTemplate = $DT_GENERIC_PC_AS_MD
				OR
				Device.FK_DeviceTemplate = $DT_RPI_B_AS_MD)
	"
	R=$(RunSQL "$Q")
fi

ModulesList=$(Field "1" "$R")
for module in $( echo $ModulesList  | tr ',' ' ') ;do
	modprobe $module
done
