#!/bin/bash


###########################################################
### Setup global variables
###########################################################
error_log=/var/log/initial_mce_setup_errors.log
DISTRO="$(lsb_release -c -s)"
COMPOS="beta2"
LOCAL_REPO_BASE=/usr/pluto/deb-cache
LOCAL_REPO_DIR=./
DT_MEDIA_DIRECTOR=3
DT_CORE=1
DT_HYBRID=2
mce_wizard_data_shell=/tmp/mce_wizard_data.sh
#Setup Pathing
export PATH=$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin


###########################################################
### Setup Functions - Error checking and logging
###########################################################

Setup_Error_Logging () {
if [ -f ${error_log} ]; then
	touch ${error_log}
	if [ $? = 1 ]; then
		echo "`date` - Unable to write to ${error_log} - re-run script as root"
		exit 1
	else
		echo "`date` - Logging for issues initiatilized to ${error_log}"
	fi
else
	#0 out an existing file
	echo > ${error_log}
	echo "`date` - Setup has run before, clearing old log file at ${error_log}"
fi
}

VerifyExitCode () {
        local EXITCODE=$?
        if [ "$EXITCODE" != "0" ] ; then
        	echo "An error (Exit code $EXITCODE) occured during the last action"
        	echo "$1"
                exit 1
        fi
}

StatsMessage () {
	echo "`date` - $*"
}

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

asteriskNoloadModule () {
	local module="$1"


	if ! grep -q "^[^;]*noload => ${module}" /etc/asterisk/modules.conf
	then
		sed -e "s/\[global\]/noload => ${module}\n&/" -i /etc/asterisk/modules.conf
	else
		echo "module $module already disabled"
	fi

}


###########################################################
### Setup Functions - General functions
###########################################################

UpdateUpgrade () {
#perform an update and a dist-upgrade
StatsMessage "Performing an update and an upgrade to all components"
apt-get -qq update 
apt-get -y -q -f --force-yes dist-upgrade
VerifyExitCode "dist-upgrade"
}

TimeUpdate () {
StatsMessage "Synchronizing time with an online server"
#Update system time to match ntp server
ntpdate ntp.ubuntu.com
}

CreateBackupSources () {
if [ ! -e /etc/apt/sources.list.pbackup ]
then
	StatsMessage "Backing up sources.list file"
	cp -a /etc/apt/sources.list /etc/apt/sources.list.pbackup
fi
}

AddAptRetries () {
        local changed
        if [ -f /etc/apt/apt.conf ]; then
			if ! grep -q "^[^#]*APT::Acquire { Retries" /etc/apt/apt.conf; then
				echo 'APT::Acquire { Retries  "20" }'>>/etc/apt/apt.conf
				changed=0
			else
				StatsMessage "APT preference on number of retries already set "
				changed=1
			fi
		else
			echo 'APT::Acquire { Retries  "20" }'>>/etc/apt/apt.conf
		fi
		StatsMessage "APT preference on number of retries set"
        return $changed
}

Pre-InstallNeededPackages () {
	StatsMessage "Updating apt cache and installing necessary packages"
	apt-get -qq update 
	VerifyExitCode "apt-get update"
	apt-get -y -q install dpkg-dev debconf-utils
	VerifyExitCode "dpkg-dev and debconf-utils"
	mkdir -p "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"

#Check that kubuntu-desktop is installed - which it should be
dpkg -s kubuntu-desktop 1>/dev/null
if [ $? == 1 ]; then
	apt-get install -y -q --force-yes kubuntu-desktop
fi

# Disable compcache
if [ -f /usr/share/initramfs-tools/conf.d/compcache ]; then
	rm -f /usr/share/initramfs-tools/conf.d/compcache && update-initramfs -u
fi
}

CreatePackagesFiles () {
	StatsMessage "Creating necessary package files"
	( cd "${LOCAL_REPO_BASE}"; \
		dpkg-scanpackages -m "${LOCAL_REPO_DIR}" /dev/null | \
		tee "${LOCAL_REPO_DIR}/Packages" | \
		gzip -9c >"${LOCAL_REPO_DIR}/Packages.gz" )
}

AddRepoToSourcesTop () {
        local repository="$1"
        local changed


        if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list
        then
                sed -e "1ideb ${repository}" -i /etc/apt/sources.list
                changed=0
        else
                sed -e "/${repository}/d" -i /etc/apt/sources.list
                sed -e "1ideb ${repository}" -i /etc/apt/sources.list
                echo "`date` - Repository ${repository} already active, moved to top"
                changed=1
        fi
}

