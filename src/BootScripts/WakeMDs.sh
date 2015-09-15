#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

P="$1"

DEVICECATEGORY_Computer=6
DEVICECATEGORY_MD=8
DEVICETEMPLATE_Fiire_Station=1893
DEVICEDATA_Diskless_Boot=9
DEVICEDATA_PowerOff_Mode=290

if [ "$P" == "D" ]; then
	# Selecting diskless MDs only
	Q="SELECT MACaddress
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate IN (
		SELECT PK_DeviceTemplate
		FROM DeviceTemplate
		WHERE FK_DeviceCategory IN (${DEVICECATEGORY_MD}) )
	AND IPaddress IS NOT NULL
	AND MACaddress IS NOT NULL
	AND (FK_DeviceData=${DEVICEDATA_Diskless_Boot}
		 AND IK_DeviceData='1')
	AND (PK_Device IN (
		SELECT PK_Device
		FROM Device_DeviceData
		JOIN Device ON PK_Device=FK_Device
		AND (FK_DeviceData=${DEVICEDATA_PowerOff_Mode}
		AND IK_DeviceData <> 'N') ) )
	"
else
	# Selecting all MDs
	Q="SELECT MACaddress
	FROM Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate IN (
		SELECT PK_DeviceTemplate
		FROM DeviceTemplate
		WHERE FK_DeviceCategory IN (${DEVICECATEGORY_MD}) )
	AND IPaddress IS NOT NULL
	AND MACaddress IS NOT NULL
	AND (PK_Device IN (
		SELECT PK_Device
		FROM Device_DeviceData
		JOIN Device ON PK_Device=FK_Device
		AND (FK_DeviceData=${DEVICEDATA_PowerOff_Mode}
		AND IK_DeviceData <> 'N') ) )
	"
fi

R=$(RunSQL "$Q")

MaxIt=5
for i in $(seq 1 $MaxIt); do
	for HostMAC in $R; do
		echo "Sending WOL magic packet (iteration $i of $MaxIt) to $HostMAC"
		/usr/sbin/etherwake -b -i "$IntIf" "$HostMAC"

		#For motherboards with Nvidia Ethernet chipset with reversed MAC address, we issue a wake command with reversed MAC
		Reversed_HostMAC="${HostMAC:15:2}:${HostMAC:12:2}:${HostMAC:9:2}:${HostMAC:6:2}:${HostMAC:3:2}:${HostMAC:0:2}";
		/usr/sbin/etherwake -b -i "$IntIf" "$Reversed_HostMAC"
	done
	sleep 1
done
