#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DD_MODULES=106
DT_GENERIC_PC_AS_MD=28
DT_RPI_B_AS_MD=2216
DC_COMPUTERS_MEDIA_DIRECTOR=8

R=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $PK_Device")
FK_DeviceTemplate=$(Field "1" "$R")

R=$(RunSQL "SELECT FK_DeviceCategory FROM DeviceTemplate WHERE PK_DeviceTemplate = $FK_DeviceTemplate")
FK_DeviceCategory=$(Field "1" "$R")

if [[ "$FK_DeviceCategory" == "$DC_COMPUTERS_MEDIA_DIRECTOR" ]]; then
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
	"
	R=$(RunSQL "$Q")
fi

ModulesList=$(Field "1" "$R")
for module in $( echo $ModulesList  | tr ',' ' ') ;do
	modprobe $module
done
