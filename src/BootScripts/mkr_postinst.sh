#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :


. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

## Update the PlutoVersion variable from pluto.conf
Version=$(dpkg -s pluto-boot-scripts | grep '^Version:' | sed  's/Version: //')
ConfSet "PlutoVersion" "$Version"

if [ ! -e /etc/apt/apt.conf.d/30pluto.pbackup ] ;then
	cp /etc/apt/apt.conf.d/30pluto /etc/apt/apt.conf.d/30pluto.pbackup || :
fi

if [ ! -e /etc/logrotate.d/pluto.pbackup ] ;then
	cp /etc/logrotate.d/pluto /etc/logrotate.d/pluto.pbackup || :
fi

if [ ! -e /etc/asound.conf.pbackup ] ;then
	cp /etc/asound.conf /etc/asound.conf.pbackup || :
fi

if [ ! -e /etc/updatedb.conf.pbackup ] ;then
	cp /etc/updatedb.conf /etc/updatedb.conf.pbackup || :
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
if ! BlacklistConfFiles '/etc/asound.conf' ;then
	cp /usr/pluto/templates/asound.conf /etc/asound.conf
fi

## Prevent updatedb from running from cron
if ! BlacklistConfFiles '/etc/updatedb.conf' ;then
	if ! grep -qF 'exit 0 # Pluto' /etc/updatedb.conf; then
		echo "exit 0 # Pluto" >>/etc/updatedb.conf
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
