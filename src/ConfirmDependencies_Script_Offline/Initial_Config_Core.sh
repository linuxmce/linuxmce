#!/bin/bash

. /usr/pluto/bin/Utils.sh

## Test to see if we are in upgrade mode or not
UpgradeMode="false"
if [[ -d /.backup ]] ;then
	UpgradeMode="true"
fi


. /usr/pluto/install/Common.sh
. /usr/pluto/install/AptSources.sh
DIR="/usr/pluto/install"
# Initial firewall rules to block services use at install
/sbin/iptables -P INPUT DROP
/sbin/iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
/sbin/iptables -A INPUT -i lo -j ACCEPT
/sbin/iptables -A INPUT -m tcp -p tcp --dport 22 -j ACCEPT
# End initial firewall rules

mkdir -p /home/pluto/logs
mkdir -p /home/public/data
mkdir -p /usr/pluto/var
Activation_Code=1111

## Setup apt sources.list
if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi
	AptSrc_AddSource "deb file:/usr/pluto/deb-cache/ sarge main"
	AptSrc_AddSource "deb http://deb.plutohome.com/debian/ 20dev_debian main"
	AptSrc_AddSource "deb http://deb.plutohome.com/debian/ replacements_debian main"
	AptSrc_AddSource "deb http://deb.plutohome.com/debian/ sarge main non-free contrib"
	AptSrc_AddSource "deb http://deb.plutohome.com/debian/ unstable mythtv"
	AptSrc_AddSource "deb http://www.yttron.as.ro/ sarge main"
	AptSrc_WriteSourcesList >/etc/apt/sources.list
fi

SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ sarge main
# Pluto sources offline - end"
echo "$SourcesOffline" >/etc/apt/sources.list.offline

## Setup pluto's apt.conf
pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
'
echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto


## Ask the questions
sleep 0.5
exec 3>&1 1>/dev/tty
if [[ $UpgradeMode == "false" ]]; then
	clear

	echo "** Initial config script"
	echo ""
	echo "I need to ask some basic questions"	
	echo "Please see http://plutohome.com/support"
	echo "click Quick Start Guide, and Installing Software"
	echo "for details."
	echo ""
	echo "Note: Hybrid means this PC will not only be the main server, but you will"
	echo "also hook it up to a TV and use it as a media director.  Core means it is"
	echo "a server only, and will not be used as a media station."

	while :; do
		Type=$(Ask "Should this be a Core, Hybrid or Other? [c/H/o]")

		CoreDT=7
		if [[ "$Type" == o || "$Type" == O ]]; then
			CoreDT=$(Ask "Enter the device template number")
			break
		elif [[ "$Type" == h || "$Type" == H || "$Type" == c || "$Type" == C || -z "$Type" ]]; then
			if [[ -z "$Type" ]]; then
				Type="H"
			fi
			break
		else
			echo "Invalid answer '$Type'"
		fi
	done

	clear
	echo ""
	echo "If you have an active internet connection, Pluto can use it to perform"
	echo "various task : syncronize your computer clock, automaticaly send bug"
	echo "reports, install aditional software, perform updates ..."
	echo "You can also switch from online/offline mode later by using the web"
	echo "administration interface"
	echo
	echo "Important : Keep in mind that when you don't have an active internet"
	echo "connection, this tasks can slow down your computer and an negative"
	echo "answer here would be the right choice."

	while :; do
		UseInternet=$(Ask "Should Pluto use your internet connection ? [Y/n]")
		if [[ -z "$UseInternet" || "$UseInternet" == y || "$UseInternet" == Y || "$UseInternet" == n || "$UseInternet" == N ]]; then
			break
		else
			echo "Invalid answer '$UseInternet'"
		fi
	done

#<-mkr_B_via_b->
	DefaultNetwork="192.168.80"
