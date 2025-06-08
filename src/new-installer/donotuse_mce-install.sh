#!/bin/bash


###########################################################
### Setup global variables
###########################################################
log_file=/var/log/LinuxMCE_Setup.log

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
DEB_CACHE=$HOST_DISTRO-$HOST_RELEASE-$HOST_ARCH

DISTRO="$(lsb_release -c -s)"
COMPOS="main"
DT_MEDIA_DIRECTOR=3
LOCAL_REPO_BASE=/usr/pluto/deb-cache/$DEB_CACHE
LOCAL_REPO_DIR=./
DT_CORE=1
DT_HYBRID=2
mce_wizard_data_shell=/tmp/mce_wizard_data.sh
#Setup Pathing
export PATH=$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin


###########################################################
### Setup Functions - Error checking and logging
###########################################################

Setup_Logfile () {
	if [ ! -f ${log_file} ]; then
		touch ${log_file}
		if [ $? = 1 ]; then
			echo "`date` - Unable to write to ${log_file} - re-run script as root"
			exit 1
		fi
	else
		#zero out an existing file
		echo > ${log_file}
	fi
	TeeMyOutput --outfile ${log_file} --stdboth --append -- "$@"
	VerifyExitCode "Log Setup"
	echo "`date` - Logging initiatilized to ${log_file}"
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
	printf "`date` - $* \n"
}

TeeMyOutput () {
	# Usage:
	# source TeeMyOutput.sh --outfile <file> [--infile <file>] [--stdout|--stderr|--stdboth] [--append] [--exclude <egrep pattern>] -- "$@"
	#   --outfile <file>         the file to tee our output to
	#   --infile <file>          the file to feed ourselves with on stdin
	#   --stdout                 redirect stdout (default)
	#   --stderr                 redirect stderr
	#   --stdboth                redirect both stdout and stderr
	#   --append                 run tee in append mode
	#   --exclude <pattern>      strip matching lines from output; pattern is used with egrep
	#
	# Environment:
	#   SHELLCMD="<shell command>" (ex: bash -x)

	if [[ -n "$TeeMyOutput" ]]; then
	        return 0
	fi
	Me="TeeMyOutput"

	# parse parameters
	for ((i = 1; i <= "$#"; i++)); do
	        Parm="${!i}"
	        case "$Parm" in
	                --outfile) ((i++)); OutFile="${!i}" ;;
	                --infile) ((i++)); InFile="${!i}" ;;
	                --stdout|--stderr|--stdboth) Mode="${!i#--std}" ;;
	                --append) Append=yes ;;
	                --exclude) ((i++)); Exclude="${!i}" ;;
	                --) LastParm="$i"; break ;;
	                *) echo "$Me: Unknown parameter '$Parm'"; exit 1
	        esac
	done

	if [[ -z "$OutFile" ]]; then
	        echo "$Me: No outfile"
	        exit 1
	fi

	if [[ -z "$LastParm" ]]; then
	        LastParm="$#"
	fi

	# original parameters
	for ((i = "$LastParm" + 1; i <= "$#"; i++)); do
	        OrigParms=("${OrigParms[@]}" "${!i}")
	done

	# construct command components
	case "$Mode" in
	        out) OurRedirect=() ;;
	        err) OurRedirect=("2>&1" "1>/dev/null") ;;
	        both) OurRedirect=("2>&1") ;;
	esac

	if [[ "$Append" == yes ]]; then
	        TeeParm=(-a)
	fi

	if [[ -n "$InFile" ]]; then
	        OurRedirect=("${OurRedirect[@]}" "<$InFile")
	fi

	# do our stuff
	export TeeMyOutput=yes
	ExitCodeFile="/tmp/TeeMyOutputExitCode_$$"
	trap "rm -rf '$ExitCodeFile'" EXIT

	Run() {
	        eval exec "${OurRedirect[@]}"
	        $SHELLCMD "$0" "${OrigParms[@]}"
	        echo $? >"$ExitCodeFile"
	}

	if [[ -z "$Exclude" ]]; then
	        Run | tee "${TeeParm[@]}" "$OutFile"
	else
	        Run | grep --line-buffered -v "$Exclude" | tee "${TeeParm[@]}" "$OutFile"
	fi

	ExitCode=$(<"$ExitCodeFile")
	exit "$ExitCode"
	exit 1 # just in case
}

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

AddGpgKeyToKeyring () {
	local gpg_key="$1"
	wget -q "$gpg_key" -O- | apt-key add -
}

AddRepoToSources () {
	local repository="$1"
	local changed

	if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list; then
		echo "deb ${repository}" >>/etc/apt/sources.list
		changed=0
	else
		echo "Repository ${repository} seems already active"
		changed=1
	fi
	return $changed
}

