#!/bin/bash

function BlacklistConfFiles {
	return 1
}

. /usr/pluto/bin/Utils.sh || :

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

if [[ "$1" == "configure" ]] ;then
	if [ ! -e /etc/ld.so.conf.d ] ;then
		if ! BlacklistConfFiles '/etc/ld.so.conf' ;then
			if [ ! -e /etc/ld.so.conf.pbackup ] ;then
				cp /etc/ld.so.conf /etc/ld.so.conf.pbackup || :
			fi

			if ! grep -qF '/usr/pluto/lib' /etc/ld.so.conf; then
				echo "/usr/pluto/lib" >> /etc/ld.so.conf	
			fi
		fi	
	else
		echo "/usr/pluto/lib" > /etc/ld.so.conf.d/plutolibs.conf
	fi
	/sbin/ldconfig.real || ldconfig
fi
