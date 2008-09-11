#!/bin/sh

Device="$1"
Cmd="$2"

if [[ -z "$Device" ]] || [[ -z "$Cmd" ]]; then
	exit
fi

if [[ -n "$(ps | grep -q "Start_Device.sh $Device" | grep -v grep)" ]]; then
	exit
fi

while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/"$Cmd" -r 127.0.0.1 -d "$Device"
	sleep 8
done | /usr/bin/interfeed $(expr 20000 + $Device)
