#!/bin/bash

#
# Configuration script for : Device Template 1963 - Phoenix Solo USB Conference Microphone
#   Set SimplePhone's default sound card to the Phoenix Solo USB's Card ID. 
#

. /usr/pluto/bin/SQL_Ops.sh

DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST=1759
DEVICETEMPLATE_Onscreen_Orbiter_CONST=62
DEVICEDATA_Location_on_PCI_bus_CONST=175
DEVICEDATA_Sound_Card_CONST=288
MANUFACTURER=default # dunno why...

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

# Find the relevant SimplePhone near this Mic.
PK_Device_MD=$(RunSQL "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=$Device_ID")
PK_Device_Orbiter=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=$DEVICETEMPLATE_Onscreen_Orbiter_CONST AND FK_Device_ControlledVia=$PK_Device_MD")
PK_Device_SimplePhone=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=$DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST AND FK_Device_ControlledVia=$PK_Device_Orbiter")

# Find the relevant device's PCI address
Raw_Location=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DEVICEDATA_Location_on_PCI_bus_CONST")

# Translate it into what the Sound Card device data wants
id=$(echo "$Raw_Location" | sed -r 's,^.*(pci.*)/usb[0-9]+/[0-9./-]+/[0-9]+-([0-9.]+):[0-9.]+,\1+\2,g; s,/sound/.*$,,g')
Sound_Card_Location="$MANUFACTURER;$id"

# And finally drop the relevant Device Data in place, or update it.
Q="REPLACE INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) 
      VALUES ('$PK_Device_SimplePhone','$DEVICEDATA_Sound_Card_CONST','$Sound_Card_Location')"
RunSQL "$Q"
