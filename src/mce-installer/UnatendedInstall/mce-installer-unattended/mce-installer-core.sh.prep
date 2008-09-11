#!/bin/bash

echo > /var/log/messages
echo > /var/log/syslog
dhclient eth0
ntpdate ntp.ubuntu.com

. ./mce-installer-common.sh

trap 'RemoveOfflineSource' EXIT

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%s).log"
exec &> >(tee $InstallerLogFile)

echo "--- start mce wizard data ---" 
cat /tmp/mce_wizard_data.sh 
echo "--- end mce wizard data ---"

function FakeUpdates {
	mkdir -p /home/update
#	for i in `seq 1 4` ;do
#		mkdir -p "/home/updates/$i"
#		touch "/home/updates/$i/update.xml"
#	done
}

function ExitInstaller {
	echo 
	echo "ERROR : $*" >&2
	echo
	echo "$*" > /tmp/mce_installer_error
	exit 1
}


function StatsMessage {
	echo "MESSAGE: $*"
}

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
	ExitInstaller "Wizard Information is corrupted or missing."
fi
. /tmp/mce_wizard_data.sh

function Setup_Network_Intefaces {
	StatsMessage "Seting Up Networking"
	# If user is was using dhcp and now wants static ip is safe to kill this here
	killall dhclient  &>/dev/null
	killall dhclient3 &>/dev/null

	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "iface $c_netExtName inet dhcp" > /etc/network/interfaces
		dhclient $c_netExtName || ExitInstaller "Failed to get an IP address for the external nic via DHCP."
	else
		echo > /etc/network/interfaces
		echo "auto lo" >> /etc/network/interfaces
		echo "iface lo inet loopback" >> /etc/network/interfaces

		if [[ "$c_netExtIP" != "" ]] && [[ "$c_netExtName" != "" ]] &&
		   [[ "$c_netExtMask" != "" ]] && [[ "$c_netExtGateway" != "" ]] ;then
			echo "" >> /etc/network/interfaces
			echo "auto $c_netExtName" >> /etc/network/interfaces
			echo "iface $c_netExtName inet static" >> /etc/network/interfaces
			echo "address $c_netExtIP" >> /etc/network/interfaces
			echo "netmask $c_netExtMask" >> /etc/network/interfaces
			echo "gateway $c_netExtGateway" >> /etc/network/interfaces
		fi

		echo > /etc/resolv.conf
		echo "nameserver $c_netExtDNS1" >> /etc/resolv.conf


		ifconfig $c_netIntName down
		ifconfig $c_netExtName down
		ifconfig $c_netExtName $c_netExtIP netmask $c_netExtMask up || ExitInstaller "Cannot set an IP address to '$c_netExtName'."

		route del default gw 2>/dev/null
		route add default gw $c_netExtGateway 2>/dev/null || ExitInstaller "Cannot setup default gateway to '$c_netExtGateway'."
	fi

}

function Setup_Pluto_Conf {
	if [[ "$c_startupType" == 1 ]]; then
		AutostartCore=1
		AutostartMedia=1
	else
		AutostartCore=0
		AutostartMedia=0
	fi
	StatsMessage "Generating Default Config File"
	PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = 1
Activation_Code = 1111
PK_Installation = 1
PK_Users = 1
PK_Distro = 15
Display = 0
SharedDesktop = 1
OfflineMode = false
#<-mkr_b_videowizard_b->
UseVideoWizard = 1
#<-mkr_b_videowizard_e->
LogLevels = 1,5,7,8
#ImmediatelyFlushLog = 1
AutostartCore=$AutostartCore
AutostartMedia=$AutostartMedia
"
	echo "$PlutoConf" > /etc/pluto.conf
	chmod 777 /etc/pluto.conf &>/dev/null
}

function Install_DCERouter {
	StatsMessage "Installing MySQL Server"
	apt-get update
	apt-get -y -f install mysql-server || ExitInstaller "Failed to install mysql server"
	invoke-rc.d mysql start # Because of this : https://bugs.launchpad.net/bugs/107224

	apt-get -y -f install pluto-sample-media
	apt-get -y -f install video-wizard-videos
	apt-get -y -f install pluto-mysql-wrapper
	apt-get -y -f install pluto-default-tftpboot

	StatsMessage "Installing LinuxMCE Base Software"
	apt-get -y -f install pluto-dcerouter || ExitInstaller "Failed to install and configure the base software"
}

