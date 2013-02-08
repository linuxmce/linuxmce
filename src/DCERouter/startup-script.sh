#!/bin/bash

LogFile="/var/log/startup-script.log"
exec &> >(tee $LogFile)


DisplayProgress() {
	Text=$1
	echo "${Progress}% ${Text}"
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
rm -f /var/run/plutoX*.pid		 # clean up x11 locks
rm -f /mnt/optical/*.checksum
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


StartService "Starting MySQL Server" "/etc/init.d/mysql start"
RunSQL "UPDATE Orbiter set Regen=2,RegenInProgress=0 where RegenInProgress=1"

StartService "Starting DHCP Server" "/etc/init.d/dhcp3-server start"
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
	#StartService "Updating Software Database" "/usr/pluto/bin/getxmls" "&"
	/usr/pluto/bin/getxmls &> /var/log/pluto/add_software_debug.log &
	ConfSet "FirstBoot" "false" 2>/dev/null 1>/dev/null
fi

StartService "Setting SSH Keys" "/usr/pluto/bin/SSH_Keys.sh" "&"
StartService "Confirm Installation" "/usr/pluto/bin/ConfirmInstallation.sh"
StartService "Setting Coredump Location" "/usr/pluto/bin/corefile.sh"
StartDaemon  "Starting Dhcp Plugin" "/usr/pluto/bin/Dhcpd-Plugin.sh" "DhcpdPlugin"
StartDaemon  "Start PNP MD Plugin" "/usr/pluto/bin/Start_NewMD_interactor.sh" "NewMDinteractor"
StartDaemon  "Start Voicemail Monitor" "/usr/pluto/bin/VoiceMailMonitor.sh" "VoiceMailMonitor"
StartService "Creating Firewire 2 Video4Linux Pipes" "/usr/pluto/bin/Firewire2Video4Linux.sh"
StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh"
/etc/init.d/samba start || /etc/init.d/smbd start
# StartService "Starting UPNP Server" "/usr/pluto/bin/StorageDevices_UPNPServer.sh" "&"
# StartService "Samba Server Detection" "/usr/pluto/bin/StorageDevices_SambaRadar.sh"
StartService "Detecting Timezone" "/usr/pluto/bin/Timezone_Detect.sh" "&"
StartService "Status Radar" "/usr/pluto/bin/StorageDevices_StatusRadar.sh"
StartService "Samba Radar" "/usr/pluto/bin/StorageDevices_SambaRadar.sh"
StartService "PVR-250 tuner restore" "/usr/pluto/bin/CaptureCards_BootConfig_PVR-250.sh"
