#!/bin/bash
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

if [ "$AutostartMedia" -ne "1" ] ; then
	# We don't want to start X, if we don't need the media director
	exit 0
fi

function assureXorgSane()
{
	xorgLines=$(cat ${XOrgConf} | wc -l)
	if [ $xorgLines -le 15 ] ;then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "${XOrgConf} has only $xorgLines lines, rebuilding"
		EVICETEMPLATE_OnScreen_Orbiter=62
		DEVICETEMPLATE_OrbiterPlugin=12
		DEVICECATEGORY_Media_Director=8
		DEVICEDATA_ScreenWidth=100
		DEVICEDATA_ScreenHeight=101
		DEVICEDATA_PK_Size=25
		DEVICEDATA_Video_settings=89
		DEVICEDATA_Spacing=150
		DEVICEDATA_Offset=167

		. /usr/pluto/bin/Utils.sh
		. /usr/pluto/bin/SQL_Ops.sh

		ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')

		Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings' LIMIT 1"
		VideoSetting=$(RunSQL "$Q")
		VideoSetting=$(Field "1" "$VideoSetting")
		
		refresh=$(echo $VideoSetting | cut -d '/' -f2)
		resolution=$(echo $VideoSetting | cut -d '/' -f1)
		width=$(echo $resolution | cut -d' ' -f1)
		height=$(echo $resolution | cut -d' ' -f2)

		bash -x /usr/pluto/bin/Xconfigure.sh --defaults --resolution "${width}x${height}@${refresh}" | tee-pluto /var/log/pluto/Xconfigure.log
	fi
}

AlphaBlending=$(AlphaBlendingEnabled)

#XClient=/usr/pluto/bin/Start_IceWM.sh
if [[ -e /usr/bin/xfwm4 ]] ;then 
	XClient=/usr/bin/xfwm4
else 
	XClient=/usr/bin/kwin
fi
XClientParm=()
XOrgConf="/etc/X11/xorg.conf"

XServerParm=(-logverbose 9 -br)
Background=y
XDisplay=":$Display"

Xcompmgr=/bin/true
if [[ "$AlphaBlending" != 1 && "$XClient" != "/usr/bin/kwin" ]]; then
	XClientParm=(--compositor=off)
fi

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		-client) ((i++)); XClient="${!i}" ;;
		-parm) ((i++)); XClientParm=("${XClientParm[@]}" "${!i}") ;;
		-fg) Background=n ;;
		-srvparm) ((i++)); XServerParm=("${XServerParm[@]}" "${!i}") ;;
		-display) ((i++)); XDisplay="${!i}" ;;
		-flags) ((i++)); WrapperFlags=("${WrapperFlags[@]}" "${!i}") ;;
		-config) ((i++)); XOrgConf=${!i} ;;
	esac
done

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server (client: $XClient; parms: ${XClientParm[*]})"

VT=${XDisplay#:}
VT=vt"$((7+VT))"

# Start X11
ConfGet "PK_Distro"
if [[ 19 -eq "$PK_Distro" ]] ;then
	# Raspbian does not get an xorg.conf
	Xcmd=(/usr/pluto/bin/Start_X_Wrapper.sh --parms "$@" --client "$XClient" "${XClientParm[@]}" --server "$XDisplay" -ignoreABI -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}" --flags "${WrapperFlags[@]}")
else
	Xcmd=(/usr/pluto/bin/Start_X_Wrapper.sh --parms "$@" --client "$XClient" "${XClientParm[@]}" --server "$XDisplay" -ignoreABI -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}" --flags "${WrapperFlags[@]}" -config "${XOrgConf}")
fi
if [[ "$Background" == y ]]; then
	screen -d -m -S XWindowSystem "${Xcmd[@]}"
	# Start everouter for gyration mouse
	#if [[ -x /usr/pluto/bin/StartGyrationEvrouter.sh ]]; then
	#	screen -d -m -S GyrationMouse /usr/pluto/bin/StartGyrationEvrouter.sh
	#fi
	sleep 1

	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "X server: backround; AlphaBlending: $AlphaBlending"
	if [[ "$AlphaBlending" == 1 ]]; then
		DISPLAY=:0 "$Xcompmgr" &>/dev/null </dev/null &
		disown -a
	fi
else
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "X server: foreground"
	"${Xcmd[@]}"
fi
/usr/pluto/bin/SetupAudioVideo.sh
