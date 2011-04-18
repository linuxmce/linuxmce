#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh

WaitLock "UpdateAvailableSerialPorts" "UASP"
SerialPorts=$(/usr/pluto/bin/ListSerialPorts.sh | tr '\n' ',')
SerialPorts=${SerialPorts%,}

DEVICEDATA_Available_Serial_Ports=133

Q="INSERT INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	VALUES ('$PK_Device', '$DEVICEDATA_Available_Serial_Ports', '${SerialPorts//\'/\'}')
	ON DUPLICATE KEY UPDATE IK_DeviceData = '${SerialPorts//\'/\'}' "
RunSQL "$Q" || /bin/true

Unlock "UpdateAvailableSerialPorts" "UASP"
