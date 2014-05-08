#!/bin/bash

#Set default blocklist on
blocklist=$(cat /etc/pluto.conf | grep fw_blocklist)
if [[ "$blocklist" == "" ]]; then
	echo fw_blocklist=1 >> /etc/pluto.conf
fi

DisableIPv4Firewall=$(cat /etc/pluto.conf | grep DisableIPv4Firewall)
if [[ "$DisableIPv4Firewall" == "" ]]; then
	echo DisableIPv4Firewall=0 >> /etc/pluto.conf
fi

DisableIPv6Firewall=$(cat /etc/pluto.conf | grep DisableIPv6Firewall)
if [[ "$DisableIPv6Firewall" == "" ]]; then
	echo DisableIPv6Firewall=0 >> /etc/pluto.conf
fi

AdvancedFirewall=$(cat /etc/pluto.conf | grep AdvancedFirewall)
if [[ "$AdvancedFirewall" == "" ]]; then
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

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

AllowMark=0x01
Rulenumber=0
#####
# Function to open a port on the core
#####
OpenPort ()
{
	local Protocol Port FilterIP
	local FilterMsg args
	local parmPort parmIntIf parmExtIf parmDescription
	local PortBegin PortEnd Port

	Chain="$1"
	Matchname="$2"
	Protocol="$3"
	IPversion="$4"
	Port="$5"
	FilterIP="$6"
	IntIf="$7"
	ExtIf="$8"
	RPolicy="$9"
	Description="${10}"

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

	if [[ "$IntIf" != "" ]] &&  [[ "$IntIf" != "NULL" ]]; then
		parmIntIf="-i $IntIf"
	fi

	if [[ "$ExtIf" != "" ]] && [[ "$ExtIf" != "NULL" ]]; then
		parmExtIf="-o $ExtIf"
	fi

	if [[ "$Description" != "" ]] && [[ "$Description" != "NULL" ]]; then
		Description=$(echo $Description | sed -e 's/ /_/g')
		parmDescription="-m comment --comment $Description"
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

	if [[ "$Port" == "" ]] || [[ "$Port" == "NULL" ]] || [[ "$Port" == "NULL:NULL" ]]; then
		parmPort=""
	else
		# if protocol = ip then use port number as protocol number
		if [[ "$Protocol" == "ip" ]]; then
			parmPort=""
			Protocol="$(echo $Port | cut -d: -f1)"
		fi
		if [[ "$Protocol" == "icmpv6" ]]; then
			parmPort=""
		fi
	fi

	# apply rule to IPv4 chain if needed
  	if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
		if [[ "$Matchname" != "" ]] && [[ "$Matchname" != "NULL" ]]; then
	            echo iptables -A $Chain $Matchname -p $Protocol $parmPort -s $FilterIP4 -j $RPolicy $parmIntIf $parmExtIf  $parmDescription
	            iptables -A $Chain $Matchname -p "$Protocol" $parmPort -s "$FilterIP4" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
        	else
	            echo "iptables -A $Chain -p $Protocol "$parmPort" -s $FilterIP4 -j $RPolicy" $parmIntIf $parmExtIf $parmDescription
	            iptables -A "$Chain" -p "$Protocol" $parmPort -s $FilterIP4 -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
	        fi
 	fi

  	# apply rule to IPv6 chain if needed
  	if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
    	if [[ "$Matchname" != "" ]] && [[ "$Matchname" != "NULL" ]]; then
			echo ip6tables -A $Chain -p "$Protocol" $parmPort $Matchname -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
			ip6tables -A $Chain -p "$Protocol" $parmPort $Matchname -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
		else
			echo ip6tables -A "$Chain" -p "$Protocol" $parmPort -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
			ip6tables -A "$Chain" -p "$Protocol" $parmPort -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
		fi
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
	local parmPort parmIntIf parmExtIf parmDescription parmDest

	Forward="$1"
	Matchname="$2"
	Protocol="$3"
	IPversion="$4"
	ExtIP="$5"
	SrcPort="$6"
	DestIP="$7"
	DestPort="$8"
	RPolicy="$9"
	FilterIP="${10}"
	IntIf="${11}"
	ExtIf="${12}"
	Description="${13}"

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
	grep -qv "[:]" <<< $DestPort
	[ $? -eq 0 ] && [[ "$DestPort" -eq 0 ]] && DestPort="$SrcPort"
	[[ "$ExtIP" == dhcp ]] && ExtIP="0.0.0.0/0"

	echo "  Source: $FilterIP4:$SrcPort/$Protocol; Destination: $DestIP:$DestPort$FilterMsg"

	if [[ "$Matchname" == "" ]] || [[ "$Matchname" == "NULL" ]]; then
		Matchname=""
	fi
	
	if [[ "$IntIf" != "" ]] &&  [[ "$IntIf" != "NULL" ]]; then
		parmIntIf="-i $IntIf"
	fi

	if [[ "$ExtIf" != "" ]] && [[ "$ExtIf" != "NULL" ]]; then
		parmExtIf="-o $ExtIf"
	fi

	if [[ "$Description" != "" ]] && [[ "$Description" != "NULL" ]]; then
		Description=$(echo $Description | sed -e 's/ /_/g')
		parmDescription="-m comment --comment $Description"
	fi
	
	if [[ "$SrcPort" == "" ]] || [[ "$SrcPort" == "NULL" ]] || [[ "$SrcPort" == "NULL:NULL" ]]; then
		parmSPort=""
	else
		# if protocol = ip then use port number as protocol number
		if [[ "$Protocol" == "ip" ]]; then
			parmDPort=""
			parmDest="--to-destination "$DestIP""
			Protocol="$SrcPort"
		else
			grep -qv "[!]" <<< $SrcPort
			if [ ! $? -eq 0 ]; then
				SrcPort=${SrcPort:1}
				parmSPort="! --sport $SrcPort"
			else
				parmSPort="--sport $SrcPort"
			fi
		fi
	fi
	
	if [[ "$DestPort" == "" ]] || [[ "$DestPort" == "NULL" ]] || [[ "$DestPort" == "NULL:NULL" ]]; then
		parmDPort=""
		DestPort=""
	else
		grep -qv "[:]" <<< $DestPort
		if [ ! $? -eq 0 ]; then
			DestPort1=`echo $DestPort | cut -d \: -f 1`
			DestPort2=`echo $DestPort | cut -d \: -f 2`
			parmDPort="--dport $DestPort1"
			DestPort="$DestPort2"
		else
			parmDPort=""
			DestPort="$DestPort"
		fi
	fi
	
if [[ "$DestIP" != "" ]] && [[ "$DestIP" != "NULL" ]]; then
	grep -qv "[!]" <<< $DestIP
	if [ ! $? -eq 0 ]; then
		DestIP=${DestIP:1}
		if [[ "$Forward" == "OUTPUT" ]]; then
			parmDest="--to "$DestPort""			
		else
			parmDest="--to "$DestIP:$DestPort""
		fi
		DestIP="! -d $DestIP"
    else
		parmDest="--to-destination "$DestIP:$DestPort""
		if [[ ! $Forward == "PREROUTING" ]];then
			DestIP="-d $DestIP"
		else
			DestIP=""
		fi
	fi
fi

	if [[ "$Forward" == "FORWARD" ]]; then
			# apply rule to IPv4 chain if needed
		if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
			if [[ "$Matchname" != "" ]] && [[ "$Matchname" != "NULL" ]]; then
				echo "iptables -A FORWARD $Matchname -p $Protocol -s $FilterIP4 "$parmSPort $parmDPort" -j $RPolicy" $parmIntIf $parmExtIf $parmDescription
				iptables -A FORWARD $Matchname -p "$Protocol" -s "$FilterIP4" $parmSPort $parmDPort -j $RPolicy $parmIntIf $parmExtIf $parmDescription
			else
				echo "iptables -A FORWARD -p $Protocol -s $FilterIP4 "$parmSPort $parmDPort" -j $RPolicy" $parmIntIf $parmExtIf $parmDescription
				iptables -A FORWARD -p "$Protocol" -s "$FilterIP4" $parmSPort $parmDPort -j $RPolicy $parmIntIf $parmExtIf $parmDescription
			fi
		fi

		# apply rule to IPv6 chain if needed
		if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
			if [[ "$Matchname" != "" ]] && [[ "$Matchname" != "NULL" ]]; then
				echo "ip6tables -A FORWARD $Matchname -p $Protocol -s $FilterIP6 "$parmSPort $parmPDort" -j $RPolicy" $parmIntIf $parmExtIf $parmDescription
				ip6tables -A FORWARD -p $Protocol $Matchname -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
			else
				echo "ip6tables -A FORWARD -p $Protocol -s $FilterIP6 "$parmSPort $parmDPort" -j $RPolicy" $parmIntIf $parmExtIf $parmDescription
				ip6tables -A FORWARD -p $Protocol -s "$FilterIP6" -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
			fi
		fi
	elif [[ "$Forward" == "PREROUTING" ]] || [[ "$Forward" == "POSTROUTING" ]] || [[ "$Forward" == "OUTPUT" ]]; then
		if [[ "$IPversion" == ipv4 ]] || [[ "$IPversion" == both ]]; then
			case "$DestIP" in
				127.0.0.1|0.0.0.0)
					echo "iptables -t mangle -A $Forward -p $Protocol -s $FilterIP4 -d $ExtIP $parmSPort $parmDPort -j MARK --set-mark $AllowMark"
					echo "iptables -t nat -A $Forward -p $Protocol -s $FilterIP4 -d $DestIP $parmSPort $parmDPort -j REDIRECT --to-ports $DestPort"
					iptables -t mangle -A $Forward -p "$Protocol" -s "$FilterIP4" -d "$DestIP" $parmSPort $parmDPort -j MARK --set-mark "$AllowMark"
					iptables -t nat -A $Forward -p "$Protocol" -s "$FilterIP4"  "$DestIP" $parmSPort $parmDPort -j REDIRECT --to-port "$DestPort"
				;;
				*)
					if [[ "$Forward" == "POSTROUTING" ]]; then
						echo "iptables -t nat -A $Forward $Matchname -p $Protocol -s $FilterIP4 $DestIP $parmExtIf $parmSPort $parmDPort -j SNAT $parmDest $parmDescription"
						iptables -t nat -A $Forward $Matchname -p "$Protocol" -s "$FilterIP4" $DestIP $parmExtIf $parmSPort $parmDPort -j SNAT $parmDest $parmDescription
					elif [[ "$Forward" == "PREROUTING" ]]; then
						echo "iptables -t nat -A $Forward $Matchname -p $Protocol -s $FilterIP4 $DestIP $parmIntIf $parmExtIf $parmSPort $parmDPort -j DNAT $parmDest $parmDescription"
						iptables -t nat -A $Forward $Matchname -p "$Protocol" -s "$FilterIP4" $DestIP $parmIntIf $parmExtIf $parmSPort $parmDPort -j DNAT $parmDest $parmDescription
					elif [[ "$Forward" == "OUTPUT" ]]; then
						echo "iptables -t nat -A $Forward $Matchname -p $Protocol -s $FilterIP4 $DestIP  $parmExtIf $parmSPort $parmDPort -j REDIRECT $parmDest $parmDescription"
						iptables -t nat -A $Forward $Matchname -p "$Protocol" -s "$FilterIP4" $DestIP  $parmExtIf $parmSPort $parmDPort -j REDIRECT $parmDest $parmDescription
					fi
				;;
			esac
		fi
	fi
}

