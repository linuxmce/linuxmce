#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Hostname=188
DEVICEDATA_Domainname=187
DEVICEDATA_Network_Interfaces=32
DEVICEDATA_Network_Interfaces_IPv6=302
DEVICEDATA_Network_Interfaces_IPv6_tunnel=292
DEVICEDATA_Network_VPN=295
DEVICEDATA_Network_PPPoE=303

CommaField()
{
	echo "$2" | cut -d, -f "$1"
}

CalculateNetworkAddress()
{
	typeset -i mask=255

	SaveIFS=$IFS
	IFS=.
	typeset -a IParr=($IntIP)
	typeset -a NMarr=($IntNetmask)
	IFS=$SaveIFS

	typeset -i ipbin1=${IParr[0]}
	typeset -i ipbin2=${IParr[1]}
	typeset -i ipbin3=${IParr[2]}
	typeset -i ipbin4=${IParr[3]}

	typeset -i nmbin1=${NMarr[0]}
	typeset -i nmbin2=${NMarr[1]}
	typeset -i nmbin3=${NMarr[2]}
	typeset -i nmbin4=${NMarr[3]}

	IntNetworkAddress=$"$((ipbin1 & nmbin1)).$((ipbin2 & nmbin2)).$((ipbin3 & nmbin3)).$((ipbin4 & nmbin4))"
}

