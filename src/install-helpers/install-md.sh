#!/bin/bash

if [[ -n "$HEADER_install_md" ]]; then
	return 0
fi
HEADER_install_md=included

# include all the common install helpers
. /usr/pluto/install/install-common.sh

[[ -e $BASE_DIR/bin/Config_Ops.sh ]] && . $BASE_DIR/bin/Config_Ops.sh
[[ -e $BASE_DIR/bin/SQL_Ops.sh ]] && . $BASE_DIR/bin/SQL_Ops.sh
[[ -e $BASE_DIR/bin/Utils.sh ]] && . $BASE_DIR/bin/Utils.sh

###########################################################
### Setup global variables
###########################################################

FSTAB_FILE="/etc/fstab"
DEVID_FILE="/etc/Disked_DeviceID"

#INSTALL_KUBUNTU_DESKTOP="yes"
INSTALL_KUBUNTU_DESKTOP="no"

# Get the Core's IP Address using the locator
CORE_IP=$($BASE_DIR/bin/core-locator)

###########################################################
### Config setup fns.
###########################################################

MD_Setup_Fstab() {
	# Hmm, let's see if the mounts are here, if not add them
	check=$(grep "${CORE_IP}" "${FSTAB_FILE}" || :)
	if [ -z "$check" ] ; then
		cat <<-EOF >> "${FSTAB_FILE}"
			${CORE_IP}:$BASE_DIR/var/		$BASE_DIR/var/		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:$BASE_DIR/orbiter		$BASE_DIR/orbiter	nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:$BASE_DIR/keys		$BASE_DIR/keys		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:$BASE_DIR/deb-cache		$BASE_DIR/deb-cache    nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/var/spool/asterisk		/var/spool/asterisk     nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home			/home			nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home/cameras		/home/cameras		nfs4 retrans=10,timeo=50 1 1
			EOF
	fi
	mkdir -p $BASE_DIR/var/
	mkdir -p $BASE_DIR/orbiter
	mkdir -p $BASE_DIR/keys
	mkdir -p $BASE_DIR/deb-cache
	mkdir -p /var/spool/asterisk
	mkdir -p /home/cameras
	mount -a || :
}

MD_Setup_Plutoconf() {
	. $BASE_DIR/bin/Config_Ops.sh

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
	[[ -f $BASE_DIR/keys/id_dsa_pluto.pub ]] && mkdir -p /root/.ssh && cat $BASE_DIR/keys/id_dsa_pluto.pub >> /root/.ssh/authorized_keys
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
	PreSeed_DebConf		# install-common.sh
}

MD_Update () {
	StatsMessage "Setting up apt gpg keys"
	gpgUpdate		# install-common.sh

	apt-get -f -y install

	AptDistUpgrade		# install-common.sh
}

MD_Install_Kernel () {
	Install_Kernel		# install-common.sh
}

MD_Install_Packages () {
	Install_X		# install-common.sh
	Install_KUbuntu_Desktop	# install-common.sh
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
	#### This is a generic MD firstboot file

	if [ ! -f "$DEVID_FILE" ]; then
		echo "ERR: interactor has not yet created '$DEVID_FILE'."
		return 10
	fi

	#TODO get as much of this from database as possible
	# run any device specific firstboot add-on scripts here
	for f in $BASE_DIR/install/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: ${f}_preinst - Begin"
		. "$f" || :
		$(basename "$f")_preinst || :
		StatsMessage "Running device specific script: ${f}_preinst - End"
	done

	MD_Config_MySQL_Client
	MD_System_Level_Prep
	#MD_Seamless_Compatability
	#MD_Preseed
	MD_Update
	MD_Setup_Fstab
	MD_Setup_Plutoconf

        # run any device specific firstboot add-on kernel config here
        ret=""
        for f in $BASE_DIR/install/firstboot_lmce_* ; do
                StatsMessage "Running device specific script: ${f}_kernel - Begin"
                . "$f" || :
                $(basename "$f")_kernel || :
                ret="0"
                StatsMessage "Running device specific script: ${f}_kernel - End"
        done
        # Default kernel install, if the above is not run
        if [[ "$ret" != "0" ]] ; then
		MD_Install_Kernel
        fi

	MD_Install_Packages	# X and kubuntu-desktop
	Disable_DisplayManager
	Disable_NetworkManager
	MD_Populate_Debcache
	MD_Cleanup

	# run any device specific firstboot add-on scripts here
	for f in $BASE_DIR/install/firstboot_lmce_* ; do
		StatsMessage "Running device specific script: ${f}_postinst - Begin"
		. "$f" || :
		$(basename "$f")_postinst || :
		StatsMessage "Running device specific script: ${f}_postinst - End"
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