Manual_CHain_Rules()
{
	echo "Add rules to manual defined chain"
	IPVersion="$1"
	chain="$2"
		
	if [[ "$IPVersion" == "ipv4" ]] && [[ ! "$DisableIPv4Firewall" == "1" ]] || [[ "$IPVersion" == "ipv6" ]] && [[ ! "$DisableIPv6Firewall" == "1" ]]; then
		Q="SELECT Matchname,Protocol,IntIf,ExtIf,SourcePort,SourcePortEnd,SourceIP,DestinationIP,DestinationPort,Disabled,RPolicy,Description FROM Firewall WHERE RuleType='$chain' AND Protocol LIKE '%-$IPVersion' AND Offline='0' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")
	
		for Port in $R; do
			tmp=$(Field 2 "$Port")
			echo $tmp | grep -q '-'
			if [ $? -eq 0 ]; then
	    		Protocol="$(echo $tmp | cut -d"-" -f 1)"
			else
    			Protocol=$(Field 2 "$Port")
			fi

			Matchname=$(Field 1 "$Port")
			IntIf=$(Field 3 "$Port")
			ExtIf=$(Field 4 "$Port")
			SrcPort=$(Field 5 "$Port")
			DestPort=$(Field 6 "$Port")
			FilterIP=$(Field 7 "$Port")
			DestIP=$(Field 8 "$Port")
			Disabled=$(Field 10 "$Port")
			RPolicy=$(Field 11 "$Port")
			Description=$(Field 12 "$Port")
			
			if [[ "$FilterIP" == "" ]] || [[ "$FilterIP" == "NULL" ]]; then
				if [[ "$IPVersion" == "ipv4" ]] || [[ "$IPVersion" == both ]]; then 
					FilterIP="0.0.0.0/0" 
				fi
				if [[ "$IPVersion" == "ipv6" ]] || [[ "$IPVersion" == both ]]; then 
					FilterIP="::/0" 
				fi
				FilterMsg="; Not limited to specific IP"
			else
				FilterMsg="; Limited to: $FilterIP"
				FilterIP="$FilterIP"
			fi

			[[ "$DestPort" -eq 0 ]] && DestPort="$SrcPort"
			[[ "$ExtIP" == dhcp ]] && ExtIP="0.0.0.0/0"

			if [[ ! "$DestIP" == "" ]]; then
				parmDest="-d $DestIP"
			fi

			echo "  Source: $FilterIP:$SrcPort/$Protocol; Destination: $DestIP:$DestPort$FilterMsg"

			parmPort="--dport $SrcPort"
