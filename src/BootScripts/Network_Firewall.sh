#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

AllowMark=0x01

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
	
	if [[ "$FilterIP" == "" ]]; then
    if [[ "$IPversion" == ipv4 ]]; then 
      FilterIP="0.0.0.0/0" 
    fi
    if [[ "$IPversion" == ipv6 ]]; then 
      FilterIP="::/0" 
    fi
    FilterMsg="; Not limited to specific IP"
  else
    FilterMsg="; Limited to: $FilterIP"
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

  # apply rule to IPv4 chain if needed
  if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
    iptables -A INPUT -p "$Protocol" -s "$FilterIP" $parmPort -j ACCEPT
  fi

  # apply rule to IPv6 chain if needed
  if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
    ip6tables -A INPUT -p "$Protocol" -s "$FilterIP" $parmPort -j ACCEPT
  fi

	# samba 139/tcp ports come paired with explicit rejects on 445/tcp
	# reason: to avoid timeout connecting to 445/tcp in smbclient
	if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
		iptables -A INPUT -p tcp -s "$FilterIP" --dport 445 -j REJECT
		ip6tables -A INPUT -p tcp -s "$FilterIP" --dport 445 -j REJECT
	fi
}

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

  case "$DestIP" in
    127.0.0.1|0.0.0.0)
      iptables -t mangle -A PREROUTING -p "$Protocol" -s "$FilterIP" -d "$ExtIP" --dport "$SrcPort" -j MARK --set-mark "$AllowMark"
      iptables -t nat -A PREROUTING -p "$Protocol" -s "$FilterIP" -d "$ExtIP" --dport "$SrcPort" -j REDIRECT --to-ports "$DestPort"
    ;;
    *)
      iptables -t nat -A PREROUTING -p "$Protocol" -s "$FilterIP" -i "$ExtIf" --dport "$SrcPort" -j DNAT --to-destination "$DestIP:$DestPort"
    ;;
  esac
}

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

trap 'Unlock "Firewall" "Firewall"' EXIT
WaitLock "Firewall" "Firewall"

ClearFirewall

echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding

DefaultPolicy=DROP
if [[ "$DisableFirewall" == 1 ]]; then
	DefaultPolicy=ACCEPT
fi

echo "Setting up firewall"
iptables -P INPUT "$DefaultPolicy"
iptables -F INPUT
iptables -t nat -F POSTROUTING
iptables -t nat -F PREROUTING
iptables -A INPUT -i lo -j ACCEPT
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT

ip6tables -P INPUT "$DefaultPolicy"
ip6tables -F INPUT
ip6tables -A INPUT -i lo -j ACCEPT
ip6tables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
ip6tables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT

if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24
	iptables -A INPUT -p udp --dport 67 -j ACCEPT # BOOTP/DHCP
	ip6tables -A INPUT -p udp --dport 67 -j ACCEPT # BOOTP/DHCP
	iptables -A INPUT -s "$IntNet/$IntBitmask" -j ACCEPT

	iptables -t mangle -A PREROUTING -j TTL --ttl-set 255
	ip6tables -t mangle -A PREROUTING -j HL --hl-set 255
	
	if [[ "$ExtIP" != "dhcp" ]]; then
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" -d \! "$IntNet/$IntBitmask" -o $ExtIf -j SNAT --to-source $ExtIP
	else
		iptables -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" -d \! "$IntNet/$IntBitmask" -o $ExtIf -j MASQUERADE
	fi

fi

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

iptables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
ip6tables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