AddRepoToSources () {
	local repository="$1"
	local changed


	if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list
	then
		echo "deb ${repository}" >>/etc/apt/sources.list
		changed=0
	else
		echo "Repository ${repository} seems already active"
		changed=1
	fi

	return $changed
}

AddGpgKeyToKeyring () {
	local gpg_key="$1"

	wget -q "$gpg_key" -O- | apt-key add -

}

ConfigSources () {
StatsMessage "Configuring sources.list for MCE install"
# Make sure sources.conf has EOL at EOF
echo>>/etc/apt/sources.conf

AddRepoToSourcesTop "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
AddRepoToSources "http://deb.linuxmce.org/ubuntu/ ${DISTRO}  ${COMPOS}"
AddRepoToSources "http://debian.slimdevices.com/ stable  main"

# Check where our distro is hosted and assign the proper variable
DISTRO_HOST=
#Silently gather and assign appropriate repo based on the distribution
wget -q http://archive.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=archive
wget -q http://old-releases.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=old-releases
AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO} main restricted universe multiverse"
AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO}-security main restricted universe multiverse"
AddRepoToSources "http://archive.canonical.com/ubuntu ${DISTRO} partner"

if AddRepoToSources "http://packages.medibuntu.org/ ${DISTRO}  free non-free"; then
	AddGpgKeyToKeyring http://packages.medibuntu.org/medibuntu-key.gpg
fi

## Setup apt sources.list
#
######I'm unclear as to what the purpose of this is - commenting out for now
#local Sources="# Pluto sources - start
#deb http://linuxmce.com/ubuntu ./
# Pluto sources - end"
#echo "$Sources" >/etc/apt/sources.list.d/pluto.list

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

#HACK: ensure the local repo is the first in sources.list
#Because Ubiquity doesn't seem to allow us to control the order in sources.list
sed -e "/deb-cache/d" -i /etc/apt/sources.list
sed -e "1ideb file:/usr/pluto/deb-cache ./" -i /etc/apt/sources.list


apt-get -qq update
VerifyExitCode "apt-get update"

echo >> /etc/apt/sources.list

}

PreSeed_Prefs () {
StatsMessage "PreSeeding package installation preferences"

#create preseed file
echo "debconf debconf/frontend	select Noninteractive
# Choices: critical, high, medium, low
debconf debconf/priority        select critical
msttcorefonts   msttcorefonts/http_proxy        string
msttcorefonts   msttcorefonts/defoma    note
msttcorefonts   msttcorefonts/dlurl     string
msttcorefonts   msttcorefonts/savedir   string
msttcorefonts   msttcorefonts/baddldir  note
msttcorefonts   msttcorefonts/dldir     string
msttcorefonts   msttcorefonts/blurb     note
msttcorefonts   msttcorefonts/accepted-mscorefonts-eula boolean true
msttcorefonts   msttcorefonts/present-mscorefonts-eula  boolean false
sun-java6-bin   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   sun-java6-jre/jcepolicy note
sun-java6-jre   sun-java6-jre/stopthread        boolean true
debconf debconf/frontend        select  Noninteractive
# Choices: critical, high, medium, low
debconf debconf/priority        select  critical
" > /tmp/preseed.cfg

debconf-set-selections /tmp/preseed.cfg
VerifyExitCode "debconf-set-selections - preseed data"

#For some odd reason, set-selections adds a space for Noninteractive that needs to be removed
sed -i 's/Value:  Noninteractive/Value: Noninteractive/g' /var/cache/debconf/config.dat

#remove preseed file, no need to clutter things up
rm /tmp/preseed.cfg

#Seeding mythweb preferences to not override the LMCE site on install
touch /etc/default/mythweb
echo "[cfg]" >> /etc/default/mythweb
echo "enable = false" >> /etc/default/mythweb
echo "only = false" >> /etc/default/mythweb
echo "username = " >> /etc/default/mythweb
echo "password = " >> /etc/default/mythweb 

}

