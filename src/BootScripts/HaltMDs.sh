#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

## If the runlevel is wrong, do nothing
if [[ "$(runlevel)" != *[016] ]]; then
	exit 0
fi

## Only the first copy will do anything
## The rest will synchronized with the first copy
if ! ln -sn /proc/$$ /tmp/haltMDs 2>/dev/null; then
	while [[ -d /tmp/haltMDs ]]; do
		sleep .1
	done
	exit 0
fi

P="$1"

if [ "$P" == "D" ]; then
	# Selecting diskless MDs only
	Q="SELECT IPaddress
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL AND IK_DeviceData='1'

	UNION
	SELECT IPaddress FROM Device WHERE FK_DeviceTemplate=1893 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL
	"
else
	# Selecting all MDs
	Q="SELECT IPaddress
	FROM Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL
	UNION
        SELECT IPaddress FROM Device WHERE FK_DeviceTemplate=1893 AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL
	"
fi

R=$(RunSQL "$Q")
for Host in $R; do
	ShutDownRemote "$Host" &
done

offline=0
tries=60
while [ "$offline" -eq 0 -a "$tries" -gt 0 ]; do
	offline=1
	for Host in $R; do
#		nc -z "$Host" "$DCERouterPort" && offline=0
		# TODO: remember which hosts are offline as to not ping them anymore
		ping -qnc 1 -W 1 "$Host" &>/dev/null && offline=0 && Logging $TYPE $SEVERITY_WARNING "$0" "$Host still online"
	done
	((tries--))
	sleep 1
done

# wait 10 seconds more, just to be sure
#sleep 10
if [ "$offline" -eq 1 ]; then
	Msg="All MDs are down. Continuing."
elif [ "$tries" -eq 0 ]; then
	Msg="Not waiting for MDs to shutdown anymore. Continuing."
else
	Msg="Loop exited prematurely"
fi
Logging $TYPE $SEVERITY_WARNING "$0" "$Msg"
