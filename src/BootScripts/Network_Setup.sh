#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

# vars:
# CORE_INTERNAL_ADDRESS

Vars="CORE_INTERNAL_ADDRESS"

if [[ "$NetIfConf" == 0 || "$NPflagReconfNetwork" == yes ]]; then
	if [[ "$NetIfConf" == 0 ]]; then
		echo "First network config"
	elif [[ "$NPflagReconfNetwork" == yes ]]; then
		echo "Forced network config"
	fi
	echo "Populating network settings from current system config"
	NetSettings=$(ParseInterfaces)
	ExtData=$(echo "$NetSettings" | head -1)
	ExtractData "$ExtData"
	if [[ -z "$DHCPsetting" ]]; then
		echo "No DHCP Setting found. Not storing internal interface data"
		if [ "$ExtIP" == "dhcp" ]; then
			NetIfConf="$ExtIf,dhcp|"
		else
			NetIfConf="$ExtIf,$ExtIP,$ExtNetmask,$Gateway,$DNS|"
		fi
		if [[ "$NCards" -eq 1 ]]; then
			IntIf="eth0:0"
		else
			[[ "$ExtIf" == "eth0" ]] && IntIf="eth1" || IntIf="eth0"
		fi

		Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
	        IntIP=$(RunSQL "$Q")
		if [[ "$IntIP" == "" ]] ;then
			IntIP="192.168.80.1"
		fi
		#IntIP=192.168.80.1
		IntNetmask=255.255.255.0
	else
		echo "Using DHCP Setting to set up internal interface data: '$DHCPsetting'"
		echo "Found $NCards network cards"
		IntNetmask="255.255.255.0"
		IntIP="$(echo "$DHCPsetting" | cut -d. -f-3).1"
		if [[ "$NCards" -eq 1 ]]; then
			IntIf="eth0:0"
		else
			[[ "$ExtIf" == "eth0" ]] && IntIf="eth1" || IntIf="eth0"
		fi
		if [[ "$ExtIP" == "dhcp" ]]; then
			NetIfConf="$ExtIf,dhcp|$IntIf,$IntIP,$IntNetmask"
		else
			NetIfConf="$ExtIf,$ExtIP,$ExtNetmask,$Gateway,$DNS|$IntIf,$IntIP,$IntNetmask"
		fi
	fi
	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$PK_Device',32,'$NetIfConf')"
	RunSQL "$Q"
fi

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=292"
IPv6TunnelSettings=$(RunSQL "$Q")
IPv6Active=`echo $IPv6TunnelSettings | cut -d"," -f 10`
# check if IPv6 tunnel settings enabled
if [[ "$IPv6Active" == "on" ]]; then
	IPv6TunnelBroker=`echo $IPv6TunnelSettings | cut -d"," -f 1`
	echo "IPv6 tunnel activated ($IPv6TunnelBroker), fetching settings from core data"
	IPv6If=ipv6tunnel
	IPv6TunnelID=`echo $IPv6TunnelSettings | cut -d"," -f 2`
	IPv6Endpoint=`echo $IPv6TunnelSettings | cut -d"," -f 3`
	IPv6IP=`echo $IPv6TunnelSettings | cut -d"," -f 4`
	IPv6Netmask=`echo $IPv6TunnelSettings | cut -d"," -f 5`
	IPv6Prefix=`echo $IPv6TunnelSettings | cut -d"," -f 6`
	IPv6PrefixNetmask=`echo $IPv6TunnelSettings | cut -d"," -f 7`
	IPv6Username=`echo $IPv6TunnelSettings | cut -d"," -f 8`
	IPv6Password=`echo $IPv6TunnelSettings | cut -d"," -f 9`
fi


Q="GRANT ALL PRIVILEGES ON *.* TO '$MySqlUser'@$IntIP"
RunSQL "$Q"
echo "Writing network configuration with one in database"


service radvd stop
service networking stop
service dhcp3-server stop

if ! BlacklistConfFiles '/etc/network/interfaces' ;then
	if [ ! -e /etc/network/interfaces.backup ] ;then
		cp /etc/network/interfaces /etc/network/interfaces.backup
	fi	
	File=/etc/network/interfaces
	#cp "$File" "$File.%(date +%F-%T)"
	IfConf="auto lo
	iface lo inet loopback
	"
	echo "$IfConf" >"$File"
fi

[[ "$ExtIP" == "dhcp" ]] && Setting="dhcp" || Setting="static"
if [[ "$Setting" == "static" ]]; then
	IfConf="auto $ExtIf
