#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh

for ((i = 1; i <= $#; i++)); do
	case "${!i}" in
		-d) ((i++)); gc100_PK_Device="${!i}" ;;
	esac
done

if [[ -z "$gc100_PK_Device" ]]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "gc100" "No Device number specified"
fi

chmod 660 /dev/ttyS_"$gc100_PK_Device"_*

/usr/pluto/bin/gc100 "$@"
Ret=$?

if [[ "$Ret" -eq 0 || "$Ret" -eq 2 ]]; then
	if DeviceIsDisabled "$gc100_PK_Device"; then
		Logging "$TYPE" "$SEVERITY_WARNING" "gc100:$gc100_PK_Device" "Device was disabled or removed. Stopping and marking as not running. Updating list of available ports."
		rm -f /dev/ttyS_"$gc100_PK_Device"_*
		/usr/pluto/bin/UpdateAvailableSerialPorts.sh
	fi
fi

exit "$Ret"