AddRepoToSourcesTop () {
	local repository="$1"
	local changed

	if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list; then
		sed -e "1ideb ${repository}" -i /etc/apt/sources.list
		changed=0
	else
		sed -e "/${repository}/d" -i /etc/apt/sources.list
		sed -e "1ideb ${repository}" -i /etc/apt/sources.list
		echo "`date` - Repository ${repository} already active, moved to top"
		changed=1
	fi
}

###########################################################
### Setup Functions - General functions
###########################################################

UpdateUpgrade () {
	#perform an update and a dist-upgrade
	StatsMessage "Performing an update and an upgrade to all components"
	apt-get -qq update
	VerifyExitCode "apt-get update"
	apt-get -y -q -f --force-yes dist-upgrade
	VerifyExitCode "dist-upgrade"
}

TimeUpdate () {
	StatsMessage "Synchronizing time with an online server"
	#Update system time to match ntp server
	ntpdate ntp.ubuntu.com
}

CreateBackupSources () {
	if [ ! -e /etc/apt/sources.list.pbackup ]; then
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
	StatsMessage "Installing necessary prep packages"

	#Create local deb-cache dir
	mkdir -p "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"

	#Install dpkg-dev and debconf-utils for pre-seed information
	#Install makedev due to mdadm issue later in the install process - logged bug https://bugs.launchpad.net/ubuntu/+source/mdadm/+bug/850213 with ubuntu
	apt-get -y -q install dpkg-dev debconf-utils makedev
	VerifyExitCode "dpkg-dev and debconf-utils"

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

ConfigSources () {
	StatsMessage "Configuring sources.list for MCE install"
	# Make sure sources.conf has EOL at EOF
	echo >>/etc/apt/sources.conf

	AddRepoToSourcesTop "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
	AddRepoToSources "http://deb.linuxmce.org/ubuntu/ ${DISTRO} ${COMPOS}"

#	# Check where our distro is hosted and assign the proper variable
#	DISTRO_HOST=
#	#Silently gather and assign appropriate repo based on the distribution
#	[[ -z "$DISTRO_HOST" ]] && wget -q http://archive.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=archive
#	[[ -z "$DISTRO_HOST" ]] && wget -q http://old-releases.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=old-releases

	if [[ $(wget -q http://old-releases.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz) ]]; then
		DISTRO_HOST=old-releases
		AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO} main restricted universe multiverse"
		AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO}-security main restricted universe multiverse"
	fi
#	AddRepoToSources "http://archive.canonical.com/ubuntu ${DISTRO} partner"

	if AddRepoToSources "http://download.videolan.org/pub/debian/stable/ /"; then
		AddGpgKeyToKeyring "http://download.videolan.org/pub/debian/videolan-apt.asc"
	fi

	# Setup pluto's apt.conf
	cat <<-EOF >/etc/apt/apt.conf.d/30pluto
		// Pluto apt conf add-on
		#//Apt::Cache-Limit "12582912";
		Dpkg::Options { "--force-confold"; };
		//Acquire::http::timeout "10";
		//Acquire::ftp::timeout "10";
		APT::Get::AllowUnauthenticated "true";
		//APT::Get::force-yes "yes";
		EOF

	#ensure the local repo is the first in sources.list
	#Because Ubiquity doesn't seem to allow us to control the order in sources.list
	sed -e "/deb-cache/d" -i /etc/apt/sources.list
	sed -e "1ideb file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}" -i /etc/apt/sources.list

	apt-get -qq update
	VerifyExitCode "apt-get update"

	echo >>/etc/apt/sources.list
}

PreSeed_Prefs () {
	StatsMessage "PreSeeding package installation preferences"

	#create preseed file
	cat <<-EOF >/tmp/preseed.cfg
		debconf	debconf/frontend	select	Noninteractive
		# Choices: critical, high, medium, low
		debconf	debconf/priority	select	critical
		msttcorefonts	msttcorefonts/http_proxy	string
		msttcorefonts	msttcorefonts/defoma	note
		msttcorefonts	msttcorefonts/dlurl	string
		msttcorefonts	msttcorefonts/savedir	string
		msttcorefonts	msttcorefonts/baddldir	note
		msttcorefonts	msttcorefonts/dldir	string
		msttcorefonts	msttcorefonts/blurb	note
		msttcorefonts	msttcorefonts/accepted-mscorefonts-eula	boolean	true
		msttcorefonts	msttcorefonts/present-mscorefonts-eula	boolean	false
		sun-java6-bin	shared/accepted-sun-dlj-v1-1	boolean	true
		sun-java6-jre	shared/accepted-sun-dlj-v1-1	boolean	true
		sun-java6-jre	sun-java6-jre/jcepolicy	note
		sun-java6-jre	sun-java6-jre/stopthread	boolean	true
		debconf	debconf/frontend	select	Noninteractive
		# Choices: critical, high, medium, low
		debconf	debconf/priority	select	critical
		EOF

	debconf-set-selections /tmp/preseed.cfg
	VerifyExitCode "debconf-set-selections - preseed data"

	#For some odd reason, set-selections adds a space for Noninteractive and Critical that needs to be removed - debconf doesn't handle extra white space well
	sed -i 's/Value:  /Value: /g' /var/cache/debconf/config.dat

	#remove preseed file, no need to clutter things up
	rm /tmp/preseed.cfg

	#Seeding mythweb preferences to not override the LMCE site on install - for some odd reason, mythweb packages don't accept the set-selections
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
	LATEST_KERNEL=`ls /lib/modules --sort time --group-directories-first|head -1`
	KERNEL_TO_USE=`uname -r`

	if [ -f "/boot/initrd.img-$LATEST_KERNEL" ]; then
		KERNEL_TO_USE=$LATEST_KERNEL
	fi
	ln -s -f /boot/initrd.img-$KERNEL_TO_USE /initrd.img
	ln -s -f /boot/vmlinuz-$KERNEL_TO_USE /vmlinuz
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
		"precise")
			PK_DISTRO=20
			;;
		"trusty")
			PK_DISTRO=21
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
		AutostartCore=$AutostartCore
		AutostartMedia=$AutostartMedia
		EOF
	chmod 777 /etc/pluto.conf &>/dev/null
}

