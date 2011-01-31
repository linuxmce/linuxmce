#!/bin/bash

DISTRO="$(lsb_release -c -s)"

function CopySkeletonToFirstUser
{
	# The first user in a kubuntu 810 install has the ID 1000
	cp -r /etc/skel/* `grep 1000 /etc/passwd|head -1|cut -d: -f6`
}

function setupRunlevel3
{

	rm -rfv /etc/rc3.d/*
	cp -av /etc/rc2.d/* /etc/rc3.d/
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce
	rm -fv /etc/rc3.d/S99kdm /etc/rc3.d/S990start_avwizard
	
}



function setupRunlevel4
{

	rm -rfv /etc/rc4.d/*
	cp -av /etc/rc2.d/* /etc/rc4.d/
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce

}



function setupRunlevel5
{

	rm -rfv /etc/rc5.d/*
	cp -av /etc/rc2.d/* /etc/rc5.d/
#	ln -sfv /usr/pluto/bin/Start_X.sh /etc/rc5.d/S30Start_X.sh
#	ln -sfv /usr/pluto/bin/startup-script.sh /etc/rc5.d/S93startup-script.sh
#	ln -sfv /usr/pluto/bin/lmce_launch_manager.sh /etc/rc5.d/S99lmce_launch_manager.sh
	ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce
#	rm -fv /etc/rc5.d/S99kdm

}



function modifyUpstartFile
{
	local upstart_file="$1"


	sed -e 's/stop\( on runlevel [2-5]\)/start\1/' -i $upstart_file

}


function createIninttabConfig
{

	cat >/etc/inittab <<"EOF"
# WARNING: Do NOT set the default runlevel to 0 (shutdown) or 6 (reboot).
#id:2:initdefault: # KDE
#id:3:initdefault: # Core
#id:4:initdefault: # Core + KDE
id:5:initdefault: # Launch Manager
EOF

}

function addAdditionalTTYStart
{
	if [[ "$DISTRO" = "lucid" ]] ; then
		sed -i 's/23/235/' /etc/init/tty2.conf
		sed -i 's/23/235/' /etc/init/tty3.conf
		sed -i 's/23/235/' /etc/init/tty4.conf
		# disable plymouth splash for now. Could be replaced by own LMCE splash later
		sed -i 's/ splash//' /etc/default/grub
                /usr/sbin/update-grub
	else
        	echo "start on runlevel 5">>/etc/event.d/tty2
        	echo "start on runlevel 5">>/etc/event.d/tty3
        	echo "start on runlevel 5">>/etc/event.d/tty4
	fi
}



function createDefaultEventFile
{

	cat >/etc/event.d/rc-default <<"EOF"
# rc - runlevel compatibility
#
# This task guesses what the "default runlevel" should be and starts the
# appropriate script.

start on stopped rcS

script
	runlevel --reboot || true


	if grep -q -w -- "-s\|single\|S" /proc/cmdline
	then
		RL="S"
	else
		RL="$(sed -n -e 's/.*runlevel=\([1-5]\).*/\1/p' /proc/cmdline || true)"
	fi

	if [ -z "$RL" -a -r /etc/inittab ]
	then
		RL="$(sed -n -e 's/^id:\([1-5]\):initdefault:.*/\1/p' /etc/inittab || true)"
	fi

	if [ -z "$RL" ]
	then
		RL="2"
	fi

	telinit $RL

end script
EOF

}



function CleanCoreSetup
{

#	createStartdevicesScript
#	createStartcoreScript
#	createCoreInitScript

#	restoreKdmInitScript

	setupRunlevel3
	setupRunlevel4
	setupRunlevel5

#	modifyUpstartFile /etc/event.d/media-center-startup
#	modifyUpstartFile /etc/event.d/pluto-dhcpd-plugin
#	if [ -e /etc/event.d/pluto-dhcpd-plugin ]
#	then
#		mv -fv /etc/event.d/pluto-dhcpd-plugin /var/tmp
#		rm -fv /etc/event.d/pluto-dhcpd-plugin
#	fi

	createIninttabConfig
#	createDefaultEventFile

}



function asteriskNoloadModule
{
	local module="$1"


	if ! grep -q "^[^;]*noload => ${module}" /etc/asterisk/modules.conf
	then
		sed -e "s/\[global\]/noload => ${module}\n&/" -i /etc/asterisk/modules.conf
	else
		echo "module $module already disabled"
	fi

}



function FixAsteriskConfig
{

	asteriskNoloadModule app_directory_odbc.so
	asteriskNoloadModule app_voicemail_odbc.so
	asteriskNoloadModule app_voicemail_imap.so

}

