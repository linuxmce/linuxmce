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

StatsMessage "Setting up mysql host"
# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
cat <<-EOF > /root/.my.cnf
	[client]
	host = dcerouter
	EOF

StatsMessage "Setting up $DEB_CACHE"
#Make sure there is are Packages files on the MD so apt-get update does not fail
#/usr/pluto/bin/UpdateDebCache.sh
rm -f /var/cache/apt/archives/*
mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
touch /usr/pluto/deb-cache/Packages
gzip -9c < /usr/pluto/deb-cache/Packages > /usr/pluto/deb-cache/Packages.gz
cp /usr/pluto/deb-cache/Packages* /usr/pluto/deb-cache/$DEB_CACHE

case "$TARGET_DISTRO" in
	raspbian)
		# raspbian doesn't come with lsb-release by default???
		cat <<-EOF > /etc/lsb-release
			DISTRIB_ID=Raspbian
			DISTRIB_CODENAME=$TARGET_RELEASE
			EOF
		;;
esac

# TOCO: FIXME: setup eth0, need to look at wlan0 setup perhaps?
if [[ ! $(grep eth0 /etc/network/interfaces) ]] ; then
	echo "" >> /etc/network/interfaces
	echo "auto eth0" >> /etc/network/interfaces
	echo "iface eth0 inet dhcp" >> /etc/network/interfaces
	echo "" >> /etc/network/interfaces
fi


#Exit successfully
exit 0
