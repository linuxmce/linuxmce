#!/bin/bash

#<-mkr_b_via_b->
exit
#<-mkr_b_via_e->

. /usr/pluto/bin/Config_Ops.sh
XDisplay=":$(($Display+1))"

DEVICECATEGORY_Media_Director=8
DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICEDATA_Video_settings=89

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)

        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT

Answer="$1"

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

pidOfX="$(ps ax|grep "X $XDisplay -ignoreABI -ac -config /etc/X11/xorg.conf.test"|grep -v grep|awk '{print $1}')"
if [[ -z "$pidOfX" ]]; then
	echo "test X not running. nothing to kill"
fi
if [[ ! -f "/etc/X11/xorg.conf.test" ]]; then
	echo "test config not found. no new settings to apply"
	exit 10
fi

echo "Killing test X"
kill $pidOfX

if [[ "${Answer:0:1}" == "y" || "${Answer:0:1}" == "Y" ]]; then
	VideoSetting=$(</tmp/Pluto_VideoSetting.txt)

	if [[ -n "$VideoSetting" ]]; then
		ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
		Q="
			UPDATE Device_DeviceData
			SET IK_DeviceData='$VideoSetting'
			WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'
		"
		RunSQL "$Q"
	fi
	OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")

	mv /etc/X11/xorg.conf{.test,}
	echo "Killing real X (to restart it with new config)"
	/usr/pluto/bin/RestartX.sh $OrbiterDev 127.0.0.1
	rm -f /tmp/Pluto_VideoSetting.txt
else
	rm -f /etc/X11/xorg.conf.test /tmp/Pluto_VideoSetting.txt
fi
echo
