#!/bin/bash

. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

result=$(cat /etc/mdadm/mdadm.conf | grep 'PROGRAM /usr/pluto/bin/monitoring_RAID.sh' || /bin/true)
if [[ -z $result ]] ;then
	if ! BlacklistConfFiles '/etc/mdadm/mdadm.conf' ;then
		if [ ! -e /etc/mdadm/mdadm.conf.pbackup ] && [ -e /etc/mdadm/mdadm.conf ] ;then
			cp /etc/mdadm/mdadm.conf /etc/mdadm/mdadm.conf.pbackup || :
		fi

		echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf
		service mdadm reload || :
	fi
fi
