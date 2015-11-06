#!/bin/bash

# Define paths, as just "iptables" won't do it when run from apache
IPTABLES=/sbin/iptables
IP6TABLES=/sbin/ip6tables

#Set defaults settings to conf file (pluto.conf
#Set default blocklist on
blocklist=$(cat /etc/pluto.conf | grep fw_blocklist)
if [[ "$blocklist" == "" ]]; then
	echo fw_blocklist=1 >> /etc/pluto.conf
fi

confDisableIPv4Firewall=$(cat /etc/pluto.conf | grep DisableIPv4Firewall)
if [[ "$confDisableIPv4Firewall" == "" ]]; then
	echo DisableIPv4Firewall=0 >> /etc/pluto.conf
fi

confDisableIPv6Firewall=$(cat /etc/pluto.conf | grep DisableIPv6Firewall)
if [[ "$confDisableIPv6Firewall" == "" ]]; then
	echo DisableIPv6Firewall=0 >> /etc/pluto.conf
fi

confAdvancedFirewall=$(cat /etc/pluto.conf | grep AdvancedFirewall)
if [[ "$confAdvancedFirewall" == "" ]]; then
	echo AdvancedFirewall=0 >> /etc/pluto.conf
fi

Show_all_rules=$(cat /etc/pluto.conf | grep Show_all_rules)
if [[ "$Show_all_rules" == "" ]]; then
	echo Show_all_rules=0 >> /etc/pluto.conf
fi

fwVersion=$(cat /etc/pluto.conf | grep fwVersion)
if [[ "$fwVersion" == "" ]]; then
	echo fwVersion=ipv4 >> /etc/pluto.conf
fi

#get configs and set other default values
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

AllowMark=0x01
Rulenumber=0

if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24

	Intv6Net="$(echo "$Intv6IP" | cut -d":" -f-4):"
fi

