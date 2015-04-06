#!/bin/bash

###########################################################
### Setup global variables
###########################################################

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

DEB_CACHE="$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH"

###########################################################
### Main execution area
###########################################################

# setup the firstboot script
update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null

update-rc.d -f kdm remove
update-rc.d -f lightdm remove
update-rc.d -f NetworkManager remove

MyIF=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $8 }')
MyWPA=$( cat /etc/network/interfaces | grep wpa )

cat <<-EOF > /etc/network/interfaces
	auto lo
	iface lo inet loopback

	allow hot-plug $MyIF
	iface $MyIF inet dhcp
	$MyWPA
	EOF

cat <<-EOF > /etc/apt/apt.conf.d/30pluto
	// Pluto apt conf add-on
	APT::Cache-Limit "43554432";
	Dpkg::Options { "--force-confold"; };
	Acquire::http::timeout "10";
	Acquire::ftp::timeout "10";
	APT::Get::AllowUnauthenticated "true";
	EOF

# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
cat <<-EOF > /root/.my.cnf
	[client]
	host = dcerouter
	EOF

#StatsMessage "Setting up debconf"
## Setup debconf interface to 'noninteractive'
#create preseed file
cat <<-EOF >/tmp/preseed.cfg
	debconf debconf/frontendselect Noninteractive
	# Choices: critical, high, medium, low
	debconf debconf/priorityselect critical
	msttcorefonts   msttcorefonts/http_proxystring
	msttcorefonts   msttcorefonts/defoma    note
	msttcorefonts   msttcorefonts/dlurl     string
	msttcorefonts   msttcorefonts/savedir   string
	msttcorefonts   msttcorefonts/baddldir  note
	msttcorefonts   msttcorefonts/dldir     string
	msttcorefonts   msttcorefonts/blurb     note
	msttcorefonts   msttcorefonts/accepted-mscorefonts-eula boolean true
	msttcorefonts   msttcorefonts/present-mscorefonts-eula  boolean false
	sun-java6-bin   shared/accepted-sun-dlj-v1-1    boolean true
	sun-java6-jre   shared/accepted-sun-dlj-v1-1    boolean true
	sun-java6-jre   sun-java6-jre/jcepolicy note
	sun-java6-jre   sun-java6-jre/stopthreadboolean true
	debconf debconf/frontendselect Noninteractive
	# Choices: critical, high, medium, low
	debconf debconf/priorityselect critical
	EOF
	debconf-set-selections /tmp/preseed.cfg

#For some odd reason, set-selections adds a space for Noninteractive and Critical that needs to be removed - debconf $
sed -i 's/Value:  /Value: /g' /var/cache/debconf/config.dat

#TODO: add a dpkg-diverstion for default-display-manager so it is not overwritten
echo '/bin/false' >"/etc/X11/default-display-manager"

#FIXME: this should be in a pkg, not created in a postinst.
#StatsMessage "Setting up kernel symlink fix"
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

#StatsMessage "Setting up $DEB_CACHE"
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
