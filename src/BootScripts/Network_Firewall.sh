#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

AllowMark=0x01

#####
# Function to open a port on the core
#####
OpenPort()
{
	local Protocol Port FilterIP
	local FilterMsg
	local parmPort
	local PortBegin PortEnd
	
	Protocol="$1"
	IPversion="$2"
	Port="$3"
	FilterIP="$4"
	
	if [[ "$FilterIP" == "" ]] || [[ "$FilterIP" == "NULL" ]]; then
	    if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then 
		FilterIP4="0.0.0.0/0" 
	    fi
	    if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then 
		FilterIP6="::/0" 
	    fi
	    FilterMsg="; Not limited to specific IP"
	else
	    FilterMsg="; Limited to: $FilterIP"
	    FilterIP4="$FilterIP"
	    FilterIP6="$FilterIP"
	fi
  
	if [[ "$Port" == *:* ]]; then
		PortBegin="${Port%:*}"
		PortEnd="${Port#*:}"
	else
		PortBegin="$Port"
		PortEnd="$Port"
	fi

	echo "  Port: $Port/$Protocol-$IPversion$FilterMsg"

	if [[ -n "$Port" && ( "$Port" == *:* || "$Port" -ne 0 ) ]]; then
		parmPort="--dport $Port"
	fi

	# if protocol = ip then use port number as protocol number
	if [[ "$Protocol" == "ip" ]]; then
		parmPort=""
		Protocol="$(echo $Port | cut -d: -f1)"
	fi

	# apply rule to IPv4 chain if needed
  	if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
    	iptables -A INPUT -p "$Protocol" -s "$FilterIP4" $parmPort -j ACCEPT
  	fi

  	# apply rule to IPv6 chain if needed
  	if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
    	ip6tables -A INPUT -p "$Protocol" -s "$FilterIP6" $parmPort -j ACCEPT
  	fi

	# samba 139/tcp ports come paired with explicit rejects on 445/tcp
	# reason: to avoid timeout connecting to 445/tcp in smbclient
	if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
		iptables -A INPUT -p tcp -s "$FilterIP4" --dport 445 -j REJECT
		ip6tables -A INPUT -p tcp -s "$FilterIP6" --dport 445 -j REJECT
	fi
}

#####
# Function to forward a specific port to internal lan
#####
ForwardPort()
{
	local Protocol ExtIP SrcPort DestIP DestPort FilterIP
	local FilterMsg

	Protocol="$1"
	IPversion="$2"
	ExtIP="$3"
	SrcPort="$4"
	DestIP="$5"
	DestPort="$6"
	FilterIP="${7:-0.0.0.0/0}"

	[[ "$DestPort" -eq 0 ]] && DestPort="$SrcPort"
	[[ "$FilterIP" != "0.0.0.0/0" ]] && FilterMsg="; Limited to: $FilterIP"
	[[ "$ExtIP" == dhcp ]] && ExtIP="0.0.0.0/0"
	
	echo "  Source port: $SrcPort/$Protocol; Destination: $DestIP:$DestPort$FilterMsg"
	
	parmPort="--dport $SrcPort"
	parmDest="--to-destination "$DestIP:$DestPort""
	
	# if protocol = ip then use port number as protocol number
	if [[ "$Protocol" == "ip" ]]; then
		parmPort=""
		parmDest="--to-destination "$DestIP""
		Protocol="$SrcPort"
	fi

  	case "$DestIP" in
    	127.0.0.1|0.0.0.0)
      		iptables -t mangle -A PREROUTING -p "$Protocol" -s "$FilterIP" -d "$ExtIP" $parmPort -j MARK --set-mark "$AllowMark"
      		iptables -t nat -A PREROUTING -p "$Protocol" -s "$FilterIP" -d "$ExtIP" $parmPort -j REDIRECT --to-ports "$DestPort"
    	;;
    	*)
      		iptables -t nat -A PREROUTING -p "$Protocol" -s "$FilterIP" -i "$ExtIf" $parmPort -j DNAT $parmDest
    	;;
  	esac
}

