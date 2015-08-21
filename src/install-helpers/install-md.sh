#!/bin/bash

if [[ -n "$HEADER_install_md" ]]; then
        return 0
fi
HEADER_install_md=included

. /usr/pluto/install/install-common.sh

[[ -e /usr/pluto/bin/Config_Ops.sh ]] && . /usr/pluto/bin/Config_Ops.sh
[[ -e /usr/pluto/bin/SQL_Ops.sh ]] && . /usr/pluto/bin/SQL_Ops.sh
[[ -e /usr/pluto/bin/Utils.sh ]] && . /usr/pluto/bin/Utils.sh

###########################################################
### Setup global variables
###########################################################

FSTAB_FILE="/etc/fstab"
DEVID_FILE="/etc/Disked_DeviceID"

#INSTALL_KUBUNTU_DESKTOP="yes"
INSTALL_KUBUNTU_DESKTOP="no"

##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################

setup_fstab() {
	# Hmm, let's see if the mounts are here, if not add them
	check=$(grep 192.168.80.1 "${FSTAB_FILE}" || :)
	if [ -z "$check" ] ; then
		cat <<-EOF >> "${FSTAB_FILE}"
			192.168.80.1:/usr/pluto/var/		/usr/pluto/var/		nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/usr/pluto/orbiter		/usr/pluto/orbiter	nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/usr/pluto/keys		/usr/pluto/keys		nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/usr/pluto/deb-cache	/usr/pluto/deb-cache    nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/var/spool/asterisk	/var/spool/asterisk     nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/home			/home			nfs4 retrans=10,timeo=50 1 1
			192.168.80.1:/home/cameras		/home/cameras		nfs4 retrans=10,timeo=50 1 1
			EOF
	fi
	mkdir -p /usr/pluto/var/
	mkdir -p /usr/pluto/orbiter
	mkdir -p /usr/pluto/keys
	mkdir -p /usr/pluto/deb-cache
	mkdir -p /var/spool/asterisk
	mkdir -p /home/cameras
	mount -a || :
}

setup_plutoconf() {
	. /usr/pluto/bin/Config_Ops.sh

	# get PK_Device from "/etc/Disked_DeviceID", created by interactor
	DEVICE=$(cat "$DEVID_FILE")
	[ -n "$PK_Device" ] || ConfSet "PK_Device" "$DEVICE"

	# TODO: start_interactor already did this and put it in the DB
	# TODO: get the data from the DB instead?
	# get PK_Distro from OS
	DISTRO=$(lsb_release -cs)
	case "$DISTRO" in
		precise)
			distro=$DEVICEDATA_DISTRO_Ubuntu_Precise_CONST 	;;
		trusty)
			distro=$DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST 	;;
		wheezy)
			distro=$DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST ;;
		jessie)
			distro=$DEVICEDATA_DISTRO_Raspbian_Jessie_CONST ;;
	esac
	[ -n "$PK_Distro" ] || [ -n "$distro" ] && ConfSet "PK_Distro" "$distro"

	[ -n "$MySqlHost" ] || ConfSet "MySqlHost" "192.168.80.1"
	[ -n "$MySqlUser" ] || ConfSet "MySqlUser" "root"
	[ -n "$MySqlPassword" ] || ConfSet "MySqlPassword" ""
	[ -n "$MySqlDBName" ] || ConfSet "MySqlDBName" "pluto_main"
	[ -n "$DCERouter" ] || ConfSet "DCERouter" "192.168.80.1"
	[ -n "$MySqlPort" ] || ConfSet "MySqlPort" "3306"
	[ -n "$DCERouterPort" ] || ConfSet "DCERouterPort" "3450"
	[ -n "$AutostartCore" ] || ConfSet "AutostartCore" "0"
	[ -n "$AutostartMedia" ] || ConfSet "AutostartMedia" "1"
	[ -n "$AVWizardDone" ] || ConfSet "AVWizardDone" "0"
}


##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################


###########################################################
### Setup Functions - Error checking and logging and trapping
###########################################################

VerifyExitCode () {
	local EXITCODE=$?
	if [ "$EXITCODE" != "0" ] ; then
		echo "An error (Exit code $EXITCODE) occured during the last action"
		echo "$1"
		exit 1
	fi
}

