#!/bin/bash

if [[ -n "$HEADER_install_md" ]]; then
	return 0
fi
HEADER_install_md=included

. /usr/pluto/install/install-common.sh

###########################################################
### Setup global variables
###########################################################

FSTAB_FILE="/etc/fstab"
DEVID_FILE="/etc/Disked_DeviceID"

#INSTALL_KUBUNTU_DESKTOP="yes"
INSTALL_KUBUNTU_DESKTOP="no"

# Get the Core's IP Address using the locator
CORE_IP=$(${BASE_DIR}/bin/core-locator)

###########################################################
### Config setup fns.
###########################################################

MD_Setup_Fstab() {
	# Hmm, let's see if the mounts are here, if not add them
	check=$(grep "${CORE_IP}" "${FSTAB_FILE}" || :)
	if [ -z "$check" ] ; then
		cat <<-EOF >> "${FSTAB_FILE}"
			${CORE_IP}:${BASE_DIR}/var/		${BASE_DIR}/var/		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:${BASE_DIR}/orbiter		${BASE_DIR}/orbiter	nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:${BASE_DIR}/keys		${BASE_DIR}/keys		nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:${BASE_DIR}/deb-cache		${BASE_DIR}/deb-cache    nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/var/spool/asterisk		/var/spool/asterisk     nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home			/home			nfs4 retrans=10,timeo=50 1 1
			${CORE_IP}:/home/cameras		/home/cameras		nfs4 retrans=10,timeo=50 1 1
			EOF
	fi
	mkdir -p ${BASE_DIR}/var/
	mkdir -p ${BASE_DIR}/orbiter
	mkdir -p ${BASE_DIR}/keys
	mkdir -p ${BASE_DIR}/deb-cache
	mkdir -p /var/spool/asterisk
	mkdir -p /home/cameras
	mount -a || :
}

MD_Setup_Plutoconf() {
	. ${BASE_DIR}/bin/Config_Ops.sh

	# get PK_Device from "$DEVID_FILE", created by interactor
	DEVICE=$(cat "$DEVID_FILE")

	[ -n "$MySqlHost" ] || ConfSet "MySqlHost" "${CORE_IP}"
	[ -n "$MySqlUser" ] || ConfSet "MySqlUser" "root"
	[ -n "$MySqlPassword" ] || ConfSet "MySqlPassword" ""
	[ -n "$MySqlDBName" ] || ConfSet "MySqlDBName" "pluto_main"
	[ -n "$DCERouter" ] || ConfSet "DCERouter" "${CORE_IP}"
	[ -n "$MySqlPort" ] || ConfSet "MySqlPort" "3306"
	[ -n "$DCERouterPort" ] || ConfSet "DCERouterPort" "3450"

	[ -n "$PK_Device" ] || ConfSet "PK_Device" "$DEVICE"
	[ -n "$TARGET_DISTRO_ID" ] && ConfSet "PK_Distro" "$TARGET_DISTRO_ID"
	[ -n "$LogLevels" ] || ConfSet "LogLevels" "1,5,7,8"
	[ -n "$AutostartCore" ] || ConfSet "AutostartCore" "0"
	[ -n "$AutostartMedia" ] || ConfSet "AutostartMedia" "1"
	[ -n "$LTS_HES" ] || ConfSet "LTS_HES" "$TARGET_LTS_HES"

	# Set the AVWizard to NOT done
	[ -n "$AVWizardDone" ] || ConfSet "AVWizardDone" "0"
	[ -n "$UseVideoWizard" ] || ConfSet "UseVideoWizard" "1"
}

MD_Setup_Hostname() {
	. ${BASE_DIR}/bin/Config_Ops.sh

	NEW_HOSTNAME="moon${PK_Device}"
	echo ${NEW_HOSTNAME} > /etc/hostname
	sed -i 's/127.0.1.1.*/127.0.1.1\t'"$NEW_HOSTNAME"'/g' /etc/hosts
}

###########################################################
### Setup Functions - General functions
###########################################################

MD_Copy_SSH_Keys () {
	## Setup initial ssh access
	StatsMessage "Setting up SSH"
	[[ -f ${BASE_DIR}/keys/id_dsa_pluto.pub ]] && mkdir -p /root/.ssh && cat ${BASE_DIR}/keys/id_dsa_pluto.pub >> /root/.ssh/authorized_keys
}

MD_Set_Timezone () {
	. ${BASE_DIR}/bin/Config_Ops.sh
	StatsMessage "Setting MDs timezone to core's timezone"
	TimeZone=$(ssh $DCERouter cat /etc/timezone)
	echo $TimeZone > /etc/timezone
	dpkg-reconfigure --frontend noninteractive tzdata
}

MD_System_Level_Prep () {
	ConfigSources		# install-common.sh
	MD_Copy_SSH_Keys	# install-md.sh
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
	#StatsMessage "Creating deb-cache dir"
	#CreateBasePackagesFiles		# install-common.sh
	#UpdateDebCache "$DEB_CACHE"	# install-common.sh
	:
}

MD_Cleanup () {
	StatsMessage "Cleaning up from package installations..."

	#Remove the xorg file(s) from installation, need to start with a fresh slate
	rm -f /etc/X11/xorg.conf*

	#Create a list of installed packages
	#COLUMNS=1024 dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder
}

MD_ClearInstalledPackages () {
	StatsMessage "Clearing DB of installed packages..."
	Q="DELETE FROM Package_Device WHERE FK_Device='${PK_Device}'"
	R=$(RunSQL "$Q")
}

return 0