#####
# Function to clear all existing firewall rules
#####
ClearFirewall()
{
	local table
	
	echo "Clearing IPv4 firewall"
	for table in $(< /proc/net/ip_tables_names); do
		iptables -t "$table" -F
		iptables -t "$table" -X
		for chain in $(iptables -t "$table" -nL|grep ^Chain|cut -d' ' -f2); do
			iptables -t "$table" -P "$chain" ACCEPT
		done
	done

	echo "Clearing IPv6 firewall"
	for table in $(< /proc/net/ip6_tables_names); do
		ip6tables -t "$table" -F
		ip6tables -t "$table" -X
		for chain in $(ip6tables -t "$table" -nL|grep ^Chain|cut -d' ' -f2); do
			ip6tables -t "$table" -P "$chain" ACCEPT
		done
	done

}
#####
# Main working spaghetti
#####
trap 'Unlock "Firewall" "Firewall"' EXIT
WaitLock "Firewall" "Firewall"

# If we use xDSL PPPoE then use that interface as external interface for firewall
OldExtIf=$ExtIf
if [[ "$PPPoEEnabled" == "on" ]]; then
	echo "xDSL PPPoE enabled, using dsl-provider interface as external interface"
	ExtIf="ppp0"
fi

# sanitize old installations, could be removed somewhen
Q="UPDATE Firewall SET Protocol='tcp-ipv4' WHERE Protocol='tcp'"
R=$(RunSQL "$Q")
Q="UPDATE Firewall SET Protocol='udp-ipv4' WHERE Protocol='udp'"
R=$(RunSQL "$Q")


# Clear all firewall rules
ClearFirewall

# Enabling forwarding on all network interfaces
echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding

# Per default entirely close the firewall
DefaultPolicy=DROP
DefaultIPv6Policy=DROP

# If user choosed to disabled one or both firewalls, then accept everything
if [[ "$DisableFirewall" == 1 ]]; then
	DefaultPolicy=ACCEPT
fi

if [[ "$DisableIPv6Firewall" == 1 ]]; then
	DefaultIPv6Policy=ACCEPT
fi

# Set some default firewall parameters before opening ports
echo "Setting up firewall"
iptables -P INPUT "$DefaultPolicy"
iptables -F INPUT
iptables -t nat -F POSTROUTING
iptables -t nat -F PREROUTING
iptables -A INPUT -i lo -j ACCEPT
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT
ip6tables -P INPUT "$DefaultIPv6Policy"
ip6tables -F INPUT
ip6tables -A INPUT -i lo -j ACCEPT
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 128 -j ACCEPT -m comment --comment "Allow PING requests"
ip6tables -A INPUT -p ipv6-icmp --icmpv6-type 129 -j ACCEPT -m comment --comment "Allow PING replies"
ip6tables -A INPUT -p ipv6-icmp -m icmp6 --icmpv6-type 133 -m hl --hl-eq 255 -j ACCEPT # -m comment --comment "Router solicitation"
ip6tables -A INPUT -p ipv6-icmp -m icmp6 --icmpv6-type 134 -m hl --hl-eq 255 -j ACCEPT # -m comment --comment "Router solicitation"
ip6tables -A INPUT -p ipv6-icmp -m icmp6 --icmpv6-type 135 -m hl --hl-eq 255 -j ACCEPT # -m comment --comment "Neighbor solicitation"
ip6tables -A INPUT -p ipv6-icmp -m icmp6 --icmpv6-type 136 -m hl --hl-eq 255 -j ACCEPT # -m comment --comment "Neighbor advertisement"
ip6tables -A INPUT -p ipv6-icmp -m icmp6 --icmpv6-type 137 -m hl --hl-eq 255 -j ACCEPT # -m comment --comment "Redirect message"
ip6tables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
ip6tables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT

# Log denied packets
iptables -I INPUT -m limit --limit 5/min -j LOG --log-prefix "IPv4 packet denied: " --log-level 7
ip6tables -I INPUT -m limit --limit 5/min -j LOG --log-prefix "IPv6 packet denied: " --log-level 7

