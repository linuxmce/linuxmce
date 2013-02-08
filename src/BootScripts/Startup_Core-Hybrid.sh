#!/bin/bash

service mysql start

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_Generic_PC_as_MD="28"
DEVICETEMPLATE_Orbiter="62"

if [[ -f /etc/pluto/install_cleandb ]]; then
	. /usr/pluto/bin/SQL_Ops.sh

	chvt 8
	whiptail --infobox "Restoring settings.\nPlease wait. This may take a while." 10 0 >/dev/tty8

	# Reconfigure all devices after install
	Q="UPDATE Device SET NeedConfigure=1, Status='**RUN_CONFIG**'"
	RunSQL "$Q"

	# Install the software again for the devices
	Q="DELETE FROM Package_Device"
	RunSQL "$Q"

	/usr/pluto/bin/Diskless_Setup.sh
	/usr/pluto/bin/DHCP_config.sh

	if [[ -d /.upgrade-diskless ]]; then
		pushd /.upgrade-diskless &>/dev/null
		while read MD; do
			Dir="/usr/pluto/diskless/$MD/etc"
			if [[ -d "$Dir" ]]; then
				mv "$MD"/etc/pluto.conf "$Dir"
			fi
		done < <(find . -mindepth 1 -maxdepth 1 -type d)
		popd &>/dev/null
		rm -rf /.upgrade-diskless
	fi

	rm -f /etc/pluto/install_cleandb
	clear >/dev/tty8
	chvt 1
fi

USE_WHIPTAIL=Use_Whiptail /usr/pluto/bin/PlutoRestorePoint.sh --restore

## Setup ALSA mixers
amixer sset Capture 90%
#amixer sset 'Mic Boost (+20dB)' unmute
alsactl store

modprobe ztdummy

export DISPLAY=:${Display}
if [ -r /var/run/plutoX${Display}.pid ]
then
	XPID=$(</var/run/plutoX${Display}.pid)
else
	XPID=""
fi
if [ -z "$XPID" -o ! -d /proc/"$XPID" ]
then
	/usr/pluto/bin/Start_X.sh -config /etc/X11/xorg.conf
fi

export KDE_DEBUG=1
if [ -x /usr/pluto/bin/lmce_launch_manager.sh ]
then
	/usr/pluto/bin/lmce_launch_manager.sh
else
	HybridID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Generic_PC_as_MD" norecursion)
	OrbiterID=$(FindDevice_Template "$HybridID" "$DEVICETEMPLATE_Orbiter" norecursion)
	/usr/bin/screen -d -m -S OnScreen_Orbiter-${OrbiterID} /usr/pluto/bin/Spawn_Device.sh $OrbiterID $DCERouter LaunchOrbiter.sh
fi

