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

# TODO: check if ver <= 20.0.0.45 (1004), only run this if it's that old
if [[ -n "$LastVersion" ]] ; then
	/usr/pluto/bin/UpdateDebCache.sh
fi

