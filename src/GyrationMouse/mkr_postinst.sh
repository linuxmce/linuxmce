
. /usr/pluto/bin/Utils.sh

if [ ! -e /etc/modules.pbackup ] && [ -e /etc/modules ] ;then
	cp /etc/modules /etc/modules.pbackup || :
fi

if ! grep -q "evdev" /etc/modules; then
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	if ! BlacklistConfFiles '/etc/modules' ;then
		echo "evdev" >> /etc/modules
	fi
fi

/sbin/modprobe evdev

service udev reload || :

