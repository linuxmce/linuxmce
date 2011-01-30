#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Network_Interfaces=32
DEVICEDATA_Network_Interfaces_IPv6=292


CommaField()
{
	echo "$2" | cut -d, -f "$1"
}

ExtractData()
{
	local R IntPart ExtPart
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <Ext If>,<Ext IP>,<Ext Netmask>,<Gateway>,<Ext DNS CSV>|<Int If>,<Int IP>,<Int Netmask>
		# <Ext If>,DHCP|<Int If>,<Int IP>,<Int Netmask>
		ExtPart=$(echo "$R" | cut -d'|' -f1)
		IntPart=$(echo "$R" | cut -d'|' -sf2)

		if [ -n "$IntPart" ]; then
			IntIf=$(CommaField 1 "$IntPart")
			IntIP=$(CommaField 2 "$IntPart")
			IntNetmask=$(CommaField 3 "$IntPart")
		fi

		if [ -n "$ExtPart" ]; then
			ExtIf=$(CommaField 1 "$ExtPart")
			ExtIP=$(CommaField 2 "$ExtPart")
			if [ "$ExtIP" != "DHCP" -a "$ExtIP" != "dhcp" ]; then
				ExtNetmask=$(CommaField 3 "$ExtPart")
				Gateway=$(CommaField 4 "$ExtPart")
				DNS=$(CommaField 5- "$ExtPart")
			fi
		fi
		NetIfConf=1
	fi
}

ExtractIPv6Data()
{
	local R IntPart ExtPart
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <Tunnelbroker>,<TunnelID>,<Endpoint>,<IPv6>,<IPv6 netmask>,<LAN prefix>,<LAN netmask>,<UserID>,<Password>,<Activate>,<Dynamic IPv4>
		
		IPv6TunnelBroker=$(CommaField 5 "$R")
		IPv6TunnelID=$(CommaField 5 "$R")
		IPv6Endpoint=$(CommaField 5 "$R")
		IPv6=$(CommaField 4 "$R")
		IPv6Netmask=$(CommaField 5 "$R")
		IPv6Net=$(CommaField 6 "$R")
		IPv6NetNetmask=$(CommaField 7 "$R")
		IPv6UserID=$(CommaField 8 "$R")
		IPv6Password=$(CommaField 8 "$R")
		IPv6Active=$(CommaField 10 "$R")
		IPv6DynamicIPv4=$(CommaField 11 "$R")
		IPv6RAenabled=$(CommaField 12 "$R")
	fi
}

ParseInterfaces()
{
	local Script File

	if [ ! -e /etc/network/interfaces.pbackup ] ;then
		cp /etc/network/interfaces /etc/network/interfaces.pbackup
	fi

	if [ ! -e /etc/resolv.conf.pbackup ] ;then
		cp /etc/resolv.conf /etc/resolv.conf.pbackup
	fi	

if ! BlacklistConfFiles '/etc/network/interfaces' ;then
	File="/etc/network/interfaces"

	Script='
function Display(ParsingInterface, interface, address, netmask, gateway, dns)
{
	if (ParsingInterface == 0)
		return;
	if (substr(dns, length(dns), 1) == ",")
		dns = substr(dns, 1, length(dns) - 1);
	if (address == "dhcp")
		print (interface, address);
	else
		print (interface, address, netmask, gateway, dns);
}

BEGIN { ParsingInterface = 0; OFS = ","; }
/#/ {
	$0 = substr($0, 1, match($0, /#/) - 1);
}
/iface/ && $3 == "inet" {
	if (ParsingInterface == 1)
	{
		Display(ParsingInterface, interface, address, netmask, gateway, dns);
	}
	interface = $2;
	if (interface == "lo")
	{
		ParsingInterface = 0;
		next;
	}
	setting = $4;
	if (setting == "static")
	{
		ParsingInterface = 1;
		dns="";
	}
	else if (setting == "dhcp")
	{
		ParsingInterface = 1;
		address = "dhcp";
	}
}
ParsingInterface == 1 {
	if ($1 == "address")
		address = $2;
	else if ($1 == "netmask")
		netmask = $2;
	else if ($1 == "gateway")
		gateway = $2;
	else if ($1 == "dns-nameservers")
		dns = dns $2 ",";
}
END { Display(ParsingInterface, interface, address, netmask, gateway, dns); }'
	awk "$Script" "$File"
fi
	if ! BlacklistConfFiles '/etc/resolv.conf' ;then
		grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g' | tr '\n' ',' | sed 's/,$//'
	fi
}

IntIf=
IntIP=
IntNetmask=
ExtIf=
ExtIP=
ExtNetmask=
Gateway=
DNS=
NetIfConf=0

IPv6If=ipv6tunnel
IPv6TunnelBroker=
IPv6TunnelID=
IPv6Endpoint=
IPv6=
IPv6Netmask=
IPv6Net=
IPv6NetNetmask=
IPv6UserID=
IPv6Password=
IPv6Active=0
IPv6DynamicIPv4=0
IPv6RAenabled=0

NCards=$(ip addr | grep "^[0-9]*:" | grep -v "^[0-9]*: lo" | grep -v "^[0-9]*: pan" | grep -c ".")

Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_Interfaces"
R=$(RunSQL "$Q")

ExtractData "$R"

Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_Interfaces_IPv6"
R=$(RunSQL "$Q")

ExtractIPv6Data "$R"

Q="SELECT IK_DeviceData
FROM Device_DeviceData
JOIN Device ON FK_Device=PK_Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE FK_DeviceCategory=7 AND FK_DeviceData=28"
DHCPsetting=$(RunSQL "$Q")

if [[ "$DHCPsetting" == eth*, ]]; then
	DHCPcard=${DHCPsetting%%,*}
	DHCPsetting=${DHCPsetting#eth*,}
fi

if [[ -n "$DHCPsetting" && -z "$IntIf" ]]; then
	NPflagReconfNetwork="yes"
fi

# Distinct data correction for MDs and Core
if PackageStatus pluto-dcerouter | grep -q '^Status: install '; then
	# Core always has an internal interface, even if it's an alias
	if [[ -z "$IntIf" ]]; then
		if [[ "$NCards" -eq 1 ]]; then
			IntIf="$ExtIf:0"
		elif [[ "$ExtIf" == eth0 ]]; then
			IntIf="eth1"
		else
			IntIf="eth0"
		fi
		Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
                IntIP=$(RunSQL "$Q")
		if [[ "$IntIP" == "" ]] ;then
			IntIP=192.168.80.1
		fi
		#IntIP=192.168.80.1
		IntNetmask=255.255.255.0
	fi
	if [[ "$ExtIP" == dhcp ]]; then
		ExtIPreal=$(ip a ls dev "$ExtIf" | grep -m 1 'inet ' | awk '{print $2}' | cut -d/ -f1)
	fi
else
	# MDs don't have an internal interface and their network details aren't stored in the database
	IntIf=
	IntIP=
	IntNetmask=
	ExtractData "$(ParseInterfaces | head -1)"
fi
