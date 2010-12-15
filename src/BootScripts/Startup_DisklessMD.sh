#!/bin/bash

DEVICETEMPLATE_Orbiter="62"

Message() {
    echo -en "\033[1m# $*"
    tput sgr0
}

StartService() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceBkg=$3

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		echo -n "$ServiceDescription ... "
		if [[ "$ServiceBkg" == "&" ]] ;then
			$ServiceCmd 1>/dev/null 2>/dev/null &
		else
			$ServiceCmd 1>/dev/null 2>/dev/null	
		fi
		err=$?

		if [[ "$err" == "0" ]] ;then
			echo "ok"
		else
			echo "fail"
		fi
	fi

	return $err
}

StartDaemon() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceScreen=$3

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		echo -n "$ServiceDescription ... "
		screen -d -m -S "$ServiceScreen" $ServiceCmd 
		err=$?

		if [[ "$err" == "0" ]] ;then
			echo "ok"
		else
			echo "fail"
		fi
	fi

	return $err
}

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf 2>/dev/null    # ensure access rights on pluto.conf
rm /var/log/pluto/running.pids 2>/dev/null
chmod 777 /var/log/pluto 2>/dev/null
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks
rm -f /var/run/plutoX*.pid		 # clean up x11 locks
rm -f /mnt/optical/*.checksum
rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}
service nis start

## workaround for gutsy bug #139155
if [[ ! -f /var/cache/hald/fdi-cache ]] ;then
	/usr/lib/hal/hald-generate-fdi-cache
	service hal restart
fi

if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	 . /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/pluto.func ]] ;then
	. /usr/pluto/bin/pluto.func
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
fi
if [[ -f /usr/pluto/bin/Utils.sh ]] ;then
	. /usr/pluto/bin/Utils.sh
fi

## Assure that /home is mounted
for i in `seq 1 5` ;do
	if ! mountpoint /home ;then
		echo "$(date -R) /home is not mounted, retrying ($i of 5) ..." >> /var/log/pluto/umount-wrapper.log
		mount /home
		sleep $( echo "0.5 * $i" | bc  -l )
	else
		break
	fi
done

## Setup ALSA mixers
amixer sset Capture 90%
#amixer sset 'Mic Boost (+20dB)' unmute
alsactl store

export DISPLAY=":${Display}"

StartService "Loading Kernel Modules" "/usr/pluto/bin/LoadModules.sh"
if [[ -f /usr/pluto/bin/SQL_Ops.sh  && -f /usr/pluto/bin/Config_Ops.sh ]] ;then
	Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
	R="$(RunSQL "$Q")"
	ConfSet PK_Installation "$R"

	Q="SELECT PK_Users FROM Users LIMIT 1"
	R="$(RunSQL "$Q")"
	ConfSet PK_Users "$R"
fi

StartService "Enable Wake on LAN" "/usr/pluto/bin/enable_wol.sh"
StartService "Setting SSH Keys" "/usr/pluto/bin/SSH_Keys.sh" "&"
StartService "Configure Device Changes" "/usr/pluto/bin/Config_Device_Changes.sh"
StartService "Setting Coredump Location" "/usr/pluto/bin/corefile.sh"
StartService "Creating Firewire 2 Video4Linux Pipes" "/usr/pluto/bin/Firewire2Video4Linux.sh"

# hack: cleaning lockfile on M/D start to allow
# local devices to start
# TODO: remove this when correct locking will be implemented
rm -f /usr/pluto/locks/pluto_spawned_local_devices.txt

StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh" "&"
#StartDaemon "Report machine is on" "/usr/pluto/bin/Report_MachineOn.sh" "ReportingOn"
StartService "Status Radar" "/usr/pluto/bin/StorageDevices_StatusRadar.sh"
StartService "PVR-250 tuner restore" "/usr/pluto/bin/CaptureCards_BootConfig_PVR-250.sh"

. /usr/pluto/bin/Config_Ops.sh
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
	/usr/pluto/bin/StartHAL.sh
	OrbiterID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Orbiter" norecursion)
	/usr/bin/screen -d -m -S OnScreen_Orbiter-${OrbiterID} /usr/pluto/bin/Spawn_Device.sh $OrbiterID $DCERouter LaunchOrbiter.sh
fi

