#!/bin/bash

. /usr/pluto/install/install-common.sh	# lmce-install-scripts

if [[ -n "$HEADER_install_core" ]]; then
	return 0
fi
HEADER_install_core=included

###########################################################
### Setup Functions - Reference functions
###########################################################

Create_Wizard_Data-Double_Nic_Shell () {
	echo "c_deviceType=2 # 1-Core, 2-Hybrid, 3-DiskedMD
c_netIfaceNo=1
c_netExtName='{extif}'
c_netExtIP=''
c_netExtMask=''
c_netExtGateway=''
c_netExtDNS1=''
c_netExtDNS2=''
c_netExtUseDhcp=1 # 1 - Yes / 0 - No
c_runDhcpServer=1 # 1 - Yes / 0 - No
c_netIntName='{intif}'
c_netIntIPN='192.168.80'
c_startupType=1 #0 - Start Kde / 1 - Start LMCE
c_installType=1
c_installMirror='http://archive.ubuntu.com/ubuntu/'
c_netExtKeep='true'
c_installUI=0 # 0 - UI1, 1 - UI2M, 2 - UI2A
c_linuxmceCdFrom=1 # 1 - CD, 2 -ISO
c_linuxmceCdIsoPath='' 
c_ubuntuExtraCdFrom=1
c_ubuntuExtraCdPath=''
c_ubuntuLiveCdFrom=1
c_ubuntuLiveCdPath=''
"
}

Create_Wizard_Data-Single_Nic_Shell () {
echo "c_deviceType=2 # 1-Core, 2-Hybrid, 3-DiskedMD
c_netIfaceNo=1
c_netExtName='{extif}'
c_netExtIP='{extip}'
c_netExtMask='{extMask}'
c_netExtGateway='{extGW}'
c_netExtDNS1='{extDNS}'
c_netExtDNS2=''
c_netExtUseDhcp={extUseDhcp} # 1 - Yes / 0 - No
c_runDhcpServer={runDhcp} # 1 - Yes / 0 - No
c_netIntName='{extif}:1'
c_netIntIPN='192.168.80'
c_startupType=1 #0 - Start Kde / 1 - Start LMCE
c_installType=1
c_installMirror='http://archive.ubuntu.com/ubuntu/'
c_netExtKeep='true'
c_installUI=0 # 0 - UI1, 1 - UI2M, 2 - UI2A
c_linuxmceCdFrom=1 # 1 - CD, 2 -ISO
c_linuxmceCdIsoPath='' 
c_ubuntuExtraCdFrom=1
c_ubuntuExtraCdPath=''
c_ubuntuLiveCdFrom=1
c_ubuntuLiveCdPath=''
c_singleNIC=1
"
}


###########################################################
### Setup Functions - General functions
###########################################################

