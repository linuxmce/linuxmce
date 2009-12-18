#!/bin/bash

. /usr/pluto/bin/CaptureCards_Utils.sh

Device="$1"
Slot="$2"

UpdatePorts_old()
{
	local Device="$1" Slot="$2"

	Skip=1

	while read line; do
		if [[ "$line" == card:* ]]; then
			FoundSlot="${line##*bus info }"
			FoundSlot="${FoundSlot%%, ivtv*}"
			FoundSlot="${FoundSlot#*:}"

			if [[ "$FoundSlot" == "$Slot" ]]; then
				Skip=0
			fi
		elif [[ "$Skip" -eq 0 ]]; then
			if [[ "$line" != *"MPG encoding"* ]]; then
				DevEntry="#ERROR: device doesn't support MPEG encoding"
			else
				DevEntry="${line##*/dev/}"
				DevEntry="${DevEntry%%: MPG*}"
			fi
			Skip=1
		fi
	done < <(ivtv-detect | egrep '^card:|/dev/.*: MPG encoding')

	UpdatePorts_NoFind "$Device" "$DevEntry"
}

UpdatePorts_new()
{
	local Device="$1" Slot="$2"
	local Ports dev DevSlot Port PixelFmt
	Ports=()

	if [[ ! -d /sys/class/video4linux ]]; then
		return 0
	fi

	pushd /sys/class/video4linux >/dev/null
	for dev in *; do
		if [[ "$dev" != video* ]]; then
			continue
		fi
		DevSlot=$(readlink -f "$dev"/device)
		DevSlot="${DevSlot##*/}"
		DevSlot="${DevSlot#*:}"

		if [[ "$DevSlot" != "$Slot" ]]; then
			continue
		fi

		PixelFmt=$(v4l2-ctl -V -d /dev/"$dev" | grep 'Pixel Format.*MPEG')
		if [[ -z "$PixelFmt" ]]; then
			continue
		fi

		Ports=("${Ports[@]}" "$dev")
	done

	Port=$(echo "${Ports[@]}" | sed 's/ /\n/g' | sort | head -1)
	echo "Dev '$Device' Ports: ${Ports[*]}; Chosen: $Port"
	UpdatePorts_NoFind "$Device" "$Port"

	popd >/dev/null
}

UpdatePorts()
{
	local Kver=$(uname -r)
	local Device="$1" Slot="$2"
	
	if [[ "$Kver" == "2.6.17" || "$Kver" > "2.6.17" ]]; then
		UpdatePorts_new "$Device" "$Slot"
	else
		UpdatePorts_old "$Device" "$Slot"
	fi
}

Slot="${Slot##*/}"
Slot="${Slot#*:}"

UpdatePorts "$Device" "$Slot"
