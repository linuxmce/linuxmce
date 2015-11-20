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

Q="GRANT ALL PRIVILEGES ON *.* TO '$MySqlUser'@$IntIP"
RunSQL "$Q"
echo "Writing network configuration files with settings from database"

# Shut down PPPoE related services
/usr/bin/poff -a > /dev/null

# Shut down DHCP (v4 & v6) and RA related services
if pidof dhcpd > /dev/null; then
	service isc-dhcp-server stop
fi
if pidof radvd > /dev/null; then
	service radvd stop
fi
if pidof dhcp6s > /dev/null; then
	service wide-dhcpv6-server stop
fi
if pidof dhcp6c > /dev/null; then
	service wide-dhcpv6-client stop
fi

# Stop VPN related services
if pidof xl2tpd > /dev/null; then
	service xl2tpd stop
fi
if pidof ipsec/pluto > /dev/null; then
	service ipsec stop
fi

# Empty resolv.conf files. Will be filled with static entries, or autofilled by dhcp
if [ ! -e /etc/resolv.conf.pbackup ] ;then
	cp /etc/resolv.conf /etc/resolv.conf.pbackup
fi
: >/etc/resolv.conf

# configure PPPoE peer file
if [[ "$PPPoEEnabled" == "on" ]]; then
papsecrets=("${papsecrets[@]}" "#PPPOE Configuration\n")
	echo "Configuring PPPoE xDSL provider"
	File=/etc/ppp/peers/dsl-provider
	echo "#####
# LinuxMCE xDSL PPPoE configuration file
#####
noipdefault
defaultroute
replacedefaultroute
hide-password
noauth
persist
noaccomp
default-asyncmap
plugin rp-pppoe.so $ExtIf
user \"$PPPoEUser\"
usepeerdns
debug" >"$File"
	if [[ $PPPoEIPv6Enabled == "on" ]]; then
		echo "Enabling native IPv6 on PPPoE connection"
		echo "+ipv6 ipv6cp-use-ipaddr" >>"$File"
		dhcpv6=("${dhcpv6[@]}" "ppp0")
	fi

	# Add actual secret to the new file
	papsecrets=("${papsecrets[@]}" "\"$PPPoEUser\" * \"$PPPoEPass\"\n")
	
fi

if ! BlacklistConfFiles '/etc/network/interfaces' ;then
	if [ ! -e /etc/network/interfaces.backup ] ;then
		cp /etc/network/interfaces /etc/network/interfaces.backup
	fi	
	File=/etc/network/interfaces
	# cp "$File" "$File.%(date +%F-%T)"
	auto=("${auto[@]}" "lo")
	IfConf="#####
# Loopback interface
#####
iface lo inet loopback"
	echo "$IfConf" >"$File"
fi

# xDSL PPPoE interface configuration
if [[ "$PPPoEEnabled" == "on" ]];then
	echo "PPPoE interface activated for xDSL connection"
	auto=("${auto[@]}" "dsl-provider")
	echo "
#####
# xDSL PPPoE interface
#####
iface dsl-provider inet ppp
	pre-up    /sbin/ifconfig $ExtIf up
	up /etc/wide-dhcpv6/dhcp6c-ifupdown start
	down /etc/wide-dhcpv6/dhcp6c-ifupdown stop
	provider dsl-provider">>"$File"
fi

# IPv4 interfaces configuration
## get Domain for Internal DNS configuration
DD_Domain=187

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DD_Domain AND FK_Device=$PK_Device"
DomainName=$(RunSQL "$Q")
DomainName=$(Field "1" "$DomainName")
if [[ "$DomainName" == "" ]] ;then
        DomainName="LinuxMCE"
fi

echo "
#####
# IPv4 network interfaces
#####

