#!/bin/bash

## Includes
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server
. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1
. /usr/pluto/bin/Utils.sh 2>/dev/null || exit 1
. /usr/pluto/bin/pluto.func 2>/dev/null || exit 1

## Read command line params
Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); DeviceID="${Params[$i]}" ;;
        esac
done

## SDL Exports 
export SDL_VIDEO_X11_WMCLASS="Orbiter"
export SDL_MOUSE_RELATIVE=0

## Check to see if we use OpenGL efects or not
DD_USE_OPENGL_EFECTS=172
Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$DeviceID' AND FK_DeviceData=${DD_USE_OPENGL_EFECTS}"
UseOpenGL=$(RunSQL "$Q")

Executable=./Orbiter
if [[ "$UseOpenGL" == "1"  ]]; then
	Executable=./OrbiterGL
fi

killall WatchGyroRemote  # In case it was running it will be grabbing the hid device

export DISPLAY=:$Display

/usr/bin/xset -dpms s off
N_Desktops=$(wmctrl -d | wc -l) # zero based

Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Number of desktops found: $N_Desktops ; desktops activated? $DesktopActivated"

## Increase number of desktops by 2
if [[ "$DesktopActivated" != 1 ]]; then
	Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Increasing number of desktops to: $((N_Desktops + 2))"
	for ((i = 0; i < 5; i++)); do
		wmctrl -n $((N_Desktops + 2))
		N_Desktops_Result=$(wmctrl -d | wc -l)
		if ((N_Desktops_Result == N_Desktops + 2)); then
			ConfSet DesktopActivated 1
			break
		fi
		sleep 1
	done
	if [[ "$DesktopActivated" != 1 ]]; then
		Logging $TYPE $SEVERITY_WARNING "LaunchOrbiter" "Failed to increase number of desktops"
		exit 2
	fi
elif [[ "$N_Desktops" -lt 4 ]]; then
	Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Number of desktops below 4. Setting to 4"
	for ((i = 0; i < 5; i++)); do
		wmctrl -n 4
		N_Desktops_Result=$(wmctrl -d | wc -l)
		if ((N_Desktops_Result == 4)); then
			break
		fi
		sleep 1
	done
	if ((N_Desktops_Result != 4)); then
		Logging $TYPE $SEVERITY_WARNING "LaunchOrbiter" "Failed to increase number of desktops"
		exit 2
	fi
	N_Desktops=2
else
	((N_Desktops -= 2))
fi

Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Primary desktop: $N_Desktops and secondary desktop $((N_Desktops + 1))"


## Prevent kde to display a message every time a cd/dvd is inserted
mkdir -p /usr/share/kubuntu-default-settings/kde-profile/default/share/config

echo "[Global]" > /usr/share/kubuntu-default-settings/kde-profile/default/share/config/mediamanagerrc
echo "AutostartEnabled=false" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/mediamanagerrc

echo "[Auto Actions]"  > /usr/share/kubuntu-default-settings/kde-profile/default/share/config/medianotifierrc
for file in /usr/share/mimelnk/media/*  ;do 
	echo "media/$(basename $file | cut -d '.' -f1)=#NothinAction" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/medianotifierrc  
done


## Export Orbiter desktop information variables
export ORBITER_PRIMARY_DESKTOP=$((N_Desktops))
export ORBITER_SECONDARY_DESKTOP=$((N_Desktops + 1))

Counter=0
isIceWmRunning=0
while [[ "$isIceWmRunning" -eq 0 ]]; do
	isIceWmRunning=$(ps -A | egrep 'xfwm4|icewm' | wc -l)
	if [[ "$isIceWmRunning" -eq 0 ]]; then
		((Counter++))
		if [[ "$Counter" -gt 20 ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "LaunchOrbiter" "xfwm4 failed to start"
			break
		fi
		sleep 1
	fi
done


## Run Orbiter
xset m 2 2 

if [[ "$Executable" == *"$Valgrind"* ]]; then
	$VGcmd "$Executable" "$@"
	Orbiter_RetCode=$?
else
	"$Executable" "$@"
	Orbiter_RetCode=$?
fi

exit ${Orbiter_RetCode}
