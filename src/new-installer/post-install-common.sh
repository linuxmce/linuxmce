#!/bin/bash

DISTRO="$(lsb_release -c -s)"

function CopySkeletonToFirstUser
{
	# The first user in a kubuntu 810 install has the ID 1000
	cp -r /etc/skel/* /etc/skel/.[!.]** `grep 1000 /etc/passwd|head -1|cut -d: -f6`
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