Nic_Config () {
	StatsMessage "Starting NIC Discovery and Configuration"
	# Find out, what nic configuration we have. This is needed for later on to fill the database
	# correctly.
	if  [[ `ifconfig -s -a  | awk '$1 != "Iface" && $1 != "lo" && $1 != "pan0" { print $1 }' | wc -l` > 1 ]]; then
		Create_Wizard_Data-Double_Nic_Shell > ${mce_wizard_data_shell}
		#Use these for the defaults if we cannot automatically determine which to use
		#TODO: Error out and instruct the user to setup a working connection? Or ask them to manually choose?
		extif="eth0"
		intif="eth1"
		if route -n | grep -q '^0.0.0.0'; then
			#We have a default route, use it for finding external interface.
			extif=`route -n | awk '$1 == "0.0.0.0" { print $8 }'`
			#Use the first available interface as the internal interface.
			for if in `ifconfig -s -a | awk '$1 != "Iface" && $1 != "lo"  && $1 != "pan0" { print $1 }'`
			do
				if [ "$if" != "$extif" ]
				then
					intif=$if
					break
				fi
			done
		fi
		echo "Using $extif for external interface"
		echo "Using $intif for internal interface"

		sed --in-place -e "s,\({extif}\),$extif,g" ${mce_wizard_data_shell}
		sed --in-place -e "s,\({intif}\),$intif,g" ${mce_wizard_data_shell}
	else
		extif=eth0
		if route -n | grep -q '^0.0.0.0'; then
			#We have a default route, use it for finding external interface.
			extif=`route -n | awk '$1 == "0.0.0.0" { print $8 }'`
		fi
		Create_Wizard_Data-Single_Nic_Shell > ${mce_wizard_data_shell}
		echo "Using $extif for single nic install"
		sed --in-place -e "s,\({extif}\),$extif,g" ${mce_wizard_data_shell}

		# set c_netExtIP and friends , as this is used in Configure_Network_Options (i.e. before Network_Setup...)
		extIP=$(ip addr | grep "$extif" | grep -m 1 'inet ' | awk '{print $2}' | cut -d/ -f1)
		sed --in-place -e "s,\({extip}\),$extIP,g" ${mce_wizard_data_shell}

		# Set use external DHCP and run own dhcp based on extifs current setting
		ExtUsesDhcp=$(grep "iface $extif " /etc/network/interfaces | grep -cF 'dhcp')
		if [[ $ExtUsesDhcp == 0 ]]; then
			# Not dhcp defined in config file, test if dhclient got us an IP
			# /var/run/dhcp for newer than 1004
			if [[ (`ls /var/lib/dhcp/dhclient-*-$extif.lease && [[ $? == 0 ]]` || -e /var/run/dhclient-$extif.pid) && `pgrep -c dhclient` == 1 ]]; then
				ExtUsesDhcp=1
			fi
		fi
		RunDHCP=0
		if [[ $ExtUsesDhcp == 0 ]]; then
			echo "$extif does not use DHCP, setting ExtUseDhcp=0 and RunDHCPServer=1 and detecting current network settings"
			RunDHCP=1

			ExtGateway=$(grep -A 10 "^iface $extif" /etc/network/interfaces | grep '^\s*gateway' -m 1 | grep -o  '[0-9.]*')
			ExtMask=$(grep -A 10 "^iface $extif" /etc/network/interfaces | grep '^\s*netmask' -m 1 | grep -o '[0-9.]*')
			ExtDNS=$(grep 'nameserver' /etc/resolv.conf | grep -o '[0-9.]*' -m 1)
		fi

		sed --in-place -e "s,\({extMask}\),$ExtMask,g" ${mce_wizard_data_shell}
		sed --in-place -e "s,\({extGW}\),$ExtGateway,g" ${mce_wizard_data_shell}
		sed --in-place -e "s,\({extDNS}\),$ExtDNS,g" ${mce_wizard_data_shell}

		sed --in-place -e "s,\({extUseDhcp}\),$ExtUsesDhcp,g" ${mce_wizard_data_shell}
		sed --in-place -e "s,\({runDhcp}\),$RunDHCP,g" ${mce_wizard_data_shell}
	fi

	if [[ ! -r ${mce_wizard_data_shell} ]]; then
		echo "`date` - Wizard Information is corrupted or missing."
		exit 1
	fi
	. ${mce_wizard_data_shell}
	VerifyExitCode "MCE Wizard Data"

	Core_PK_Device="0"

	#Setup the network interfaces
	echo > /etc/network/interfaces
	echo "auto lo" >> /etc/network/interfaces
	echo "iface lo inet loopback" >> /etc/network/interfaces
	echo >> /etc/network/interfaces
	echo "auto $c_netExtName" >> /etc/network/interfaces
	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "    iface $c_netExtName inet dhcp" >> /etc/network/interfaces
	else
		if [[ "$c_netExtIP" != "" ]] && [[ "$c_netExtName" != "" ]] &&
		   [[ "$c_netExtMask" != "" ]] && [[ "$c_netExtGateway" != "" ]] ;then
			echo "" >> /etc/network/interfaces
			echo "    iface $c_netExtName inet static" >> /etc/network/interfaces
			echo "    address $c_netExtIP" >> /etc/network/interfaces
			echo "    netmask $c_netExtMask" >> /etc/network/interfaces
			echo "    gateway $c_netExtGateway" >> /etc/network/interfaces
		fi
	fi
	echo "" >> /etc/network/interfaces
	echo "auto $c_netIntName" >> /etc/network/interfaces
	echo "    iface $c_netIntName inet static" >> /etc/network/interfaces
	echo "    address $c_netIntIPN" >> /etc/network/interfaces
	echo "    netmask 255.255.255.0" >> /etc/network/interfaces
}

Configure_Network_Options () {
	# Updating hosts file and the Device_Data for the core with the internal and external network
	# addresses - uses Initial_DHCP_Config.sh from the lmce-install-scripts package.
	StatsMessage "Configuring your internal network"
	#Source the SQL Ops file
	. /usr/pluto/bin/SQL_Ops.sh	# pluto-boot-scripts

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

	if [[ "$c_singleNIC" == "1" ]] ;then
		#Disable firewalls on single NIC operation, refs #396
		echo "We are in single NIC mode -> internal firewalls disabled"
		echo "DisableFirewall=1" >>/etc/pluto.conf
		echo "DisableIPv6Firewall=1" >>/etc/pluto.conf
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

	# create empty IPv6 tunnel settings field
	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$Core_PK_Device',292,'')"
	RunSQL "$Q"
}

#######################################################
#######################################################
#######################################################