#			parmDest="-d $DestIP"

			if [[ "$IntIf" != "" ]] &&  [[ "$IntIf" != "NULL" ]]; then
				parmIntIf="-i $IntIf"
			fi

			if [[ "$ExtIf" != "" ]] && [[ "$ExtIf" != "NULL" ]]; then
				parmExtIf="-o $ExtIf"
			fi

			if [[ "$SrcPort" == "" ]] || [[ "$SrcPort" == "NULL" ]] || [[ "$SrcPort" == "NULL:NULL" ]] || [[ "$SrcPort" == "0" ]]; then
				parmPort=""
			fi

			if [[ $IPVersion == "ipv4" ]]; then
				iptablesversion="iptables"
			elif [[ $IPVersion == "ipv6" ]]; then
				iptablesversion="ip6tables";
			fi

			if [[ "$Disabled" == 0 ]] || [[  "$Disabled" == "0" ]] || [[ "$Disabled" == "NULL" ]]; then
				if [[ "$Matchname" != "" ]] && [[ "$Matchname" != "NULL" ]]; then
					echo  "$iptablesversion -A $chain -p $Protocol $parmPort $Matchname -s $FilterIP $parmDest $DestPort -j $RPolicy $parmIntIf $parmExtIf $parmDescription"
					"$iptablesversion" -A "$chain" -p $Protocol $parmPort $Matchname -s "$FilterIP" $parmDest -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
				else
					echo  "$iptablesversion -A $chain -p $Protocol $parmPort -s $FilterIP $parmDest -j $RPolicy $parmIntIf $parmExtIf $parmDescription"
					"$iptablesversion" -A "$chain" -p $Protocol $parmPort -s "$FilterIP" $parmDest -j "$RPolicy" $parmIntIf $parmExtIf $parmDescription
				fi
			fi
		done
		# Log denied packets, i.e. packets that make it through the ruleset without being accepted (myaybe a setting to get logging on for manual chains)
		#"$iptablesversion" -A "$chain" -m limit --limit 5/min -j LOG --log-prefix "IPv4 packet denied: " --log-level 7
	fi
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
Q="UPDATE Firewall SET RPolicy='ACCEPT' WHERE RPolicy=''"
R=$(RunSQL "$Q")

# Clear all firewall rules
ClearFirewall

# Enabling forwarding on all network interfaces
echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding

# Set some default firewall parameters before opening ports
echo "Setting up firewall"
iptables -F INPUT
iptables -F FORWARD
iptables -F OUTPUT
iptables -t nat -F POSTROUTING
iptables -t nat -F PREROUTING