# --- External NIC ---" >>"$File"
case $ExtIP in
	"disabled")
		Setting="disabled"
	;;
	"dhcp")
		Setting="dhcp"
		if [[ ! "$PPPoEEnabled" == "on" ]];then
			auto=("${auto[@]}" $ExtIf)
			IfConf="iface $ExtIf inet dhcp"
			echo "$IfConf" >>"$File"
			if [[ "$Extv6IP" == "disabled" ]]; then
				echo "	pre-up sysctl -q -e -w  net.ipv6.conf.$ExtIf.disable_ipv6=1" >>"$File"
			else
				echo "	pre-up sysctl -q -e -w  net.ipv6.conf.$ExtIf.disable_ipv6=0" >>"$File"
			fi
		fi
	;;
	*)
		Setting="static"
		auto=("${auto[@]}" $ExtIf)
		IfConf="iface $ExtIf inet static
	address $ExtIP
	netmask $ExtNetmask
	gateway $Gateway"
		echo "$IfConf" >>"$File"
		if [[ "$Extv6IP" == "disabled" ]]; then
			echo "	pre-up sysctl -q -e -w  net.ipv6.conf.$ExtIf.disable_ipv6=1" >>"$File"
		else
			echo "	pre-up sysctl -q -e -w  net.ipv6.conf.$ExtIf.disable_ipv6=0" >>"$File"
		fi
		DNSservers=$(echo "$DNS,"; echo "$NetSettings" | tail -1)
		DNSservers=$(echo "$DNSservers" | tr ',' '\n' | sort -u | tr '\n' ' ')
		if ! BlacklistConfFiles '/etc/resolv.conf' ;then
			for i in $DNSservers; do
				echo "nameserver $i" >>/etc/resolv.conf
			done
		fi
	;;
esac

auto=("${auto[@]}" $IntIf)
if [[ $IntIf == br* ]]; then
        #find the interfacces for this bridge
i="0"
j="2"
while [ $i -lt $amount_otherInterfaces ] #traverse trough elements
  do
        otherinterfaceIf=$(CommaField 1 "${interfaces_array[$j]}")
        otherinterfaceIP=$(CommaField 2 "${interfaces_array[$j]}")
         if [ $otherinterfaceIP == $IntIf ]; then
                BridgedIF=("${BridgedIF[@]}" $otherinterfaceIf)
        fi
    i=$[$i+1]
    j=$[$j+1]
  done
        echo "
# --- Internal NIC ---
iface $IntIf inet static
        bridge_ports ${BridgedIF[@]}
        address $IntIP
        netmask $IntNetmask" >>"$File"

else
echo "
# --- Internal NIC ---
iface $IntIf inet static
        address $IntIP
        netmask $IntNetmask" >>"$File"
fi
if [[ "$Intv6IP" == "disabled" ]]; then
        echo "  pre-up sysctl -q -e -w  net.ipv6.conf.$IntIf.disable_ipv6=1" >>"$File"
else
        echo "  pre-up sysctl -q -e -w  net.ipv6.conf.$IntIf.disable_ipv6=0" >>"$File"
fi
echo "        # DNS Settings for Internal Net
        dns-nameservers $IntIP
        dns-search $DomainName" >>"$File"
echo "IPv4: External $ExtIf=$Setting, internal $IntIf=static"

#other interfaces configuration
i="0"
j="2"
while [ $i -lt $amount_otherInterfaces ] #traverse trough elements
  do
        otherinterfaceIf=$(CommaField 1 "${interfaces_array[$j]}")
        otherinterfaceIP=$(CommaField 2 "${interfaces_array[$j]}")
        if [ -n $otherinterfaceIP  -a $otherinterfaceIP != $otherinterfaceIf ]; then
                if [[ $otherinterfaceIP == br* ]]; then
                        otherinterfaceModus="bridged"
                elif [ $otherinterfceIP == "DHCP" -a $otherinterfaceIP == "dhcp" ]; then
                        otherinterfaceModus="dhcp"
                elif [ $otherinterfaceIP == "vlan" ]; then
                        otherinterfaceModus="static"
                        otherinterfaceIP=$(CommaField 3 "${interfaces_array[$j]}")
                        otherinterfaceNM=$(CommaField 4 "${interfaces_array[$j]}")
                else
                        otherinterfaceModus="static"
                        otherinterfaceNM=$(CommaField 3 "${interfaces_array[$j]}")
                fi
        auto=("${auto[@]}" $otherinterfaceIf)
	otheripv6IP=$otherinterfaceIP
	if [ $otherinterfaceModus != "bridged" ]; then