# If on multiple NIC's, accept incoming on LAN and NAT or masquerade on external
if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24
	iptables -A INPUT -p udp --dport 67 -j ACCEPT # BOOTP/DHCP
	ip6tables -A INPUT -p udp --dport 546 -j ACCEPT # DHCP
	
	iptables -A INPUT -s "$IntNet/$IntBitmask" -j ACCEPT
	ip6tables -A INPUT -i $IntIf -j ACCEPT

	## Workaround for some ISPs that don't allow routers and drop packets based on TTL.
	iptables -t mangle -A POSTROUTING -o $ExtIf -j TTL --ttl-set 255
	ip6tables -t mangle -A POSTROUTING -o $ExtIf -j HL --hl-set 255
	
	# If on static external interface do source nating. If on DHCP do masquerading
	if [[ "$ExtIP" != "dhcp" ]]; then
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $OldExtIf -j SNAT --to-source $ExtIP
	else
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $OldExtIf -j MASQUERADE
	fi
	
	# If PPPoE is used masquerade to that interface too
	if [[ "$ExtIf" == "ppp0" ]]; then
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $ExtIf -j MASQUERADE
	fi
fi

# Configuring all port forwards
echo "Setting up forwarded ports"
Q="SELECT Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP,SourceIP FROM Firewall WHERE RuleType='port_forward' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Port in $R; do
	tmp=$(Field 1 "$Port")
	echo $tmp | grep -q '-'
	if [ $? -eq 0 ]; then
    Protocol="$(echo $tmp | cut -d"-" -f 1)"
    IPversion="$(echo $tmp | cut -d"-" -f 2)"
	else
    Protocol=$(Field 1 "$Port")
    IPversion="ipv4"
	fi
	SrcPort=$(Field 2 "$Port")
	SrcPortEnd=$(Field 3 "$Port")
	DestPort=$(Field 4 "$Port")
	DestIP=$(Field 5 "$Port")
	SrcIP=$(Field 6 "$Port")

  # apply rule to IPv4 chain only, not supported by IPv6
  if [[ "$IPversion" == ipv4 ]]; then
    if [[ "$SrcPortEnd" -eq 0 ]]; then
      ForwardPort "$Protocol" "$IPversion" "$ExtIP" "$SrcPort" "$DestIP" "$DestPort" "$SrcIP"
    else
      DPort="$DestPort"
      for ((SPort=SrcPort; SPort<=SrcPortEnd; SPort++)); do
        ForwardPort "$Protocol" "$IPversion" "$ExtIP" "$SPort" "$DestIP" "$DPort" "$SrcIP"
        [[ "$DPort" -ne 0 ]] && ((++DPort))
      done
    fi
  else
    echo "Ignoring IPv6 port forward rule for port $SrcPort to $DestIP as not (yet) supported by ip6tables"
  fi
done

# Configuring open ports on core
echo "Opening specified ports to exterior"
Q="SELECT Protocol,SourcePort,SourcePortEnd,SourceIP FROM Firewall WHERE RuleType='core_input' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Port in $R; do
	tmp=$(Field 1 "$Port")
	echo $tmp | grep -q '-'
	if [ $? -eq 0 ]; then
    	Protocol="$(echo $tmp | cut -d"-" -f 1)"
    	IPversion="$(echo $tmp | cut -d"-" -f 2)"
	else
    	Protocol=$(Field 1 "$Port")
    	IPversion="ipv4"
	fi
	Port1=$(Field 2 "$Port")
	Port2=$(Field 3 "$Port")
	SrcIP=$(Field 4 "$Port")

	[[ "$Port2" -eq 0 ]] && Port2="$Port1"
	OpenPort "$Protocol" "$IPversion" "$Port1:$Port2" "$SrcIP"
done

# Set discovered MSS to make PMTU work on PPPoE interfaces
if [[ "$PPPoEEnabled" == "on" ]]; then
	iptables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
	ip6tables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
fi

ExtIf=$OldExtIf