Setup_NIS () {
	# Put a temporary nis config file that will prevent ypbind to start
	# Temporary NIS setup, disabling NIS server and client.
	StatsMessage "Temporarily modifying the NIS configuration file disabling the NIS server and client"
	cat <<-EOF >/etc/default/nis
		NISSERVER=false
		NISCLIENT=false
		YPPWDDIR=/etc
		YPCHANGEOK=chsh
		NISMASTER=
		YPSERVARGS=
		YPBINDARGS=
		YPPASSWDDARGS=
		YPXFRDARGS=
		EOF
}

Install_DCERouter () {
	#Install mysql server
	StatsMessage "Installing MySQL Server"
	apt-get -y -q --force-yes -f install mysql-server
	VerifyExitCode "MySQL Server Installation"
	#Start mysql server
	service mysql start
	#Run mysql statement
	echo "UPDATE user SET Create_view_priv = 'Y', Show_view_priv = 'Y', \
		Create_routine_priv = 'Y', Alter_routine_priv = 'Y', \
		Create_user_priv = 'Y' WHERE User = 'debian-sys-maint'; \
		FLUSH PRIVILEGES; \
		" | mysql --defaults-extra-file=/etc/mysql/debian.cnf mysql

	#Create logical link for MAKEDEV for the mdadm installation
	ln -s /sbin/MAKEDEV /dev/MAKEDEV

	#Install the router and media
	StatsMessage "Installing LinuxMCE Base DCE Router Software"
	apt-get -y -q --force-yes -f install lmce-core video-wizard-videos pluto-sample-media
	VerifyExitCode "Initial media and dcerouter install"
}

Create_And_Config_Devices () {
	StatsMessage "Cycling MySQL server"
	# Create the initial core device using CreateDevice, and the MD for the core in case we create a Hybrid (the default).

	#Cycle the mysql server
	service mysql stop
	killall -9 mysqld_safe
	service mysql start

	#sleep to ensure the mysql server is fully started
	sleep 5

	#Source the SQL_OPS file
	. /usr/pluto/bin/SQL_Ops.sh

	DEVICE_TEMPLATE_Core=7
	DEVICE_TEMPLATE_MediaDirector=28

	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '1111')"
	RunSQL "$Q"

	## Create the Core device and set it's description
	StatsMessage "Setting up your computer to act as a 'Core'"
	apt-get install lmce-asterisk -y
	Core_PK_Device=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"

	#Setup media director with core
	StatsMessage "Setting up your computer to act as a 'Media Director'"
	/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
	Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")
	Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
	RunSQL "$Q"

	## Set UI interface
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Hybrid_DT' AND FK_DeviceTemplate=62"
	OrbiterDevice=$(RunSQL "$Q")

	StatsMessage "Updating Startup Scripts"
	# "DCERouter postinstall"
	/usr/pluto/bin/Update_StartupScrips.sh

	StatsMessage "Installing & Configuring Packages"
	/usr/pluto/bin/Config_Device_Changes.sh
}

