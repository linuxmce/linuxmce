#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

function displayNetworkSettings {
	local Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=32"
	local R=$(RunSQL "$Q")
	local ExtPart=$(echo "$R" | cut -d'|' -f1)
	local IntPart=$(echo "$R" | cut -d'|' -sf2)

	local netExtName="$(echo $ExtPart | cut -d',' -f1)"
	local netExtIP=$(ifconfig $netExtName | grep "inet addr" | sed 's/.*inet addr:\([0-9.]*\).*Bcast.*/\1/g')
	local netExtMac=$(ifconfig $netExtName | grep "HWaddr" | sed 's/.*HWaddr \([0-9.:A-F]*\).*$/\1/g')
	local netExtMask=$(ifconfig $netExtName | grep "inet addr" | sed 's/.*Mask:\([0-9.]*\).*$/\1/g')
	local netExtGate=$(route -n | grep "^0.0.0.0" | head -1 | awk '{print $2}')
	local netExtDNS1=$(cat /etc/resolv.conf | grep "^nameserver" | head -1 | awk '{print $2}')
	local netExtDNS2=$(cat /etc/resolv.conf | grep "^nameserver" | tail -1 | awk '{print $2}')

	netUseDhcp=0
	if ps ax | egrep -q " (/sbin/)?dhclient.? .*${netExtName}\$" ;then
		netUseDhcp=1
	fi

	local netIntName="$(echo $IntPart | cut -d',' -f1)"
	local netIntMac=$(ifconfig $netIntName | grep "HWaddr" | sed 's/.*HWaddr \([0-9.:A-F]*\).*$/\1/g')
	local netIntMask="$(echo $IntPart | cut -d',' -f3)"
	local netIntIP="$(echo $IntPart | cut -d',' -f2)"


	echo "EXTERNAL_IFACE=$netExtName"
	echo "EXTERNAL_MAC=$netExtMac"
	echo "EXTERNAL_IP=$netExtIP"
	echo "EXTERNAL_NETMASK=$netExtMask"
	echo "EXTERNAL_DHCP=$netUseDhcp"
	echo "INTERNAL_IFACE=$netIntName"
	echo "INTERNAL_MAC=$netIntMac"
	echo "INTERNAL_IP=$netIntIP"
	echo "INTERNAL_NETMASK=$netIntMask"
	echo "GATEWAY=$netExtGate"
	echo "DNS1=$netExtDNS1"
	echo "DNS2=$netExtDNS2"

}


for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--orbiter) Orbiter=y ;;
		--all)
			displayNetworkSettings
			exit 0
			;;
		*) echo "Unknown option '${!i}'" ;;
	esac
done

. /usr/pluto/bin/Network_Parameters.sh

case "$DCERouter" in
	localhost) RouterIP="127.0.0.1" ;;
	dcerouter) 
		Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
		RouterIP=$(RunSQL "$Q")
		if [[ "$RouterIP" == "" ]] ;then
			 RouterIP="192.168.80.1"
		fi
	;; # TODO: remove hardcoded assumption
	*) RouterIP="$DCERouter" ;;
esac

if [[ "$ExtIP" == dhcp ]]; then
	ExtIP="$ExtIPreal dhcp"
fi

if [[ "$Orbiter" == y ]]; then
	if [[ -n "$IntIf" ]]; then
		echo "router=$RouterIP; internal=$IntIP($IntIf); external=$ExtIP($ExtIf)"
	else
		echo "router=$RouterIP; this MD=$ExtIP"
	fi
else
	echo "DCE Router: $DCERouter"

	if [[ -n "$IntIf" ]]; then
		echo "Internal interface: $IntIf; IP: $IntIP"
		echo "External interface: $ExtIf; IP: $ExtIP"
	else
		echo "this MD: $ExtIP"
	fi
fi
