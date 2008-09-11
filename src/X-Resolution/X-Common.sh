#!/bin/bash

GenModeline()
{
	local Driver="$1" ResX="$2" ResY="$3" Refresh="${4:-60}" ScanType="$5"
	local Modeline
	
	local ResName="$ResX"x"$ResY"
	if [[ "$ScanType" == interlace ]]; then
		ResName="$ResName"i
	fi

	Modeline=$(/usr/pluto/bin/X-GetModeline "$Driver" "$ResName" "$Refresh")

	if [[ -z "$Modeline" ]]; then
		# The is no modeline available, neither builtin nor explicit, so generate one
		Modeline=$(/usr/pluto/bin/xtiming.pl "$ResX" "$ResY" "$Refresh" "$ScanType")
		Modeline="${Modeline/@*\"/\"}"
	elif [[ "$Modeline" == *builtin* ]]; then
		# The modeline is builtin
		Modeline=
	fi

	echo "$Modeline"
}

TestXConfig()
{
	local Display="${1:-0}" ConfigFile="${2:-/etc/X11/xorg.conf}"
	X :$((Display+2)) -ignoreABI -probeonly -config "$ConfigFile" -logverbose 9 &>/dev/null
	return $?
}
