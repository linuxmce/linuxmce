#!/bin/bash

. /usr/pluto/install/install-core.sh

if [[ -n "$HEADER_install_livedvd" ]]; then
	return 0
fi
HEADER_install_livedvd=included

###########################################################
### Live DVD Specific Fn's
###########################################################

FirstNetwork () {
	### This is a Live DVD Fn.
	:

	echo "creating dumb DHCP interfaces template" > /var/log/pluto/firstnet.log
	nic_num=$(lspci | grep -ic "Ethernet")
	if [[ $nic_num -gt "1" ]]; then
		cat <<-EOF > /etc/network/interfaces
			auto lo
			iface lo inet loopback
			auto eth0
			iface eth0 inet dhcp
			auto eth1
			iface eth1 inet dhcp
			EOF
		#ifconfig eth1 del 192.168.80.0 || :
	else
		cat <<-EOF > /etc/network/interfaces
			auto lo
			iface lo inet loopback
			auto eth0
			iface eth0 inet dhcp
			EOF
	fi
	/etc/init.d/networking stop
	service networking start --no-wait
}

SetupNetworking () {
	### This is a Live DVD Fn.
	:

	### This is all done in /etc/init.d/linuxmce
	rm -f /etc/X11/xorg.conf
	rm -f /etc/network/interfaces
	## Reconfigure networking
	${BASE_DIR}/bin/Network_Setup.sh
	${BASE_DIR}/bin/ConfirmInstallation.sh
	${BASE_DIR}/bin/Timezone_Detect.sh
}

VideoDriverLive () {
	### This is a Live DVD Fn.
	:

	vga_pci=$(lspci -v | grep ' VGA ')
	prop_driver="fbdev"
	chip_man=$(echo "$vga_pci" | grep -Ewo '(\[1002|\[1106|\[10de|\[8086)')

	case "$chip_man" in
		[10de)
			prop_driver="nouveau" ;;
		[1002)
			prop_driver="radeon" ;;
		[8086)
			prop_driver="intel"
			if echo $vga_pci | grep "i740"; then
				prop_driver="i740"
			fi
			if echo $vga_pci | grep "i128"; then
				prop_driver="i128"
			fi
			if echo $vga_driver | grep "mach"; then
				prop_driver="mach64"
			fi ;;
		[1106)
			prop_driver="openchrome"
			if echo $vga_pci | grep -i "Savage"; then
				prop_driver="savage"
			fi
			#if echo $vga_pci | grep -i "s3"; then
			#	prop_driver="via"
			#fi
			if echo $vga_pci | grep -i "virge"; then
				prop_driver="virge"
			fi ;;
		*)
			prop_driver="fbdev" ;;
	esac

	### Install driver based on the type of video card used
	#Install nouveau driver to avoid reboot if nvidia
	case $prop_driver in
		nouveau)
			apt-get -yf install xserver-xorg-nouveau-video$LTS_HES ;;
		radeon)
			apt-get -yf install xserver-xorg-video-radeon$LTS_HES ;;
		r128)
			apt-get -yf install xserver-xorg-video-r128$LTS_HES ;;
		mach64)
			apt-get -yf install xserver-xorg-video-mach64$LTS_HES ;;
		intel)
			apt-get -yf install xserver-xorg-video-intel$LTS_HES ;;
		i128)
			apt-get -yf install xserver-xorg-video-i128$LTS_HES ;;
		i740)
			apt-get -yf install xserver-xorg-video-i740$LTS_HES ;;
		openchrome)
			apt-get -yf install xserver-xorg-video-openchrome$LTS_HES ;;
	esac
	if [[ "$chip_man" == "Intel" ]]; then
		if ping -c 1 google.com; then
			apt-get -yf install libva-driver-i965
		fi
	fi
	VideoDriver="$prop_driver"
	export Best_Video_Driver="$prop_driver"
}

InitialBootPrep () {
	### This is a Live DVD Fn.
	:

	# Remove deprecated options from /etc/default/grub
	#sed -i '/GRUB_HIDDEN_/d' /etc/default/grub
	#/usr/sbin/update-grub
}

PackageCleanUp () {
	### This is a Live DVD Fn.
	### This is i386 ONLY!!!
	:

	# Remove all but the latest package
	thedir="${LOCAL_REPO_BASE}"
	X=$(dpkg -l|grep "^ii"|awk '{print $2,$3}')
	odd="1"
	for var in $X ; do
		if [[ "$odd" -eq "1" ]]; then
			package="$var"
			odd="0"
		else
			version="$var"
			odd="1"
			all="$package"
			all+="_*"
			latest="${thedir}/${package}"
			latest+="_$version"
			latest+="_i386.deb"
			Alls=$(find "$thedir" -iname "$all")
			for each in $Alls ; do
				if [[ "$each" != "$latest" ]]; then
					rm "$each"
				fi
			done
		fi
	done > /dev/null
}

TempEMIFix () {
	# Placeholder fn.  This is done elsewhere if appropriate
	:
}

SetupAptSources () {
	ConfigSources	# install-common.sh
}


return 0

###########################################################
###########################################################
### Deprecated Functions
###########################################################
###########################################################
