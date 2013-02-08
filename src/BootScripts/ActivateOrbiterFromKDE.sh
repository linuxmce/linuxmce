#!/bin/bash
#
# This little script checks the device id of the current system, and sends an Activate PC Desktop = False to the Orbiter of the current
# system. That way, the KDE Desktop goes into the background, and the Orbiter apears.
# 

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# Get Orbiter device (DeviceTemplate 62) that is controlled via this device
if [[ "$PK_Device" -eq "1" ]]; then
	# We are on the core
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = "
	Q="$Q (SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = $PK_Device AND FK_DeviceTemplate = 28) And FK_DeviceTemplate = 62"
else
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = $PK_Device AND FK_DeviceTemplate = 62"
fi
PK_Orbiter=$(RunSQL "$Q")
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Orbiter 1 912 119 "0"