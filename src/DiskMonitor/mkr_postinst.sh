#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

## Add a cron entry
cronEntry="*/3 * * * * root bash -c '/usr/pluto/bin/DiskSpaceMonitor.sh &>/dev/null'"
#if ! grep -qF '/usr/pluto/bin/DiskSpaceMonitor.sh' /etc/crontab; then
#        echo "$cronEntry" >>/etc/crontab
#        invoke-rc.d cron reload
#fi

if [[ ! -e /etc/cron.d/DiskSpaceMonitor ]] ;then
	echo "$cronEntry" >>/etc/cron.d/DiskSpaceMonitor
	# We reload cron just because, and if it doesn't work out, we don't care.
	invoke-rc.d cron reload || :
fi
		