StatsMessage () {
	printf "$(date) - $* \n"
}

###########################################################
### Setup Functions - General functions
###########################################################

MD_Find_Core () {
	# TODO: Use service discovery to find the Core's IP address
	:
}

MD_System_Level_Prep () {
	StatsMessage "Setting up /etc/apt/sources.list"
	APT_CONFIG=$(grep "linuxmce" /etc/apt/sources.list)
	if [ -z "$APT_CONFIG" ] ; then
		TARGET_REPO_NAME="main"
		cat <<-EOF > /etc/apt/sources.list
			#deb http://10.10.42.99/$TARGET_RELEASE-$TARGET_ARCH/ ./
			deb file:/usr/pluto/deb-cache/$DEB_CACHE ./
			deb http://deb.linuxmce.org/ubuntu/ $TARGET_RELEASE $TARGET_REPO_NAME
			deb http://deb.linuxmce.org/ $TARGET_RELEASE $TARGET_REPO_NAME
			deb $TARGET_REPO $TARGET_RELEASE main restricted universe multiverse
			deb $TARGET_REPO $TARGET_RELEASE-updates main restricted universe multiverse
			deb $SECURITY_REPO $TARGET_RELEASE-security main restricted universe multiverse
			EOF
	fi

	## Setup initial ssh access
	StatsMessage "Setting up SSH"
	[[ -f /usr/pluto/keys/id_dsa_pluto.pub ]] && mkdir -p /root/.ssh && cat /usr/pluto/keys/id_dsa_pluto.pub >> /root/.ssh/authorized_keys
}

MD_Seamless_Compatability () {
	: # no-op
}

MD_Preseed () {
	: # no-op
}

MD_Update () {
	StatsMessage "Setting up apt gpg keys"
	gpgUpdate	# install-common.sh

	apt-get -f -y install

	# FIXME: need to fix bootscripts?  need to fix/update install-scripts
	apt-get -f -y install --no-install-recommends pluto-boot-scripts
	VerifyExitCode "pluto-boot-scripts failed"
}

MD_Install_Kernel () {
	StatsMessage "Installing kernel headers"

	#TODO get as much of this from database as possible
	# run any device specific firstboot add-on scripts here
	ret=""
	for f in /etc/init.d/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: $f _kernel - Begin"
		. "$f" || :
		$(basename "$f")_kernel || :
		ret="0"
		StatsMessage "Running device specific script: $f _kernel - End"
	done

	if [[ "$ret" != "0" ]] ; then
		StatsMessage "Setting up HardWare Enablement stack"
		. /usr/pluto/bin/Config_Ops.sh
		ConfSet "LTS_HES" "$TARGET_KVER_LTS_HES"

		StatsMessage "Installing ubuntu kernel headers"
		#Install headers and run depmod for the seamless integraton function, ensure no errors exist
		apt-get -f -y --no-install-recommends install linux-headers-generic"$TARGET_KVER_LTS_HES"
		VerifyExitCode "Install linux headers package failed"

		StatsMessage "Installing kernel"
		apt-get -f -y install --install-recommends linux-generic"$TARGET_KVER_LTS_HES"
		VerifyExitCode "Install linux kernel package failed"
	fi

	#StatsMessage "Installing firmware"
	#apt-get -f -y install --no-install-recommends linux-firmware || :
	#VerifyExitCode "Install linux firmware failed" || :

	StatsMessage "Running depmod"
	TARGET_KVER=$(ls -vd /lib/modules/[0-9]* | sed 's/.*\///g' | tail -1)
	depmod -v "$TARGET_KVER"
	VerifyExitCode "depmod failed for $TARGET_KVER"
}

