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

###########################################################
### Main execution area
###########################################################

StatsMessage "Setting up firstboot"
# setup the firstboot script
update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null

StatsMessage "Setting up mysql host"
# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
cat <<-EOF > /root/.my.cnf
	[client]
	host = dcerouter
	EOF

StatsMessage "Setting up $DEB_CACHE"
#Make sure there is are Packages files on the MD so apt-get update does not fail
/usr/pluto/bin/UpdateDebCache.sh

case "$TARGET_DISTRO" in
	raspbian)
		# raspbian doesn't come with lsb-release by default???
		cat <<-EOF > /etc/lsb-release
			DISTRIB_ID=Raspbian
			DISTRIB_CODENAME=$TARGET_RELEASE
			EOF
		;;
esac

#Exit successfully
exit 0
