#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="select PK_Device,IPaddress,IK_DeviceData from Device,DeviceTemplate,Device_DeviceData where FK_DeviceCategory = 177 and Device.FK_DeviceTemplate = PK_DeviceTemplate and Device_DeviceData.FK_Device = PK_Device;"

R=$(RunSQL $Q)

for Device in $R
do
	PK_Device=$(Field 1 "$Device")
	IPaddress=$(Field 2 "$Device")
	Port=$(Field 3 "$Device")
	
	echo Spawning socat for device $PK_Device - $IPaddress : $Port
	Device="/dev/ttyS_"${PK_Device}

	/usr/pluto/bin/gc100-serial-bridge.sh $IPaddress $Port $Device &
done
