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
c_netExtIP='{extip}'
c_netExtMask='{extMask}'
c_netExtGateway='{extGW}'
c_netExtDNS1='{extDNS}'
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
### Network Setup Functions - FIRSTBOOT
###########################################################

Nic_Config () {
	StatsMessage "Starting NIC Discovery and Configuration"
	# Find out, what nic configuration we have. This is needed for later on to fill the database correctly.
	# Grab available interface names
	NICS=$(ip -o link | grep "link/ether" | awk '{ print $2; }' | cut -d':' -f1)
	NIC_COUNT=$(wc -w <<< $NICS)
	if  [[ "$NIC_COUNT" > 1 ]]; then
		Create_Wizard_Data-Double_Nic_Shell > ${mce_wizard_data_shell}
		# Use these for the defaults if we cannot automatically determine which to use
		# TODO: Error out and instruct the user to setup a working connection? Or ask them to manually choose?
		extif=$(awk '{ print $1; }' <<< $NICS)
		intif=$(awk '{ print $2; }' <<< $NICS)
		if route -n | grep -q '^0.0.0.0'; then
			# We have a default route, use it for finding external interface.
			extif=`route -n | awk '$1 == "0.0.0.0" { print $8 }'`
			# Use the first available interface as the internal interface.
			for interface in $NICS ; do
				if [ "$interface" != "$extif" ] ; then
					intif=$interface
					break
				fi
			done
		fi
		echo "Using $extif for external interface"
		echo "Using $intif for internal interface"

		sed --in-place -e "s,\({extif}\),$extif,g" ${mce_wizard_data_shell}
		sed --in-place -e "s,\({intif}\),$intif,g" ${mce_wizard_data_shell}
	else
		# Only one interface
		extif="$NICS"
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
}

Configure_Network_Options () {
	# Updating hosts file and the Device_Data for the core with the internal and external network
	# addresses - uses Initial_DHCP_Config.sh from the lmce-install-scripts package.
	StatsMessage "Configuring your internal network"
	. ${BASE_DIR}/bin/SQL_Ops.sh	# pluto-boot-scripts

	. ${mce_wizard_data_shell}
	VerifyExitCode "MCE Wizard Data - Configure_Network_Options"

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

	echo "IntIP='${IntIP}'" >> ${mce_wizard_data_shell}
	echo "IntNetmask='${IntNetmask}'" >> ${mce_wizard_data_shell}
	echo "IntIf='${IntIf}'" >> ${mce_wizard_data_shell}

	if [[ "$c_singleNIC" == "1" ]] ;then
		#Disable firewalls on single NIC operation, refs #396
		echo "We are in single NIC mode -> internal firewalls disabled"
		ConfSet "DisableFirewall" "1"
		ConfSet "DisableIPv6Firewall" "1"
	fi
}

Configure_Network_Files () {
	. ${mce_wizard_data_shell}
	VerifyExitCode "MCE Wizard Data - Configure_Network_Files"

	# Setup /etc/network/interfaces
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
	echo "    address $IntIP" >> /etc/network/interfaces
	echo "    netmask 255.255.255.0" >> /etc/network/interfaces

	## Setup /etc/hosts
	echo > /etc/hosts
	echo "127.0.0.1 localhost.localdomain localhost" >> /etc/hosts
	#echo "$c_netExtIP dcerouter $(/bin/hostname)"    >> /etc/hosts
	echo "127.0.1.1 dcerouter $(/bin/hostname)"    >> /etc/hosts

	ifup $c_netExtName $c_netIntName >/dev/null 2>/dev/null &
}

Configure_Network_Database () {
	### !!!!! This must run after Create_And_Config_Core to properly update the database

	. ${BASE_DIR}/bin/Config_Ops.sh	# pluto-boot-scripts
	. ${BASE_DIR}/bin/SQL_Ops.sh	# pluto-boot-scripts

	. ${mce_wizard_data_shell}
	VerifyExitCode "MCE Wizard Data - Configure_Network_Database"

	Core_PK_Device="$PK_Device"

	if [[ "$c_netExtUseDhcp" == "0" ]] ;then
		NETsetting="$c_netExtName,$c_netExtIP,$c_netExtMask,$c_netExtGateway,$c_netExtDNS1|$IntIf,$IntIP,$IntNetmask"
	else
		NETsetting="$c_netExtName,dhcp|$IntIf,$IntIP,$IntNetmask"
	fi

	DHCPsetting=$(${BASE_DIR}/install/Initial_DHCP_Config.sh "$Network" "$Digits_Count")

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

Configure_SSH_Server() {
	# generate SSH keys for openssh-server
	dpkg-reconfigure -pcritical openssh-server
}

Configure_SSH_Client() {
	# add additional ciphers to ssh client to permit connections to jessie from trusty.
	if ! grep -q aes128-ctr /etc/ssh/ssh_config ; then
		sed -i 's/Ciphers.*/&,aes128-ctr,aes192-ctr,aes256-ctr/' /etc/ssh/ssh_config
	fi
}

Configure_NTP_Server()
{
	. ${BASE_DIR}/bin/Config_Ops.sh # pluto-boot-scripts
	. ${BASE_DIR}/bin/SQL_Ops.sh    # pluto-boot-scripts

	Core_PK_Device="$PK_Device"

	# FIXME: get from DB?? this should be loaded from above
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Core_PK_Device' AND FK_DeviceData='32'"
        R=$(RunSQL "$Q")

	IntIP=$(echo $R | cut -d"|" -f2 | cut -d"," -f2)

	if [[ -n "$IntIP" ]]; then
		BCastIP=$(echo "$IntIP" | cut -d"." -f1-3).255
		sed -i 's/^#broadcast [1-9.]*.[1-9.]*/broadcast $BCastIP/' /etc/ntp.conf
		service ntp restart || :
	fi
}

#######################################################
### Other Setup Functions
#######################################################



Create_And_Config_Core () {
	# TODO: FIXME: Check and do only if not done!

	### CORE
	. ${BASE_DIR}/bin/Config_Ops.sh	# pluto-boot-scripts
	. ${BASE_DIR}/bin/SQL_Ops.sh	# pluto-boot-scripts
	## Create the Core device and set it's description
	StatsMessage "Setting up your computer to act as a 'Core'"
	Core_PK_Device=$(${BASE_DIR}/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"
	ConfSet "PK_Device" "$Core_PK_Device"
}

Create_And_Config_Hybrid_MD () {
	# TODO: FIXME: Check and do only if not done!

	### HYBRID
	. ${BASE_DIR}/bin/Config_Ops.sh	# pluto-boot-scripts
	. ${BASE_DIR}/bin/SQL_Ops.sh	# pluto-boot-scripts
	Core_PK_Device="$PK_Device"
	#Setup media director with core
	StatsMessage "Setting up your computer to act as a 'Media Director'"
	${BASE_DIR}/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
	Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")
	Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
	RunSQL "$Q"
}

Create_And_Config_Devices () {
	Create_And_Config_Core
	Create_And_Config_Hybrid_MD
}

CreateDisklessImage () {
	# This runs in the background '&'
	local diskless_log=/var/log/pluto/Diskless_Create-`date +"%F"`.log
	nohup ${BASE_DIR}/bin/Diskless_CreateTBZ.sh >> ${diskless_log} 2>&1 &
}

return 0

###########################################################
###########################################################
### Deprecated Functions
###########################################################
###########################################################