iface $ExtIf inet static
	address $ExtIP
	netmask $ExtNetmask
	gateway $Gateway"
	echo "$IfConf" >>"$File"

	DNSservers=$(echo "$DNS,"; echo "$NetSettings" | tail -1)
	DNSservers=$(echo "$DNSservers" | tr ',' '\n' | sort -u | tr '\n' ' ')
	if [ ! -e /etc/resolv.conf.pbackup ] ;then
		cp /etc/resolv.conf /etc/resolv.conf.pbackup
	fi
	: >/etc/resolv.conf
	if ! BlacklistConfFiles '/etc/resolv.conf' ;then
		for i in $DNSservers; do
			echo "nameserver $i" >>/etc/resolv.conf
		done
	fi
else
	IfConf="auto $ExtIf
iface $ExtIf inet dhcp"
	echo "$IfConf" >>"$File"
fi

IfConf="auto $IntIf
iface $IntIf inet static
	address $IntIP
	netmask $IntNetmask"
echo "$IfConf" >>"$File"

echo "">"/etc/radvd.conf"

if [[ "$IPv6RAenabled" == "on" ]]; then
	# Config IPv6 tunnel if enabled in advanced network setup
	echo "IPv6 tunnel activated ($IPv6TunnelBroker), configuring interface"
	IfConf="auto $IPv6If
	iface $IPv6If inet6 v4tunnel
	address $IPv6IP
	netmask $IPv6Netmask
	local $IntIP
	endpoint $IPv6Endpoint
	up ip -6 route add $IPv6Prefix/$IPv6PrefixNetmask dev $IntIf;ip -6 route add default dev $IPv6If
	down ip -6 route flush dev $IPv6If
	mtu 1480"
	echo "$IfConf" >>"$File"
	# Configure RA daemon
	RAConf="interface $IntIf {
	AdvSendAdvert on;
	MinRtrAdvInterval 3;
	MaxRtrAdvInterval 10;
	prefix $IPv6Prefix/$IPv6PrefixNetmask {
		AdvOnLink on;
		AdvAutonomous on;
		AdvRouterAddr off;
	};
	};"
	echo "$RAConf">"/etc/radvd.conf"	
	if [[ ! $(grep "net.ipv6.conf.default.forwarding=1" /etc/sysctl.conf) ]]; then
		echo "net.ipv6.conf.default.forwarding=1">>"/etc/sysctl.conf"
	fi
fi

if ! BlacklistConfFiles '/etc/default/dhcp3-server' ;then
	if [ ! -e /etc/default/dhcp3-server.pbackup ] && [ -e /etc/default/dhcp3-server ] ;then
		cp /etc/default/dhcp3-server /etc/default/dhcp3-server.pbackup
	fi

	if [[ -n "$DHCPcard" ]]; then
		echo "INTERFACES=\"$DHCPcard\"" >/etc/default/dhcp3-server
	elif [[ "$IntIf" != *:* ]]; then
		echo "INTERFACES=\"$IntIf\"" >/etc/default/dhcp3-server
	else
		echo "INTERFACES=\"$ExtIf\"" >/etc/default/dhcp3-server
	fi
fi

/usr/pluto/bin/DHCP_config.sh

service networking start
service dhcp3-server start
if [[ "$IPv6RAenabled" == "on" ]]; then
	echo "starting RA daemon to hand out IPv6 addresses to local network"
	service radvd start
fi

if ! BlacklistConfFiles '/etc/bind/named.conf.forwarders' && ! BlacklistConfFiles '/etc/bind/named.conf.options';then
if [ ! -e /etc/bind/named.conf.forwarders.pbackup ] && [ -e /etc/bind/named.conf.forwarders ] ;then
	cp /etc/bind/named.conf.forwarders /etc/bind/named.conf.forwarders.pbackup
fi	

if [[ "$Setting" == static ]]; then
	/usr/pluto/bin/Network_DNS.sh
else
	touch /etc/bind/named.conf.forwarders # Make sure file exists so BIND starts
fi

if [ ! -e /etc/bind/named.conf.options.pbackup ] ;then
	cp /etc/bind/named.conf.options /etc/bind/named.conf.options.pbackup
fi
awk 'BEGIN { Replace = 0 }
/\/\/ forwarders/ {
	Replace = 3;
	print("include \"/etc/bind/named.conf.forwarders\";\n");
}
Replace == 0 { print }
Replace != 0 { Replace-- }
' /etc/bind/named.conf.options >/etc/bind/named.conf.options.$$
mv /etc/bind/named.conf.options.$$ /etc/bind/named.conf.options
fi

rndc reload

AskPort80="
Right now your Pluto Core will only allow you to access from within the local
network in your home. (Technical: All incoming ports are blocked).  

Do you want to access the Pluto web sites from a web browser outside the home
(ie open port 80)?  Note that if you answer 'yes' we recommend you get an SSL
certificate to encrypt the connection, otherwise it would be possible for
someone to get your username and password and gain access to the system. If in
doubt, just answer 'no'.

