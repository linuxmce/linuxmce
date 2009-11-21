#!/bin/bash
# vim:set ft=sh:

set -e 

. ./filesd-parms.sh $@

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Section_Ops.sh

## Startup sequence
# Startup sequence of core and media director
# is identical and being taken care of by 
# /etc/init.d/linuxmce

#File="/etc/init.d/kdm"
#Content='#!/bin/bash
#if [[ "$1" == "start" ]] ;then
#	/usr/pluto/bin/Startup_DisklessMD.sh
#fi
#'
#
#mkdir -p "${Parm_RootLocation}/$(dirname $File)"
#echo "${Content}" > "${Parm_RootLocation}/${File}"
#chmod +x "${Parm_RootLocation}/${File}"

## Shutdown sequence
File="/etc/event.d/pluto-shutdown"
Content="
start on shutdown

script
/usr/pluto/bin/Report_MachineOff.sh
end script
"

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
echo "$Content" > "$Parm_RootLocation/$File"
