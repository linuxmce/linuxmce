#!/bin/bash

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)

        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

BaseDir=/usr/pluto/bin

Wiz="$BaseDir"/AVWizard
AudioMixerSettingsAwk="$BaseDir"/AudioMixerSettings.awk

AVWizard_Port=28949 # AVWIZ

Resolutions_VESA=(
	'640x480'
	'800x600'
	'1024x768'
	'1280x1024'
	'1600x1200'
)
Resolutions_HDTV=(
	'480p=720x480'
	'1024-HDready=1024x768'
    	'720p=1280x720'
	'1080i=1920x1080i'
	'1080p=1920x1080'
	'1280x800'
)
Refresh_Rates=(
	'50'
	'60'
	'65'
	'72'
	'75'
	'80'
	'85'
	'100'
	'120'
)

XF86Config="/etc/X11/xorg.conf.pluto.avwizard"
XineConf="/etc/pluto/xine.conf.avwizard"

GetAudioMixerSettings()
{
	local Settings
	
	Settings="$(/usr/pluto/bin/SoundCards_AudioVolume.sh get-unprocessed | awk -f "$AudioMixerSettingsAwk")"
	eval "$Settings"
}

Resolution_GetFullName()
{
	local Prefix="$1"
	local Idx Resolution_Array FullName

	Resolution_Array=("${Resolutions_VESA[@]}" "${Resolutions_HDTV[@]}")
	
	Idx=$(FindInArray_Prefix "$Prefix" "${Resolution_Array[@]}")
	if [[ -n "$Idx" ]]; then
		FullName="${Resolution_Array[$Idx]}"
	fi
	echo "$FullName"
}

Resolution_Get()
{
	local Ratio="$1" Resolution="$2" Op="$3"
	local Idx Resolution_Array

	case "$Ratio" in
		4_3) Resolution_Array=("${Resolutions_VESA[@]}") ;;
		16_9) Resolution_Array=("${Resolutions_HDTV[@]}") ;;
	esac
	
	Idx=$(FindInArray_Prefix "$Resolution" "${Resolution_Array[@]}")
	[[ -z "$Idx" ]] && Idx=0

	case "$Op" in
		+) [[ "$Idx" -lt "${#Resolution_Array[*]}" ]] && ((Idx++)) ;;
		-) [[ "$Idx" -gt 0 ]] && ((Idx--)) ;;
	esac
	echo "${Resolution_Array[$Idx]}"
}

Resolution_Next()
{
	local Ratio="$1" Resolution="$2"

	Resolution_Get "$Ratio" "$Resolution" +
}

Resolution_Prev()
{
	local Ratio="$1" Resolution="$2"

	Resolution_Get "$Ratio" "$Resolution" -
}

Refresh_Get()
{
	local Refresh="$1" Op="$2"
	local Idx

	Idx=$(FindInArray "$Refresh" "${Refresh_Rates[@]}")
	[[ -z "$Idx" ]] && Idx=0

	case "$Op" in
		+) [[ "$Idx" -lt "${#Refresh_Rates[@]}" ]] && ((Idx++)) ;;
		-) [[ "$Idx" -gt 0 ]] && ((Idx--)) ;;
	esac
	echo "${Refresh_Rates[$Idx]}"
}

Refresh_Next()
{
	local Refresh="$1"

	Refresh_Get "$Refresh" +
}

Refresh_Prev()
{
	local Refresh="$1"

	Refresh_Get "$Refresh" -
}

StartX()
{
	local Parm XParm
	XParm=()

	for Parm in "$@"; do
		XParm=("${XParm[@]}" -parm "$Parm")
	done
	"$BaseDir"/Start_X.sh -fg -client /usr/pluto/bin/AVWizard -srvparm '-config' -srvparm "$XF86Config" -display ":$((Display+1))" "${XParm[@]}" -flags noreconf
	#"$BaseDir"/Start_X.sh -fg -client /usr/bin/valgrind -parm --tool=memcheck -parm --leak-check=yes -parm --show-reachable=yes -parm --num-callers=15 -parm /usr/pluto/bin/AVWizard -srvparm '-config' -srvparm "$XF86Config" -display $((Display+1)) "${XParm[@]}"
}

WizSet()
{
	local Var="$1" Value="$2"
	
	[[ -z "$Var" ]] && return 1
	"$Wiz" -set "$Var" "$Value" 2>/dev/null
}

WizGet()
{
	local Var="$1"
	
	[[ -z "$Var" ]] && return 1
	"$Wiz" -get "$Var" 2>/dev/null
}

SpcField()
{
	local Field="$1" Data="$2"

	echo "$Data" | cut -d' ' -f"$Field"
}