Allow web access? [y/N] "

AskPort22="
Do you want to allow incoming SSH connections by opening port 22?  This is a
secure way of accessing the system, but is only for techies and Linux users.

Allow SSH connections? [y/N]?"

#clear

#echo -n "$AskPort80"
#read Answer

#if [ "$Answer" == "y" -o "$Answer" == "Y" ]; then
#	echo "Storing your option to open port 80 (allowing web access)"
#	Q="INSERT INTO Firewall(Protocol,SourcePort,RuleType) VALUES('tcp',80,'core_input')"
#	RunSQL "$Q"
#fi

#echo -n "$AskPort22"
#read Answer

#if [ "$Answer" == "y" -o "$Answer" == "Y" ]; then
#	echo "Storing your option to open port 22 (allowing SSH access)"
#	Q="INSERT INTO Firewall(Protocol,SourcePort,RuleType) VALUES('tcp',22,'core_input')"
#	RunSQL "$Q"
#fi

RealExtIP="$ExtIP"
dcerouterIP="$IntIP"
if [[ "$ExtIP" == "dhcp" ]]; then
	RealExtIP=$(ip addr ls "$ExtIf" | grep "inet .*$ExtIf\$" | awk '{print $2}' | cut -d/ -f1)
fi
if [[ -z "$dcerouterIP" ]]; then
	dcerouterIP="$RealExtIP" #dcerouterIP="127.0.0.1"
fi
Q="UPDATE Device SET IPaddress='$dcerouterIP' WHERE PK_Device='$PK_Device'"
RunSQL "$Q"

if ! BlacklistConfFiles '/etc/hostname' ;then
	if [ ! -e /etc/hostname.pbackup ] ;then
		cp /etc/hostname /etc/hostname.pbackup
	fi

	# set the hostname to 'dcerouter'
	echo dcerouter >/etc/hostname
	hostname dcerouter
fi

PopulateSection "/etc/hosts" "NetworkSetup" "${dcerouterIP} dcerouter"


Q="FLUSH PRIVILEGES"
RunSQL "$Q"


## Setup Samba Domain / Hostname
DD_Domain=187
DD_ComputerName=188

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DD_Domain AND FK_Device=$PK_Device"
DomainName=$(RunSQL "$Q")
DomainName=$(Field "1" "$DomainName")
if [[ "$DomainName" == "" ]] ;then
	DomainName="LinuxMCE"
fi

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DD_ComputerName AND FK_Device=$PK_Device"
ComputerName=$(RunSQL "$Q")
ComputerName=$(Field "1" "$ComputerName")

## Configure Samba Server
if [ ! -e /etc/samba/smb.conf.pbackup ] ;then
	cp /etc/samba/smb.conf /etc/samba/smb.conf.pbackup
fi

SambaDomainHost="
	workgroup = $DomainName
	server string =	$ComputerName
	netbios name = $ComputerName
"
PopulateSection "/etc/samba/smb.conf" "Domain and Hostname" "$SambaDomainHost"

if [[ "$(pidof smbd)" != "" ]] ;then
	service samba reload || service smbd reload
fi

if ! BlacklistConfFiles '/etc/defaultdomain' && ! BlacklistConfFiles '/etc/default/nis' ;then
	if [ ! -e /etc/defaultdomain.pbackup ] ;then
		cp /etc/defaultdomain /etc/defaultdomain.pbackup
	fi


	if [ ! -e /etc/default/nis.pbackup ] ;then
		cp /etc/default/nis /etc/default/nis.pbackup
	fi

	## Configure NIS Server
	echo "pluto" > /etc/defaultdomain
	cp /usr/pluto/templates/nis-server.template /etc/default/nis
fi

## Configure mediatomb
if [ -e /etc/default/mediatomb ] ;then
	sed -i "s/^INTERFACE=\"*.*\"*/INTERFACE=\"${IntIf}\"/" /etc/default/mediatomb
fi

## Configure UPnP Internet Gateway Device daemon
if [ -n "$IntIf" ] && [ -z "$( echo $IntIf | grep : )"  ] && [ -e /etc/default/linux-igd ] ;then
	sed -i "s/^#INTIFACE/INTIFACE/" /etc/default/linux-igd
	sed -i "s/^#EXTIFACE/EXTIFACE/" /etc/default/linux-igd
	sed -i "s/^INTIFACE=\"*\(.*\)\"*/INTIFACE=${IntIf}/" /etc/default/linux-igd
	sed -i "s/^EXTIFACE=\"*\(.*\)\"*/EXTIFACE=${ExtIf}/" /etc/default/linux-igd
fi

/usr/pluto/bin/Network_NIS.sh