Configure_Network_Options () {
	# Updating hosts file and the Device_Data for the core with the internal and external network
	# addresses - uses Initial_DHCP_Config.sh from the pluto-install-scripts package.
	StatsMessage "Configuring your internal network"
	#Source the SQL Ops file
	. /usr/pluto/bin/SQL_Ops.sh

	. ${mce_wizard_data_shell}
	VerifyExitCode "MCE Wizard Data"

	Core_PK_Device="0"

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


	# TODO : move to a new fn?
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
	echo "    address $IntIP" >> /etc/network/interfaces
	echo "    netmask 255.255.255.0" >> /etc/network/interfaces

	## Setup /etc/hosts
	echo > /etc/hosts
	echo "127.0.0.1 localhost.localdomain localhost" >> /etc/hosts
	echo "$c_netExtIP dcerouter $(/bin/hostname)"    >> /etc/hosts

}

VideoDriverSetup () {
	StatsMessage "Starting video driver setup"
	touch /etc/X11/xorg.conf

	## Install driver based on the type of video card used
	if lspci | grep -qi 'VGA .* nvidia'; then
		. /usr/pluto/bin/nvidia-install.sh
		installCorrectNvidiaDriver
	fi
}

addAdditionalTTYStart () {
	if [[ "$DISTRO" = "lucid" ]] || [[ "$DISTRO" = "precise" ]] || [[ "$DISTRO" == "trusty" ]]; then
		sed -i 's/23/235/' /etc/init/tty2.conf
		sed -i 's/23/235/' /etc/init/tty3.conf
		sed -i 's/23/235/' /etc/init/tty4.conf
		# disable plymouth splash for now. Could be replaced by own LMCE splash later
		sed -i 's/ splash//' /etc/default/grub
		#Setup vmalloc for video drivers
		sed -i 's/GRUB_CMDLINE_LINUX=\"\"/GRUB_CMDLINE_LINUX=\"vmalloc=256m\"/' /etc/default/grub
		/usr/sbin/update-grub
	else
		echo "start on runlevel 5">>/etc/event.d/tty2
		echo "start on runlevel 5">>/etc/event.d/tty3
		echo "start on runlevel 5">>/etc/event.d/tty4
	fi
}

TempEMIFix () {
	#Until the new id-my-disc package is implemented, this will allow the external_media_identifier to launch
	ln -s /usr/lib/libdvdread.so.4 /usr/lib/libdvdread.so.3
}

