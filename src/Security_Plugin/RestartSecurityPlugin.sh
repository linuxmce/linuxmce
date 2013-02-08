#!/bin/bash

source /usr/pluto/bin/SQL_Ops.sh
source /usr/pluto/bin/Config_Ops.sh
Q="select PK_Device from Device where FK_DeviceTemplate=33";
R=$(RunSQL "$Q")

/usr/pluto/bin/MessageSend "$DCERouter" -targetType device 0 $R 2 18