#set rules to the db what are not set and need to work for the network,
#set the rules only as advanced firewall is not set as advanced firewall is set user can use the default or set his own rules.
if [[ "$AdvancedFirewall" == "0" ]]; then
   #IPV4
	# Set default policy's
	Q="SELECT Matchname, Protocol FROM Firewall WHERE RuleType='policyipv4' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'input', 'policyipv4')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'forward', 'policyipv4')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('ACCEPT', 'output', 'policyipv4')"
		$(RunSQL "$Q")
	fi

	# Set loopback interface
	Q="SELECT Description FROM Firewall WHERE Description='Allow Loopback' AND Protocol LIKE '%-ipv4' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        Q="INSERT INTO Firewall (Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv4', 'input', 'ACCEPT')"
		$(RunSQL "$Q")
        Q="INSERT INTO Firewall (Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv4', 'output', 'ACCEPT')"
		$(RunSQL "$Q")
        Q="INSERT INTO Firewall (Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'lo', 'ip-ipv4', 'forward', 'ACCEPT')"
		$(RunSQL "$Q")
	fi

	# Set stateful inspection
	Q="SELECT Matchname, Protocol FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv4' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")		
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'output', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")
	fi
	
	#DHCP/BOOTP
	Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv4' AND SourcePort='67' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv4', '67', 'input', 'ACCEPT', 'Allow DHCP')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv4', '68', 'output', 'ACCEPT', 'Allow DHCP')"
		$(RunSQL "$Q")
		if [[ "$ExtIP" == "DHCP" ]] || [[ "$ExtIP" == "dhcp" ]]; then
			Q="INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv4', '68', 'input', 'ACCEPT', 'Allow DHCP')"
			$(RunSQL "$Q")
			Q="INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv4', '67', 'output', 'ACCEPT', 'Allow DHCP')"
			$(RunSQL "$Q")
		fi
	fi
	
	#Local Network
	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Local Network')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Local Network')"
		$(RunSQL "$Q")
	fi
	
	# Allow certain ICMP traffic
	
	#IPV6
	# Set default policy's
	Q="SELECT Matchname, Protocol FROM Firewall WHERE RuleType='policyipv6' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'input', 'policyipv6')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'forward', 'policyipv6')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('ACCEPT', 'output', 'policyipv6')"
		$(RunSQL "$Q")
	fi
	
	# Set loopback interface
	Q="SELECT Description FROM Firewall WHERE Description='Allow Loopback' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        Q="INSERT INTO Firewall (Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv6', 'input', 'ACCEPT')"
		$(RunSQL "$Q")
        Q="INSERT INTO Firewall (Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv6', 'output', 'ACCEPT')"
		$(RunSQL "$Q")
        Q="INSERT INTO Firewall (Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'lo', 'ip-ipv6', 'forward', 'ACCEPT')"
		$(RunSQL "$Q")
	fi

	# Set stateful inspection
	Q="SELECT Matchname FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'output', 'ACCEPT', 'Allow Established')"
		$(RunSQL "$Q")
	fi

	#DHCP/BOOTP
		Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv6' AND SourcePort='546' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		Q="INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv6', '546', 'input', 'ACCEPT', 'Allow DHCP')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv6', '547', 'output', 'ACCEPT', 'Allow DHCP')"
		$(RunSQL "$Q")
		if [[ "$Extv6IP" == "DHCP" ]] || [[ "$Extv6IP" == "dhcp" ]]; then
			Q="INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv6', '547', 'input', 'ACCEPT', 'Allow DHCP')"
			$(RunSQL "$Q")
			Q="INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv6', '546', 'output', 'ACCEPT', 'Allow DHCP')"
			$(RunSQL "$Q")
		fi
	fi
	
	#Local Network
	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$Intv6Net/$Intv6Netmask' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		[[ ! "$Intv6Net" == "disabled::/" ]] && Q="INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Local Network')" && $(RunSQL "$Q")
		[[ ! "$Intv6Net" == "disabled::/" ]] && Q="INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Local Network')" && $(RunSQL "$Q")
	fi
	
	# Allow certain ICMP traffic
	Q="SELECT Description FROM Firewall WHERE Description='Allow PING requests' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		echo "ipv6 icmp"
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6  --icmpv6-type 128', 'icmp-ipv6', 'input', 'ACCEPT', 'Allow PING requests')"
		$(RunSQL "$Q")
		echo $Q
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 129', 'icmp-ipv6', 'input', 'ACCEPT', 'Allow PING replys')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 133 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 134 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 135 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 136 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor advertisement
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 137 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Redirect message
		$(RunSQL "$Q")

		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 128', 'icmp-ipv6', 'output', 'ACCEPT', 'Allow PING requests')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 129', 'icmp-ipv6', 'output', 'ACCEPT', 'Allow PING replys')"
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 133 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 134 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 135 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor solicitation
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 136 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor advertisement
		$(RunSQL "$Q")
		Q="INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m icmp6 --icmpv6-type 137 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Redirect message
		$(RunSQL "$Q")
	fi
fi

#include a list with Firewall aplications to scripts what set the settings for the applications listed.
. /usr/pluto/bin/Firewall_applications.sh