function Create_And_Config_Devices {

	/etc/init.d/mysql stop
	killall -9 mysqld_safe
	/etc/init.d/mysql start
	sleep 30

	. /usr/pluto/bin/SQL_Ops.sh

	DEVICE_TEMPLATE_Core=7
	DEVICE_TEMPLATE_MediaDirector=28

	## Update the 'LastUpdate' Device Data on the templates
#	RunSQL "UPDATE DeviceTemplate_DeviceData SET IK_DeviceData=1 WHERE FK_DeviceTemplate = 7 AND FK_DeviceData=234"
#	RunSQL "UPDATE DeviceTemplate_DeviceData SET IK_DeviceData=2 WHERE FK_DeviceTemplate = 28 AND FK_DeviceData=234"
#	RunSQL "UPDATE DeviceTemplate_DeviceData SET IK_DeviceData=3 WHERE FK_DeviceTemplate = 1893 AND FK_DeviceData=234"

	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '1111')"
	RunSQL "$Q"

	## Create the Core device and set it's description
	StatsMessage "Setting up your computer to act as a 'Core'"
	Core_PK_Device=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"

	## Create a hybrid if needed
	if [[ "$c_deviceType" == "2" ]] ;then
		StatsMessage "Setting up you computer to act as a 'Media Director'"
		/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
		Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")

		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
		RunSQL "$Q"
 
		## Set UI interface
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Hybrid_DT' AND FK_DeviceTemplate=62"
		OrbiterDevice=$(RunSQL "$Q")

		case "$c_installUI" in
			"0")
				# select UI1
				UI_SetOptions "$OrbiterDevice" 0 0 1
			;;
			"1")
				# select UI2 without alpha blending
				UI_SetOptions "$OrbiterDevice" 1 0 4
			;;
			"2")
				# select UI2 with alpha blending
				UI_SetOptions "$OrbiterDevice" 1 1 4
			;;
			*)
				echo "Unknown UI version: $c_installUI"
			;;
		esac
	fi

	StatsMessage "Configuring the LinuxMCE devices"
        # "DCERouter postinstall"
        devices=$(echo "SELECT PK_Device FROM Device;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

	for i in $devices; do
		echo "Running device $i"

		#run the following with the device
		# Add missing data by template
		Q1="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceTemplate_DeviceData.FK_DeviceData,DeviceTemplate_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceTemplate_DeviceData on DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceTemplate_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for the category
		Q2="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceTemplate.FK_DeviceCategory
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for parent's category
		Q3="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		(echo "$Q1"; echo "$Q2"; echo "$Q3";) | /usr/bin/mysql pluto_main
	done

	/usr/pluto/bin/Update_StartupScrips.sh
}

function Configure_Network_Options {
	StatsMessage "Configuring your internal network"
	. /usr/pluto/bin/SQL_Ops.sh

	## Setup /etc/hosts
	echo > /etc/hosts
	echo "127.0.0.1 localhost.localdomain localhost" >> /etc/hosts
	echo "$c_netExtIP dcerouter $(/bin/hostname)"    >> /etc/hosts

	error=false
	Network=""
	Digits_Count=0

	for Digits in $(echo "$c_netIntIPN" | tr '.' ' ') ;do
		[[ "$Digits" == *[^0-9]* ]]            && error=true
		[[ $Digits -lt 0 || $Digits -gt 255 ]] && error=true

		if [[ "$Network" == "" ]] ;then
			Network="$Digits"
		else
			Network="${Network}.${Digits}"
		fi

		Digits_Count=$(( $Digits_Count + 1 ))
	done
	[[ $Digits_Count -lt 1 || $Digits_Count -gt 3 ]] && error=true

	if [[ "$error" == "true" ]] ;then
		Network="192.168.80"
		Digits_Count="3"
	fi

	IntIP="$Network"
	IntNetmask=""
	for i in `seq 1 $Digits_Count` ;do
		if [[ "$IntNetmask" == "" ]] ;then
			IntNetmask="255"
		else
			IntNetmask="${IntNetmask}.255"
		fi
	done
	for i in `seq $Digits_Count 3` ;do
		if [[ $i == "3" ]] ;then
			IntIP="${IntIP}.1"
		else
			IntIP="${IntIP}.0"
		fi

		IntNetmask="${IntNetmask}.0"
	done

	if [[ "$c_netIntName" == "" ]] ;then
		IntIf="$c_netExtName:0"
	else
		IntIf="$c_netIntName"
	fi

	if [[ "$c_netExtUseDhcp" == "0" ]] ;then
		NETsetting="$c_netExtName,$c_netExtIP,$c_netExtMask,$c_netExtGateway,$c_netExtDNS1|$IntIf,$IntIP,$IntNetmask"
	else
		NETsetting="$c_netExtName,dhcp|$IntIf,$IntIP,$IntNetmask"
	fi
	
	DHCPsetting=$(/usr/pluto/install/Initial_DHCP_Config.sh "$Network" "$Digits_Count")

	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$Core_PK_Device',32,'$NETsetting')"
	RunSQL "$Q"
	if [[ "$c_runDhcpServer" == "1" ]]; then
		Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
			VALUES($Core_PK_Device, 28, '$DHCPsetting')"
		RunSQL "$Q"
	fi

}

