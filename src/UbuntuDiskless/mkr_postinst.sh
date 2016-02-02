#!/bin/bash

# summary of how this script can be called:
#        * <postinst> `configure' <most-recently-configured-version>
#if [[ "$2" != "" ]] ;then
#	/usr/pluto/bin/Diskless_CreateTBZ.sh || echo "WARNING: Diskless_CreateTBZ.sh returned an error!"
#fi

if [ -x /usr/pluto/bin/Debug_LogKernelModules.sh ]; then
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
fi

#################################################################
## move deb-cache to newer multi-arch friendly location
## update sourcs.list on core and all MDs to reflect new locations
if [ ! -d /usr/pluto/deb-cache ]; then
	exit 0
fi

LastVersion="$2"

[[ -x "/usr/pluto/bin/UpdateDebCache.sh" ]] && /usr/pluto/bin/UpdateDebCache.sh

