#!/bin/bash

# summary of how this script can be called:
#        * <postinst> `configure' <most-recently-configured-version>
#if [[ "$2" != "" ]] ;then
#	/usr/pluto/bin/Diskless_CreateTBZ.sh || echo "WARNING: Diskless_CreateTBZ.sh returned an error!"
#fi

if [ -x /usr/pluto/bin/Debug_LogKernelModules.sh ]; then
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
fi

#update-rc.d -f haltMDs start 01 0 6 . || /bin/true
#update-rc.d -f haltMDs stop 01 0 6 . || /bin/true
update-rc.d -f haltMDs remove || /bin/true

## delete bad file from previous installs
rm -f /etc/init/md_halt.conf