function RemoveOfflineSource {

	#if [ -e  /etc/apt/apt.conf.d/00ubuntu_offline ] ;then
	#	rm -f /etc/apt/apt.conf.d/00ubuntu_offline
	#fi
	mv /etc/apt/sources.list.original /etc/apt/sources.list
}


Core_PK_Device="0"
if [[ "$c_netExtKeep" != "true" ]] ;then
	Setup_Network_Intefaces
fi
Setup_Apt_Conffiles
FakeUpdates
Setup_Pluto_Conf
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options
Setup_XOrg

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Timezone_Detect.sh
/usr/pluto/bin/Network_Setup.sh

#StatsMessage "Building a disk image for your Diskless Media Directors"
/usr/pluto/bin/Diskless_CreateTBZ.sh

mkdir -p /usr/pluto/deb-cache

if [[ "$c_startupType" == "1" ]] ;then
	mv /etc/init.d/gdm /etc/init.d/gdm.saved
	mv /etc/init.d/kdm /etc/init.d/kdm.saved
	echo '#!/bin/bash
if [[ "$1" == start ]]; then
	/usr/pluto/bin/Startup_Core-Hybrid.sh
fi
' > /etc/init.d/kdm
chmod +x /etc/init.d/kdm
mv /etc/rc2.d/*kdm /etc/rc2.d/S99kdm
	
#elif [[ "$c_startupType" == "2" ]] ;then
#	echo "
#start on runlevel 2
#
#stop on shutdown
#stop on runlevel 3
#stop on runlevel 4
#stop on runlevel 5
#
#script
#/usr/share/mce-launcher/scripts/mce-launcher-core.sh
#end script
#" > /etc/event.d/pluto
fi

echo "
start on runlevel 2

stop on shutdown
stop on runlevel 3
stop on runlevel 4
stop on runlevel 5

script
screen -d -m -S DhcpdPlugin /usr/pluto/bin/Dhcpd-Plugin.sh
end script
" > /etc/event.d/pluto-dhcpd-plugin

## Remove 'Install LinuxMCE' icon after LinuxMCE was installed
for dir in /home/* ;do
	if [ -d "$dir/Desktop" ] ;then
		rm -f "$dir/Desktop/mce-installer.desktop"
	fi
done

#Install libdvdcss
#apt-get -f -y install  libdvdread3
#/usr/share/doc/libdvdread3/install-css.sh


#Remove everything from the Installation table
RunSQL "UPDATE Installation SET PK_Installation = 1"
RunSQL "UPDATE Installation SET Description = 'LinuxMCE DVD Build $(date +%d-%m-%y)'"
RunSQL "UPDATE Installation SET Name = NULL" 
RunSQL "UPDATE Installation SET Address = NULL" 
RunSQL "UPDATE Installation SET City = NULL" 
RunSQL "UPDATE Installation SET State = NULL" 
RunSQL "UPDATE Installation SET Zip = NULL" 
RunSQL "UPDATE Installation SET FK_Country = 0" 
RunSQL "UPDATE Installation SET FK_City = 0" 
RunSQL "UPDATE Installation SET FK_PostalCode = 0" 


#if [[ "$c_ubuntuExtraCDFrom" == "3" ]] ;then
#	apt-get -y dist-upgrade || ExitInstaller "Failed while upgrading the system. Installation finished but you system might be left in a unstable state."
#fi

apt-get -y -f dist-upgrade

./NuForce.sh

StatsMessage "Installation Finished"
init 0