# Set default policy's
Q="SELECT Matchname, Protocol FROM Firewall WHERE RuleType='policyipv4' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'input', 'policyipv4')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'forward', 'policyipv4')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('ACCEPT', 'output', 'policyipv4')"
fi

# Set logging level
#iptables -A INPUT -j LOG --log-prefix "IN "
#iptables -A FORWARD -j LOG --log-prefix "FW "
#iptables -A OUTPUT -j LOG --log-prefix "OU "

# Set loopback interface
Q="SELECT Description FROM Firewall WHERE Description='Allow Loopback' AND Protocol LIKE '%-ipv4' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv4', 'input', 'ACCEPT')"
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv4', 'output', 'ACCEPT')"
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'lo', 'ip-ipv4', 'forward', 'ACCEPT')"
fi

# Set stateful inspection
Q="SELECT Matchname, Protocol FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv4' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Established')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Established')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'output', 'ACCEPT', 'Allow Established')"
fi

iptables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT

echo "Setting up ipv6 firewall"
ip6tables -F INPUT
ip6tables -F FORWARD
ip6tables -F OUTPUT

# Set default policy's
Q="SELECT Matchname, Protocol FROM Firewall WHERE RuleType='policyipv6' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'input', 'policyipv6')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('DROP', 'forward', 'policyipv6')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType) VALUES ('ACCEPT', 'output', 'policyipv6')"
fi

# Set logging level
#ip6tables -A INPUT -j LOG --log-prefix "IN "
#ip6tables -A FORWARD -j LOG --log-prefix "FW "
#ip6tables -A OUTPUT -j LOG --log-prefix "OU "

# Set loopback interface
Q="SELECT Description FROM Firewall WHERE Description='Allow Loopback' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv6', 'input', 'ACCEPT')"
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'ip-ipv6', 'output', 'ACCEPT')"
        mysql pluto_main -ss -e"INSERT INTO Firewall (Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('Allow Loopback', 'lo', 'lo', 'ip-ipv6', 'forward', 'ACCEPT')"
fi

# Set stateful inspection
Q="SELECT Matchname FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Established')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Established')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'output', 'ACCEPT', 'Allow Established')"
fi

# Allow certain ICMP traffic
Q="SELECT Description FROM Firewall WHERE Description='Allow PING requests' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")
if ! [ "$R" ]; then
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 128', 'icmpv6-ipv6', 'input', 'ACCEPT', 'Allow PING requests')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 129', 'icmpv6-ipv6', 'input', 'ACCEPT', 'Allow PING replys')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 133 -m hl --hl-eq 255', 'icmpv6-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 134 -m hl --hl-eq 255', 'icmpv6-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 135 -m hl --hl-eq 255', 'icmpv6-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 136 -m hl --hl-eq 255', 'icmpv6-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor advertisement
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 137 -m hl --hl-eq 255', 'icmpv6-ipv6', 'input', 'ACCEPT', '')" #Allow Redirect message

	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 128', 'icmpv6-ipv6', 'output', 'ACCEPT', 'Allow PING requests')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 129', 'icmpv6-ipv6', 'output', 'ACCEPT', 'Allow PING replys')"
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 133 -m hl --hl-eq 255', 'icmpv6-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 134 -m hl --hl-eq 255', 'icmpv6-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 135 -m hl --hl-eq 255', 'icmpv6-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor solicitation
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 136 -m hl --hl-eq 255', 'icmpv6-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor advertisement
	mysql pluto_main -ss -e"INSERT INTO Firewall (Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('--icmpv6-type 137 -m hl --hl-eq 255', 'icmpv6-ipv6', 'output', 'ACCEPT', '')" #Allow Redirect message
fi

ip6tables -A INPUT -m mark --mark "$AllowMark" -j ACCEPT

# If on multiple NIC's, accept incoming on LAN and NAT or masquerade on external
if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24

	Intv6Net="$(echo "$Intv6IP" | cut -d":" -f-4)::"

	Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv4' AND SourcePort='67' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv4', '67', 'input', 'ACCEPT', 'Allow DHCP')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv4', '68', 'output', 'ACCEPT', 'Allow DHCP')"
		if [[ "$ExtIP" == "DHCP" ]] || [[ "$ExtIP" == "dhcp" ]]; then
			mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv4', '68', 'input', 'ACCEPT', 'Allow DHCP')"
			mysql pluto_main -ss -e"INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv4', '67', 'output', 'ACCEPT', 'Allow DHCP')"
		fi
	fi

	Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv6' AND SourcePort='546' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv6', '546', 'input', 'ACCEPT', 'Allow DHCP')"
		mysql pluto_main -ss -e"INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', 'udp-ipv6', '547', 'output', 'ACCEPT', 'Allow DHCP')"
		if [[ "$Extv6IP" == "DHCP" ]] || [[ "$Extv6IP" == "dhcp" ]]; then
			mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv6', '547', 'input', 'ACCEPT', 'Allow DHCP')"
			mysql pluto_main -ss -e"INSERT INTO Firewall (ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$ExtIf', 'udp-ipv6', '546', 'output', 'ACCEPT', 'Allow DHCP')"
		fi
	fi

	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Local Network')"
	fi

	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$Intv6Net/$Intv6Netmask' AND RuleType='input' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		[[ ! "$Intv6Net" == "disabled::" ]] && mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Local Network')"
		[[ ! "$Intv6Net" == "disabled::" ]] && mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Local Network')"
	fi

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

