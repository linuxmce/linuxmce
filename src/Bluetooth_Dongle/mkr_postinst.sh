#!/bin/bash

. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

if ! BlacklistConfFiles '/etc/bluetooth/hcid.conf' ;then
	if [ ! -e /etc/bluetooth/hcid.conf.pbackup ] ;then 
		cp /etc/bluetooth/hcid.conf /etc/bluetooth/hcid.conf.pbackup || :
	fi

	sed -i 's!\<pin_helper.*$!pin_helper /usr/pluto/bin/pluto-pinhelper.sh;!' /etc/bluetooth/hcid.conf
	service bluetooth restart || :
fi