MD_Install_Packages () {
	StatsMessage "Installing packages to MD"

	StatsMessage "Setting locales"
	# FIXME: locales are already generated, trigger regen?
	echo "en_US.UTF-8 UTF-8" >/etc/locale.gen

	StatsMessage "Installing X.Org"
	#Install X prior to linuxmce to get lts_hwe pkgs.

	case "$TARGET_RELEASE" in
		"precise")	# 1204
			apt-get -f -y --install-recommends install \
				xserver-xorg"$TARGET_KVER_LTS_HES" \
				xserver-xorg-video-all"$TARGET_KVER_LTS_HES" \
				libgl1-mesa-glx"$TARGET_KVER_LTS_HES"
				VerifyExitCode "Installing X.Org failed"
			;;
		"trusty")	# 1404
			apt-get -f -y --install-recommends install \
				xserver-xorg-core-"$TARGET_KVER_LTS_HES" \
				xserver-xorg"$TARGET_KVER_LTS_HES" \
				xserver-xorg-video-all"$TARGET_KVER_LTS_HES" \
				xserver-xorg-input-all"$TARGET_KVER_LTS_HES" \
				libwayland-egl1-mesa"$TARGET_KVER_LTS_HES"
				VerifyExitCode "Installing X.Org failed"
			;;
	esac

	StatsMessage "Installing primary MD packages"
	apt-get -f -y --no-install-recommends install lmce-md-meta nfs-common openssh-server pastebinit locales
	VerifyExitCode "Installing lmce-md-meta failed"

	if [[ "$INSTALL_KUBUNTU_DESKTOP" != "no" ]]; then
		StatsMessage "Installing kubuntu desktop packages"
		apt-get -f -y --no-install-recommends install kubuntu-desktop
		VerifyExitCode "kubuntu-desktop"
	fi

	StatsMessage "Disabling display manager"
	#FIXME: dpkg-divert it so it does not get over-written?
	echo '/bin/false' >/etc/X11/default-display-manager

	# Update startup to remove kdm and network manager
	update-rc.d -f kdm remove

	StatsMessage "Disabling NetworkManager"
	update-rc.d -f NetworkManager remove
}

MD_Populate_Debcache () {
	StatsMessage "Creating deb-cache dir"
	# FIXME: is this the correct point here?
	mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
#	find /var/cache/apt/archives/ -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/$DEB_CACHE \;
#	/usr/pluto/bin/update-debcache.sh /usr/pluto/deb-cache/$DEB_CACHE
	:
}

MD_Cleanup () {
	StatsMessage "Cleaning up from package installations..."
	#Copy the orbiter activation command to the MD's desktop
	mkdir -p /root/Desktop
	cp -r /etc/skel/Desktop/* /root/Desktop

	#Remove the xorg file(s) from installation, need to start with a fresh slate
	rm -f /etc/X11/xorg.conf*

	#Clean up apt from the installs
	apt-get -y clean
	VerifyExitCode "APT Clean"

	#Create a list of installed packages
	#COLUMNS=1024 dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder
}

##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################


dontrun() {
	if [ ! -f "$DEVID_FILE" ]; then
		echo "ERR: interactor has not yet created '$DEVID_FILE'."
		return 10
	fi

	#TODO get as much of this from database as possible
	# run any device specific firstboot add-on scripts here
	for f in /etc/init.d/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: $f _preinst - Begin"
		. "$f" || :
		$(basename "$f")_preinst || :
		StatsMessage "Running device specific script: $f _preinst - End"
	done

	MD_Find_Core
	MD_System_Level_Prep
	#MD_Seamless_Compatability
	#MD_Preseed
	MD_Update
	setup_fstab
	setup_plutoconf
	MD_Install_Kernel
	MD_Install_Packages
	MD_Populate_Debcache
	MD_Cleanup

	# run any device specific firstboot add-on scripts here
	for f in /etc/init.d/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: $f _postinst - Begin"
		. "$f" || :
		$(basename "$f")_postinst || :
		StatsMessage "Running device specific script: $f _postinst - End"
	done

	StatsMessage "Setting firstboot = false"
	ConfSet "FirstBoot" "false"
	sync

	StatsMessage "Rebooting in 5 seconds"
	sleep 1
	StatsMessage "Rebooting in 4 seconds"
	sleep 1
	StatsMessage "Rebooting in 3 seconds"
	sleep 1
	StatsMessage "Rebooting in 2 seconds"
	sleep 1
	StatsMessage "Rebooting in 1 seconds"
	sleep 1
	StatsMessage "Rebooting"
	reboot
	return 0
}