#<-mkr_B_via_e->
#
	
	clear
	echo ""
	echo "Enter either 1, 2, or 3 numbers separated with periods, such as 192.168.1, 10.12, or 10"
	echo "Press enter to use the default range of $DefaultNetwork"

	while :;do
		NetworkInput=$(Ask "What is the internal network adress ?")

		if [[ "$NetworkInput" != "" ]] ;then
			error=false
			Network=""
			Digits_Count=0
			for Digits in $(echo $NetworkInput | tr '.' ' ') ;do
				[[ "$Digits" == *[^0-9]* ]] && error=true

				[[ $Digits -lt 0 || $Digits -gt 255 ]] && error=true


				if [[ "$Network" == "" ]] ;then
					Network="$Digits"
				else
					Network="${Network}.${Digits}"
				fi

				Digits_Count=$(( $Digits_Count + 1 ))
			done
			[[ $Digits_Count -lt 1 || $Digits_Count -gt 3 ]] && error=true

			[[ "$error" == "true" ]] && continue
		else
			Network="$DefaultNetwork"
			Digits_Count="3"
		fi

		NETsetting=$(/usr/pluto/install/Initial_Network_Config.sh "$Network" "$Digits_Count")
		DHCPsetting=$(/usr/pluto/install/Initial_DHCP_Config.sh "$Network" "$Digits_Count")
		break
	done


	clear
	echo ""
	echo "You need to answer 'Y' below if you want Plug-and-play or extra media"
	echo "directors."

	while :; do
#<-mkr_B_via_b->
		DHCP=$(Ask "Run a DHCP server? [Y/n]")
#<-mkr_B_via_e->
#
		if [[ -z "$DHCP" || "$DHCP" == y || "$DHCP" == Y || "$DHCP" == n || "$DHCP" == N ]]; then
			break
		else
			echo "Invalid answer '$DHCP'"
		fi
	done
fi

exec 1>&3 3>&-
sleep 0.5

mkdir -p /etc/apt/apt.conf.d/
ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline

## Setup Offline Mode
if [[ "$UpgradeMode" == "true" ]]; then
	tmp_OfflineMode=$(grep ^OfflineMode /.backup/pluto.conf)
	tmp_OfflineMode="${tmp_OfflineMode#*=}"
	tmp_OfflineMode="${tmp_OfflineMode// }"
	if [[ "$tmp_OfflineMode" == true || "$tmp_OfflineMode" == 1 ]]; then
		UseInternet=N
	fi
fi

OfflineMode="false"
if [[ "$UseInternet" == "N" || "$UseInternet" == "n" ]] ;then
	OfflineMode="true"
	OfflineApt='
Acquire::http::timeout "-1";
Acquire::ftp::timeout "-1";
APT::Get::Download "false";
APT::Get::Fix-Missing "true";
APT::Get::quiet "1";
Dir::Etc::sourcelist "sources.list.offline";
'
	mkdir -p /usr/pluto/var
	echo -e $OfflineApt > /usr/pluto/var/apt.conf.offline
fi

## installing BonusCD at this point
sleep 0.5
exec 3>&1 1>/dev/tty
. /usr/pluto/install/BonusCdMenu.sh
exec 1>&3 3>&-
sleep 0.5

## Setup pluto.conf
PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
#<-mkr_b_videowizard_b->
UseVideoWizard = 1
#<-mkr_b_videowizard_e->
TestInstallation = 0
PK_Device = 1
Activation_Code = $Activation_Code
PK_Installation = 1
PK_Users = 1
OfflineMode = $OfflineMode
Display = 0
LogLevels = 1,5,7,8
#ImmediatelyFlushLog = 1
"
echo "$PlutoConf" >/etc/pluto.conf

