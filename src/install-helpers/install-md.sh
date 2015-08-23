#!/bin/bash

if [[ -n "$HEADER_install_md" ]]; then
	return 0
fi
HEADER_install_md=included

# include all the common install helpers
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

# Get the Core's IP Address using the locator
CORE_IP=$(/usr/pluto/bin/core-locator)

###########################################################
### Config setup fns.
###########################################################

MD_Setup_Fstab() {
	# Hmm, let's see if the mounts are here, if not add them
	check=$(grep "${CORE_IP}" "${FSTAB_FILE}" || :)
	if [ -z "$check" ] ; then
		cat <<-EOF >> "${FSTAB_FILE}"
			${CORE_IP}:/usr/pluto/var/		/usr/pluto/var/		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/usr/pluto/orbiter		/usr/pluto/orbiter	nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/usr/pluto/keys		/usr/pluto/keys		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/usr/pluto/deb-cache		/usr/pluto/deb-cache    nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/var/spool/asterisk		/var/spool/asterisk     nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home			/home			nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home/cameras		/home/cameras		nfs4 retrans=10,timeo=50 1 1
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

MD_Setup_Plutoconf() {
	. /usr/pluto/bin/Config_Ops.sh

	# get PK_Device from "/etc/Disked_DeviceID", created by interactor
	DEVICE=$(cat "$DEVID_FILE")
	[ -n "$PK_Device" ] || ConfSet "PK_Device" "$DEVICE"

	[ -n "$PK_Distro" ] || [ -n "$TARGET_DISTRO_ID" ] && ConfSet "PK_Distro" "$TARGET_DISTRO_ID"
	[ -n "$MySqlHost" ] || ConfSet "MySqlHost" "${CORE_IP}"
	[ -n "$MySqlUser" ] || ConfSet "MySqlUser" "root"
	[ -n "$MySqlPassword" ] || ConfSet "MySqlPassword" ""
	[ -n "$MySqlDBName" ] || ConfSet "MySqlDBName" "pluto_main"
	[ -n "$DCERouter" ] || ConfSet "DCERouter" "${CORE_IP}"
	[ -n "$MySqlPort" ] || ConfSet "MySqlPort" "3306"
	[ -n "$DCERouterPort" ] || ConfSet "DCERouterPort" "3450"
	[ -n "$AutostartCore" ] || ConfSet "AutostartCore" "0"
	[ -n "$AutostartMedia" ] || ConfSet "AutostartMedia" "1"
	[ -n "$AVWizardDone" ] || ConfSet "AVWizardDone" "0"
}

###########################################################
### Setup Functions - General functions
###########################################################

MD_Copy_SSH_Keys () {
	## Setup initial ssh access
	StatsMessage "Setting up SSH"
	[[ -f /usr/pluto/keys/id_dsa_pluto.pub ]] && mkdir -p /root/.ssh && cat /usr/pluto/keys/id_dsa_pluto.pub >> /root/.ssh/authorized_keys
}

MD_System_Level_Prep () {
	ConfigSources		# install-common.sh
	MD_Copy_SSH_Keys	# install-md.sh
}

MD_Seamless_Compatability () {
	# FIXME: no longer required
	: # no-op
}

MD_Preseed () {
	PreSeed_DebConf	# install-common.sh
}

MD_Update () {
	StatsMessage "Setting up apt gpg keys"
	gpgUpdate	# install-common.sh

	apt-get -f -y install

	AptDistUpgrade	# install-common.sh
}

MD_Install_Kernel () {
	StatsMessage "Installing kernel headers"

	# TODO: get as much of this from database as possible
	# run any device specific firstboot add-on kernel config here
	ret=""
	for f in /usr/pluto/install/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: $f _kernel - Begin"
		. "$f" || :
		$(basename "$f")_kernel || :
		ret="0"
		StatsMessage "Running device specific script: $f _kernel - End"
	done

	# Default kernel installation is ubuntu based, if the above is not run
	if [[ "$ret" != "0" ]] ; then
		StatsMessage "Setting up Ubuntu HardWare Enablement Stack"
		. /usr/pluto/bin/Config_Ops.sh
		ConfSet "LTS_HES" "$LTS_HES"

		StatsMessage "Installing Ubuntu kernel"
		apt-get -f -y install --install-recommends linux-generic"$LTS_HES" linux-image-generic"$LTS_HES"
		VerifyExitCode "Install linux kernel package failed"

		StatsMessage "Installing Ubuntu kernel headers"
		#Install headers and run depmod for the seamless integraton function, ensure no errors exist
		apt-get -f -y --no-install-recommends install linux-headers-generic"$LTS_HES"
		VerifyExitCode "Install linux headers package failed"
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
				xserver-xorg"$LTS_HES" \
				xserver-xorg-video-all"$LTS_HES" \
				libgl1-mesa-glx"$LTS_HES"
				VerifyExitCode "Installing X.Org failed"
			;;
		"trusty")	# 1404
			apt-get -f -y --install-recommends install \
				xserver-xorg-core-"$LTS_HES" \
				xserver-xorg"$LTS_HES" \
				xserver-xorg-video-all"$LTS_HES" \
				xserver-xorg-input-all"$LTS_HES" \
				libwayland-egl1-mesa"$LTS_HES"
				VerifyExitCode "Installing X.Org failed"
			;;
		*)	# *
			apt-get -f -y --install-recommends install \
				xserver-xorg \
				xserver-xorg-video-all
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
}

MD_Config_MySQL_Client () {
	StatsMessage "Setting up mysql client - server hostname"
	# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
	cat <<-EOF > /root/.my.cnf
		[client]
		host = dcerouter
		EOF
}

MD_Populate_Debcache () {
	StatsMessage "Creating deb-cache dir"
	CreateBasePackagesFiles		# install-common.sh
	#UpdateDebCache "$DEB_CACHE"	# install-common.sh
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


return 0

###########################################################
###########################################################
### Deprecated Functions
###########################################################
###########################################################



##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################
##########################################################################################################


dontrun() {
#### this is the generic MD firstrun file
	if [ ! -f "$DEVID_FILE" ]; then
		echo "ERR: interactor has not yet created '$DEVID_FILE'."
		return 10
	fi

	#TODO get as much of this from database as possible
	# run any device specific firstboot add-on scripts here
	for f in /usr/pluto/install/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: $f _preinst - Begin"
		. "$f" || :
		$(basename "$f")_preinst || :
		StatsMessage "Running device specific script: $f _preinst - End"
	done

MD_Config_MySQL_Client
	MD_System_Level_Prep
	#MD_Seamless_Compatability
	#MD_Preseed
	MD_Update
	MD_Setup_Fstab
	MD_Setup_Plutoconf
	MD_Install_Kernel
	MD_Install_Packages
	Disable_DisplayManager
	Disable_NetworkManager
	MD_Populate_Debcache
	MD_Cleanup

	# run any device specific firstboot add-on scripts here
	for f in /usr/pluto/install/firstboot_lmce_* ; do
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