#####
# Function to set the rules
#####
IPTables ()
{

		IPversion="$1"
		RuleType="$2"
		IntIf="${3}"
		ExtIf="${4}"
		Matchname="$5"
		Protocol="$6"
		FilterIP="${7}"
		SrcPort="$8"
		DestIP="$9"
		DestPort="${10}"
		RPolicy="${11}"
		Description="${12}"

		if [[ "$IntIf" != "" ]] &&  [[ "$IntIf" != "NULL" ]]; then
			if [[ "$IntIf" == "ipv6tunn" ]]; then
				parmIntIf="-i ipv6tunnel"
			else
				parmIntIf="-i $IntIf"
			fi
		else
			parmIntIf=""
		fi

		if [[ "$ExtIf" != "" ]] && [[ "$ExtIf" != "NULL" ]]; then
			if [[ "$ExtIf" == "ipv6tunn" ]]; then
				parmExtIf="-o ipv6tunnel"
			else
				parmExtIf="-o $ExtIf"
			fi
		else
			parmExtIf=""
		fi
	
		if [[ "$Matchname" == "" ]] || [[ "$Matchname" == "NULL" ]]; then
			Matchname=""
		fi
	
		#ToDo check Protocol
		if [[ "$Protocol" == "ip" ]]; then
			parmSPort=""
			tmp="$(echo $SrcPort | cut -d: -f1)"
			if [[ -n "$tmp" && ("$tmp" != "NULL" || "$tmp" -ne 0) ]]; then
				Protocol="-p $tmp"
			else
				Protocol=""
			fi	
		elif [[ "$Protocol" == "all" ]]; then
			Protocol="all"
		else
			Protocol="-p $Protocol"
		fi
	
		if [[ "$FilterIP" == "" ]] || [[ "$FilterIP" == "NULL" ]]; then
			if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then 
				FilterIP4="-s 0.0.0.0/0" 
			fi
			if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then 
				FilterIP6="-s ::/0" 
			fi
			FilterMsg="; Not limited to specific IP"
		else
			 grep -qv "[!]" <<< $FilterIP
                        if [ ! $? -eq 0 ]; then
			FilterMsg="; Limited to: !$FilterIP"
			FilterIP="$(echo $FilterIP | cut -d"!" -f 2)"
			FilterIP4="!-s $FilterIP"
			FilterIP6="!-s $FilterIP"
			else
				FilterMsg="; Limited to: $FilterIP"
        	                FilterIP4="-s $FilterIP"
	                        FilterIP6="-s $FilterIP"
			fi
		fi

		if [[ "$SrcPort" == ":" ]] || [[ "$SrcPort" == "NULL" ]] || [[ "$SrcPort" == "NULL:NULL" ]] || [[ "$parmSPort" == "" ]]; then
			parmSPort=""
		else
			parmSPort="--sport $SrcPort"
		fi
	
		if [[ "$DestIP" == "" ]] || [[ "$DestIP" == "NULL" ]]; then
			if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then 
				DestIP4="-d 0.0.0.0/0" 
			fi
			if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then 
				DestIP6="-d ::/0" 
			fi
			DestMsg="; Not limited to specific IP"
		else
			grep -qv "[!]" <<< $DestIP
			if [ ! $? -eq 0 ]; then
				DestMsg="; Limited to: !$DestIP"
				DestIP="$(echo $DestIP | cut -d"!" -f 2)"
				DestIP4="! -d $DestIP"
				DestIP6="! -d $DestIP"
			else
				DestMsg="; Limited to: $DestIP"
				DestIP4="-d $DestIP"
				DestIP6="-d $DestIP"
			fi
		fi
	
		if [[ "$DestPort" == "" ]] || [[ "$DestPort" == "0" ]] || [[ "$DestPort" == "NULL" ]] || [[ "$DestPort" == "NULL:NULL" ]]; then
			parmDPort=""
		else
			parmDPort="--dport $DestPort"
		fi
	
		if [[ "$Description" != "" ]] && [[ "$Description" != "NULL" ]]; then
			Description=$(echo $Description | sed -e 's/ /_/g')
			parmDescription="-m comment --comment $Description"
		else
			parmDescription=""
		fi
	
		# apply rule to IPv4 chain if needed
	if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
		if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
			if [[ ! "$Protocol" == "all" ]]; then
				echo iptables $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
				$IPTABLES $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
				# samba 139/tcp ports come paired with explicit rejects on 445/tcp
				# reason: to avoid timeout connecting to 445/tcp in smbclient
				if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
					$IPTABLES -A INPUT -p tcp -s "$FilterIP4" --dport 445 -j REJECT
				fi
			else
				echo iptables $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
				$IPTABLES $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
				echo iptables $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
				$IPTABLES $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP4 $parmSPort $DestIP4 $parmDPort -j $RPolicy $parmDescription
			fi
			
		fi
	fi

		# apply rule to IPv6 chain if needed
	if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
		if [[ ! "$DisableIPv6Firewall" == "1" ]]; then
			if [[ ! "$Protocol" == "all" ]]; then
				echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
					$IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
				# samba 139/tcp ports come paired with explicit rejects on 445/tcp
				# reason: to avoid timeout connecting to 445/tcp in smbclient
				if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
					$IP6TABLES -A INPUT -p tcp -s "$FilterIP6" --dport 445 -j REJECT
				fi
			else
				echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
				$IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
				echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
				$IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP6 $parmSPort $DestIP6 $parmDPort -j $RPolicy $parmDescription
			fi
		fi
	fi
		# Log denied packets, i.e. packets that make it through the ruleset without being accepted (myaybe a setting to get logging on for manual chains)
		#"$iptablesversion" -A "$chain" -m limit --limit 5/min -j LOG --log-prefix "IPv4 packet denied: " --log-level 7
}

