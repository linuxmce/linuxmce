#!/bin/bash

#<-mkr_b_via_b->
exit
#<-mkr_b_via_e->

. /usr/pluto/bin/Config_Ops.sh
XDisplay=":$(($Display+1))"

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
		
        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT

trap true SIGINT

Params=(Width Height Refresh Type)
ParmIdx=0

for Param in "$@"; do
	case "$Param" in
		-f) : ;; # obsolete
		*)
			eval ${Params[$ParamIdx]}="$Param"
			((ParamIdx++))
		;;
	esac
done

Type="${Type:+--scantype $Type}"

ShowDialog()
{
	local pidOfX="$1"
	local Msg="$2"

	DISPLAY=$XDisplay xsetbg -tile /usr/pluto/share/resolutions.png
	DISPLAY=$XDisplay Xdialog --ignore-eof --cancel-label "Close" --infobox "$Msg" 0x0 0
	kill "$pidOfX"
}

cp /etc/X11/xorg.conf{,.test}

if ! /usr/pluto/bin/Xconfigure.sh --conffile /etc/X11/xorg.conf.test --resolution "${Width}x${Height}@${Refresh}" $Type; then
	echo "X configuration script exited with error"
	exit 10
fi

X $XDisplay  -ignoreABI -ac -config /etc/X11/xorg.conf.test -logverbose 9 </dev/null &>/dev/null &
pidOfX=
Timeout=5
while [[ -z "$pidOfX" && $Timeout > 0 ]]; do
	pidOfX="$(ps ax|grep "X $XDisplay -ignoreABI -ac -config /etc/X11/xorg.conf.test"|grep -v grep|awk '{print $1}')"
	((Timeout--))
	sleep 1
done

if [[ -z "$pidOfX" ]]; then
	echo "X didn't start"
	exit 10
fi

setting="$(xrandr -d $XDisplay -q|grep '^\*')"
resolution="${setting:5:11}"
refresh="${setting:39:4}"
Msg="Current resolution: $resolution @ $refresh Hz"
echo "$Msg"
ShowDialog "$pidOfX" "$Msg" </dev/null &>/dev/null &

echo "$Width $Height/$Refresh" >/tmp/Pluto_VideoSetting.txt

disown -a
exit 0
