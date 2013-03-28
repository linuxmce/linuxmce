#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

DisplayProgress() {
	Text=$1
	[[ -z "$(pidof mce-launcher)" ]] || echo "${Progress}%${Text}" > /tmp/mce-launcher-pipe
}

StartService() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceBkg=$3
	Progress=$(( $Progress + $ProgressUnit ))


	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		DisplayProgress "$ServiceDescription"
		echo -n "$ServiceDescription ..."
		$ServiceCmd $ServiceBkg
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
	Progress=$(( $Progress + $ProgressUnit ))

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		DisplayProgress "$ServiceDescription"
		echo -n "$ServiceDescription ..."
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

## Permanent lock: it is never unlocked
## Only one MCE Launcher should be run per session (i.e. after the machine is powered)
if ! Lock "MCE_Launcher" "MCE_Launcher" nolog; then
	echo "MCE Launcher has already been started this session"
	exit 0
fi

ThisFile=$0
Progress=0
ProgressChunks=$(grep "^ *StartService \|^ *StartDaemon " $ThisFile | wc -l)
ProgressUnit=$(( 100 / $ProgressChunks ))

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf 2>/dev/null    # ensure access rights on pluto.conf
rm /var/log/pluto/running.pids 2>/dev/null
chmod 777 /var/log/pluto 2>/dev/null
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks
rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}
#rm /etc/modprobe.d/lrm-video 


if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	 . /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/pluto.func ]] ;then
	. /usr/pluto/bin/pluto.func
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
fi


export DISPLAY=":${Display}"

StartService "Starting MySQL Server" "/etc/init.d/mysql start"
StartService "Starting DHCP Server" "/etc/init.d/isc-dhcp-server start"
StartService "Starting DNS Server" "/etc/init.d/bind9 start"
StartService "Configuring Network Firewall" "/usr/pluto/bin/Network_Firewall.sh"
StartService "Loading Kernel Modules" "/usr/pluto/bin/LoadModules.sh"
StartService "Reporting Machine Status" "/usr/pluto/bin/Report_Machine_Status.sh" "&"

if [[ -f /usr/pluto/bin/SQL_Ops.sh  && -f /usr/pluto/bin/Config_Ops.sh ]] ;then
	Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
	R="$(RunSQL "$Q")"
	ConfSet PK_Installation "$R"

	Q="SELECT PK_Users FROM Users LIMIT 1"
	R="$(RunSQL "$Q")"
	ConfSet PK_Users "$R"
fi


if [[ "$FirstBoot" != "false" && ! -f /usr/pluto/install/.notdone ]] ;then
	StartService "Updating Software Database" "/usr/pluto/bin/getxmls"
	ConfSet "FirstBoot" "false" 2>/dev/null 1>/dev/null
fi

StartService "Setting SSH Keys" "/usr/pluto/bin/SSH_Keys.sh" "&"
StartService "Confirm Installation" "/usr/pluto/bin/ConfirmInstallation.sh"
StartService "Applying Updates" "/usr/pluto/bin/ApplyUpdates.sh"
StartService "Starting DCE Router" "/usr/pluto/bin/Start_DCERouter.sh"
StartService "Configure Device Changes" "/usr/pluto/bin/Config_Device_Changes.sh"
StartService "Setting Coredump Location" "/usr/pluto/bin/corefile.sh"

if [[ "$AVWizardDone" != "1" ]] ;then
	StartService "Starting Audio/Video Wizard" "/usr/pluto/bin/AVWizard_Run.sh"
fi

StartService "Generating Orbiter Screens" "/usr/pluto/bin/Start_OrbiterGen.sh"
StartDaemon  "Starting Dhcp Plugin" "/usr/pluto/bin/Dhcpd-Plugin.sh" "DhcpdPlugin"
StartDaemon  "Start PNP MD Plugin" "/usr/pluto/bin/Start_NewMD_interactor.sh" "NewMDinteractor"
StartService "Creating Firewire 2 Video4Linux Pipes" "/usr/pluto/bin/Firewire2Video4Linux.sh"
StartService "Starting Local Devices" "/usr/pluto/bin/Start_LocalDevices.sh"
StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh" "&"
StartService "Samba Server Detection" "/usr/pluto/bin/StorageDevices_SambaRadar.sh"
StartDaemon  "Starting Update Media Daemon" "/usr/pluto/bin/UpdateMediaDaemon.sh" "UpdateMedia"
StartService "Detecting Timezone" "/usr/pluto/bin/Pluto_Timezone_Detect.sh" "&"

if [[ -d /home/backup/upload ]]; then
	StartService "Searching System for Restore Points" "/usr/pluto/bin/PlutoRestorePoint.sh --restore"
	apt-get update 1>/dev/null 2>/dev/null
	apt-get -f install --reinstall pluto-local-database 1>/dev/null 2>/dev/null
fi

echo "100%Done" > /tmp/mce-launcher-pipe
sleep 2