#Pre define manual chain rules.
Manual_CHain_Rules()
{
	echo "Add rules to manual defined chain"
	IPVersion="$1"
	chain="$2"

	Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='$chain' AND Protocol LIKE '%-$IPVersion' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
	R=$(RunSQL "$Q")

	for Port in $R; do
		IntIf=$(Field 1 "$Port")
		ExtIf=$(Field 2 "$Port")
		Matchname=$(Field 3 "$Port")
		tmp=$(Field 4 "$Port")
		echo $tmp | grep -q '-'
		if [ $? -eq 0 ]; then
    			Protocol="$(echo $tmp | cut -d"-" -f 1)"
		fi
		SrcIP=$(Field 5 "$Port")
		SrcPort1=$(Field 6 "$Port")
		SrcPort2=$(Field 7 "$Port")
		SrcPort2="$SrcPort1"
		DestIP=$(Field 8 "$Port")
		DestPort=$(Field 9 "$Port")
		RPolicy=$(Field 10 "$Port")
		Description=$(Field 11 "$Port")
			
		#echo IPTables "$IPVersion" "-A $chain" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
			  IPTables "$IPVersion" "-A $chain" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
	done
}

#####
# Function to clear all existing firewall rules
#####
ClearFirewall()
{
	local table

	echo "Clearing IPv4 firewall"
	for table in $(< /proc/net/ip_tables_names); do
		$IPTABLES -t "$table" -F
		$IPTABLES -t "$table" -X
		for chain in $($IPTABLES -t "$table" -nL|grep ^Chain|cut -d' ' -f2); do
			$IPTABLES -t "$table" -P "$chain" ACCEPT
		done
	done

	echo "Clearing IPv6 firewall"
	for table in $(< /proc/net/ip6_tables_names); do
		$IP6TABLES -t "$table" -F
		$IP6TABLES -t "$table" -X
		for chain in $($IP6TABLES -t "$table" -nL|grep ^Chain|cut -d' ' -f2); do
			$IP6TABLES -t "$table" -P "$chain" ACCEPT
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

# Clear all firewall rules
ClearFirewall

# Enabling forwarding on all network interfaces
echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding

# Set some default firewall parameters before opening ports
echo "Setting up ipv4 firewall"
$IPTABLES -F INPUT
$IPTABLES -F FORWARD
$IPTABLES -F OUTPUT
$IPTABLES -t nat -F POSTROUTING
$IPTABLES -t nat -F PREROUTING
#if ipv4 firewall are not disabled drop illegal ipv4 packets
if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
$IPTABLES -A INPUT -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP
$IPTABLES -A INPUT -p tcp --tcp-flags ALL ALL -j DROP
$IPTABLES -A INPUT -p tcp --tcp-flags ALL NONE -j DROP # NULL packets
$IPTABLES -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
$IPTABLES -A INPUT -p tcp --tcp-flags SYN,FIN SYN,FIN -j DROP #XMAS
$IPTABLES -A INPUT -p tcp --tcp-flags FIN,ACK FIN -j DROP # FIN packet scans
$IPTABLES -A INPUT -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP
$IPTABLES -A INPUT -m mark --mark "$AllowMark" -j ACCEPT
fi

echo "Setting up ipv6 firewall"
$IP6TABLES -F INPUT
$IP6TABLES -F FORWARD
$IP6TABLES -F OUTPUT

#if ipv6 firewall are not disabled drop illegal ipv6 packets
if [[ ! "$DisableIPv6Firewall" == "1" ]]; then
$IP6TABLES -A INPUT -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP
$IP6TABLES -A INPUT -p tcp --tcp-flags ALL ALL -j DROP
$IP6TABLES -A INPUT -p tcp --tcp-flags ALL NONE -j DROP # NULL packets
$IP6TABLES -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
$IP6TABLES -A INPUT -p tcp --tcp-flags SYN,FIN SYN,FIN -j DROP #XMAS
$IP6TABLES -A INPUT -p tcp --tcp-flags FIN,ACK FIN -j DROP # FIN packet scans
$IP6TABLES -A INPUT -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP
$IP6TABLES -A INPUT -m mark --mark "$AllowMark" -j ACCEPT
fi



#Set default policy
echo "Setting default Policy"
	Q="SELECT Matchname, Protocol, RuleType FROM Firewall WHERE RuleType LIKE 'policy%' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")

	for Protocol in $R; do
		Policy=$(Field 1 "$Protocol")
		Chain=$(Field 2 "$Protocol")
		RuleType=$(Field 3 "$Protocol")
		IPVersion=$(echo $RuleType | cut -dy -f2)
		Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
		Chain=$(echo $Chain | tr '[:lower:]' '[:upper:]')
		if [[ "$IPVersion" == "ipv4" ]]; then
			if [[ "$DisableIPv4Firewall" == 1 ]]; then
				$IPTABLES -P $Chain ACCEPT
			else
				if [ $Policy = "REJECT" ]; then
					$IPTABLES -P $Chain DROP
					$IPTABLES -A $Chain -j REJECT --reject-with icmp-port-unreachable
				else
					$IPTABLES -P $Chain $Policy
				fi
			fi
		else
			if [[ "$DisableIPv6Firewall" == 1 ]]; then
				$IP6TABLES -P $Chain ACCEPT
			else
				if [ $Policy = "REJECT" ]; then
					$IP6TABLES -P $Chain DROP
					$IP6TABLES -A $Chain -j REJECT --reject-with icmp-port-unreachable
				else
					$IP6TABLES -P $Chain $Policy
				fi
			fi
		fi
	done

	# If on multiple NIC's, accept incoming on LAN and NAT or masquerade on external
	## Workaround for some ISPs that don't allow routers and drop packets based on TTL.
	$IPTABLES -t mangle -A POSTROUTING -o $ExtIf -j TTL --ttl-set 255
	$IP6TABLES -t mangle -A POSTROUTING -o $ExtIf -j HL --hl-set 255

	# If on static external interface do source nating. If on DHCP do masquerading
	if [[ "$ExtIP" != "dhcp" ]]; then
		$IPTABLES -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $OldExtIf -j SNAT --to-source $ExtIP
	else
		$IPTABLES -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $OldExtIf -j MASQUERADE
	fi

	# If PPPoE is used masquerade to that interface too
	if [[ "$ExtIf" == "ppp0" ]]; then
		$IPTABLES -t nat -A POSTROUTING -s "$IntNet/$IntBitmask" \! -d "$IntNet/$IntBitmask" -o $ExtIf -j MASQUERADE
	fi

