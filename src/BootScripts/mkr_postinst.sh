#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :


. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

## Update the PlutoVersion variable from pluto.conf
Version=$(dpkg -s pluto-boot-scripts | grep '^Version:' | sed  's/Version: //')
ConfSet "PlutoVersion" "$Version"

if [ ! -e /etc/apt/30pluto.pbackup ] ;then
	if [ -e /etc/apt/apt.conf.d/30pluto ] ; then
		cp /etc/apt/apt.conf.d/30pluto /etc/apt/30pluto.pbackup || :
	fi
fi

rm -f /etc/logrotate.d/plut*

if [ ! -e /etc/asound.conf.pbackup ] ;then
	if [ -e /etc/asound.conf ]; then
		cp /etc/asound.conf /etc/asound.conf.pbackup || :
	fi
fi

if [ ! -e /etc/updatedb.conf.pbackup ] ;then
	if [ -e /etc/updatedb.conf ]; then
		cp /etc/updatedb.conf /etc/updatedb.conf.pbackup || :
	fi
fi

## Generate the /etc/apt/apt.conf.d/30pluto file
if ! BlacklistConfFiles '/etc/apt/apt.conf.d/30pluto' ;then
	rm -rf /var/cache/polipo/*
	pluto_apt_conf='// Pluto apt conf add-on
APT::Cache-Limit "33554432";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
'
	echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
fi

## Ouch 
chmod 777 /usr/pluto/locks


## Copy our logrotate policy -- every package should have its own policy but we are from Pluto :)
if ! BlacklistConfFiles '/etc/logrotate.d/pluto' ;then
	rm -f /etc/logrotate.d/pluto
	cp /usr/pluto/templates/logrotate.pluto.tmpl /etc/logrotate.d/linuxmce
fi

## Copy our asound.conf to the system
# This causes alsa to break on first install with bad values. Let's stop doing that. L3mce
#if ! BlacklistConfFiles '/etc/asound.conf' ;then
#	cp /usr/pluto/templates/asound.conf /etc/asound.conf
#fi

## Advertise SSH and file sharing via AVAHI
test -d "/etc/avahi" || mkdir -p "/etc/avahi"
if ! BlacklistConfFiles '/etc/avahi/services/ssh.service' ;then
        cp /usr/pluto/templates/ssh.service.tmpl /etc/avahi/services/ssh.service
fi
if ! BlacklistConfFiles '/etc/avahi/services/samba.service.tmpl' ;then
        cp /usr/pluto/templates/samba.service.tmpl /etc/avahi/services/samba.service
fi
## Restart to enable modifications
## Note: If restart is not successful, still continue the upgrade.
which avahi-daemon > /dev/null && service avahi-daemon restart || :

## Prevent updatedb from running from cron
if ! BlacklistConfFiles '/etc/updatedb.conf' ;then
	# only do stuff if updatedb exists
	if [ -e /etc/updatedb.conf ]; then
		if ! grep -qF 'exit 0 # Pluto' /etc/updatedb.conf; then
			echo "exit 0 # Pluto" >>/etc/updatedb.conf
		fi
	fi
fi

## Add alias for 'less -Rn' to .profile
if ! grep -qF '# Pluto alias for less log viewing' /root/.profile; then
	echo "alias les='less -Rn' # Pluto alias for less log viewing" >>/root/.profile
fi

## If pluto.conf has no LogLevels entry, add: LogLevels = 1,5,7,8
[ -e /etc/pluto.conf ] || touch /etc/pluto.conf
if ! grep -qF 'LogLevels = 1,5,7,8' /etc/pluto.conf ;then
	echo "LogLevels = 1,5,7,8" >> /etc/pluto.conf
fi

## Remove dash divertion of /bin/sh
if [[ -f /bin/sh.distrib ]] ;then
	mv /bin/sh.distrib /bin/sh || :
	dpkg-divert --remove /bin/sh || :
fi

## Add shortcut to the desktop to get back to LinuxMCE orbiter
mkdir -p /etc/skel/Desktop
cat <<eol >/etc/skel/Desktop/LinuxMCE
[Desktop Entry]
Encoding=UTF-8
Version=8.10
Type=Application
Exec=/usr/pluto/bin/ActivateOrbiterFromKDE.sh
Path=/usr/pluto/bin
Name=Back To LinuxMCE Orbiter
Icon=gnome-panel-launcher
eol

if [[ -f /root/Desktop ]] ; then
	rm -f /root/Desktop
fi
mkdir -p /root/Desktop
cp -r /etc/skel/Desktop/* /root/Desktop
