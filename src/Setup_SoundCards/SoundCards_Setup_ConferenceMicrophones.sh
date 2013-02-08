#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICECATEGORY_Sound_Cards=176
DEVICEDATA_Audio_settings=88
DEVICEDATA_Setup_Script=189

SoundCard=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Sound_Cards")

if [[ -z "$SoundCard" ]]; then
	# no sound card device found; nothing to do
	exit 0
fi

Q="
	SELECT DDDD.IK_DeviceData
	FROM DeviceTemplate_DeviceData DDDD
	JOIN Device D ON D.FK_DeviceTemplate=DDDD.FK_DeviceTemplate
		AND D.PK_Device='$SoundCard'
		AND DDDD.FK_DeviceData='$DEVICEDATA_Setup_Script'
"
CommandLine="$(RunSQL "$Q")"

Q="
	SELECT IK_DeviceData FROM Device_DeviceData
	JOIN Device ON FK_Device=PK_Device
	WHERE FK_DeviceData=$DEVICEDATA_Audio_settings AND (PK_Device=$PK_Device OR FK_Device_ControlledVia=$PK_Device)
"
SoundSettings="$(RunSQL "$Q")"

if [[ -n "$CommandLine" ]]; then
	echo $CommandLine
	/usr/pluto/bin/"$CommandLine" "$SoundSettings"
fi