if [[ "$UpgradeMode" == "true" ]];then
	cp /.backup/pluto.conf /etc/pluto.conf
	cp /.backup/timezone /etc/
	ln -sf /usr/share/zoneinfo/"$(</etc/timezone)" /etc/localtime
	mkdir -p /var/lib/mysql
	mv /.backup/mysql/* /var/lib/mysql
fi

## Put a temporar nis config files that will prevent ypbind to start
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


## Install the software
apt-get update
if apt-get -y -f install libsepol1 libselinux1 binutils patch perl-modules dpkg dpkg-dev; then
	if [[ -f /usr/bin/dpkg-scanpackages ]]; then
		# regenerate packages.gz
		pushd /usr/pluto/deb-cache/dists/sarge/main/binary-i386 >/dev/null
		echo "Regenerating Packages.gz in debcache. This will require 1 minute. Please wait."
		dpkg-scanpackages . /dev/null | sed 's,\./,dists/sarge/main/binary-i386/,g' | tee Packages | gzip -9c > Packages.gz
		popd >/dev/null
		# update apt cache
		echo "Updating apt cache files ..."
		apt-get update 1>/dev/null 2>/dev/null
	fi

	Release="Origin: Pluto
Label: Pluto
Suite: 20dev
Version: 2.0.0.44
Codename: the_thing
Architectures: i386
Components: main
Description: Pluto Home, Brilliant Living
MD5Sum:"
	pushd /usr/pluto/deb-cache/dists/sarge >/dev/null
	echo "$Release" >Release
	gunzip -c main/binary-i386/Packages.gz >main/binary-i386/Packages
	find main/ -type f -exec md5sum '{}' ';' |
		while read line; do
			MD5sum=$(echo "$line" | awk '{ print $1 }')
			Filename=$(echo "$line" | awk '{ print $2 }')
			Size=$(stat -c '%s' "$Filename")
			printf " %s %16u %s\n" "$MD5sum" "$Size" "$Filename" >>Release
		done
	# end find
	popd >/dev/null
else
	echo "Failed to install initial set of packages"
	read
fi
if ! apt-get -y -f install pluto-dcerouter; then
	echo "Failed to install Pluto software"
	read
fi

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

## Initial config
if [[ "$UpgradeMode" == "false" ]] ;then
	# Create installation
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '$Activation_Code')"
	RunSQL "$Q"

	# Add Core
	CoreDev=$(NewDev -d "$CoreDT")
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$CoreDev'"
	RunSQL "$Q"

	# Create hybrid
	if [[ "$Type" == "H" || "$Type" == "h" ]]; then
		HybDev=$(NewDev -d 28 -C "$CoreDev")
		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$HybDev'"
		RunSQL "$Q"
	fi

	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$CoreDev',32,'$NETsetting')"
	RunSQL "$Q"
#<-mkr_B_via_b->
	[[ "$DHCP" != n && "$DHCP" != N ]]
#<-mkr_B_via_e->
#

## Install extra packages
sleep 0.5
exec 3>&1 1>/dev/tty
while :; do
	clear
	echo
	ExtraPkg=$(Ask "Do you want to add extra packages? [y/N]")
	if [[ "$ExtraPkg" == y || "$ExtraPkg" == Y ]]; then
		while :; do
			ExtraRepository=$(Ask "Add a new repository? [y/N]")
			if [[ "$ExtraRepository" == y || "$ExtraRepository" == Y ]]; then
				if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
					ExtraRepositoryPath=$(Ask "Enter repository path")
					AptSrc_AddSource "$ExtraRepositoryPath"
					AptSrc_WriteSourcesList >/etc/apt/sources.list
					apt-get update
				fi
				break
			elif [[ "$ExtraRepository" == n || "$ExtraRepository" == N || -z "$ExtraRepository" ]]; then
				break
			else
				echo "--> Invalid answer: '$ExtraRepository'"
			fi
		done
		ExtraPkgName=$(Ask "Enter package name")
		apt-get install "$ExtraPkgName"
	elif [[ "$ExtraPkg" == n || "$ExtraPkg" == N || -z "$ExtraPkg" ]]; then
		break
	else
		echo "--> Invalid answer: '$ExtraPkg'"
	fi
done
exec 1>&3 3>&-
sleep 0.5
# XXX: No error checking

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/generateRcScripts.sh

# remove ide-generic from /etc/modules
sed -i 's/^ide-generic$/#&/g' /etc/modules

# add mousedev to /etc/modules
if ! grep -q '^mousedev$' /etc/modules; then
	echo mousedev >>/etc/modules
fi

# detect the timezone of this machine
if [[ "$OfflineMode" == false ]]; then
	/usr/pluto/bin/Timezone_Detect.sh
fi

/usr/pluto/install/Initial_Config_Finish.sh


## Remove the backup
if [[ "$UpgradeMode" == "true" ]];then
	rm -rf /.backup
fi

exit 0