#Do something for VPN perhaps
if [ $VPNenabled == "on" ]; then
	echo "VPN is enabled. Setting up Firewall to accept VPN traffic."
	
	Q="SELECT Matchname FROM Firewall WHERE Protocol='chain-ipv4' AND Matchname='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
	Q="INSERT INTO Firewall (Protocol, Matchname) VALUES('chain-ipv4', 'VPN');"
	RunSQL "$Q"
	fi
	Q="SELECT Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='ip-ipv4' AND RuleType='input' AND RPolicy='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        	Q="INSERT INTO Firewall (Protocol,RuleType,RPolicy,Description) VALUES('ip-ipv4','input','VPN', 'Allow VPN');"
			RunSQL "$Q"
	fi
	Q="SELECT Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='ip-ipv4' AND RuleType='forward' AND RPolicy='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        	Q="INSERT INTO Firewall (Protocol,RuleType,RPolicy,Description) VALUES('ip-ipv4','forward','VPN', 'Allow VPN');"
			RunSQL "$Q"
	fi
	Q="SELECT Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='udp-ipv4' AND SourcePort='500' AND RuleType='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        	Q="INSERT INTO Firewall (Matchname,Protocol,SourcePort,RuleType,RPolicy,Description) VALUES('-m policy --dir in --pol none','udp-ipv4',500,'VPN','ACCEPT', 'Allow VPN');"
			RunSQL "$Q"
	fi
	Q="SELECT Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='udp-ipv4' AND SourcePort='4500' AND RuleType='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
        	Q="INSERT INTO Firewall (Matchname,Protocol,SourcePort,RuleType,RPolicy, Description) VALUES('-m policy --dir in --pol none','udp-ipv4',4500,'VPN','ACCEPT', 'Allow VPN');"
			RunSQL "$Q"
	fi
	Q="SELECT Matchname, Protocol, SourcePort, Ruletype FROM Firewall WHERE Matchname='-m policy --dir in --pol ipsec' AND Protocol='udp-ipv4' AND SourcePort='1701' AND RuleType='VPN' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
	        Q="INSERT INTO Firewall (Matchname,Protocol,SourcePort,RuleType, RPolicy, Description) VALUES('-m policy --dir in --pol ipsec','udp-ipv4',1701,'VPN', 'ACCEPT', 'Allow VPN');"
			RunSQL "$Q"
	fi
fi

#Do something for webfilter or proxy perhaps
#Set default when both ar not active or set.
Q="SELECT Disabled, IK_DeviceData FROM Device as Device INNER JOIN DeviceTemplate_DeviceData as Template on Device.FK_DeviceTemplate = Template.FK_DeviceTemplate WHERE Device.FK_DeviceTemplate='2289'"
R=$(RunSQL "$Q")

if [ "$R" ]; then
        for ProxyData in $R; do
                DisableProxy=$(Field 1 "$ProxyData")
                ProxyPort=$(Field 2 "$ProxyData")
        done
else
        DisableProxy=1
fi

Q="SELECT Disabled, IK_DeviceData FROM Device as Device INNER JOIN DeviceTemplate_DeviceData as Template on Device.FK_DeviceTemplate = Template.FK_DeviceTemplate WHERE Device.FK_DeviceTemplate='2290'"
R=$(RunSQL "$Q")
if [ "$R" ]; then
         for WebfilterData in $R; do
                DisableWebfilter=$(Field 1 "$WebfilterData")
                WebfilterPort=$(Field 2 "$WebfilterData")
        done
else
        DisableWebfilter=1
fi
if [ $DisableProxy = 1 ] && [ $DisableWebfilter = 1 ] ; then
	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND SourcePort='!80' AND RuleType='forward' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if [ "$R" ]; then
		mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='tcp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
		mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='udp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
	fi
	Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='forward' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if ! [ "$R" ]; then
		echo "forward all"
		mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Local Network')"
	fi
	echo "no proxy and webfilter"
fi
# Webfilter
    if [ $DisableWebfilter = 0 ]; then
		#setup webfilter route to firewall
		Q="SELECT Matchname, Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='tcp-ipv4' AND DestinationPort='80:$WebfilterPort' AND DestinationIP='!192.168.81.1'  AND RuleType='nat-PREROUTING' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")
		if ! [ "$R" ]; then
			Q="INSERT INTO Firewall (Protocol,DestinationPort,DestinationIP,RuleType, RPolicy, Description) VALUES('tcp-ipv4','80:$WebfilterPort','!192.168.81.1','nat-PREROUTING', 'ACCEPT', 'Prerouting WEBFILTER');"
			RunSQL "$Q"
			#Delete existend proxy rules because this need before them.
			Q="SELECT * FROM Firewall WHERE Description LIKE '%PROXY'"
			R=$(RunSQL "$Q")
			if  [ "$R" ]; then
				Q="DELETE FROM Firewall WHERE Description LIKE '%PROXY'"
				RunSQL "$Q"
			fi	
		fi
		Q="SELECT Matchname, Protocol, Ruletype FROM Firewall WHERE Protocol='tcp-ipv4' AND DestinationPort='80:$WebfilterPort' AND DestinationIP='!192.168.81.1' AND RuleType='nat-OUTPUT' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")
		if ! [ "$R" ]; then
			Q="INSERT INTO Firewall (Protocol,Matchname,DestinationPort,DestinationIP,RuleType, RPolicy, Description) VALUES('tcp-ipv4','-m owner ! --uid-owner proxy','80:$WebfilterPort','!192.168.81.1','nat-OUTPUT', 'REDIRECT', 'Output WEBFILTER');"
			RunSQL "$Q"
		fi
		if [[ -z $DisableProxy ]] || [[ $DisableProxy = 1 ]]; then
			Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='forward' ORDER BY PK_Firewall"
			R=$(RunSQL "$Q")
			if [ "$R" ]; then
				mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='ip-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
			fi
			Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourcePort='!80' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='forward' ORDER BY PK_Firewall"
			R=$(RunSQL "$Q")
			if ! [ "$R" ]; then
				mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'tcp-ipv4', '!80', 'forward', 'ACCEPT', 'Allow Local Network')"
				mysql pluto_main -ss -e"INSERT INTO Firewall (IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('$IntIf', '$IntNet/$IntBitmask', 'udp-ipv4', 'forward', 'ACCEPT', 'Allow Local Network')"
			fi
			echo "webfilter only"
		fi
	else
		echo "no webfilter"
		Q="SELECT Description FROM Firewall WHERE Description LIKE '%WEBFILTER'"
		R=$(RunSQL "$Q")
		if [ "$R" ]; then
			echo "delete webfilter"
			Q="DELETE FROM Firewall WHERE Description LIKE '%WEBFILTER'"
			RunSQL "$Q"
		fi
	fi

