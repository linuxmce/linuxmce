#!/bin/sh

Once=false
while :; do
	Model=$(cat /proc/diag/model)
	case "$Model" in
		"ASUS WL-500g Premium")
			WANstate=$(robocfg show|grep '^Port 0'|awk '{print $3}')
			;;
		"ASUS WL-500g Premium V2")
			WANstate=$(robocfg show|grep '^Port 4'|awk '{print $3}')
			;;
	esac
	if [[ "$WANstate" == DOWN ]]; then
		if [[ "$Once" == false ]]; then
			/etc/init.d/network restart
			Once=true
		fi
		/usr/bin/NetLED.sh >/dev/null 2>&1 </dev/null &
	else
		Once=false
	fi
	sleep 1
done