Setup_Pluto_Conf () {
	StatsMessage "Seting Up MCE Configuration file"
	AutostartCore=1
	AutostartMedia=1

	case "$TARGET_RELEASE" in
		"intrepid")
			# select UI1
			PK_DISTRO=17
			;;
		"lucid")
			# select UI2 without alpha blending
			PK_DISTRO=18
			;;
		"precise")
			PK_DISTRO=20
			LTS_HES=-lts-trusty
			;;
		"trusty")
			PK_DISTRO=21
			LTS_HES=-lts-utopic
			;;
		"wheezy")
			PK_DISTRO=19
			;;
		"jessie")
			PK_DISTRO=22
			;;
	esac


	StatsMessage "Generating Default Config File"
	cat <<-EOF >/etc/pluto.conf
		# Pluto config file
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
		PK_Distro = $PK_DISTRO
		Display = 0
		SharedDesktop = 1
		OfflineMode = false
		#<-mkr_b_videowizard_b->
		UseVideoWizard = 1
		#<-mkr_b_videowizard_e->
		LogLevels = 1,5,7,8
		#ImmediatelyFlushLog = 1
		AutostartCore = $AutostartCore
		AutostartMedia = $AutostartMedia
		LTS_HES = $LTS_HES
		EOF
	chmod 777 /etc/pluto.conf &>/dev/null
}

UpdateStartupScripts () {
	StatsMessage "Updating Startup Scripts"
	# "DCERouter postinstall"
	/usr/pluto/bin/Update_StartupScrips.sh  # << Note the mis-spelling
}

Create_And_Config_Devices () {
	#Source the SQL_OPS file
	. /usr/pluto/bin/SQL_Ops.sh	# pluto-boot-scripts

	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('LinuxMCE', '1111')"
	RunSQL "$Q"

## CORE
	## Create the Core device and set it's description
	StatsMessage "Setting up your computer to act as a 'Core'"
	Core_PK_Device=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"

## HYBRID
	#Setup media director with core
	StatsMessage "Setting up your computer to act as a 'Media Director'"
	/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
	Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")
	Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
	RunSQL "$Q"

	UpdateStartupScripts
}

CleanInstallSteps () {
	# FIXME: This fn has old data and could be dangerous
	return;

	# This is old and needs work but would allow you to wipe a db and start fresh without re-installing from scratch.
	if [[ -f /etc/pluto/install_cleandb ]]; then
		# on upgrade, the old keys are already in place, so keep them
		rm -f /etc/ssh/ssh_host_*
		dpkg-reconfigure -pcritical openssh-server

		PostInstPkg=(
			pluto-local-database pluto-media-database pluto-security-database pluto-system-database
			pluto-telecom-database lmce-asterisk
		)

		for Pkg in "${PostInstPkg[@]}"; do
			/var/lib/dpkg/info/"$Pkg".postinst configure
		done

		. /usr/pluto/bin/SQL_Ops.sh	# pluto-boot-scripts
		. /usr/pluto/bin/Config_Ops.sh	# pluto-boot-scripts

		# Raise max char limit on php.ini
		if ! grep -q 'max_input_vars' /etc/php5/apache2/php.ini; then 
			echo "max_input_vars = 15000" >> /etc/php5/apache2/php.ini
		fi

		# Mark remote assistance as diabled
		ConfDel remote

		arch=$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture.*"\(.*\)".*/\1/g')

		Queries=(
			"UPDATE Device_DeviceData
				SET IK_DeviceData=15
			WHERE PK_Device IN (
				SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN (7, 28)
			)
			AND FK_DeviceData=7"
			"UPDATE Device_DeviceData SET IK_DeviceData='LMCE_CORE_u0804_$arch' WHERE IK_DeviceData='LMCE_CORE_1_1'"
			"UPDATE Device_DeviceData SET IK_DeviceData='LMCE_MD_u0804_i386'   WHERE IK_DeviceData='LMCE_MD_1_1'"
			"UPDATE Device_DeviceData SET IK_DeviceData='0' WHERE FK_DeviceData='234'"
			"UPDATE Device_DeviceData SET IK_DeviceData='i386' WHERE FK_DeviceData='112' AND IK_DeviceData='686'"
		 )

		for Q in "${Queries[@]}"; do
			RunSQL "$Q"
		done

		DT_DiskDrive=11
		DiskDrives=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DT_DiskDrive'")
		for DiskDrive in $DiskDrives ;do
			DiskDrive_DeviceID=$(Field 1 "$DiskDrive")
			for table in 'CommandGroup_Command' 'Device_Command' 'Device_CommandGroup' 'Device_DeviceData' 'Device_DeviceGroup' 'Device_Device_Related' 'Device_EntertainArea' 'Device_HouseMode' 'Device_Orbiter' 'Device_StartupScript' 'Device_Users' ;do
				RunSQL "DELETE FROM \\`$table\\` WHERE FK_DeviceID = '$DiskDrive_DeviceID' LIMIT 1"
			done
			RunSQL "DELETE FROM Device WHERE PK_Device = '$DiskDrive_DeviceID' LIMIT 1"
		done
	fi
}

CreateDisklessImage () {
	local diskless_log=/var/log/pluto/Diskless_Create-`date +"%F"`.log
	nohup /usr/pluto/bin/Diskless_CreateTBZ.sh >> ${diskless_log} 2>&1 &
}


return 0

###########################################################
###########################################################
### Deprecated Functions
###########################################################
###########################################################

