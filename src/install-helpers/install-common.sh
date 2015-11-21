#!/bin/bash

if [[ -n "$HEADER_install_common" ]]; then
	return 0
fi
HEADER_install_common=included

###########################################################
### Setup global variables
###########################################################
log_file=/var/log/LinuxMCE_Setup.log

BASE_DIR=/usr/pluto

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
DEB_CACHE="${TARGET_DISTRO}-${TARGET_RELEASE}-${TARGET_ARCH}"
REPO="main"

LOCAL_REPO_BASE=${BASE_DIR}/deb-cache/${DEB_CACHE}
LOCAL_REPO_DIR=./

DEVICE_TEMPLATE_Core=7
DEVICE_TEMPLATE_MediaDirector=28

DEVICEDATA_DISTRO_Ubuntu_Lucid_CONST=18
DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST=19
DEVICEDATA_DISTRO_Ubuntu_Precise_CONST=20
DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST=21
DEVICEDATA_DISTRO_Raspbian_Jessie_CONST=22
DEVICEDATA_DISTRO_Ubuntu_Xenial_CONST=23

mce_wizard_data_shell=/tmp/mce_wizard_data.sh

MESSGFILE=/tmp/messenger

#Setup Pathing
export PATH=$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# get PK_Distro from OS
case "$TARGET_RELEASE" in
	lucid)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Ubuntu_Lucid_CONST
		TARGET_REPO_DISTRO_SRC=20
		TARGET_REPO_LMCE_SRC=21	;;
	precise)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Ubuntu_Precise_CONST
		TARGET_REPO_DISTRO_SRC=24
		TARGET_REPO_LMCE_SRC=25
		TARGET_LTS_HES="-lts-trusty"	;;
	trusty)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST
		TARGET_REPO_DISTRO_SRC=24
		TARGET_REPO_LMCE_SRC=25
		TARGET_LTS_HES="-lts-utopic"	;;
	xenial)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Ubuntu_Xenial_CONST
		TARGET_REPO_DISTRO_SRC=24
		TARGET_REPO_LMCE_SRC=25
		TARGET_LTS_HES=""	;;
	wheezy)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST
		TARGET_REPO_DISTRO_SRC=22
		TARGET_REPO_LMCE_SRC=23	;;
	jessie)
		TARGET_DISTRO_ID=$DEVICEDATA_DISTRO_Raspbian_Jessie_CONST
		TARGET_REPO_DISTRO_SRC=22
		TARGET_REPO_LMCE_SRC=23	;;
esac

case "$TARGET_DISTRO" in
	ubuntu)
		case "$TARGET_ARCH" in
			i386|amd64)
				TARGET_REPO="http://archive.ubuntu.com/ubuntu/"
				SECURITY_REPO="http://security.ubuntu.com/ubuntu/"
				;;
			armhf)
				TARGET_REPO="http://ports.ubuntu.com/"
				SECURITY_REPO="http://ports.ubuntu.com/"
				;;
		esac	;;
	raspbian)
		TARGET_REPO="http://archive.raspbian.org/raspbian/"
		;;
esac

###########################################################
### Setup Functions - Error checking and logging
###########################################################

StatsMessage () {
	printf "`date` - $* \n"
}

StatusMessage () {
	printf "`date` - $* \n"
}

