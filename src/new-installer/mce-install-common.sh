#!/bin/bash

DT_MEDIA_DIRECTOR=3
DT_CORE=1
DT_HYBRID=2



function Replace_Mirror {
	local mirror=$1
	local flag='0'
	if [[ "$mirror" == "(null)" ]] ;then
		echo "WARNING: Replace_Mirror called with (null) mirror"
	fi

	local newlines="
deb file:/usr/pluto/deb-cache/ ./
deb $mirror hardy main restricted universe multiverse
deb $mirror hardy-security main restricted universe multiverse
deb $mirror hardy-updates  main restricted universe multiverse
"

	while read line ;do
		if [[ "$line" == '# Choosed mirror - start' ]] ;then
			flag='1'
			echo "# Choosed mirror - start
$newlines
# Choosed mirror - end" >> /etc/apt/sources.list.$$
		fi

		if [[ "$flag" == '0' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$
		fi
                
		if [[ "$line" == '# Choosed mirror - end' ]] ;then
			flag='0'
		fi

	done < /etc/apt/sources.list

	mv -f /etc/apt/sources.list.$$ /etc/apt/sources.list
}


function PinMythTVAt22 {
	# Make sure, we never use a newer version that 0.22
	echo "
	Package: mythtv mythtv-frontend mythtv-backend mythtv-common mythtv-database mythtv-transcode-utils mythweb
	Pin: version 2:0.22*
	Pin-Priority: 1001
	">>/etc/apt/preferences
}
	

function Setup_Apt_Conffiles {	
	## Backup initial apt sources.list
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi


	## Setup apt sources.list
	local Sources="# Pluto sources - start
# Choosed mirror - start
deb file:/usr/pluto/deb-cache/ ./
deb $c_installMirror hardy main restricted multiverse universe
deb $c_installMirror hardy-security main restricted multiverse universe
deb $c_installMirror hardy-updates main restricted multiverse universe
deb http://linuxmce.com/ubuntu ./
# Choosed mirror - end
#deb http://10.0.0.82/ ./
# Pluto sources - end"

	local SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ ./
# Pluto sources offline - end"

if [[ ! -z "$c_deviceType" ]] && [[ "$c_deviceType" != "3" ]] ;then
	if ! grep -qF '# Pluto sources - start' /etc/apt/sources.list ;then
		echo "$Sources" > /etc/apt/sources.list.$$
		cat /etc/apt/sources.list.$$ /etc/apt/sources.list > /etc/apt/sources.list.$$.all
		mv -f /etc/apt/sources.list.$$.all /etc/apt/sources.list
		rm -f /etc/apt/sources.list.$$
	else
		Replace_Mirror "$c_installMirror"
	fi
else
	echo "$SourcesOffline" > /etc/apt/sources.list.$$
	cat /etc/apt/sources.list.$$ /etc/apt/sources.list > /etc/apt/sources.list.$$.all
	mv -f /etc/apt/sources.list.$$.all /etc/apt/sources.list
	rm -f /etc/apt/sources.list.$$
fi

	echo "$SourcesOffline" >/etc/apt/sources.list.offline

#	if [[ "$c_ubuntuExtraCdFrom" != "3" ]] && [[ ! -z "$c_ubuntuExtraCdFrom" ]] ;then
		#echo "Dir::Etc::sourcelist sources.list.offline;" >/etc/apt/apt.conf.d/00ubuntu_offline
		cp /etc/apt/sources.list /etc/apt/sources.list.original
		mv /etc/apt/sources.list.offline /etc/apt/sources.list
#	fi

	Setup_Pluto_Apt_Conf

# this need to be discussed. For now we remove it so duplicate packages get fetched from repo with newest package. 
	pluto_apt_preferences='
Package: *
Pin: origin
Pin-Priority: 9999

Package: *
Pin: release v=8.04,o=Ubuntu,a=hardy,l=Ubuntu
Pin-Priority: 9998
'
#	echo -n "$pluto_apt_preferences" >/etc/apt/preferences

	ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline

	./mce-install-preseed.sh
	apt-get update
}



function Setup_Pluto_Apt_Conf {

	# Setup pluto's apt.conf
	cat >/etc/apt/apt.conf.d/30pluto <<EOF
// Pluto apt conf add-on
//Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
//Acquire::http::timeout "10";
//Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
//APT::Get::force-yes "yes";
EOF

}



function Setup_NIS {
	## Put a temporary nis config file that will prevent ypbind to start
	echo "
NISSERVER=false
NISCLIENT=false
YPPWDDIR=/etc
YPCHANGEOK=chsh
NISMASTER=
YPSERVARGS=
YPBINDARGS=
YPPASSWDDARGS=
YPXFRDARGS=
" > /etc/default/nis
}



function wmtweaks_default()
{
	echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mcs-option SYSTEM "mcs-option.dtd">

<mcs-option>
	<option name="Xfwm/UseCompositing" type="int" value="1"/>
</mcs-option>'
}



function Setup_XOrg {
	## Make X accessible by Pluto software, for all existing and new users
	KDExhost="#!/bin/bash
	xhost +
	"

	for user in /home/*; do
		if [[ ! -d "$user" ]]; then
			continue
		fi
		owner=$(stat -c '%u:%g' "$user")

		Dir="$user/.kde/Autostart"
		mkdir -p "$Dir"
		echo "$KDExhost" >"$Dir/xhost"
		chown "$owner" "$Dir/xhost"
		chmod +x "$Dir/xhost"

		## Configure window manager trasparancy manager
		WMTweaksFile="$user/.config/xfce4/mcs_settings/wmtweaks.xml"
		mkdir -p "$(dirname "$WMTweaksFile")"

		if [[ -f "$WMTweaksFile" ]]; then
			sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
		else
			wmtweaks_default >"$WMTweaksFile"
		fi
		
		chown -R "$owner" "$user"/.kde
		chown -R "$owner" "$user"/.config
	done

	Dir="/etc/skel/.kde/Autostart"
	mkdir -p "$Dir"
	echo "$KDExhost" >"$Dir/xhost"
	chmod +x "$Dir/xhost"
	
	File="/etc/profile"
	if ! grep -q "export KDEWM=/usr/bin/xfwm4" "$File" 2>/dev/null; then
		echo "export KDEWM=/usr/bin/xfwm4" >>"$File"
	fi

	WMTweaksFile="/etc/skel/.config/xfce4/mcs_settings/wmtweaks.xml"
	mkdir -p "$(dirname "$WMTweaksFile")"
	wmtweaks_default >"$WMTweaksFile"
	
	## Add xrecord extention if missing
	if grep -q ".*Load.*\"record\"" /etc/X11/xorg.conf ;then
		echo "Found xrecord , skiping ..."
	else 
		sed -i 's/.*Section.*\"Module\"/Section "Module"\n Load "record"/g' /etc/X11/xorg.conf
	fi
}



function UI_SetOptions {
	local OrbiterDev="$1"
	local OpenGLeffects="$2"
	local AlphaBlending="$3"
	local UI_Version="$4"

	DEVICEDATA_Use_OpenGL_effects=172
	DEVICEDATA_Use_alpha_blended_UI=169
	DEVICEDATA_PK_UI=104

	# disable OpenGL effects
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_OpenGL_effects', '$OpenGLeffects')"
	RunSQL "$Q"

	# disable alpha blending
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_alpha_blended_UI', '$AlphaBlending')"
	RunSQL "$Q"

	# set UI	
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_PK_UI', '$UI_Version')"
	RunSQL "$Q"


	local xorg_file="/etc/X11/xorg.conf"
	local composite_value="true"
	if [[ "$AlphaBlending" == "0" ]] ;then
		composite_value="false"
	fi

	if grep -qi "Section.*\"Extensions\"" "$xorg_file" ;then
		echo 'Found `Section "Extensions"` in xorg.conf'

		if grep -qi "Option \"Composite\"" "$xorg_file" ;then
			echo 'Found `Option "Composite"` in xorg.conf'

			sed -i "s/Option.*\"Composite\".*/Option \"Composite\" \"$composite_value\"/ig" "$xorg_file"
		else
			echo 'Not Found `Option "Composite"` in xorg.conf'

			sed -i "s/Section.*\"Extensions\"/Section \"Extensions\"\\nOption \"Composite\" \"$composite_value\"/ig" "$xorg_file"
		fi
	else
		echo 'Not Found `Section "Extenstions"` or `Option "Composite"`'
		echo 'Section "Extensions"' >> "$xorg_file"
		echo "    Option \"Composite\" \"$composite_value\"" >> "$xorg_file"
		echo 'EndSection' >> "$xorg_file"
	fi
}



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
PK_Distro = 17
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

	# Fix for http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=512309
	# to address http://svn.linuxmce.org/trac.cgi/ticket/397
	# This will not be necessary for 9.10+
	echo "UPDATE user SET Create_view_priv = 'Y', Show_view_priv = 'Y', \
 Create_routine_priv = 'Y', Alter_routine_priv = 'Y', \
 Create_user_priv = 'Y' WHERE User = 'debian-sys-maint'; \
FLUSH PRIVILEGES; \
" | mysql --defaults-extra-file=/etc/mysql/debian.cnf mysql

	apt-get -y -f install pluto-sample-media
	apt-get -y -f install video-wizard-videos
#	apt-get -y -f install pluto-mysql-wrapper
#	apt-get -y -f install pluto-default-tftpboot

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
		StatsMessage "Setting up your computer to act as a 'Media Director'"
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
        # TODO: I really wonder, what the original author wants to do here, as it never worked before
        #       It now worked (thanks to tail -2 instead of tail +2), and returns the last two device IDs.
        devices=$(echo "SELECT PK_Device FROM Device;" | /usr/bin/mysql pluto_main | tail -2 | tr '\n' ' ')

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


function MangleStartupFiles {

#	if [[ "$c_startupType" == "1" ]] ;then
#		# Make sure non of the other display manager starts
#		[ -f /etc/init.d/gdm ] && mv /etc/init.d/gdm /etc/init.d/gdm.saved
#		[ -f /etc/init.d/kdm ] && mv /etc/init.d/kdm /etc/init.d/kdm.saved
#		echo '#!/bin/bash
#	if [[ "$1" == start ]]; then
#		/usr/pluto/bin/Startup_Core-Hybrid.sh
#	fi
#	' > /etc/init.d/kdm
#	chmod +x /etc/init.d/kdm
#	mv /etc/rc2.d/*kdm /etc/rc2.d/S99kdm
		
#	elif [[ "$c_startuptype" == "2" ]] ;then
#		echo "
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
#	fi

#	echo "
#start on runlevel 2
#
#stop on shutdown
#stop on runlevel 3
#stop on runlevel 4
#stop on runlevel 5
#
#script
#screen -d -m -S DhcpdPlugin /usr/pluto/bin/Dhcpd-Plugin.sh
#end script
#" > /etc/event.d/pluto-dhcpd-plugin
echo We dont mangle startup files anymore
}



function RemoveInstallerIcons {

	## Remove 'Install LinuxMCE' icon after LinuxMCE was installed
	for dir in /home/* ;do
		if [ -d "$dir/Desktop" ] ;then
			rm -f "$dir/Desktop/mce-installer.desktop"
		fi
	done
}



function ClearInstallationTable {

	#Remove everything from the Installation table
	RunSQL "UPDATE Installation SET PK_Installation = 1"
	RunSQL "UPDATE Installation SET Description = 'LinuxMCE $(date +%Y%m%d)'"
	RunSQL "UPDATE Installation SET Name = NULL" 
	RunSQL "UPDATE Installation SET Address = NULL" 
	RunSQL "UPDATE Installation SET City = NULL" 
	RunSQL "UPDATE Installation SET State = NULL" 
	RunSQL "UPDATE Installation SET Zip = NULL" 
	RunSQL "UPDATE Installation SET FK_Country = 0" 
	RunSQL "UPDATE Installation SET FK_City = 0" 
	RunSQL "UPDATE Installation SET FK_PostalCode = 0" 

}


