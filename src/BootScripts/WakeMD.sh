#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/SQL_Ops.sh

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--dev) ((i++)); Parm_Device="${!i}" ;;
		--mac) ((i++)); Parm_MAC="${!i}" ;;
		*) echo "Unknown parameter '${!i}'"; exit 1 ;;
	esac
done

if [[ -z "${!Parm_*}" ]]; then
	echo "Usage: WakeMD.sh --dev <Device>|--mac <MAC>"
	exit 1
fi

if [[ -n "$Parm_Device" ]]; then
	Q="
		SELECT MACaddress
		FROM Device
		WHERE PK_Device='$Parm_Device'
	"
	Parm_MAC=$(RunSQL "$Q")

	if [[ -z "$Parm_MAC" ]]; then
		echo "No MAC address found for device '$Parm_Device'"
		exit 1
	fi
fi

/usr/sbin/etherwake -b -i "$IntIf" "$Parm_MAC"

#For motherboards with Nvidia Ethernet chipset with reversed MAC address, we issue a wake command with reversed MAC
Reversed_Parm_MAC="${Parm_MAC:15:2}:${Parm_MAC:12:2}:${Parm_MAC:9:2}:${Parm_MAC:6:2}:${Parm_MAC:3:2}:${Parm_MAC:0:2}";
/usr/sbin/etherwake -b -i "$IntIf" "$Reversed_Parm_MAC"
