#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

echo "LMCE Suspend Process Started" > /var/log/pluto/sus.log

## Turn on WOL

/usr/pluto/bin/enable_wol.sh

DeviceID="$PK_Device"

## Identify immediate children DCE devices that are currently Registered

FindChildrenQ="SELECT Device.PK_Device
FROM Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate
LEFT JOIN Device AS Device_Parent on Device.FK_Device_ControlledVia=Device_Parent.PK_Device
LEFT JOIN DeviceTemplate AS DeviceTemplate_Parent
ON Device_Parent.FK_DeviceTemplate=DeviceTemplate_Parent.PK_DeviceTemplate
WHERE (Device.FK_Device_ControlledVia=$DeviceID
    OR (Device_Parent.FK_Device_ControlledVia=$DeviceID AND DeviceTemplate_Parent.FK_DeviceCategory IN (6,7,8) ) )
AND DeviceTemplate.FK_DeviceCategory <> 1
AND DeviceTemplate.ImplementsDCE=1
AND Device.Registered=1;
"

DeviceList=$(RunSQL "$FindChildrenQ")

## Send SYSCOMMAND_0 (device shutdown) message to all immediate children. These devices will relay the message to all decendents

for Device in $DeviceList; do

    /usr/pluto/bin/MessageSend "$DCERouter" 0 "$Device" 7 0 163 "start_local_devices"

done

## Send SYSCOMMAND_0 message to MD device itself
/usr/pluto/bin/MessageSend dcerouter 0 $DeviceID  7 0 163 "start_local_devices"


## Wait for DCE devices to complete their shutdown

MaxLoopCount=50
for ((i = 0; i < MaxLoopCount; i++)); do
    Devices=$(cat /usr/pluto/locks/pluto_spawned_local_devices.txt | grep -v '^$' | tr '\n' ',')
    Devices="${Devices%,}"
    if [[ -z "$Devices" ]]; then
	break
	fi
    RegCount=0
    Q="SELECT COUNT(*) FROM Device WHERE PK_Device IN ($Devices) AND Registered=1"
    RegCount=$(RunSQL "$Q")
    if [[ "$RegCount" -eq 0 ]]; then
	break
	fi
    echo "Waiting for $RegCount devices to shutdown ($Devices)"
    sleep 1
done
                                                                                                                       
echo "Done waiting"

## Kill the LM processes
                                                                                                                        
killall -q -s SIGKILL -r LMCE_Launch_Manager