#Create all the chains manual defined created.
#ipv4
if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
        echo "Set manual defined chains for ipv4"
        Q="SELECT Matchname FROM Firewall WHERE Protocol='chain-ipv4' ORDER BY  Place, PK_Firewall"
        R=$(RunSQL "$Q")
        #add the chains to iptables before rules connecting to it
        for Chain in $R; do
                chain=$(Field 1 "$Chain")
                echo "iptables -N " $chain
                $IPTABLES -N $chain
        done
        #apply the rules to the chains
        for Chain in $R; do
                chain=$(Field 1 "$Chain")
                Manual_CHain_Rules "ipv4" "$chain"
        done
fi

#ipv6
if [[ ! "$DisableIPv6Firewall" == "1" ]]; then
        echo "Set manual defined chains for ipv6"

        Q="SELECT Matchname FROM Firewall WHERE Protocol='chain-ipv6' ORDER BY Place, PK_Firewall"
        R=$(RunSQL "$Q")
        #add the chains to iptables before rules connecting to it
        for Chain in $R; do
                chain=$(Field 1 "$Chain")
                echo "ip6tables -N " $chain
                $IP6TABLES -N $chain
        done
        #apply the rules to the chains
        for Chain in $R; do
                chain=$(Field 1 "$Chain")
                Manual_CHain_Rules "ipv6" "$chain"
        done
fi