VerifyExitCode () {
	local EXITCODE=$?
	if [ "$EXITCODE" != "0" ] ; then
		echo "An error (Exit code $EXITCODE) occured during the last action"
		echo "$1"
		exit 1
	fi
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


###########################################################
### Setup Functions - General functions
###########################################################

gpgUpdate () {
	# This does an update, while adding gpg keys for any that are missing.
	gpgs=$(apt-get update |& grep -s NO_PUBKEY | awk '{ print $NF }' | cut -c 9-16);
	if [ -n "$gpgs" ]; then
		echo "$gpgs" | while read gpgkeys; do
			gpg --keyserver pgp.mit.edu --recv-keys "$gpgkeys"
			gpg --export --armor "$gpgkeys" | apt-key add -
		done
	fi
}

AptUpdate () {
	apt-get -qq update
	VerifyExitCode "apt-get update"
}

AptUpgrade () {
	apt-get -y -q -f --force-yes upgrade
	VerifyExitCode "apt-get upgrade"
}

AptDistUpgrade () {
	apt-get -y -q -f --force-yes dist-upgrade
	VerifyExitCode "apt-get dist-upgrade"
}

UpdateUpgrade () {
	#perform an update and a dist-upgrade
	StatsMessage "Performing an update and an upgrade to all components"
	gpgUpdate # AptUpdate
	AptDistUpgrade
}

TimeUpdate () {
	StatsMessage "Synchronizing time with an online server"
	#Update system time to match ntp server
	ntpdate ntp.ubuntu.com
}

Disable_NetworkManager () {
	:
}

ConfigSources () {
	StatsMessage "Configuring sources.list for LinuxMCE"

	. ${BASE_DIR}/install/AptSources.sh
	AptSrc_ParseSourcesList "/etc/apt/sources.list"
	AptSrc_AddSource "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
	AptSrc_AddSource "http://deb.linuxmce.org/${TARGET_DISTRO}/ ${TARGET_RELEASE} ${REPO}"
	AptSrc_WriteSourcesList
}

UpdateDebCache () {
	StatsMessage "Updating deb-cache package files"
	${BASE_DIR}/bin/UpdateDebCache.sh
}

Config_Device_Changes () {
	StatsMessage "Running ${BASE_DIR}/bin/Config_Device_Changes.sh"
	${BASE_DIR}/bin/Config_Device_Changes.sh
}

DisableSplash () {
	# disable plymouth splash for now. Could be replaced by own LMCE splash later
	sed -i 's/ splash//' /etc/default/grub
	/usr/sbin/update-grub
}

Install_Kernel () {
	StatsMessage "Installing kernel & headers"
	LTS_HES="${TARGET_LTS_HES}"

	StatsMessage "Installing Ubuntu kernel"
	apt-get -f -y install --install-recommends linux-generic${LTS_HES} linux-image-generic${LTS_HES}
	VerifyExitCode "Install linux kernel package failed"

	StatsMessage "Installing Ubuntu kernel headers"
	#Install headers and run depmod for the seamless integraton function, ensure no errors exist
	apt-get -f -y install linux-headers-generic${LTS_HES}
	VerifyExitCode "Install linux headers package failed"

	#StatsMessage "Installing firmware"
	#apt-get -f -y install linux-firmware || :
	#VerifyExitCode "Install linux firmware failed" || :

	StatsMessage "Running depmod"
	TARGET_KVER=$(ls -vd /lib/modules/[0-9]* | sed 's/.*\///g' | tail -1)
	depmod -v "$TARGET_KVER"
	VerifyExitCode "depmod failed for $TARGET_KVER"
}

ListXPkgs () {
	LTS_HES="${TARGET_LTS_HES}"
	case "$TARGET_RELEASE" in
		"precise")      # 1204
			echo "xserver-xorg${LTS_HES} xserver-xorg-video-all${LTS_HES} libgl1-mesa-glx${LTS_HES}"	;;
		"trusty")       # 1404
			echo "xserver-xorg-core${LTS_HES} xserver-xorg${LTS_HES} xserver-xorg-video-all${LTS_HES} xserver-xorg-input-all${LTS_HES} libwayland-egl1-mesa${LTS_HES}"	;;
		"xenial")       # 1604
			echo "xserver-xorg-core${LTS_HES} xserver-xorg${LTS_HES} xserver-xorg-video-all${LTS_HES} xserver-xorg-input-all${LTS_HES} libwayland-egl1-mesa${LTS_HES}"	;;
		*)      # *
			echo "xserver-xorg xserver-xorg-video-all"	;;
	esac
}

Install_X () {
	StatsMessage "Installing X.Org"
	#Install X prior to linuxmce to get lts_hwe pkgs.

	XPKGS=$(ListXPkgs)
	apt-get -f -y --install-recommends install ${XPKGS}
}

Install_HWE () {
	LTS_HES="${TARGET_LTS_HES}"
	XPKGS=$(ListXPkgs)
	apt-get -f -y install --install-recommends linux-generic${LTS_HES} linux-image-generic${LTS_HES} ${XPKGS}

	StatsMessage "Running depmod"
	TARGET_KVER=$(ls -vd /lib/modules/[0-9]* | sed 's/.*\///g' | tail -1)
	depmod -v "$TARGET_KVER"
	VerifyExitCode "depmod failed for $TARGET_KVER"
}

Install_KUbuntu_Desktop () {
	if [[ "$INSTALL_KUBUNTU_DESKTOP" != "no" ]]; then
		StatsMessage "Installing kubuntu desktop packages"
		apt-get -f -y --no-install-recommends install kubuntu-desktop
		VerifyExitCode "kubuntu-desktop"
	fi
}

addAdditionalTTYStart () {
	# TODO: this is ubuntu specific, alter to fn properly for debian/raspbian as well
	if [[ "$TARGET_RELEASE" = "lucid" ]] || [[ "$TARGET_RELEASE" = "precise" ]] || [[ "$TARGET_RELEASE" == "trusty" ]] || [[ "$TARGET_RELEASE" == "xenial" ]] ; then
		sed -i 's/23/235/' /etc/init/tty2.conf
		sed -i 's/23/235/' /etc/init/tty3.conf
		sed -i 's/23/235/' /etc/init/tty4.conf
	else
		echo "start on runlevel 5">>/etc/event.d/tty2
		echo "start on runlevel 5">>/etc/event.d/tty3
		echo "start on runlevel 5">>/etc/event.d/tty4
	fi
}

Notify_Reboot () {
	/usr/share/update-notifier/notify-reboot-required
}


return 0

###########################################################
###########################################################
### Deprecated Functions
###########################################################
###########################################################