Fix_Initrd_Vmlinux () {
StatsMessage "Starting initrd and vmlinuz fix"
# Fix a problem with the /initrd.img and /vmlinuz links pointing to a different kernel than the 
# newest (and currently running) one
ln -s -f /boot/initrd.img-`uname -r` /initrd.img
ln -s -f /boot/vmlinuz-`uname -r` /vmlinuz

}

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
	if route -n | grep -q '^0.0.0.0'
			then
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
	if [[ $ExtUsesDhcp == 0 ]]
			then
		   # Not dhcp defined in config file, test if dhclient got us an IP
		   # /var/run/dhcp3 for newer than 810, /var/run in 810
		   if [[ (-e /var/lib/dhcp3/dhclient-$extif.lease || -e /var/run/dhclient-$extif.lease) && `pgrep -c dhclient` == 1 ]]
		   then
			   ExtUsesDhcp=1
		   fi
	fi
			RunDHCP=0
			if [[ $ExtUsesDhcp == 0 ]]
			then
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

echo "--- start mce wizard data ---" 
cat /tmp/mce_wizard_data.sh 
echo "--- end mce wizard data ---"

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
        echo "`date` - Wizard Information is corrupted or missing."
		exit 1
fi
. /tmp/mce_wizard_data.sh

Core_PK_Device="0"


}

Setup_Network_Intefaces () {
StatsMessage "Seting Up Networking interfaces"

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

Setup_NIS () {
# Put a temporary nis config file that will prevent ypbind to start
# Temporary NIS setup, disabling NIS server and client.
StatsMessage "Temporarily modifying the NIS configuration file disabling the NIS server and client"
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

Setup_Pluto_Conf () {
StatsMessage "Seting Up MCE Configuration file"
	AutostartCore=1
	AutostartMedia=1

	case "$DISTRO" in
	"intrepid")
		# select UI1
		PK_DISTRO=17
		;;
	"lucid")
		# select UI2 without alpha blending
		PK_DISTRO=18
		;;
	esac

	
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
PK_Distro = $PK_DISTRO
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
	# on lucid we don't want to run the AV Wizard
#	if [[ "$DISTRO" = "lucid" ]]; then
#		echo "AVWizardOverride = 0" >> /etc/pluto.conf
#		echo "AVWizardDone = 1" >> /etc/pluto.conf
#	fi
	chmod 777 /etc/pluto.conf &>/dev/null
}

Install_DCERouter () {
	StatsMessage "Installing MySQL Server"
	apt-get -y -q --force-yes -f install mysql-server
	VerifyExitCode "MySQL Server Installation"
	service mysql start
	# invoke-rc.d mysql start # Because of this : https://bugs.launchpad.net/bugs/107224
	# Fix for http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=512309
	# to address http://svn.linuxmce.org/trac.cgi/ticket/397
	# This will not be necessary for 9.10+
	echo "UPDATE user SET Create_view_priv = 'Y', Show_view_priv = 'Y', \
 Create_routine_priv = 'Y', Alter_routine_priv = 'Y', \
 Create_user_priv = 'Y' WHERE User = 'debian-sys-maint'; \
FLUSH PRIVILEGES; \
" | mysql --defaults-extra-file=/etc/mysql/debian.cnf mysql

	StatsMessage "Installing Pluto Sample Media"
	apt-get -y -q --force-yes -f install pluto-sample-media
	VerifyExitCode "pluto-sample-media"
	StatsMessage "Installing the Video Wizard videos"
	apt-get -y -q --force-yes -f install video-wizard-videos
	VerifyExitCode "video-wizard-videos"

	StatsMessage "Installing LinuxMCE Base DCE Router Software"
	apt-get -y -q --force-yes -f install pluto-dcerouter
	VerifyExitCode "pluto-dcerouter"
}

Create_And_Config_Devices () {
StatsMessage "Cycling MySQL server"
# Create the initial core device using CreateDevice, and the MD for the core in case we create a Hybrid (the default).

	#/etc/init.d/mysql stop
	service mysql stop
	killall -9 mysqld_safe
	#/etc/init.d/mysql start
	service mysql start
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
#	if [[ "$c_deviceType" == "2" ]] ;then
		StatsMessage "Setting up your computer to act as a 'Media Director'"
		/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
		Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")

		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
		RunSQL "$Q"
 
		## Set UI interface
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Hybrid_DT' AND FK_DeviceTemplate=62"
		OrbiterDevice=$(RunSQL "$Q")

#		case "$c_installUI" in
#			"0")
#				# select UI1
#				UI_SetOptions "$OrbiterDevice" 0 0 1
#			;;
#			"1")
#				# select UI2 without alpha blending
#				UI_SetOptions "$OrbiterDevice" 1 0 4
#			;;
#			"2")
#				# select UI2 with alpha blending
#				UI_SetOptions "$OrbiterDevice" 1 1 4
#			;;
#			*)
#				echo "Unknown UI version: $c_installUI"
#			;;
#		esac
#	fi

	StatsMessage "Configuring the LinuxMCE devices"
        # "DCERouter postinstall"
        # TODO: I really wonder, what the original author wants to do here, as it never worked before
        #       It now worked (thanks to tail -2 instead of tail +2), and returns the last two device IDs.
        devices=$(echo "SELECT PK_Device FROM Device;" | /usr/bin/mysql pluto_main | tail -2 | tr '\n' ' ')


	/usr/pluto/bin/Update_StartupScrips.sh
}