# Configuring open ports on core
echo "Opening specified ports from exterior"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='input' AND RuleType LIKE 'policy%' ORDER BY Place, PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

			Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='input' AND Protocol LIKE '%-$IPVersion' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
			   
			R=$(RunSQL "$Q")

			for Port in $R; do
				IntIf=$(Field 1 "$Port")
				ExtIf=""
				Matchname=$(Field 3 "$Port")
				tmp=$(Field 4 "$Port")
				echo $tmp | grep -q '-'
				if [ $? -eq 0 ]; then
					if [[ "$tmp" == "icmp-ipv6" ]]; then
						Protocol="icmpv6"
					else
						Protocol="$(echo $tmp | cut -d"-" -f 1)"
					fi
				else
					Protocol=$(Field 4 "$Port")
				fi
				SrcIP=$(Field 5 "$Port")
				SrcPort1=$(Field 6 "$Port")
				SrcPort2=$(Field 7 "$Port")
				if [[ "$SrcPort2" -eq 0 ]]; then
					SrcPort2="$SrcPort1"
				fi
				DestIP=$(Field 8 "$Port")
				DestPort=$(Field 9 "$Port")
				RPolicy=$(Field 10 "$Port")
				Description=$(Field 11 "$Port")

				if [[ "$RPolicy" =~ "$Policy" ]]; then
					echo "Rule Policy matches default Policy, not adding"
				else
					echo "Rule Policy not matches default Policy, adding"
					#echo IPTables "$IPVersion" "-A INPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
					IPTables "$IPVersion" "-A INPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
				fi
			done
done

# Configuring all core_forwarding
echo "Setting up core_forward"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='forward' AND RuleType LIKE 'policy%' ORDER BY Place, PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

		Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='forward' AND Protocol LIKE '%-$IPVersion' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
		R=$(RunSQL "$Q")

			for Port in $R; do
				IntIf=$(Field 1 "$Port")
				ExtIf=$(Field 2 "$Port")
				Matchname=$(Field 3 "$Port")
				tmp=$(Field 4 "$Port")
				echo $tmp | grep -q '-'
				if [ $? -eq 0 ]; then
					if [[ "$tmp" == "icmp-ipv6" ]]; then
						Protocol="icmpv6"
					else
						Protocol="$(echo $tmp | cut -d"-" -f 1)"
					fi
				else
					Protocol=$(Field 4 "$Port")
				fi
				SrcIP=$(Field 5 "$Port")
				SrcPort1=$(Field 6 "$Port")
				SrcPort2=$(Field 7 "$Port")
				if [[ "$SrcPort2" -eq 0 ]]; then
					SrcPort2="$SrcPort1"
				fi
				DestIP=$(Field 8 "$Port")
				DestPort=$(Field 9 "$Port")
				RPolicy=$(Field 10 "$Port")
				Description=$(Field 11 "$Port")

				if [[ "$RPolicy" =~ "$Policy" ]]; then
					echo "Rule Policy matches default Policy, not adding"
				else
					echo "Rule Policy not matches default Policy, adding"
					#echo IPTables "$IPVersion" "-A FORWARD" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
					IPTables "$IPVersion" "-A FORWARD" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
				fi
			done
done

# Configuring all port forwards (NAT)