ExtractData()
{
        local R IntPart Extpart
        R="$*"

        if [ -n "$R" ]; then
                # format:
                # <Ext If>,<Ext IP>,<Ext Netmask>,<Gateway>,<Ext DNS CSV>|<Int IF>,<Int IP>,<Int Netmask>
                # <Ext If>,DHCP|<Int IF>,<Int IP>,<Int Netmask>
                ExtPart=$(echo "$R" | cut -d'|' -f1)
                IntPart=$(echo "$R" | cut -d'|' -sf2)
                IFS='|' read -a array <<< "$R"
                echo ${#array[@]}

                if [ -n "$IntPart" ]; then
                        IntIf=$(CommaField 1 "$IntPart")
                        IntIP=$(CommaField 2 "$IntPart")
                        IntNetmask=$(CommaField 3 "$IntPart")
                fi

                if [ -n "$ExtPart" ]; then
                        ExtIf=$(CommaField 1 "$ExtPart")
                        ExtIP=$(CommaField 2 "$ExtPart")
                        if [ "ExtIP" != "DHCP" -a "$ExtIP" != "dhcp" ]; then
                                ExtNetmask=$(CommaField 3 "$ExtPart")
                                Gateway=$(CommaField 4 "$ExtPart")
                                DNS=$(CommaField 5- "$ExtPart")
                        fi
                fi

                IFS='|' read -a interfaces_array <<< "$R"
                amount_Interfaces=${#interfaces_array[@]}
                amount_otherInterfaces=$[$amount_Interfaces-2]
                NetIfConf=1
                CalculateNetworkAddress
        fi
}


ExtractPPPoEData()
{
	local R parts
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <Enabled>,<Username>,<Password>
		PPPoEEnabled=$(CommaField 1 "$R")
		PPPoEUser=$(CommaField 2 "$R")
		PPPoEPass=$(CommaField 3 "$R")
		PPPoEIPv6Enabled=$(CommaField 4 "$R")
	fi
}

ExtractIPv6Data()
{
	local R IntPart ExtPart
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <Ext If>,<Ext IP>,<Ext Netmask>,<Gateway>,<Ext DNS CSV>|<Int If>,<Int IP>,<Int Netmask>
		# <Ext If>,disabled|<Int If>,<Int IP>,<Int Netmask>
		# <Ext If>,dhcp|<Int If>,<Int IP>,<Int Netmask>
		# <Ext If>,ra|<Int If>,<Int IP>,<Int Netmask>
		ExtPart=$(echo "$R" | cut -d'|' -f1)
		IntPart=$(echo "$R" | cut -d'|' -sf2)
		if [ -n "$IntPart" ]; then
			Intv6If=$(CommaField 1 "$IntPart")
			Intv6IP=$(CommaField 2 "$IntPart")
			Intv6Netmask=$(CommaField 3 "$IntPart")
		fi

		if [ -n "$ExtPart" ]; then
			Extv6If=$(CommaField 1 "$ExtPart")
			Extv6IP=$(CommaField 2 "$ExtPart")
			if [[ "$Extv6IP" != "dhcp" && "$Extv6IP" != "disabled" ]]; then
				Extv6Netmask=$(CommaField 3 "$ExtPart")
				v6Gateway=$(CommaField 4 "$ExtPart")
				v6DNS=$(CommaField 5- "$ExtPart")
			fi
		fi
		
		IFS='|' read -a IPv6_interfaces_array <<< "$R"
                amount_ipv6_Interfaces=${#IPv6_interfaces_array[@]}
                amount_other6Interfaces=$[$amount_ipv6_Interfaces-2]
		Netv6IfConf=1
		#CalculateNetworkAddress
	fi
}

ExtractIPv6TunnelData()
{
	local R
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# 1-<Tunnelbroker>,2-<TunnelID>,3-<Endpoint>,4-<IPv6>,5-<IPv6 netmask>,6-<LAN prefix>,7-<LAN netmask>,8-<UserID>,9-<Password>,10-<Activate>,11-<Dynamic IPv4>,12-<RA enabled>
		
		IPv6TunnelBroker=$(CommaField 1 "$R")
		IPv6TunnelID=$(CommaField 2 "$R")
		IPv6Endpoint=$(CommaField 3 "$R")
		IPv6IP=$(CommaField 4 "$R")
		IPv6Netmask=$(CommaField 5 "$R")
		IPv6Net=$(CommaField 6 "$R")
		IPv6NetNetmask=$(CommaField 7 "$R")
		IPv6UserID=$(CommaField 8 "$R")
		IPv6Password=$(CommaField 9 "$R")
		IPv6TunnelActive=$(CommaField 10 "$R")
		IPv6DynamicIPv4=$(CommaField 11 "$R")
		IPv6RAenabled=$(CommaField 12 "$R")
	fi
}

ExtractVPNData()
{
	local R
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <enabled>,<IPrange>,<PSK>
		VPNenabled=$(CommaField 1 "$R")
		VPNrange=$(CommaField 2 "$R")
		VPNPSK=$(CommaField 3 "$R")
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

Hostname=DCERouter
Domainname=linuxmce.local

PPPoEEnabled=off
PPPoEUser=
PPPoEPass=

IntIf=
IntIP=
IntNetmask=
ExtIf=
ExtIP=
ExtNetmask=
Gateway=
DNS=
NetIfConf=0

Intv6If=
Intv6IP=
Intv6Netmask=
Extv6If=
Extv6IP=
Extv6Netmask=
v6Gateway=
v6DNS=
Netv6IfConf=0

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
IPv6TunnelActive=off
IPv6DynamicIPv4=0
IPv6RAenabled=0

VPNenabled=off
VPNrange=
VPNPSK=

NCards=$(ip addr | grep "^[0-9]*:" | grep -v "^[0-9]*: lo" | grep -v "^[0-9]*: pan" | grep -c ".")

# Host- and domainname
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Hostname"
Hostname=$(RunSQL "$Q")
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Domainname"
Domainname=$(RunSQL "$Q")

# PPPoe config
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_PPPoE"
R=$(RunSQL "$Q")
ExtractPPPoEData "$R"

# IPv4 interfaces config
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_Interfaces"
R=$(RunSQL "$Q")
ExtractData "$R"

# IPv6 interfaces config
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_Interfaces_IPv6"
R=$(RunSQL "$Q")
ExtractIPv6Data "$R"

# IPv6 tunnel data
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_Interfaces_IPv6_tunnel"
R=$(RunSQL "$Q")
ExtractIPv6TunnelData "$R"

# VPN data
Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=$DEVICEDATA_Network_VPN"
R=$(RunSQL "$Q")
ExtractVPNData "$R"

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
		else
			NICS=$(ip -o link | grep "link/ether" | awk '{ print $2; }' | cut -d':' -f1)
			for interface in $NICS ; do
				if [ "$interface" != "$extif" ] ; then
					intif=$interface
					break
				fi
			done
		fi
		Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
                IntIP=$(RunSQL "$Q")
		if [[ "$IntIP" == "" ]] ;then
			IntIP=192.168.80.1
		fi
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