echo "
# --- $otherinterfaceIf  ---
iface $otherinterfaceIf inet $otherinterfaceModus" >>"$File"
                        if [ $otherinterfaceModus == "static" ]; then
echo "  address $otherinterfaceIP
        netmask $otherinterfaceNM" >>"$File"
        if [[ "$otherv6IP" == "disabled" ]]; then
                echo "  pre-up sysctl -q -e -w  net.ipv6.conf.$otherinterfaceIf.disable_ipv6=1" >>"$File"
        else
                echo "  pre-up sysctl -q -e -w  net.ipv6.conf.$otherinterfaceIf.disable_ipv6=0" >>"$File"
        fi
        echo "        # DNS Settings for Internal Net
        dns-nameservers $otherinterfaceIP
        dns-search $DomainName" >>"$File"
                        fi
        else
echo "
# --- $otherinterfaceIf  ---
iface $otherinterfaceIf inet manual" >>"$File"
	fi
        fi
    i=$[$i+1]
    j=$[$j+1]
  done

# IPv6 interfaces configuration
v6=""
case $Extv6IP in
	"disabled")
		ExtSetting="disabled"
	;;
	"ra")
		ExtSetting="ra"
	;;
	"dhcp")
		ExtSetting="dhcp"
		auto=("${auto[@]}" $Extv6If)
		dhcpv6=("${dhcpv6[@]}" $Extv6If)
	;;
	*)
		ExtSetting="error"
		if [[ "$Extv6IP" != "" ]]; then	
			ExtSetting="static"
			auto=("${auto[@]}" $Extv6If)
			v6="$v6
# --- External NIC
iface $Extv6If inet6 static
	address $Extv6IP
	netmask $Extv6Netmask
	gateway $v6Gateway"
		
	DNSservers=$(echo "$v6DNS")
	DNSservers=$(echo "$DNSservers" | tr ',' '\n' | sort -u | tr '\n' ' ')
		if ! BlacklistConfFiles '/etc/resolv.conf' ;then
			for i in $DNSservers; do
				echo "nameserver $i" >>/etc/resolv.conf
			done
		fi
	fi
	;;
esac

case $Intv6IP in
	"disabled")
		IntSetting="disabled"
	;;
	"ra")
		IntSetting="ra"
	;;
	
	*)
		IntSetting="error"
		if [[ "$Intv6IP" != "" ]]; then
			IntSetting="static"
			auto=("${auto[@]}" $Intv6If)
			v6="$v6
# --- Internal NIC
iface $Intv6If inet6 static
	address $Intv6IP
	netmask $Intv6Netmask"
		fi
	;;
esac

#other IPv6Interfaces configuration
i=0;
j=2;

while [ $i -lt $amount_other6Interfaces ] #traverse trough elements
 do
        other6interfaceIf=$(CommaField 1 "${IPv6_interfaces_array[$j]}")
        other6interfaceIP=$(CommaField 2 "${IPv6_interfaces_array[$j]}")
        case $other6interfaceIP in
                "disabled")
                        IntSetting="disabled"
                ;;
                "ra")
                        IntSetting="ra"
                ;;

                *)
                        IntSetting="error"
                        if [[ "$other6interfaceIP" != "" && "$other6interfaceIP" != "$other6interfaceIf" ]]; then
                                IntSetting="static"
                                #auto=("${auto[@]}" $other6interfaceIf)
                                v6="$v6

# --- $other6interfaceIf
iface $other6interfaceIf inet6 static
        address $other6interfaceIP
        netmask $(CommaField 3 "${IPv6_interfaces_array[$j]}")"
                fi
        ;;
esac
   i=$[$i+1]
   j=$[$j+1]
 done


