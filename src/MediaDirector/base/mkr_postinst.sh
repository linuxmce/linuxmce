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

StatsMessage "Disable NetworkManager and Display Managers"
update-rc.d -f kdm remove
update-rc.d -f lightdm remove
update-rc.d -f NetworkManager remove

# FIXME: create a network selection application
#MyIF=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $8 }')
#MyWPA=$( cat /etc/network/interfaces | grep wpa )
#cat <<-EOF > /etc/network/interfaces
#	auto lo
#	iface lo inet loopback
#
#	allow hot-plug $MyIF
#	iface $MyIF inet dhcp
#	$MyWPA
#	EOF

StatsMessage "Setting up apt.conf.d"
cat <<-EOF > /etc/apt/apt.conf.d/30pluto
	// Pluto apt conf add-on
	APT::Cache-Limit "43554432";
	Dpkg::Options { "--force-confold"; };
	Acquire::http::timeout "10";
	Acquire::ftp::timeout "10";
	APT::Get::AllowUnauthenticated "true";
	EOF

StatsMessage "Setting up mysql host"
# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
cat <<-EOF > /root/.my.cnf
	[client]
	host = dcerouter
	EOF

StatsMessage "Setting up debconf"
## Setup debconf interface to 'noninteractive'
#create preseed file
cat <<-EOF >/tmp/preseed.cfg
	debconf	debconf/frontend	select	Noninteractive
	# Choices: critical, high, medium, low
	debconf	debconf/priority	select	critical
	msttcorefonts	msttcorefonts/http_proxy	string
	msttcorefonts	msttcorefonts/defoma	note
	msttcorefonts	msttcorefonts/dlurl	string
	msttcorefonts	msttcorefonts/savedir	string
	msttcorefonts	msttcorefonts/baddldir	note
	msttcorefonts	msttcorefonts/dldir	string
	msttcorefonts	msttcorefonts/blurb	note
	msttcorefonts	msttcorefonts/accepted-mscorefonts-eula	boolean	true
	msttcorefonts	msttcorefonts/present-mscorefonts-eula	boolean	false
	sun-java6-bin	shared/accepted-sun-dlj-v1-1	boolean	true
	sun-java6-jre	shared/accepted-sun-dlj-v1-1	boolean	true
	sun-java6-jre	sun-java6-jre/jcepolicy	note
	sun-java6-jre	sun-java6-jre/stopthread	boolean	true
	debconf	debconf/frontend	select	Noninteractive
	# Choices: critical, high, medium, low
	debconf	debconf/priority	select	critical
	EOF
	debconf-set-selections /tmp/preseed.cfg

StatsMessage "Dissabling display manager"
#TODO: add a dpkg-diversion for default-display-manager so it is not overwritten
mkdir -p "/etc/X11"
echo '/bin/false' >"/etc/X11/default-display-manager"

#FIXME: this should be in a pkg, not created in a postinst.
StatsMessage "Setting up kernel symlink fix"
# FIXME: don't overwrite on re-install, check prior to write
## Setup kernel postinst script to repair vmlinuz/initrd.img symlinks in /
cat <<-"EOF" >/etc/kernel/postinst.d/update-symlinks
	#!/bin/bash
	# LinuxMCE post kernel image install.
	#
	# We make sure we can read the image and the kernel, and the softlink is correct.
	chmod g+r /boot/* || :
	chmod o+r /boot/* || :

	pushd /
	ln -sf boot/initrd.img-$1 initrd.img || :
	ln -sf boot/vmlinuz-$1 vmlinuz || :
	popd

	exit 0
	EOF
chmod +x /etc/kernel/postinst.d/update-symlinks

StatsMessage "Setting up $DEB_CACHE"
#Make sure there is are Packages files on the MD so apt-get update does not fail
mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
touch  /usr/pluto/deb-cache/$DEB_CACHE/Packages
gzip -9c < /usr/pluto/deb-cache/$DEB_CACHE/Packages > /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz

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