echo "Setting up Port_forwarding (NAT)"
ForwardTypeArr=('PREROUTING' 'POSTROUTING' 'OUTPUT' 'MASQUERADE')
for ForwardType in "${ForwardTypeArr[@]}"; do
	echo $ForwardType
	Q="SELECT RuleType,IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType LIKE '%-$ForwardType' AND Protocol LIKE '%ipv4' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
	R=$(RunSQL "$Q")
			for Port in $R; do
				IPVersion="ipv4"
				RuleType=$ForwardType
				IntIf=$(Field 2 "$Port")
				ExtIf=$(Field 3 "$Port")
				Matchname=$(Field 4 "$Port")
				tmp=$(Field 5 "$Port")
				echo $tmp | grep -q '-'
				if [ $? -eq 0 ]; then
					if [[ "$tmp" == "icmp-ipv6" ]]; then
						Protocol="icmpv6"
					else
						Protocol="$(echo $tmp | cut -d"-" -f 1)"
					fi
				else
					Protocol=$(Field 5 "$Port")
				fi
				SrcIP=$(Field 6 "$Port")
				SrcPort1=$(Field 7 "$Port")
				SrcPort2=$(Field 8 "$Port")
				DestIP=$(Field 9 "$Port")
				DestPort=$(Field 10 "$Port")
				RPolicy=$(Field 11 "$Port")
				Description=$(Field 12 "$Port")
				
				if [[ "$DestPort" == "" ]] || [[ "$DestPort" == "NULL" ]] || [[ "$DestPort" == "NULL:NULL" ]]; then
					parmDPort=""
					DestPort=""
				else
					grep -qv "[:]" <<< $DestPort
					if [ ! $? -eq 0 ]; then
						DestPort1=`echo $DestPort | cut -d \: -f 1`
						DestPort2=`echo $DestPort | cut -d \: -f 2`
						DestPort="$DestPort1"
					else
						parmDPort=""
						DestPort="$DestPort"
						DestPort2=""
					fi
				fi				
				if [[ "$DestIP" != "" ]] && [[ "$DestIP" != "NULL" ]]; then
					grep -qv "[!]" <<< $DestIP
					if [ ! $? -eq 0 ]; then
						DestIP=${DestIP:1}
						if [[ "$RuleType" == "OUTPUT" ]]; then
							parmDest="--to "$DestPort2""			
						else
							parmDest="--to "$DestIP:$DestPort2""
						fi
						DestIP="! $DestIP"
					else
						parmDest="--to-destination "$DestIP:$DestPort2""
						if [[ ! $RuleType == "PREROUTING" ]];then
							DestIP="$DestIP"
						else
							DestIP=""
						fi
					fi
				fi
				
				if [[ "$SrcPort2" -eq 0 ]]; then
					SrcPort2="$SrcPort1"
				fi
				
				#change RPolicy to what the rule needs
				if [[ "$RuleType" == "POSTROUTING" ]] && [[ "$RPolicy" != "MASQUERADE"  ]]; then
					RPolicy="SNAT $parmDest"
				elif [[ "$RuleType" == "PREROUTING" ]]; then
					RPolicy="DNAT $parmDest"
				elif [[ "$RuleType" == "OUTPUT" ]]; then
					RPolicy="REDIRECT $parmDest"
				fi
				

				#echo IPTables "$IPVersion" "-t nat -A $RuleType" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
					 IPTables "$IPVersion" "-t nat -A $RuleType" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
			done
done

# Configuring all Output ports
echo "Opening specified ports from exterior"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='output' AND RuleType LIKE 'policy%' ORDER BY Place, PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

		Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='output' AND Protocol LIKE '%-$IPVersion' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
		R=$(RunSQL "$Q")

		for Port in $R; do
				IntIf=""
				ExtIf=$(Field 2 "$Port")
				Matchname=$(Field 3 "$Port")
				tmp=$(Field 4 "$Port")
				echo $tmp | grep -q '-'
				if [ $? -eq 0 ]; then
					if [[ "$tmp" == "icmp-ipv6" ]]; then
						Protocol="icmpv6"
					else
						Protocol="$(echo $tmp | cut -d"-" -f 1)"
					fi
				else
					Protocol=$(Field 4 "$Port")
				fi
				SrcIP=$(Field 5 "$Port")
				SrcPort1=$(Field 6 "$Port")
				SrcPort2=$(Field 7 "$Port")
				if [[ "$SrcPort2" -eq 0 ]]; then
					SrcPort2="$SrcPort1"
				fi
				DestIP=$(Field 8 "$Port")
				DestPort=$(Field 9 "$Port")
				RPolicy=$(Field 10 "$Port")
				Description=$(Field 11 "$Port")
				
				if [[ "$RPolicy" =~ "$Policy" ]]; then
					echo "Rule Policy matches default Policy, not adding"
				else
					echo "Rule Policy not matches default Policy, adding"
					#echo IPTables "$IPVersion" "-A OUTPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
					IPTables "$IPVersion" "-A OUTPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
				fi
			done
done

# Set discovered MSS to make PMTU work on PPPoE interfaces
if [[ "$PPPoEEnabled" == "on" ]]; then
	$IPTABLES --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
	$IP6TABLES --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
fi

	# Add TOP 20 known attackers to blocklist
if [[ ! "$DisableIPv4Firewall" == "1" ]]; then
	if [[ "$fw_blocklist" == "1" ]]; then
		/usr/pluto/bin/Network_Blocklist.sh
		ln -s /usr/pluto/bin/Network_Blocklist.sh /etc/cron.hourly/blocklist
	else
		rm -f /etc/cron.hourly/blocklist
	fi
else
	rm -f /etc/cron.hourly/blocklist
fi

