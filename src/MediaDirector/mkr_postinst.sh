#!/bin/bash

# setup the interactor and the firstboot script
update-rc.d -f 90_start_interactor.sh start 90 2 3 4 5 . >/dev/null
update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null

###########################################################
### Setup global variables
###########################################################

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

MD_System_Level_Prep () {
	# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
	cat <<-EOF > /root/.my.cnf
		[client]
		host = dcerouter
		EOF

	## Setup apt in pluto style
	case "$TARGET_DISTRO" in
		"ubuntu")
			# FIXME: don't overwrite on re-install, check prior to write
			TARGET_REPO_NAME="main"
			cat <<-EOF > /etc/apt/sources.list
				deb file:/usr/pluto/deb-cache/$DEB_CACHE ./
				deb http://deb.linuxmce.org/ubuntu/ $TARGET_RELEASE $TARGET_REPO_NAME
				deb http://deb.linuxmce.org/ $TARGET_RELEASE $TARGET_REPO_NAME
				deb $TARGET_REPO $TARGET_RELEASE main restricted universe multiverse
				deb $TARGET_REPO $TARGET_RELEASE-updates main restricted universe multiverse
				deb http://security.ubuntu.com/ubuntu/ $TARGET_RELEASE-security main restricted universe multiverse
				deb http://archive.canonical.com/ubuntu $TARGET_RELEASE partner
				deb http://debian.slimdevices.com/ stable  main
				deb http://download.videolan.org/pub/debian/stable/ /
				EOF
			;;
		"raspbian")
			# FIXME: don't overwrite on re-install, check prior to write
			cat <<-EOF > /etc/apt/sources.list
				#deb http://10.10.42.99/raspbian/ ./
				deb file:/usr/pluto/deb-cache/$DEB_CACHE ./
				deb http://deb.linuxmce.org/raspbian/ $TARGET_RELEASE main
				deb $TARGET_REPO $TARGET_RELEASE main contrib non-free rpi
				deb http://archive.raspberrypi.org/debian $TARGET_RELEASE main
				EOF
			;;
	esac

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
}

MD_Seamless_Compatability () {
	case "$TARGET_DISTRO" in
		ubuntu)
			: # no-op
			;;
		raspbian)
			# raspbian doesn't come with lsb-release by default???
			cat <<-EOF > /etc/lsb-release
				DISTRIB_ID=Raspbian
				DISTRIB_CODENAME=$TARGET_RELEASE
				EOF
			;;
	esac
}

MD_Preseed () {
	## Setup debconf interface to 'noninteractive'
	#create preseed file
	cat <<-EOF >/tmp/preseed.cfg
		debconf debconf/frontend	select Noninteractive
		# Choices: critical, high, medium, low
		debconf debconf/priority        select critical
		msttcorefonts   msttcorefonts/http_proxy        string
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
		sun-java6-jre   sun-java6-jre/stopthread        boolean true
		debconf debconf/frontend        select Noninteractive
		# Choices: critical, high, medium, low
		debconf debconf/priority        select critical
		EOF
	debconf-set-selections /tmp/preseed.cfg

	#For some odd reason, set-selections adds a space for Noninteractive and Critical that needs to be removed - debconf doesn't handle extra white space well
	sed -i 's/Value:  /Value: /g' /var/cache/debconf/config.dat

	# FIXME: convert to use dpkg-divert for greater reliability
	update-rc.d -f kdm remove
	update-rc.d -f NetworkManager remove

	cat <<-EOF > /etc/network/interfaces
		auto lo
		iface lo inet loopback

		auto eth0
		iface eth0 inet dhcp
		EOF

	echo '/bin/false' >"/etc/X11/default-display-manager"
}

MD_Cleanup () {
	#Make sure there is are Packages files on the MD so apt-get update does not fail
#	mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
#	/usr/pluto/bin/update-debcache.sh /usr/pluto/deb-cache/$DEB_CACHE

	#Copy the orbiter activation command to the MD's desktop
#	mkdir -p ""/root/Desktop
#	cp -r ""/etc/skel/Desktop/* ""/root/Desktop

	#Create a list of installed packages
	#COLUMS=1024 dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder
	: # no-op
}


###########################################################
### Main execution area
###########################################################

TARGET_TYPES="$TARGET_DISTRO-$TARGET_ARCH"

#TODO get as much of this from database as possible
case "$TARGET_DISTRO" in
	"ubuntu")
		case "$TARGET_ARCH" in
			i386)
				TARGET_REPO="http://archive.ubuntu.com/ubuntu/"
				;;
			armhf)
				TARGET_REPO="http://ports.ubuntu.com/ubuntu/"
				;;
		esac
		;;
	"raspbian-armhf")
		TARGET_REPO="http://archive.raspbian.org/raspbian/"
		;;
esac

DEB_CACHE="$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH"

#Function execution
MD_System_Level_Prep
MD_Seamless_Compatability
MD_Preseed
MD_Cleanup

#Exit successfully
exit 0