# Proxy
    if [ $DisableProxy = 0 ]; then
        #setup webfilter route to firewall
		Q="SELECT Matchname, Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='tcp-ipv4' AND DestinationPort='$ProxyPort' AND RuleType='nat-PREROUTING' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")
		if ! [ "$R" ]; then
			Q="INSERT INTO Firewall (Protocol,DestinationPort,DestinationIP,RuleType, RPolicy, Description) VALUES('tcp-ipv4','$ProxyPort','!192.168.81.1','nat-PREROUTING', 'ACCEPT', 'Prerouting PROXY');"
			RunSQL "$Q"
		fi
		Q="SELECT Matchname, Protocol, SourcePort, Ruletype FROM Firewall WHERE Protocol='tcp-ipv4' AND DestinationPort='$ProxyPort' AND Matchname='-m owner ! --uid-owner proxy' AND RuleType='nat-OUTPUT' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")
		if ! [ "$R" ]; then
			Q="INSERT INTO Firewall (Protocol,DestinationPort,DestinationIP,Matchname,RuleType, RPolicy, Description) VALUES('tcp-ipv4','$ProxyPort','!192.168.81.1','-m owner ! --uid-owner proxy','nat-OUTPUT', 'REDIRECT', 'Output PROXY');"
			RunSQL "$Q"
		fi
		Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND SourcePort='!80' AND RuleType='forward' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
	if [ "$R" ]; then
		mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='tcp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
		mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='udp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
	fi
		if [[ -z $DisableWebfilter ]] || [[ $DisableWebfilter = 1 ]]; then
			Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='forward' ORDER BY PK_Firewall"
			R=$(RunSQL "$Q")
			if [ "$R" ]; then
				mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='ip-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
			fi
			Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND SourcePort='!80' AND RuleType='forward' ORDER BY PK_Firewall"
			R=$(RunSQL "$Q")
			if [ "$R" ]; then
				mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='tcp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
				mysql pluto_main -ss -e"DELETE FROM Firewall WHERE SourceIP='$IntNet/$IntBitmask' AND Protocol='udp-ipv4' AND RuleType='forward' AND RPolicy='ACCEPT' AND Description='Allow Local Network'"
			fi
		echo "proxy only"
		fi
	else
		echo "no proxy"
		Q="SELECT Description FROM Firewall WHERE Description LIKE '%PROXY'"
		R=$(RunSQL "$Q")
	if [ "$R" ]; then
		echo "delete proxy"
		Q="DELETE FROM Firewall WHERE Description LIKE '%PROXY'"
		RunSQL "$Q"
	fi
fi

#Create all the chains manual defined created.
#ipv4
echo "Set manual defined chains for ipv4"
Q="SELECT Matchname FROM Firewall WHERE Protocol='chain-ipv4' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Chain in $R; do
	chain=$(Field 1 "$Chain")
	echo "iptables -N " $chain 
	iptables -N $chain
	Manual_CHain_Rules "ipv4" "$chain"
done
	
#ipv6
echo "Set manual defined chains for ipv6"
Q="SELECT Matchname FROM Firewall WHERE Protocol='chain-ipv6' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")	

for Chain in $R; do
	chain=$(Field 1 "$Chain")
	echo "ip6tables -N " $chain
	iptables -N $chain
	Manual_CHain_Rules "ipv6" "$chain"
done

#add rules to manual defined chains

	
# Configuring open ports on core
echo "Opening specified ports from exterior"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='input' AND RuleType LIKE 'policy%' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

	if ([[ "$IPVersion" == "ipv4" ]] &&  [[ ! "$DisableIPv4Firewall" == "1" ]]) || ([[ "$IPVersion" == "ipv6" ]] && [[ ! "$DisableIPv6Firewall" == "1" ]]); then
			Q="SELECT Matchname,Protocol,IntIf,ExtIf,SourcePort,SourcePortEnd,SourceIP,DestinationPort,Disabled,RPolicy,Description FROM Firewall WHERE RuleType='input' AND Protocol LIKE '%-$IPVersion' AND Offline='0' ORDER BY PK_Firewall"
			   
			R=$(RunSQL "$Q")

			for Port in $R; do
				tmp=$(Field 2 "$Port")
				echo $tmp | grep -q '-'
				if [ $? -eq 0 ]; then
			    		Protocol="$(echo $tmp | cut -d"-" -f 1)"
				else
		    			Protocol=$(Field 2 "$Port")
				fi
				Matchname=$(Field 1 "$Port")
				IntIf=$(Field 3 "$Port")
				ExtIf=$(Field 4 "$Port")
				Port1=$(Field 5 "$Port")
				Port2=$(Field 6 "$Port")
				SrcIP=$(Field 7 "$Port")
				Disabled=$(Field 9 "$Port")
				RPolicy=$(Field 10 "$Port")
				Description=$(Field 11 "$Port")

				if [[ "$RPolicy" =~ "$Policy" ]]; then
					echo "Rule Policy matches default Policy, not adding"
				else
					echo "Rule Policy not matches default Policy, adding"
					[[ "$Port2" -eq 0 ]] && Port2="$Port1"
					echo OpenPort "INPUT" "$Matchname" "$Protocol" "$IPVersion" "$Port1:$Port2" "$SrcIP" "$IntIf" "$ExtIf" "$RPolicy" "$Description"
					[[ "Disabled" -eq 0 ]] && OpenPort "INPUT" "$Matchname" "$Protocol" "$IPVersion" "$Port1:$Port2" "$SrcIP" "$IntIf" "$ExtIf" "$RPolicy" "$Description"
				fi
			done
	fi
