#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :


. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

#. /usr/pluto/install/install-common.sh

## Update the PlutoVersion variable from pluto.conf
Version=$(dpkg -s pluto-boot-scripts | grep '^Version:' | sed  's/Version: //')
ConfSet "PlutoVersion" "$Version"

if [ ! -e /etc/apt/sources.list.pbackup ]; then
	if [ -e /etc/apt/sources.list ] ; then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi
fi

if [ ! -e /etc/apt/30pluto.pbackup ] ;then
	if [ -e /etc/apt/apt.conf.d/30pluto ] ; then
		cp /etc/apt/apt.conf.d/30pluto /etc/apt/30pluto.pbackup || :
	fi
fi

# Apt complains if the .pbackup file is in conf.d
if [ -e /etc/apt/apt.conf.d/30pluto.pbackup ] ;then
	rm /etc/apt/apt.conf.d/30pluto.pbackup
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

###. /user/pluto/install/install-common.sh ; ConfigAptConf
## Generate the /etc/apt/apt.conf.d/30pluto file
if ! BlacklistConfFiles '/etc/apt/apt.conf.d/30pluto' ;then
	rm -rf /var/cache/polipo/*
	cat <<-EOF >/etc/apt/apt.conf.d/30pluto
		// Pluto apt conf add-on
		#APT::Cache-Limit "43554432";
		Dpkg::Options { "--force-confold"; };
		Acquire::http::timeout "10";
		Acquire::ftp::timeout "10";
		APT::Get::AllowUnauthenticated "true";
		EOF
fi

## FIXME: Ouch
chmod 777 /usr/pluto/locks


## Copy our logrotate policy -- every package should have its own policy but we are from Pluto :)
if ! BlacklistConfFiles '/etc/logrotate.d/pluto' ;then
	rm -f /etc/logrotate.d/pluto
	cp /usr/pluto/templates/logrotate.pluto.tmpl /etc/logrotate.d/linuxmce
fi

## Advertise SSH and file sharing via AVAHI
test -d "/etc/avahi/services" || mkdir -p "/etc/avahi/services"
if ! BlacklistConfFiles '/etc/avahi/services/ssh.service' ;then
	cp /usr/pluto/templates/ssh.service.tmpl /etc/avahi/services/ssh.service
fi
if ! BlacklistConfFiles '/etc/avahi/services/samba.service.tmpl' ;then
	cp /usr/pluto/templates/samba.service.tmpl /etc/avahi/services/samba.service
fi
## Restart to enable modifications
## Note: If restart is not successful, still continue the upgrade.
which avahi-daemon >/dev/null && service avahi-daemon restart >/dev/null || :

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
if ! grep -qF 'LogLevels' /etc/pluto.conf ;then
	echo "LogLevels = 1,5,7,8" >> /etc/pluto.conf
fi

## Remove dash diversion of /bin/sh -- FIXME: this is erroring, why?
if [[ -f /bin/sh.distrib ]] ;then
	mv /bin/sh.distrib /bin/sh || :
	dpkg-divert --remove /bin/sh || :
fi

# FIXME: wtf are we adding to /etc/skel here for?
## Add shortcut to the desktop to get back to LinuxMCE orbiter
if [[ ! -f /etc/skel/Desktop/LinuxMCE ]] ; then
	mkdir -p /etc/skel/Desktop
	cat <<-EOF >/etc/skel/Desktop/LinuxMCE
		[Desktop Entry]
		Encoding=UTF-8
		Version=8.10
		Type=Application
		Exec=/usr/pluto/bin/ActivateOrbiterFromKDE.sh
		Path=/usr/pluto/bin
		Name=Back To LinuxMCE Orbiter
		Icon=gnome-panel-launcher
		EOF
fi

# wtf?  why are we wiping out the desktop continually? doesn't seem very friendly.
#if [[ -f /root/Desktop ]] ; then
#	rm -f /root/Desktop
#fi
if [[ ! -f /root/Desktop/LinuxMCE ]] ; then
	mkdir -p /root/Desktop
	cp -r /etc/skel/Desktop/* /root/Desktop
fi

###. /usr/pluto/install/install-common.sh ; Disable_CompCache
# Disable compcache
if [ -f /usr/share/initramfs-tools/conf.d/compcache ]; then
	rm -f /usr/share/initramfs-tools/conf.d/compcache && update-initramfs -u
fi

###. /usr/pluto/install/install-common.sh ; Disable_DisplayManager
# TODO: dpkg-divert this so it doesn't come back ? - overrides work for upstart atm
# TODO: figure out what systemd will need, perhaps diversion is the answer
mkdir -p "/etc/X11"
echo "/bin/false" >/etc/X11/default-display-manager
update-rc.d kdm disable >/dev/null || :
update-rc.d sddm disable >/dev/null || :
update-rc.d lightdm disable >/dev/null || :

###. /usr/pluto/install/install-common.sh ; Fix_LSB_Data
case "$TARGET_DISTRO" in
	 raspbian)
		cat <<-EOF > /etc/lsb-release
			DISTRIB_ID=Raspbian
			DISTRIB_CODENAME=$TARGET_RELEASE
			EOF
		;;
esac

# Add a single new startup script.
#update-rc.d -f linuxmce remove >/dev/null || :
update-rc.d -f linuxmce defaults 99 1 >/dev/null

if [[ "$1" == "install" ]] ; then
	cat <<-EOF >/etc/network/interfaces
		auto lo
			iface lo inet loopback
		allow-hotplug eth0
			iface eth0 inet dhcp
		allow-hotplug eth1
			iface eth1 inet dhcp
		EOF
fi
