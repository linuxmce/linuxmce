#!/bin/bash

DEVICETEMPLATE_Stereo_virtual_sound_card=2223
DEVICEDATA_Channel_Left=311
DEVICEDATA_Channel_Right=312

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

shopt -s nullglob
if [[ -d /sys/class/sound ]]; then
	pushd /sys/class/sound &>/dev/null
	Ports=
	for dev in controlC*/device; do
		path=$(readlink -f "$dev")
		if [[ "$path" == *usb* ]]; then
			id=$(echo "$path" | sed -r 's,^.*(pci.*)/usb[0-9]+/[0-9./-]+/[0-9]+-([0-9.]+):[0-9.]+,\1+\2,g; s,/sound/.*$,,g')
		else
			id=$(echo "$path" | sed -r 's,^.*(pci.*),\1,g; s,/sound/.*$,,g')
		fi
		manufacturer=$(cat $path/id)
		Ports="$Ports $manufacturer;$id"
	done
	echo "${Ports# }" | tr ' ' '\n'
	popd &>/dev/null
fi

Devices=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Stereo_virtual_sound_card" "" "" all)
for Dev in $Devices; do
	echo "Virtual_$Dev"
done