done


# Configuring all core_forwarding
echo "Setting up core_forward"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='forward' AND RuleType LIKE 'policy%' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

	if ([[ "$IPVersion" == "ipv4" ]] &&  [[ ! "$DisableIPv4Firewall" == "1" ]]) || ([[ "$IPVersion" == "ipv6" ]] && [[ ! "$DisableIPv6Firewall" == "1" ]]); then
		Q="SELECT Matchname, Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP,SourceIP,IntIf,ExtIf,Disabled,RPolicy,Description FROM Firewall WHERE RuleType='forward' AND Protocol LIKE '%-$IPVersion' AND Offline='0' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")

		for Port in $R; do
			tmp=$(Field 2 "$Port")
			echo $tmp | grep -q '-'
			if [ $? -eq 0 ]; then
				Protocol="$(echo $tmp | cut -d"-" -f 1)"
			else
				Protocol=$(Field 2 "$Port")
			fi
			Matchname=$(Field 1 "$Port")
			SrcPort=$(Field 3 "$Port")
			SrcPortEnd=$(Field 4 "$Port")
			DestPort=$(Field 5 "$Port")
			DestIP=$(Field 6 "$Port")
			SrcIP=$(Field 7 "$Port")
			IntIf=$(Field 8 "$Port")
			ExtIf=$(Field 9 "$Port")
			Disabled=$(Field 10 "$Port")
			RPolicy=$(Field 11 "$Port")
			Description=$(Field 12 "$Port")

			if [[ "$RPolicy" =~ "$Policy" ]]; then
				echo "Rule Policy matches default Policy, not adding"
			else
				echo "Rule Policy not matches default Policy, adding"
			     [[ "Disabled" -eq 0 ]] && ForwardPort "FORWARD" "$Matchname" "$Protocol" "$IPVersion" "$ExtIP" "$SrcPort" "$DestIP" "$DestPort" "$RPolicy" "$SrcIP" "$IntIf" "$ExtIf" "$Description"
			fi
		done
	fi
done

# Configuring all port forwards

echo "Setting up Port_forwarding"
ForwardTypeArr=('PREROUTING' 'POSTROUTING' 'OUTPUT' 'MASQUERADE')
for ForwardType in "${ForwardTypeArr[@]}"; do
	echo $ForwardType
	Q="SELECT Matchname,Protocol,SourcePort,SourcePortEnd,DestinationPort,DestinationIP,SourceIP,IntIf,ExtIf,Disabled,RPolicy,RuleType,Description FROM Firewall WHERE RuleType LIKE '%-$ForwardType' AND Protocol LIKE '%ipv4' AND Offline='0' ORDER BY PK_Firewall"
	R=$(RunSQL "$Q")
			for Port in $R; do
			tmp=$(Field 2 "$Port")
			echo $tmp | grep -q '-'
			if [ $? -eq 0 ]; then
				Protocol="$(echo $tmp | cut -d"-" -f 1)"
			else
				Protocol=$(Field 2 "$Port")
			fi
			tmp=$(Field 12 "$Port")
			RuleType="$(echo $tmp | cut -d"-" -f 2)"
			IPVersion="ipv4"
			Matchname=$(Field 1 "$Port")
			SrcPort=$(Field 3 "$Port")
			SrcPortEnd=$(Field 4 "$Port")
			DestPort=$(Field 5 "$Port")
			DestIP=$(Field 6 "$Port")
			SrcIP=$(Field 7 "$Port")
			IntIf=$(Field 8 "$Port")
			ExtIf=$(Field 9 "$Port")
			Disabled=$(Field 10 "$Port")
			RPolicy=$(Field 11 "$Port")
			Description=$(Field 13 "$Port")

				
			if [[ "$IPVersion" == ipv4 ]]; then
		   		if [[ "$SrcPortEnd" -eq 0 ]]; then
					[[ "Disabled" -eq 0 ]] && ForwardPort "$RuleType" "$Matchname" "$Protocol" "$IPVersion" "$ExtIP" "$SrcPort" "$DestIP" "$DestPort" "$RPolicy" "$SrcIP" "$IntIf" "$ExtIf" "$Description"
				else
					DPort="$DestPort"
						for ((SPort=SrcPort; SPort<=SrcPortEnd; SPort++)); do
							[[ "Disabled" -eq 0 ]] && ForwardPort "$RuleType" "$Matchname" "$Protocol" "$IPVersion" "$ExtIP" "$SPort" "$DestIP" "$DPort" "$RPolicy" "$SrcIP" "$IntIf" "$ExtIf" "$Descripition"
							[[ "$DPort" -ne 0 ]] && ((++DPort))
						done
				fi
			else
				echo "Ignoring IPv6 port forward rule for port $SrcPort to $DestIP as not (yet) supported by ip6tables"
			fi
		done
	i=$[$i+1]	
	done