CreateFirstBoot () {
    #Create the firstboot file
    cat <<-NACHOS >/tmp/firstboot
	#!/bin/bash
	###########################################################
	### Setup global variables
	###########################################################
	LogFile="/var/log/pluto/firstboot.log"
	trap 'update-rc.d -f firstboot remove' EXIT

	###########################################################
	### Setup Functions - Error checking and logging
	###########################################################

	Setup_Logfile () {
	if [ -f \${log_file} ]; then
	        touch \${log_file}
	        if [ \$? = 1 ]; then
	                echo "\`date\` - Unable to write to \${log_file} - re-run script as root"
	                exit 1
	        else
	                echo "\`date\` - Logging initiatilized to \${log_file}"
	        fi
	else
	        #0 out an existing file
	        echo > \${log_file}
	        echo "\`date\` - Setup has run before, clearing old log file at \${log_file}"
	fi
	}

	VerifyExitCode () {
	        local EXITCODE=\$?
	        if [ "\$EXITCODE" != "0" ] ; then
	                echo "An error (Exit code \$EXITCODE) occured during the last action"
	                echo "\$1"
	                exit 1
	        fi
	}

	StatsMessage () {
	        printf "\`date\` - \$* \n"
	}

	###########################################################
	### Setup Functions - General functions
	###########################################################

	CreatePackagesFiles () {
		StatsMessage "Creating necessary package files"
		pushd ${LOCAL_REPO_BASE}
		dpkg-scanpackages -m . /dev/null | tee Packages | gzip -c > Packages.gz
		popd
	}

	SetupNetworking () {
		rm -f /etc/X11/xorg.conf
		rm -f /etc/network/interfaces

		## Reconfigure networking
		/usr/pluto/bin/Network_Setup.sh
		/usr/pluto/bin/ConfirmInstallation.sh
		/usr/pluto/bin/Timezone_Detect.sh
	}

	CleanInstallSteps () {

		if [[ -f /etc/pluto/install_cleandb ]]; then

		        # on upgrade, the old keys are already in place, so keep them
		        rm -f /etc/ssh/ssh_host_*
		        dpkg-reconfigure -pcritical openssh-server

		        PostInstPkg=(
		        pluto-local-database pluto-media-database pluto-security-database pluto-system-database
		        pluto-telecom-database lmce-asterisk
		        )

		        for Pkg in "\${PostInstPkg[@]}"; do
		                /var/lib/dpkg/info/"\$Pkg".postinst configure
		        done

		        . /usr/pluto/bin/SQL_Ops.sh
		        . /usr/pluto/bin/Config_Ops.sh

			# Raise max char limit on php.ini
			if ! grep -q 'max_input_vars' /etc/php5/apache2/php.ini; then 
				echo "max_input_vars = 15000" >> /etc/php5/apache2/php.ini
			fi

		        # Mark remote assistance as diabled
		        ConfDel remote

		        arch=\$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture.*"\(.*\)".*/\1/g')

		        Queries=(
		                "UPDATE Device_DeviceData
	                        SET IK_DeviceData=15
	                        WHERE PK_Device IN (
                                        SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN (7, 28)
                                )
                                AND FK_DeviceData=7"
		                "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_CORE_u0804_\$arch' WHERE IK_DeviceData='LMCE_CORE_1_1'"
		                "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_MD_u0804_i386'   WHERE IK_DeviceData='LMCE_MD_1_1'"
		                "UPDATE Device_DeviceData SET IK_DeviceData='0' WHERE FK_DeviceData='234'"
		                "UPDATE Device_DeviceData SET IK_DeviceData='i386' WHERE FK_DeviceData='112' AND IK_DeviceData='686'"
        		 )

		        for Q in "\${Queries[@]}"; do
		                RunSQL "\$Q"
		        done

		        DT_DiskDrive=11
		        DiskDrives=\$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='\$DT_DiskDrive'")
		        for DiskDrive in \$DiskDrives ;do
		                DiskDrive_DeviceID=\$(Field 1 "\$DiskDrive")
		                for table in 'CommandGroup_Command' 'Device_Command' 'Device_CommandGroup' 'Device_DeviceData' 'Device_DeviceGroup' 'Device_Device_Related' 'Device_EntertainArea' 'Device_HouseMode' 'Device_Orbiter' 'Device_StartupScript' 'Device_Users' ;do
		                        RunSQL "DELETE FROM \\\`\$table\\\` WHERE FK_DeviceID = '\$DiskDrive_DeviceID' LIMIT 1"
		                done
		                RunSQL "DELETE FROM Device WHERE PK_Device = '\$DiskDrive_DeviceID' LIMIT 1"
		        done
		fi
	}

	CreateDisklessImage () {
		StatsMessage "Building a disk image for your Diskless Media Directors"
		#local diskless_log=/var/log/pluto/Diskless_Create-\`date +"%F"\`.log
		#nohup /usr/pluto/bin/Diskless_CreateTBZ.sh >> \${diskless_log} 2>&1 &
	}

	###########################################################
	### Main execution area
	###########################################################

	#Setup logging
	. /usr/pluto/bin/TeeMyOutput.sh --outfile "\$LogFile" --stdboth --append -- "\$@"

	StatsMessage "Starting firstboot script"
	CreatePackagesFiles
	SetupNetworking
	CleanInstallSteps
	#CreateDisklessImage
	#VideoDriver # not necessary, done @ avwizard
	StatsMessage "Firstboot Script Complete"
	exit 0
	NACHOS
}

InitialBootPrep () {
	StatsMessage "Preparing initial reboot"

	mv /tmp/firstboot /etc/init.d/firstboot
	chmod +x /etc/init.d/firstboot
	update-rc.d firstboot start 90 2 3 4 5 .

	# Remove KDM startup
	echo "/bin/false" >/etc/X11/default-display-manager

	echo >> /etc/apt/sources.list

	/usr/share/update-notifier/notify-reboot-required
}


###########################################################
### Main execution area
###########################################################

#Set up logging
Setup_Logfile

#Execute Functions
UpdateUpgrade
TimeUpdate
CreateBackupSources
AddAptRetries
Pre-InstallNeededPackages
CreatePackagesFiles
ConfigSources
PreSeed_Prefs
Fix_Initrd_Vmlinux
Nic_Config
Setup_Pluto_Conf
Setup_NIS
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options
UpdateUpgrade
VideoDriverSetup
addAdditionalTTYStart
TempEMIFix
CreateFirstBoot
InitialBootPrep
StatsMessage "MCE Install Script completed without a detected error"
StatsMessage "The log file for the install process is located at ${log_file}"
StatsMessage "Reboot the system to start the final process"
exit 0
