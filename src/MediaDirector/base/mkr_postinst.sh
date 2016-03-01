#!/bin/bash

###########################################################
### Setup global variables
###########################################################

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

DEB_CACHE="$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH"

StatsMessage () {
	printf "$(date) - $* \n"
}

. /usr/pluto/install/install-md.sh

###########################################################
### Main execution area
###########################################################

StatsMessage "Setting up mysql host"
# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
cat <<-EOF > /root/.my.cnf
	[client]
	host = dcerouter
	EOF

StatsMessage "Setting up $DEB_CACHE"
#Make sure there is are Packages files on the MD so apt-get update does not fail
if [[ ! -f /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz ]] ; then
	mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
	touch /usr/pluto/deb-cache/$DEB_CACHE/Packages
	gzip -9c < /usr/pluto/deb-cache/$DEB_CACHE/Packages > /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz
fi

case "$TARGET_DISTRO" in
	raspbian)
		# raspbian doesn't come with lsb-release by default???
		cat <<-EOF > /etc/lsb-release
			DISTRIB_ID=Raspbian
			DISTRIB_CODENAME=$TARGET_RELEASE
			EOF
		;;
esac

MD_Config_NTP_Client

exit 0
