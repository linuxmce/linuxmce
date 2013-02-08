#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="insert into Device_DeviceData(FK_Device,FK_DeviceData) select PK_Device,132 FROM Device where FK_DeviceTemplate=62"
RunSQL "$Q" || true

Q="update Device_DeviceData set IK_DeviceData=209 where FK_DeviceData=132"
RunSQL "$Q" || true