Configure_Network_Options () {
# Updating hosts file and the Device_Data for the core with the internal and external network
# addresses - uses Initial_DHCP_Config.sh from the pluto-install-scripts package.
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

NvidiaDriverSetup () {
StatsMessage "Starting nVidia driver setup"
#source the nvidia-install.sh file
source /usr/pluto/bin/nvidia-install.sh
# install the correct nvidia driver
installCorrectNvidiaDriver
}

setupRunlevel3 () {

	rm -rfv /etc/rc3.d/*
	cp -av /etc/rc2.d/* /etc/rc3.d/
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce
	rm -fv /etc/rc3.d/S99kdm /etc/rc3.d/S990start_avwizard
	
}

setupRunlevel4 () {

	rm -rfv /etc/rc4.d/*
	cp -av /etc/rc2.d/* /etc/rc4.d/
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce

}

setupRunlevel5 () {

	rm -rfv /etc/rc5.d/*
	cp -av /etc/rc2.d/* /etc/rc5.d/
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce

}

createIninttabConfig () {

cat >/etc/inittab <<"EOF"
# WARNING: Do NOT set the default runlevel to 0 (shutdown) or 6 (reboot).
#id:2:initdefault: # KDE
#id:3:initdefault: # Core
#id:4:initdefault: # Core + KDE
id:5:initdefault: # Launch Manager
EOF

}

FixAsteriskConfig () {

dpkg -s asterisk 1>/dev/null

if [ $? == 0 ]; then
	asteriskNoloadModule app_directory_odbc.so
	asteriskNoloadModule app_voicemail_odbc.so
	asteriskNoloadModule app_voicemail_imap.so
fi

}

addAdditionalTTYStart () {
	if [[ "$DISTRO" = "lucid" ]] ; then
		sed -i 's/23/235/' /etc/init/tty2.conf
		sed -i 's/23/235/' /etc/init/tty3.conf
		sed -i 's/23/235/' /etc/init/tty4.conf
		# disable plymouth splash for now. Could be replaced by own LMCE splash later
		sed -i 's/ splash//' /etc/default/grub
                /usr/sbin/update-grub
	else
        	echo "start on runlevel 5">>/etc/event.d/tty2
        	echo "start on runlevel 5">>/etc/event.d/tty3
        	echo "start on runlevel 5">>/etc/event.d/tty4
	fi
}

InitialBootPrep () {
StatsMessage "Preparing initial reboot"
# Remove KDM startup
echo "/bin/false" >/etc/X11/default-display-manager
wget http://svn.linuxmce.org/svn/branches/LinuxMCE-1004/src/new-installer/firstboot
cp firstboot /etc/rc5.d/S90firstboot
echo >> /etc/apt/sources.list
/usr/share/update-notifier/notify-reboot-required
}

#Send Errors to a log file
exec 2>${error_log}

#Execute Functions
UpdateUpgrade
Setup_Error_Logging
TimeUpdate
CreateBackupSources
AddAptRetries
Pre-InstallNeededPackages
CreatePackagesFiles
ConfigSources
PreSeed_Prefs
Fix_Initrd_Vmlinux
Nic_Config
Setup_Network_Intefaces
Setup_NIS
Setup_Pluto_Conf
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options
UpdateUpgrade
setupRunlevel3
setupRunlevel4
setupRunlevel5
createIninttabConfig
FixAsteriskConfig
addAdditionalTTYStart
InitialBootPrep
Setup_NIS

#StatsMessage "Building a disk image for your Diskless Media Directors"
# TODO - find an area in the process to kick this off post reboot
# /usr/pluto/bin/Diskless_CreateTBZ.sh

StatsMessage "MCE Install Script completed without a detected error"
StatsMessage "Reboot the system to start the final process"
exit 0

