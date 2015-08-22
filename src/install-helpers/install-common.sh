#!/bin/bash

if [[ -n "$HEADER_install_common" ]]; then
	return 0
fi
HEADER_install_common=included

###########################################################
### Setup global variables
###########################################################
log_file=/var/log/LinuxMCE_Setup.log

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
DEB_CACHE="$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH"
REPO="main"

TARGET_KVER_LTS_HES=""
[[ "precise" == "$TARGET_RELEASE" ]] && TARGET_KVER_LTS_HES="-lts-trusty"
[[ "trusty" == "$TARGET_RELEASE" ]] && TARGET_KVER_LTS_HES="-lts-utopic"

DISTRO="$TARGET_RELEASE"
LOCAL_REPO_BASE=/usr/pluto/deb-cache/$DEB_CACHE
LOCAL_REPO_DIR=./

DT_CORE=1
DT_HYBRID=2
DT_MEDIA_DIRECTOR=3

DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST=19
DEVICEDATA_DISTRO_Ubuntu_Precise_CONST=20
DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST=21
DEVICEDATA_DISTRO_Raspbian_Jessie_CONST=22

mce_wizard_data_shell=/tmp/mce_wizard_data.sh

MESSGFILE=/tmp/messenger

#Setup Pathing
export PATH=$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# TODO: make compatible with raspbian
case "$TARGET_ARCH" in
	i386|amd64)
		TARGET_REPO="http://archive.ubuntu.com/ubuntu/"
		SECURITY_REPO="http://security.ubuntu.com/ubuntu/"
		;;
	armhf)
		TARGET_REPO="http://ports.ubuntu.com/"
		SECURITY_REPO="http://ports.ubuntu.com/"
		;;
esac

###########################################################
### Setup Functions - Error checking and logging
###########################################################

StatsMessage () {
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
	# check if online first? TODO: we have a fn for this
	if ping -c 1 google.com; then
		#sed -i 's/#deb/deb/g' /etc/apt/sources.list
		gpgs=$(apt-get update |& grep -s NO_PUBKEY | awk '{ print $NF }' | cut -c 9-16);
		if [ -n "$gpgs" ]; then
			echo "$gpgs" | while read gpgkeys; do
				gpg --keyserver pgp.mit.edu --recv-keys "$gpgkeys"
				gpg --export --armor "$gpgkeys" | apt-key add -
			done
		fi
	fi
}


Fix_LSB_Data () {
	case "$TARGET_DISTRO" in
		raspbian)
			# raspbian doesn't come with lsb-release by default???
			cat <<-EOF > /etc/lsb-release
				DISTRIB_ID=Raspbian
				DISTRIB_CODENAME=$TARGET_RELEASE
				EOF
			;;
	esac
}

Notify_Reboot () {
	/usr/share/update-notifier/notify-reboot-required
}

Disable_DisplayManager () {
	StatsMessage "Disabling display manager"
	mkdir -p "/etc/X11"
	echo "/bin/false" >/etc/X11/default-display-manager
	update-rc.d -f kdm remove 2>/dev/null || :
	update-rc.d -f lightdm remove 2>/dev/null || :
}

Disable_NetworkManager () {
	update-rc.d -f NetworkManager remove || :
}

CreateBasePackagesFile () {
	StatsMessage "Setting up deb-cache/ packages files"
	#Make sure there is are Packages files on the MD so apt-get update does not fail
	if [[ ! -f /usr/pluto/deb-cache/Packages.gz ]] ; then
		mkdir -p /usr/pluto/deb-cache/
		rm -f /usr/pluto/deb-cache/Packages
		touch  /usr/pluto/deb-cache/Packages
		gzip -9c < /usr/pluto/deb-cache/Packages > /usr/pluto/deb-cache/Packages.gz
	fi

	StatsMessage "Setting up deb-cache/$DEB_CACHE packages files"
	#Make sure there is are Packages files on the MD so apt-get update does not fail
	if [[ ! -f /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz ]] ; then
		mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
		rm -f /usr/pluto/deb-cache/$DEB_CACHE/Packages
		touch  /usr/pluto/deb-cache/$DEB_CACHE/Packages
		gzip -9c < /usr/pluto/deb-cache/$DEB_CACHE/Packages > /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz
	fi
}

UpdateDebCache () {
	StatsMessage "Updating deb-cache package files"
	/usr/pluto/bin/UpdateDebCache.sh
}

PreSeed_DebConf () {
	StatsMessage "PreSeeding package installation preferences"

	debconf-set-selections /usr/pluto/install/preseed.cfg
	VerifyExitCode "debconf-set-selections - preseed data"
}

Config_MySQL_Client () {
	StatsMessage "Setting up mysql client - server hostname"
	# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
	cat <<-EOF > /root/.my.cnf
		[client]
		host = dcerouter
		EOF
}

