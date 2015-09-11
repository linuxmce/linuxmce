#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh


DEVICEDATA_Diskless_Boot=9

IsDiskless=$(GetDeviceData "$PK_Device" "$DEVICEDATA_Diskless_Boot")

if [[ "$IsDiskless" == 1 ]]; then
	TheChosenOne=root
else
	Old_IFS="$IFS"
	IFS=:
	while read user x uid gid rest; do
		if ((uid < 1000 || uid >= 10000)); then
			continue
		fi
		TheChosenOne="$user"
		break
	done </etc/passwd
	IFS="$Old_IFS"
fi


if su - "$TheChosenOne" -c "DISPLAY=:$Display kcheckrunning"; then
	exit
fi

homedir=`getent passwd "$TheChosenOne" | cut -d : -f 6`

# Create a Desktop icon to activate orbiter 
mkdir -p "$homedir/Desktop" 
echo "[Desktop Entry] 
Encoding=UTF-8 
Version=8.10 
Type=Application 
Exec=/usr/pluto/bin/ActivateOrbiterFromKDE.sh 
Path=/usr/pluto/bin 
Name=Activate Orbiter" > "$homedir/Desktop/LinuxMCE.desktop"

# Disable the KDE Screensaver
mkdir -p "$homedir/.kde/share/config/"
echo "[ScreenSaver]
Enabled=false
Lock=false
LockGrace=60000
Saver=kblank.desktop
Timeout=900" > "$homedir/.kde/share/config/kscreensaverrc"

echo "[PlasmaViews][1]
Offset=0
panelVisibility=0

[PlasmaViews][2]
Offset=0
panelVisibility=2" > "$homedir/.kde/share/config/plasma-desktoprc"

echo "ShowApplicationStatus=false
ShowCommunications=false
ShowHardware=false
ShowJobs=false
ShowNotifications=false
ShowSystemServices=false" > "$homedir/.kde/share/config/plasma-desktop-appletsrc"

echo "[1]
Description=Settings for OrbiterRenderer_SDL
skiptaskbar=true
skiptaskbarrule=2
wmclass=OrbiterRenderer_SDL
wmclasscomplete=false
wmclassmatch=1


[2]
Description=Settings for Picture_Viewer
skiptaskbar=true
skiptaskbarrule=2
wmclass=Picture_Viewer
wmclasscomplete=false
wmclassmatch=1

[3]
Description=Settings for pluto-xine-playback-window
skiptaskbar=true
skiptaskbarrule=2
wmclass=pluto-xine-playback-window
wmclasscomplete=false
wmclassmatch=1

[4]
Description=(Default) Disable focus stealing prevention for XV
fsplevel=0
fsplevelrule=2
wmclass=^xv .*
wmclasscomplete=true
wmclassmatch=3

[Directories-default]
prefixes[$d]

[Emoticons]
emoticonsTheme[$d]

[General]
count=4" > "$homedir/.kde/share/config/kwinrulesrc"

chown -R "${TheChosenOne}:${TheChosenOne}" "${homedir}/.kde"
chown -R "${TheChosenOne}:${TheChosenOne}" "${homedir}/.config"
chown -R "${TheChosenOne}:${TheChosenOne}" "${homedir}/Desktop"

DISTRO="$(lsb_release -c -s)"
if [[ $DISTRO = "intrepid" ]] ; then
    su - "$TheChosenOne" -c "DISPLAY=:$Display /usr/pluto/bin/startkde-keepdcop" &>/dev/null </dev/null &
else
    su - "$TheChosenOne" -c "DISPLAY=:$Display /usr/bin/startkde" &>/dev/null </dev/null &
fi
disown -a