function InstallMSCoreFonts
{

#Version: 1.0
#Info: Bash script to install mscorefonts without using
#currently broken ttf-mscorefonts-installer (3.0) for
#Ubuntu 9.10 Karmic Koala (date: 03/11/09)

#Author: Jonathan K.
#Website: http://www.friendlytechninja.com
#Email: friendlytechninja@gmail.com

#License: This is free to use and distribute (for free only) as long as
#credit is given to original author.

# Prepare the system
apt-get remove ttf-mscorefonts-installer -y
apt-get install cabextract -y

#Create temp and mscorefonts dir
sudo mkdir /usr/share/fonts/truetype/mscorefonts
mkdir /tmp/mscorefonts
cd /tmp/mscorefonts

#Download links
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/andale32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/arial32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/arialb32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/comic32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/courie32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/georgi32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/impact32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/times32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/trebuc32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/verdan32.exe/download
wget http://sourceforge.net/projects/corefonts/files/the%20fonts/final/webdin32.exe/download

#Extract all .tff files
cabextract *.exe -F*.ttf -L

#Rename files and move files
cp andalemo.ttf /usr/share/fonts/truetype/mscorefonts/Andale_Mono.ttf
cp ariali.ttf /usr/share/fonts/truetype/mscorefonts/Arial_Italic.ttf
cp arialbd.ttf /usr/share/fonts/truetype/mscorefonts/Arial_Bold.ttf
cp arialbi.ttf /usr/share/fonts/truetype/mscorefonts/Arial_Bold_Italic.ttf
cp arial.ttf /usr/share/fonts/truetype/mscorefonts/Arial.ttf
cp ariblk.ttf /usr/share/fonts/truetype/mscorefonts/Arial_Black.ttf
cp comicbd.ttf /usr/share/fonts/truetype/mscorefonts/Comic_Sans_MS_Bold.ttf
cp comic.ttf /usr/share/fonts/truetype/mscorefonts/Comic_Sans_MS.ttf
cp cour.ttf /usr/share/fonts/truetype/mscorefonts/Courier_New.ttf
cp courbd.ttf /usr/share/fonts/truetype/mscorefonts/Courier_New_Bold.ttf
cp courbi.ttf /usr/share/fonts/truetype/mscorefonts/Courier_New_Bold_Italic.ttf
cp couri.ttf /usr/share/fonts/truetype/mscorefonts/Courier_New_Italic.ttf
cp georgiaz.ttf /usr/share/fonts/truetype/mscorefonts/Georgia_Bold_Italic.ttf
cp georgiab.ttf /usr/share/fonts/truetype/mscorefonts/Georgia_Bold.ttf
cp georgiai.ttf /usr/share/fonts/truetype/mscorefonts/Georgia_Italic.ttf
cp georgia.ttf /usr/share/fonts/truetype/mscorefonts/Georgia.ttf
cp impact.ttf /usr/share/fonts/truetype/mscorefonts/Impact.ttf
cp times.ttf /usr/share/fonts/truetype/mscorefonts/Times_New_Roman.ttf
cp timesbd.ttf /usr/share/fonts/truetype/mscorefonts/Times_New_Roman_Bold.ttf
cp timesbi.ttf /usr/share/fonts/truetype/mscorefonts/Times_New_Roman_Bold_Italic.ttf
cp timesi.ttf /usr/share/fonts/truetype/mscorefonts/Times_New_Roman_Italic.ttf
cp trebuc.ttf /usr/share/fonts/truetype/mscorefonts/Trebuchet_MS.ttf
cp trebucbd.ttf /usr/share/fonts/truetype/mscorefonts/Trebuchet_MS_Bold.ttf
cp trebucbi.ttf /usr/share/fonts/truetype/mscorefonts/Trebuchet_MS_Bold_Italic.ttf
cp trebucit.ttf /usr/share/fonts/truetype/mscorefonts/Trebuchet_MS_Italic.ttf
cp verdanab.ttf /usr/share/fonts/truetype/mscorefonts/Verdana_Bold.ttf
cp verdanai.ttf /usr/share/fonts/truetype/mscorefonts/Verdana_Italic.ttf
cp verdanaz.ttf /usr/share/fonts/truetype/mscorefonts/Verdana_Bold_Italic.ttf
cp verdana.ttf /usr/share/fonts/truetype/mscorefonts/Verdana.ttf
cp webdings.ttf /usr/share/fonts/truetype/mscorefonts/Webdings.ttf

#Clean up
cd ~
rm -r /tmp/mscorefonts
}
