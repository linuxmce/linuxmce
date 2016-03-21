#!/bin/bash
## Create divertion for NetworkManager as it destroys our settings

if [ install = "$1" -o upgrade = "$1" ]; then
	dpkg-divert --add --rename --package pluto-boot-scripts --divert /usr/sbin/NetworkManager.wraped /usr/sbin/NetworkManager
fi
if [ install = "$1" ]; then
	cat <<-EOF >/tmp/preseed.cfg
		debconf debconf/frontend select Noninteractive
		# Choices: critical, high, medium, low
		debconf debconf/priority select critical
		msttcorefonts msttcorefonts/http_proxy string
		msttcorefonts msttcorefonts/defoma note
		msttcorefonts msttcorefonts/dlurl string
		msttcorefonts msttcorefonts/savedir string
		msttcorefonts msttcorefonts/baddldir note
		msttcorefonts msttcorefonts/dldir string
		msttcorefonts msttcorefonts/blurb note
		msttcorefonts msttcorefonts/accepted-mscorefonts-eula boolean true
		msttcorefonts msttcorefonts/present-mscorefonts-eula boolean false
		sun-java6-bin shared/accepted-sun-dlj-v1-1 boolean true
		sun-java6-jre shared/accepted-sun-dlj-v1-1 boolean true
		sun-java6-jre sun-java6-jre/jcepolicy note
		sun-java6-jre sun-java6-jre/stopthread boolean true
		ola ola/daemon boolean true
		EOF
	debconf-set-selections /tmp/preseed.cfg

	# Setting up kernel symlink fix
	## Setup kernel postinst script to repair vmlinuz/initrd.img symlinks in /
	if [[ ! -f /etc/kernel/postinst.d/update-symlinks ]] ; then
		cat <<-"EOF" >/etc/kernel/postinst.d/update-symlinks
			#!/bin/bash
			# LinuxMCE post kernel image install.
			#
			# We make sure we can read the image and the kernel, and the softlink is correct.
			chmod g+r /boot/* || :
			chmod o+r /boot/* || :

			pushd / >/dev/null
			ln -sf boot/initrd.img-$1 initrd.img || :
			ln -sf boot/vmlinuz-$1 vmlinuz || :
			popd > /deb/null

			exit 0
			EOF
		chmod +x /etc/kernel/postinst.d/update-symlinks
	fi

	echo "en_US.UTF-8 UTF-8" >/etc/locale.gen
fi
