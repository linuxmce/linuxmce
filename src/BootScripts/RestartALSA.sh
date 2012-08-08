#!/bin/bash

notused=()
used=()

while read mod size count by; do
	if [[ "$mod" == snd_hda_codec_* ]]; then
		by="snd_hda_intel"
	fi
	if [[ "$count" -gt 0 ]]; then
		eval "mod_$mod='${by//,/ }'"
		used=("${used[@]}" "$mod")
	else
		notused=("${notused[@]}" "$mod")
	fi
done < <(lsmod | grep ^snd)

remove=()

walk()
{
	local mod var

	for mod in "$@"; do
		var="mod_$mod"
		if [[ -n "${!var}" ]]; then
			walk ${!var}
		fi
		var="mark_$mod"
		if [[ -z "${!var}" ]]; then
			remove=("${remove[@]}" "$mod")
			eval "mark_$mod=1"
		fi
	done
}

walk "${used[@]}"

for mod in "${notused[@]}"; do
	var="mark_$mod"
	if [[ -z "${!var}" ]]; then
		remove=("${remove[@]}" "$mod")
	fi
done

modprobe -rv "${remove[@]}"
#modprobe -va "${remove[@]}"

udevadm trigger

while ! [[ -f /proc/asound/version ]]; do
	sleep 1
done
sleep 5