if [[ "$ExtSetting" == "static" ||  "$IntSetting" == "static" || "$IPv6TunnelActive" == "on" ]]
then
	echo "
#####
# IPv6 network interfaces
#####" >>"$File"
	echo "$v6" >>"$File"
fi


echo "IPv6: External $Extv6If=$ExtSetting, internal $Intv6If=$IntSetting"

echo -n "">"/etc/radvd.conf"

if [ -e /etc/cron.d/UpdateIPv6TunnelEndPoint ] ;then
	rm /etc/cron.d/UpdateIPv6TunnelEndPoint
fi

if [[ "$IPv6TunnelActive" == "on" ]]; then
	# Config IPv6 tunnel if enabled in advanced network setup
	echo "IPv6 tunnel activated ($IPv6TunnelBroker), configuring interface"
	auto=("${auto[@]}" $IPv6If)
	if [[ $IPv6TunnelBroker == SIXXS ]]; then
			IfConf="
	# --- IPv6 tunnel to $IPv6TunnelBroker
		iface $IPv6If inet6 v4tunnel
		address $IPv6IP
		netmask $IPv6Netmask
		ttl 64
		endpoint $IPv6Endpoint
		up ip -6 route add default dev $IPv6If
		down ip -6 route flush dev $IPv6If
		mtu 1480"
	else
			IfConf="# --- IPv6 tunnel to $IPv6TunnelBroker
		iface $IPv6If inet6 v4tunnel
		address $IPv6IP
		netmask $IPv6Netmask
		local $IntIP
		endpoint $IPv6Endpoint
		up ip -6 route add $IPv6Prefix/$IPv6PrefixNetmask dev $IntIf;ip -6 route add default dev $IPv6If
		down ip -6 route flush dev $IPv6If
		mtu 1480"
	fi
	echo "$IfConf" >>"$File"
	if [[ "$IPv6DynamicIPv4" == "on" ]]; then
		echo "Dynamic IPv4 activated, creating scripts to update tunnel endpoint"
		md5pass=`echo -n $IPv6Password | md5sum | sed -e 's/  -//g'`
		echo "*/5 * * * * root bash -c 'wget -q --no-check-certificate \"https://ipv4.tunnelbroker.net/ipv4_end.php?ipv4b=AUTO&pass=$md5pass&user_id=$IPv6UserID&tunnel_id=$IPv6TunnelID\" -O /dev/null'" >> /etc/cron.d/UpdateIPv6TunnelEndPoint
	fi
fi

# Sort list of interfaces to run dhcpv6 client on and remove double entries
OLDIFS="$IFS"
IFS=$'\n'
dhcpv6=(`for R in "${dhcpv6[@]}"; do echo "$R" ; done | sort -du`)
auto=(`for R in "${auto[@]}"; do echo "$R" ; done | sort -du`)
IFS="$OLDIFS"

echo "Enabling interfaces: ${auto[@]}"
echo "
#####
# Activating interfaces
#####
auto ${auto[@]}" >>"$File"

#
# Scripts on Ipv6 UP
#
# Set IPv6 default route to ppp0 interface when IPv6 interface comes up
echo "#!/bin/bash
ip -6 route add default dev ppp0" >"/etc/ppp/ipv6-up.d/routing"
# Start IPv6 DHCP client when IPv6 interface comes up, or restart it when already running
echo "#!/bin/bash
service wide-dhcpv6-client restart" >"/etc/ppp/ipv6-up.d/dhcp-client"
# Restart Apache2 to make it listen on IPv6
echo "#!/bin/bash
service apache2 restart" >"/etc/ppp/ipv6-up.d/apache2"
#
# Scripts on IPv6 DOWN
#
# Remove IPv6 default route to ppp0 interface when IPv6 interface goes down
echo "#!/bin/bash
ip -6 route del default dev ppp0" >"/etc/ppp/ipv6-down.d/routing"
# Restart IPv6 DHCP client when IPv6 interface goes down
echo "#!/bin/bash
service wide-dhcpv6-client restart" >"/etc/ppp/ipv6-down.d/dhcp-client"
# Restart Apache2 to reinitialize available IP's
echo "#!/bin/bash
service apache2 restart" >"/etc/ppp/ipv6-down.d/apache2"

