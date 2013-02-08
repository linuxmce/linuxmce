#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

DeviceID="$1"
DeviceTemplate="${2:-0}"
Restricted="${3:-0}"

#PlutohomeHost="http://10.0.0.175/plutohome-com/"
PlutohomeHost="http://linuxmce.org/"
sqlCVS_Server="schema.linuxmce.org"

if [[ -z "$DeviceID" && "$DeviceTemplate" -eq 0 ]]; then
	echo "ERROR. Either the DeviceID or the DeviceTemplate pamaters has to be specified"
	exit 1
fi

if [[ "$DeviceTemplate" -eq 0 ]]; then
	Q="
		SELECT FK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$DeviceID'
	"
else
	Q="
		SELECT PK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer
		FROM DeviceTemplate
		WHERE PK_DeviceTemplate='$DeviceTemplate'
	"
fi
Data="$(RunSQL "$Q")"
if [[ -z "$Data" ]]; then
	echo "ERROR. Empty result"
	exit 1
fi

FK_DeviceTemplate=$(Field 1 "$Data")
FK_InfraredGroup=$(Field 2 "$Data")
FK_DeviceCategory=$(Field 3 "$Data")
FK_Manufacturer=$(Field 4 "$Data")

Q="SHOW TABLES LIKE 'InfraredGroup_Command'"
R="$(RunSQL "$Q")"

if [[ -n "$R" ]]; then
	/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetIRCodes.php?Server=$sqlCVS_Server&PK_InfraredGroup=$FK_InfraredGroup&PK_DeviceTemplate=$FK_DeviceTemplate&Restricted=$Restricted" "InfraredGroup_Command"
	Ret1=$?
	/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetIRCodes.php?Server=$sqlCVS_Server&PK_Manufacturer=$FK_Manufacturer&PK_DeviceCategory=$FK_DeviceCategory&Restricted=$Restricted" "InfraredGroup_Command"
	Ret2=$?
	Ret=$((Ret1 || Ret2))
else
	if /usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetIRCodes.php?Server=$sqlCVS_Server&Create=1"; then
		/usr/pluto/bin/WebDB_Get.sh "$PlutohomeHost/GetIRCodes.php?Server=$sqlCVS_Server&PK_InfraredGroup=$FK_InfraredGroup&PK_DeviceTemplate=$FK_DeviceTemplate&Restricted=$Restricted" "InfraredGroup_Command"
		Ret=$?
	else
		Ret=1
	fi
fi

if [[ "$Ret" -ne 0 ]]; then
	echo "$(date -R) Failed to get data from PK_InfraredGroup=$FK_InfraredGroup&PK_DeviceTemplate=$FK_DeviceTemplate&Restricted=$Restricted"
	Q="UPDATE Device SET Status='**RUN_GET_IR**' WHERE FK_DeviceTemplate=$FK_DeviceTemplate"
else
	echo "$(date -R) Got data from PK_InfraredGroup=$FK_InfraredGroup&PK_DeviceTemplate=$FK_DeviceTemplate&Restricted=$Restricted"
	Q="UPDATE Device SET Status='' WHERE FK_DeviceTemplate=$FK_DeviceTemplate AND Status='**RUN_GET_IR**'"
fi
RunSQL "$Q"

exit "$Ret"