# Configuring all Output ports
echo "Opening specified ports from exterior"

Q="SELECT Matchname,RuleType FROM Firewall WHERE Protocol='output' AND RuleType LIKE 'policy%' ORDER BY PK_Firewall"
R=$(RunSQL "$Q")

for Protocol in $R; do
	Policy=$(Field 1 "$Protocol")
	Policy=$(echo $Policy | tr '[:lower:]' '[:upper:]')
	RuleType=$(Field 2 "$Protocol")
	IPVersion=$(echo $RuleType | cut -dy -f2)

	if ([[ "$IPVersion" == "ipv4" ]] &&  [[ ! "$DisableIPv4Firewall" == "1" ]]) || ([[ "$IPVersion" == "ipv6" ]] && [[ ! "$DisableIPv6Firewall" == "1" ]]); then
		Q="SELECT Matchname, Protocol,SourcePort,SourcePortEnd,SourceIP,IntIf,ExtIf,Disabled,RPolicy,Description FROM Firewall WHERE RuleType='output' AND Protocol LIKE '%-$IPVersion' AND Offline='0' ORDER BY PK_Firewall"
		R=$(RunSQL "$Q")

		for Port in $R; do
			tmp=$(Field 2 "$Port")
			echo $tmp | grep -q '-'
			if [ $? -eq 0 ]; then
   					Protocol="$(echo $tmp | cut -d"-" -f 1)"
			    	IPversion="$(echo $tmp | cut -d"-" -f 2)"
			else
   					Protocol=$(Field 2 "$Port")
			    	IPversion="ipv4"
			fi
			Matchname=$(Field 1 "$Port")
			Port1=$(Field 3 "$Port")
			Port2=$(Field 4 "$Port")
			SrcIP=$(Field 5 "$Port")
			IntIf=$(Field 6 "$Port")
			ExtIf=$(Field 7 "$Port")
			Disabled=$(Field 8 "$Port")
			RPolicy=$(Field 9 "$Port")
			Description=$(Field 10 "$Port")

			if [[ "$RPolicy" =~ "$Policy" ]]; then
				echo "Rule Policy matches default Policy, not adding"
			else
				echo "Rule Policy not matches default Policy, adding"
				[[ "$Port2" -eq 0 ]] && Port2="$Port1"
				[[ "Disabled" -eq 0 ]] && OpenPort "OUTPUT" "$Matchname" "$Protocol" "$IPVersion" "$Port1:$Port2" "$SrcIP" "$IntIf" "$ExtIf" "$RPolicy" "$Description"
			fi
		done
	fi
done

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
			iptables -P $Chain ACCEPT
		else
			if [ $Policy = "REJECT" ]; then
				iptables -P $Chain DROP
				iptables -A $Chain -j REJECT --reject-with icmp-port-unreachable
			else
				iptables -P $Chain $Policy
			fi
		fi
	else
		if [[ "$DisableIPv6Firewall" == 1 ]]; then
			ip6tables -P $Chain ACCEPT
		else
			if [ $Policy = "REJECT" ]; then
				ip6tables -P $Chain DROP
				ip6tables -A $Chain -j REJECT --reject-with icmp-port-unreachable
			else
				ip6tables -P $Chain $Policy
			fi
		fi
	fi
done


# Set discovered MSS to make PMTU work on PPPoE interfaces
if [[ "$PPPoEEnabled" == "on" ]]; then
	iptables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
	ip6tables --append FORWARD -o ppp+ --protocol tcp --tcp-flags SYN,RST SYN --jump TCPMSS --clamp-mss-to-pmtu
fi

# Drop illegal ipv4 packets
iptables -A INPUT -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP
iptables -A INPUT -p tcp --tcp-flags ALL ALL -j DROP
iptables -A INPUT -p tcp --tcp-flags ALL NONE -j DROP# NULL packets
iptables -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
iptables -A INPUT -p tcp --tcp-flags SYN,FIN SYN,FIN -j DROP #XMAS
iptables -A INPUT -p tcp --tcp-flags FIN,ACK FIN -j DROP # FIN packet scans
iptables -A INPUT -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP

# Drop illegal ipv6 packets
ip6tables -A INPUT -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP
ip6tables -A INPUT -p tcp --tcp-flags ALL ALL -j DROP
ip6tables -A INPUT -p tcp --tcp-flags ALL NONE -j DROP# NULL packets
ip6tables -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
ip6tables -A INPUT -p tcp --tcp-flags SYN,FIN SYN,FIN -j DROP #XMAS
ip6tables -A INPUT -p tcp --tcp-flags FIN,ACK FIN -j DROP # FIN packet scans
ip6tables -A INPUT -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP

# Log denied packets, i.e. packets that make it through the ruleset without being accepted
if [[ "$DefaultPolicy" == "DROP" ]]; then
	iptables -A INPUT -m limit --limit 5/min -j LOG --log-prefix "IPv4 packet denied: " --log-level 7
fi
if [[ "$DefaultIPv6Policy" == "DROP" ]]; then
	ip6tables -A INPUT -m limit --limit 5/min -j LOG --log-prefix "IPv6 packet denied: " --log-level 7
fi

# Add TOP 20 known attackers to blocklist
if [[ "$fw_blocklist" == "1" ]]; then
	 /usr/pluto/bin/Network_Blocklist.sh
	ln -s /usr/pluto/bin/Network_Blocklist.sh /etc/cron.hourly/blocklist
else
	rm /etc/cron.hourly/blocklist
fi

ExtIf=$OldExtIf