chmod 755 /etc/ppp/ipv6-up.d/*
chmod 755 /etc/ppp/ipv6-down.d/*
	
# Writes default values for DHCPv6 client
echo "Running DHCPv6 client on interfaces: ${dhcpv6[@]}"
echo "
#####
# LinuxMCE DHCPv6 client interfaces
#####
INTERFACES=\"${dhcpv6[@]}\"" >"/etc/default/wide-dhcpv6-client"

echo "#####
# LinuxMCE DHCPv6 client configuration
#####" >"/etc/wide-dhcpv6/dhcp6c.conf"

if [[ "$PPPoEEnabled" == "on" ]]; then
	echo "
interface ppp0{
  send ia-pd 0;
  request domain-name-servers, domain-name;
  send domain-name-servers, domain-name;
  send rapid-commit;
  script \"/etc/wide-dhcpv6/dhcp6c-script\";
};" >>"/etc/wide-dhcpv6/dhcp6c.conf"
fi

for interface in "${dhcpv6[@]}"
do
	echo "
interface $interface{
  request domain-name-servers, domain-name;
  send domain-name-servers, domain-name;
  send rapid-commit;
  script \"/etc/wide-dhcpv6/dhcp6c-script\";
};" >>"/etc/wide-dhcpv6/dhcp6c.conf"
done

echo "
id-assoc pd {
  prefix-interface $Extv6If{
    sla-id 1;
    sla-len 8;
  };
    prefix-interface $Intv6If{
    sla-id 2;
    sla-len 8;
  };
};" >>"/etc/wide-dhcpv6/dhcp6c.conf"

if [[ "$IPv6RAenabled" == "on" ]]; then
	# If RA enabled and no prefix length is given use default
	if [[ "$IPv6NetNetmask" == "" ]]; then
		IPv6NetNetmask="64";
	fi
        if [[ "$IPv6Net" == "" ]]; then
                IPv6Net="::";
        fi
	# Configure RA daemon
	echo "IPv6 RA daemon enabled, creating config files"	
	RAConf="interface $IntIf {
	AdvSendAdvert on;
	MinRtrAdvInterval 3;
	MaxRtrAdvInterval 10;
	prefix $IPv6Net/$IPv6NetNetmask {
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

if [ -e /etc/ipsec.conf ] ;then
	rm /etc/ipsec.conf
fi

if [ -e /etc/ipsec.secrets ] ;then
	rm /etc/ipsec.secrets
fi

if [[ "$VPNenabled" == "on" ]]; then
    # Config L2TP/IPSEC VPN server
    echo "L2TP/IPSEC VPN server enabled, setting up"

	# Disabled redirects to prevent Netkey from sending bogus icmp packets
	for each in /proc/sys/net/ipv4/conf/*
	do
    		echo 0 > $each/accept_redirects
    		echo 0 > $each/send_redirects
	done

        # OpenSWAN IPSEC config files
	CORE_MASK=$(ip addr show $IntIf|awk '/inet .* '"$IntIf"'/ {print $2}'|awk -F \/ '{print $2}'|head -1)
        sed -r "s,%VPNPSK%,$VPNPSK,g" /usr/pluto/templates/ipsec.secrets.tmpl >/etc/ipsec.secrets
	echo $IntNetworkAddress $CORE_MASK
	sed -r "s,%CORE_NET%,$IntNetworkAddress,g;s,%CORE_MASK%,$CORE_MASK,g" /usr/pluto/templates/ipsec.conf.tmpl > /etc/ipsec.conf

        # XL2TP config files
        sed -r "s,%VPN_IP_RANGE%,$VPNrange,g;s,%CORE_INT_IP%,$IntIP,g" /usr/pluto/templates/xl2tpd.conf.tmpl >/etc/xl2tpd/xl2tpd.conf

        # PPP config files
        sed -r "s,%CORE_INT_IP%,$IntIP,g" /usr/pluto/templates/options.xl2tpd.tmpl >/etc/ppp/options.xl2tpd

        # PPP users secret file
        Q="SELECT UserName,Password,Use_VPN,VPN_Ip FROM Users"
		R=$(RunSQL "$Q")
		papsecrets=("${papsecrets[@]}" "# Secrets for authentication using VPN and System Authentacatio\n")
		for ROW in $R; do
			User=$(Field 1 "$ROW")
			username=$(echo $User | tr '[:upper:]' '[:lower:]');
			Pass="\"\""
			use_vpn=$(Field 3 "$ROW")
			if [[ "$use_vpn" == "0" ]]; then
				papsecrets=("${papsecrets[@]}" "#pluto_$username	*	$Pass	*\n")
			else
				papsecrets=("${papsecrets[@]}" "pluto_$username	*	$Pass	$(Field 4 "$ROW")\n")
			fi
		done

	#Setup ip-up and ip-down script for pppd
        grep -w "^#Setup iptables" /etc/ppp/ip-up >/dev/null
        if [[ "$?" -ne "0" ]]; then
                echo "Setting up /etc/ppp/ip-up"

                ipup=$"#Setup iptables  and update db
if [[ ! "$6" == "" ]]; then
	if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
		R=$(mysql pluto_main -ss -e\"SELECT Protocol FROM Firewall WHERE RuleType='VPN' AND SourceIP='$5' AND Protocol='ip-ipv4' ORDER BY PK_Firewall\")
		if [ "$R" ]; then
			mysql pluto_main -ss -e \"UPDATE Firewall SET Offline='0' WHERE RuleType='VPN' AND SourceIP='$5'\"
		fi
		/usr/pluto/bin/Network_Firewall.sh
	fi
fi"

                echo "$ipup" >> /etc/ppp/ip-up
        fi
        grep -w "^#Setup iptables" /etc/ppp/ip-down >/dev/null
        if [[ "$?" -ne "0" ]]; then
                echo "Setting up /etc/ppp/ip-down"

                ipdown=$"#Setup iptables  and update db
if [[ ! "$6" == "" ]]; then
        if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
                R=$(mysql pluto_main -ss -e\"SELECT Protocol FROM Firewall WHERE RuleType='VPN' AND SourceIP='$5' AND Protocol='ip-ipv4' ORDER BY PK_Firewall\")
                if [ "$R" ]; then
                        mysql pluto_main -ss -e \"UPDATE Firewall SET Offline='1' WHERE RuleType='VPN' AND SourceIP='$5'\"
                fi
                /usr/pluto/bin/Network_Firewall.sh
        fi
fi"

                echo "$ipdown" >> /etc/ppp/ip-down
        fi
fi
#write pap-secrets config file even as VPN is not enabled as PPPOE config earlyer can also have settings for this file.
echo -e "${papsecrets[@]}"
echo -e "${papsecrets[@]}" > /etc/ppp/pap-secrets
chmod 644 /etc/ppp/pap-secrets

if ! BlacklistConfFiles '/etc/default/isc-dhcp-server' ;then
	if [ ! -e /etc/default/isc-dhcp-server.pbackup ] && [ -e /etc/default/isc-dhcp-server ] ;then
		cp /etc/default/isc-dhcp-server /etc/default/isc-dhcp-server.pbackup
	fi

	if [[ -n "$DHCPcard" ]]; then
		echo "INTERFACES=\"$DHCPcard\"" >/etc/default/isc-dhcp-server
	elif [[ "$IntIf" != *:* ]]; then
		echo "INTERFACES=\"$IntIf\"" >/etc/default/isc-dhcp-server
	else
		echo "INTERFACES=\"$ExtIf\"" >/etc/default/isc-dhcp-server
	fi
fi

/usr/pluto/bin/DHCP_config.sh
/etc/init.d/networking stop
service networking start

service isc-dhcp-server start

# Start DHCPv6 client if activated on any interface other then ppp0 (will be started from ppp scripts)
if [[ ${#dhcpv6[@]} > 0 ]]; then
	service wide-dhcpv6-client restart
fi

service cron reload

# Update firewall rules
/usr/pluto/bin/Network_Firewall.sh

# Start RA daemon if enabled in webadmin
if [[ "$IPv6RAenabled" == "on" ]]; then
	echo "starting RA daemon to hand out IPv6 addresses to local network"
	service radvd start
fi

# Start VPN related services if enabled in webadmin
if [[ "$VPNenabled" == "on" ]]; then
    echo "starting L2TP / IPSEC VPN server"
	service ipsec start
	service xl2tpd start
fi

# Connect to pppoe (dsl-provider)
if [[ "$PPPoEEnabled" == "on" ]]; then
    echo "reconnect PPPOE ..."
	pon dsl-provider
fi

if ! BlacklistConfFiles '/etc/bind/named.conf.forwarders' && ! BlacklistConfFiles '/etc/bind/named.conf.options';then
if [ ! -e /etc/bind/named.conf.forwarders.pbackup ] && [ -e /etc/bind/named.conf.forwarders ] ;then
	mv /etc/bind/named.conf.forwarders /etc/bind/named.conf.forwarders.pbackup
fi	

rm /etc/bind/named.conf.forwarders
if [[ "$Setting" == static &&  "$PPPoEEnabled" != "on" ]]; then
	/usr/pluto/bin/Network_DNS.sh
else
	# Make sure file exists so BIND starts
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

/usr/pluto/bin/Network_Bind.sh
/usr/sbin/rndc reload

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
DT_Core=7
DD_Domain=187
DD_ComputerName=188

Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DT_Core'"
Core_Device=$(RunSQL "$Q")
Core_Device=$(Field 1 "$Core_Device")

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DD_Domain AND FK_Device=$Core_Device"
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
	passdb backend = smbpasswd
"
PopulateSection "/etc/samba/smb.conf" "Domain and Hostname" "$SambaDomainHost"

if [[ "$(pidof smbd)" != "" ]] ;then
	service smbd restart
fi

if ! BlacklistConfFiles '/etc/defaultdomain' && ! BlacklistConfFiles '/etc/default/nis' ;then
	if [ ! -e /etc/defaultdomain.pbackup ] ;then
		cp /etc/defaultdomain /etc/defaultdomain.pbackup
	fi


	if [ ! -e /etc/default/nis.pbackup ] ;then
		cp /etc/default/nis /etc/default/nis.pbackup
	fi

	## Configure NIS Server
	echo "$DomainName" > /etc/defaultdomain
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

## Bind afpd to local interface only
#if ! BlacklistConfFiles '/etc/netatalk/afpd.conf' ;then
#	if !(grep -q "^-ipaddr" "/etc/netatalk/afpd.conf") ;then
#		echo "-ipaddr $IntIP" >> /etc/netatalk/afpd.conf
#	fi
#	service netatalk restart
#fi

## Regenerate snakeoil certificates in case host or domain name was changed
sudo make-ssl-cert generate-default-snakeoil --force-overwrite

## Generate updated config file for CUPS printing system
if ! BlacklistConfFiles '/etc/cups/cupsd.conf' ;then
	cp /usr/pluto/templates/cupsd.conf.tmpl /etc/cups/cupsd.conf
	sed -i "s/@IntIp/${IntIP}/" /etc/cups/cupsd.conf
	sed -i "s/@IntNet/${IntNetworkAddress}/" /etc/cups/cupsd.conf
	sed -i "s/@IntNM/${IntNetmask}/" /etc/cups/cupsd.conf
	service cups reload
	rm -f /etc/avahi/services/LMCE_AirPrint-*
	/usr/pluto/bin/airprint-generate.py --directory=/etc/avahi/services --prefix=LMCE_AirPrint-
fi

/usr/pluto/bin/Network_NIS.sh
