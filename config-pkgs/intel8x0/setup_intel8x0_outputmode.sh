#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

template=1621

FindDevice()
{
    local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}";
    if [ -z "$PK_Device_Parent" -o -z "$FK_DeviceTemplate" ]; then
        echo ""
        return 1
    fi

    local i R Q
    Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate='$FK_DeviceTemplate'"
    R="$(RunSQL "$Q")"

    if [ -z "$R" ]; then
        Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
        R="$(RunSQL "$Q")"
        for i in $R; do
            FindDevice "$i" "$FK_DeviceTemplate" && return 0
        done
    else
        echo "$R"
        return 0
    fi
    return 1
}

DesiredDevice=$(FindDevice $PK_Device $template)

if [ -z "$DesiredDevice" ]; then 
	echo "No device with template $template was found on this machine ($PK_Device)";
	exit
fi

echo "Found a device ($DesiredDevice) with correct template ($template) on this machine";

QUERY="select IK_DeviceData from Device_DeviceData where FK_Device = $DesiredDevice";
DesiredDeviceData=$(RunSQL "$QUERY")
amixer scontrols | cut -f 2 -d \' | while read control; do 
	if [ "$control" = "IEC958" ]; then 
		case $DesiredDeviceData in
			A) amixer -q set IEC958 off;;
			C|D) amixer -q set IEC958 on;;
		esac
	elif [ "$control" = "IEC958 Playback AC97-SPSA" ]; then
		case $DesiredDeviceData in 
			A)		amixer -q cset name='IEC958 Playback AC97-SPSA' 0;;
			C|D)	amixer -q cset name='IEC958 Playback AC97-SPSA' 3;;
		esac
	elif [ "$vaca" = "IEC958 Playback Source" ]; then
		case $DesiredDeviceData in 
			A) 		amixer -q cset name='IEC958 Playback Source' 1;;
			C|D)	amixer -q cset name='IEC958 Playback Source' 0;;
		esac
	fi
done;


case $DesirdDeviceData in
	A)		sed -i "s/pcm..default {.*/pcm.\!default { type plug slave.pcm \"dmix-analog\" }/" /etc/asound.conf;;
	C|D)	sed -i "s/pcm..default {.*/pcm.\!default { type plug slave.pcm \"dmix-digital\" }/" /etc/asound.conf;;
esac
