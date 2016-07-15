#!/bin/bash

# Define paths, as just "iptables" won't do it when run from apache
IPTABLES=/sbin/iptables
IP6TABLES=/sbin/ip6tables


. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/LockUtils.sh

#####
# Function to set the rules
#####
IPTables()
{

        IPversion="$1"
		ACTION="$2"
		Place="$3"
		Table="$4"
        RuleType="$5"
        IntIf="${6}"
        ExtIf="${7}"
        Matchname="$8"
        Protocol="${9}"
        FilterIP="${10}"
        SrcPort="${11}"
        DestIP="${12}"
        DestPort="${13}"
        RPolicy="${14}"
        Description="${15}"
		
		grep -qv "[-]" <<< $RuleType
        if [ ! $? -eq 0 ]; then
			tmp="$(echo $RuleType | cut -d"-" -f 1)"
			if [ ! $tmp = "fail2ban" ]; then
				Table="$(echo $RuleType | cut -d"-" -f 1)"
				RuleType="$(echo $RuleType | cut -d"-" -f 2)"
			fi
		fi
		
		if [[ "$RuleType" = "input" ]] || [[ "$RuleType" = "forward" ]] || [[ "$RuleType" = "output" ]]; then
			RuleType=${RuleType^^}
		fi
		
		if [[ -z $ACTION ]] || [[ "$ACTION" == "add" ]]; then
			RuleType="-A $RuleType"
		elif [ $ACTION = "insert" ]; then
		   	RuleType="-I $RuleType $Place"
	        elif [[ $ACTION = "del" ]]; then
			RuleType="-D $RuleType $Place"
		fi

		if [[ "$Table" != "" ]] &&  [[ "$Table" != "NULL" ]]; then
			Table="-t $Table"
		else
			Table=""
		fi
		
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
                if [[ "$Protocol" = "ip" ]]; then
                        parmSPort="NULL"
                        tmp="$(echo $SrcPort | cut -d: -f 1)"
                        if [[ -n "$tmp" && ("$tmp" != "NULL" || "$tmp" -ne 0) ]]; then
                                Protocol="-p $tmp"
                        else
                                Protocol=""
                        fi
				elif [[ "$Protocol" = "tcp" ]]; then
					Protocol="-p tcp"
				elif [[ "$Protocol" = "udp" ]]; then
					Protocol="-p udp"
				elif [[ "$Protocol" = "icmp" ]]; then
					Protocol="-p icmp"
				elif [[ "$Protocol" = "icmpv6" ]]; then
					Protocol="-p icmpv6"
				elif [[ "$Protocol" = "all" ]]; then
					Protocol="all"
				else
					Protocol=""
				fi
			

                if [[ "$FilterIP" = "" ]] || [[ "$FilterIP" = "NULL" ]]; then
                        if [[ "$IPversion" = "ipv4" ]] || [[ "$IPversion" = "both" ]]; then
                                FilterIP4="-s 0.0.0.0/0"
                        fi
                        if [[ "$IPversion" = "ipv6" ]] || [[ "$IPversion" = "both" ]]; then
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
				
				echo "$SrcPort"
                if [[ "$SrcPort" = ""  ]] ||  [[ "$SrcPort" = ":" ]] || [[ "$SrcPort" = "0" ]] || [[ "$SrcPort" = ":0" ]] || [[ "$SrcPort" = "NULL" ]] || [[ "$SrcPort" = "NULL:0" ]] || [[ "$SrcPort" = "NULL:NULL" ]] || [[ "$parmSPort" = "NULL" ]]; then
                        parmSPort=""
                else
						SPortend="$(echo $SrcPort | cut -d: -f 2)"
						   SPort="$(echo $SrcPort | cut -d: -f 1)"
						   echo $SPortend 
						if [[ ! "$SPortend" = "NULL" ]] && [[ ! "$SPortend" = "0" ]] && [[ ! "$SPortend" == "$SPort" ]]; then
							parmSPort="--sports $SrcPort"
						else
							parmSPort="--sport $SPort"
							echo "$parmSPort"
							
						fi
                fi

                if [[ "$DestIP" = "" ]] || [[ "$DestIP" = "NULL" ]]; then
                        if [[ "$IPversion" = "ipv4" ]] || [[ "$IPversion" = "both" ]]; then
                                DestIP4="-d 0.0.0.0/0"
                        fi
                        if [[ "$IPversion" = "ipv6" ]] || [[ "$IPversion" = "both" ]]; then
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

                if [[ "$DestPort" == ""  ]] || [[ "$DestPort" == ":" ]] || [[ "$DestPort" == "0" ]] || [[ "$DestPort" == "NULL" ]] || [[ "$DestPort" == "NULL:NULL" ]]; then
                        parmDPort=""
                else
						DPortend="$(echo $DestPort | cut -d: -f 2)"
						   DPort="$(echo $DestPort | cut -d: -f 1)"
						if [[ ! "$DPortend" = "NULL" ]] && [[ ! "$DPortend" = "0" ]] && [[ ! "$DPortend" == "$DPort" ]]; then
							parmDPort="--dports $DestPort"
						else
							parmDPort="--dport $DPort"
						fi
                fi
				
				if [[ "$RPolicy" = "REDIRECT" ]]; then
					DestPort="$(echo $DestPort | cut -d":" -f 1)"
					parmRPolicy="-j REDIRECT --to $DestPort" 
					parmDPort=""
				elif [[ "$RPolicy" = "DNAT" ]]; then
					DestPort="$(echo $DestPort | cut -d":" -f 1)"
					parmRPolicy="-j DNAT --to-destination $DestIP:$DestPort"
					DestIP4="-d 0.0.0.0/0"
					parmDPort=""
				else
					parmRPolicy="-j $RPolicy"
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
                                echo iptables $Table $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                                $IPTABLES $Table $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                                # samba 139/tcp ports come paired with explicit rejects on 445/tcp
                                # reason: to avoid timeout connecting to 445/tcp in smbclient
                                if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
                                        $IPTABLES -A INPUT -p tcp -s "$FilterIP4" --dport 445 -j REJECT
                                fi
                        else
                                echo iptables $Table $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                                $IPTABLES $Table $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                                echo iptables $Table $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                                $IPTABLES $Table $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP4 $parmSPort $DestIP4 $parmDPort $parmRPolicy $parmDescription
                        fi

                fi
        fi

                # apply rule to IPv6 chain if needed
        if [[ "$IPversion" == ipv6 ]] || [[ "$IPversion" == both ]]; then
                if [[ ! "$DisableIPv6Firewall" == "1" ]]; then
                        if [[ ! "$Protocol" == "all" ]]; then
                                echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
                                        $IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname $Protocol $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
                                # samba 139/tcp ports come paired with explicit rejects on 445/tcp
                                # reason: to avoid timeout connecting to 445/tcp in smbclient
                                if [[ "$Protocol" == tcp && ( "$PortBegin" -le 139 && "$PortEnd" -ge 139 ) ]]; then
                                        $IP6TABLES -A INPUT -p tcp -s "$FilterIP6" --dport 445 -j REJECT
					echo "1"
 fi
                        else
                                echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
                                $IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname -p tcp $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
                                echo ip6tables $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
                                $IP6TABLES $RuleType $parmIntIf $parmExtIf $Matchname -p udp $FilterIP6 $parmSPort $DestIP6 $parmDPort $parmRPolicy $parmDescription
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

        Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RuleType,RPolicy,Description FROM Firewall WHERE RuleType='$chain' AND Protocol LIKE '%-$IPVersion' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
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
				tmp=$(Field 10 "$Port")
                echo $tmp | grep -q '-'
                if [ $? -eq 0 ]; then
					#Table="$(echo $tmp | cut -d"-" -f 1)"
					Table=""
				fi
		if [ "$Table" = "NULL" ]; then
			Table=""
		fi
                RPolicy=$(Field 11 "$Port")
                Description=$(Field 12 "$Port")

                echo IPTables "$IPVersion" "-A $chain" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                     IPTables "$IPVersion" "add" "0" "$Table" "$chain" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
        done
}

AddRuleFromExt()
{
    ActionF="$1"
    PlaceF="$2"
    TableF="$3"
    ChainF="$4"
    IntIfF="$5"
    ExtIfF="$6"
    MatchnameF="$7"
    ProtocolF="$8"
    SourceIPF="${9}"
    SourcePortF="${10}"
	SourcePortEndF="${11}"
    DestinationIPF="${12}"
    DestinationPortF="${13}"
	DestinationPortEndF="${14}" 
    RPolicyF="${15}"
    DescriptionF="${16}"
	
	if [[ "$TableF" = "nat" ]]; then
		ChainF="nat-$ChainF"
    fi
	
  #Get the highest record from the db with the associated RuleType. 
  highest="SELECT MAX( Place ) AS max FROM Firewall WHERE RuleType='$ChainF'"
  R=$(RunSQL "$highest")
  for max in $R; do
                max=$(Field 1 "$max")
  done
  
  #Get the highest record AND RPolicy with the associated RuleType.
  maxsql="SELECT Place AS max, RPolicy FROM Firewall WHERE RuleType='$ChainF' AND Place='$max'"
  R=$(RunSQL "$maxsql")
  for policy in $R; do
    max=$(Field 1 "$policy")
    policy=$(Field 2 "$policy")
  done
  
  if [[ "$PlaceF" -eq "0" ]]; then
		PlaceF=$[$PlaceF+1]
  fi
  
  if [[ -z "$PlaceF" ]] || [[ "$PlaceF" -eq "$max" ]] || [[ "$PlaceF" -gt "$max" ]]; then 
       if [[ "$policy" = "DROP" ]]; then 
	 PlaceF=$max
	 Checkplace=$PlaceF
	 Checkplace=$[$Checkplace-1]
	 highest="SELECT Place, RPolicy FROM Firewall WHERE RuleType='$ChainF' AND Place='$Checkplace'"
	 R=$(RunSQL "$highest")
	 for placedb in $R; do
           place=$(Field 1 "$placedb")
	   cpolicy=$(Field 2 "$placedb")
         done
         if [[ "$cpolicy" = "LOG" ]]; then
	      PLACEF=$place
         fi
       elif [[ "$policy" = "LOG" ]]; then
	 PlaceF=$max
       else 
	 PlaceF=$max
	 PlaceF=$[$PlaceF+1]
       fi
  fi
	echo "$PlaceF $max"

  if [[ "$PlaceF" -lt "$max" ]]; then
	echo "$PlaceF insert"
	ActionF="insert"
    sql="BEGIN; UPDATE Firewall SET Place = Place + 1 WHERE RuleType='$ChainF' AND Place >= '$PlaceF' order by Place DESC; INSERT INTO Firewall (Place,IntIF,ExtIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES ('$PlaceF','$IntIfF','$ExtIfF','$MatchnameF','$ProtocolF','$SourcePortF','$SourcePortEndF','$DestinationPortF:$DestinationPortEndF','$DestinationIPF','$ChainF','$SourceIPF','$RPolicyF','$DescriptionF'); COMMIT;"
    $(RunSQL "$sql")

  else
   echo "$PLACE add"
   ActionF="add"
   sql="INSERT INTO Firewall (Place,IntIF,ExtIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES ('$PlaceF','$IntIfF','$ExtIfF','$MatchnameF','$ProtocolF','$SourcePortF','$SourcePortEndF','$DestinationPortF:$DestinationPortEndF','$DestinationIPF','$ChainF','$SourceIPF','$RPolicyF','$DescriptionF')"
   $(RunSQL "$sql")

  fi
	echo "$ACTIONF rule to chain"
	IPVersionF="$(echo $ProtocolF | cut -d- -f2)"
    ProtocolF="$(echo $ProtocolF | cut -d- -f1)"	
	echo "1"
	IPTables "$IPVersionF" "$ActionF" "$PlaceF" "$TableF" "$ChainF" "$IntIfF" "$ExtIfF" "$MatchnameF" "$ProtocolF" "$SourceIPF" "$SourcePortF:$SourcePortEndF" "$DestinationIPF" "$DestinationPortF:$DestinationPortEndF" "$RPolicyF" "$DescriptionF"
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


Firewall()
{
  echo "
Usage: notifyme.sh (-vr) [-s Subject] [-a Application] (-p Priority {-2 => 2})  message
Try 'notifyme.sh -h' for more information."
  exit 1
}

help()
{
  echo "
Usage: notifyme.sh (-vr) [-s Subject] [-a Application] (-p Priority {-2 => 2})  message
Options:
  -S SERVICE (Required) {prowl, nma}
    The required service (prowl or nma)
  -s SUBJECT (Required)
    The subject line of the message that is being sent
  -a APPLICATION (Required)
    The application the message is coming from
  -p {-2 => 2}
    The priority of the message.
  -v
    Displays a success or failure message after receiving response using XPath if XPath is available
  -r
    Displays the raw XML output response from the API
  -h
    Shows this help text"
  exit 1
}

# process options
while getopts H:t:A:C:p:I:O:M:P:R:S:s:r:D:d:T:c:L:h o
do  case "$o" in
  H) HOST=$OPTAR;;
  t) TABLE=$OPTARG;;
  A) ACTION=$OPTARG;;
  C) CHAIN=$OPTARG;;
  p) PLACE=$OPTARG;;
  I) INIF=$OPTARG;;
  O) OUTIF=$OPTARG;;
  M) MATCHNAME=$OPTARG;;
  P) PROTOCOL=$OPTARG;;
  R) SOURCEIP=$OPTARG;;
  S) SOURCEPORT=$OPTARG;;
  s) SOURCEPORTEND=$OPTARG;;
  r) DESTINATIONIP=$OPTARG;;
  D) DESTINATIONPORT=$OPTARG;;
  d) DESTINATIONPORTEND=$OPTARG;;
  T) RPOLICY=$OPTARG;;
  c) DESCRIPTION=$OPTARG;;
  L) RULE=$OPTARG;;
  h) help;;
 [?]) reload;;
  esac
done
# shift the option values out
shift $(($OPTIND - 1))


if [[ ! -z "$RULE" ]]; then
  if [[ "$RULE" == "BLOCKLIST" ]]; then
    if [[ "$ACTION" = "add" ]]; then
		sql="SELECT * FROM Firewall WHERE RuleType='INPUT' AND RPolicy='BLOCKLIST' AND Protocol='all-ipv4'"
		R=$(RunSQL "$sql")
		if ! [ "$R" ]; then
			AddRuleFromExt "insert" "1" "filter" "INPUT" "NULL" "NULL" "NULL" "all-ipv4" "NULL" "NULL" "NULL" "NULL" "NULL" "NULL" "BLOCKLIST" "BLOCKLIST"
		fi
        echo "BLOCKLIST add"
         #Get the highest record from the db with the associated RuleType.
         highest="SELECT MAX( Place ) AS max FROM Firewall WHERE RuleType='BLOCKLIST'"
         R=$(RunSQL "$highest")
         for max in $R; do
                max=$(Field 1 "$max")
         done
         PLACE=$[$max+1]
	    #insert the rule into the database
         sql="INSERT INTO Firewall (Place,IntIF,ExtIF,Matchname, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType,SourceIP,RPolicy,Description) VALUES ('$PLACE','$INIF','$OUTIF','$MATCHNAME','$PROTOCOL','$SOURCEPORT','$SOURCEPORTEND','$DESTINATIONPORT:$DESTINATIONPORTEND','$DESTINATIONIP','$CHAIN','$SOURCEIP','$RPOLICY','$DESCRIPTION')"
         $(RunSQL "$sql")
         #add the rule to iptables
		 IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
         PROTOCOL="$(echo $PROTOCOL | cut -d- -f1)"
	    echo IPTables "$IPVersion" "$ACTION" "$PLACE" "$TABLE" "$CHAIN" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT:$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT:$DESTINATIONPORTEND" "$RPOLICY" "$DESCRIPTION"
         IPTables "$IPVersion" "$ACTION" "$PLACE" "$TABLE" "BLOCKLIST" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT:$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT:$DESTINATIONPORTEND" "$RPOLICY" "$DESCRIPTION"
	elif [[ "$ACTION" = "Flush" ]]; then
        echo "BLOCKLIST Flush"
        sql="DELETE FROM Firewall WHERE RuleType='BLOCKLIST' AND Protocol='all-ipv4' AND RPolicy='DROP'"
        $(RunSQL "$sql")
        $IPTABLES -F BLOCKLIST
    fi
  exit 0
  fi
  if [[ "$ACTION" == "addchain" ]]; then
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="SELECT * FROM Firewall WHERE Matchname='$CHAIN' AND Protocol='$PROTOCOL'"
		R=$(RunSQL "$sql")
		if ! [ "$R" ]; then
			sql="INSERT INTO Firewall (Matchname, Protocol) VALUES ('$CHAIN','$PROTOCOL')"
			$(RunSQL "$sql")	
			$IPTABLES -N $CHAIN
		fi
	else
	
		$IP6TABLES -N $CHAIN
	fi
	exit 0
  fi
  if [[ "$ACTION" == "editchain" ]]; then
	IPVersion=$PROTOCOL
	if [[ "$IPVersion" = "ipv4" ]]; then
		#Rename the chain
		sql="UPDATE Firewall SET Matchname='$CHAIN' WHERE Matchname='$DESCRIPTION'"
		$(RunSQL "$sql")
		#Rename Connections
		sql="UPDATE Firewall SET RPolicy='$CHAIN' WHERE RPolicy='$DESCRIPTION'"
		$(RunSQL "$sql")
		#Rename the rules
		sql="UPDATE Firewall SET RuleType='$CHAIN' WHERE RuleType='$DESCRIPTION'"
		$(RunSQL "$sql")
			$IPTABLES -E $DESCRIPTION $CHAIN
	else
	
		$IP6TABLES -E $DESCRIPTION $CHAIN
	fi
	exit 0
  fi
  if [[ "$ACTION" == "delchain" ]]; then
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="DELETE FROM Firewall WHERE Matchname='$CHAIN' AND Protocol='$PROTOCOL'"
		$(RunSQL "$sql")
		$IPTABLES -F $CHAIN
		$IPTABLES -X $CHAIN
	else
		
		$IP6TABLES -X $CHAIN
	fi
  exit 0
  fi
  if [[  "$ACTION" == "Policy" ]]; then
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		RULETYPE="$(echo $PROTOCOL | cut -d- -f1)""$(echo $PROTOCOL | cut -d- -f2)"
		sql="UPDATE Firewall SET Matchname='$MATCHNAME' WHERE Protocol='$CHAIN' AND RuleType='$RULETYPE'"
		$(RunSQL "$sql")
		CHAIN=${CHAIN^^}
		$IPTABLES --policy $CHAIN $MATCHNAME
	else
		sql="UPDATE Firewall SET Matchname='$MATCHNAME' WHERE Protocol='$CHAIN' AND RuleType='$PROTOCOL'"
		$(RunSQL "$sql")
		$IP6TABLES --policy $CHAIN $RPOLICY
	fi
  exit 0
  fi
  if [[ "$ACTION" = "move" ]]; then
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		if [[ "$DESCRIPTION" = "moveback" ]]; then
			PlaceN=$[PLACE-1]
			if [[ "$PlaceN" != "0" ]]; then
				move="ok"
			fi
		elif [[ "$DESCRIPTION" = "movenext" ]]; then
			PlaceN=$[PLACE+1]
			highest="SELECT MAX( Place ) AS max FROM Firewall WHERE RuleType='$CHAIN' AND Protocol LIKE '%-$IPVersion'"
			R=$(RunSQL "$highest")
			for max in $R; do
                max=$(Field 1 "$max")
			done
			if [[ "$PlaceN" -le "$max" ]]; then
				move="ok"
			fi
		fi
		if [[ ! -z "$move" ]]; then
			sql="BEGIN;
			     UPDATE Firewall SET Place = '0' WHERE RuleType='$CHAIN' AND Place='$PLACE' AND Protocol LIKE '%-$IPVersion';
				 UPDATE Firewall SET Place = Place - 1 WHERE RuleType='$CHAIN' AND Place > '$PLACE' AND Protocol LIKE '%-$IPVersion' order by Place ASC;
				 UPDATE Firewall SET Place = Place + 1 WHERE RuleType='$CHAIN' AND Place >= '$PlaceN' AND Protocol LIKE '%-$IPVersion' order by Place DESC;
				 UPDATE Firewall SET Place = '$PlaceN' WHERE RuleType='$CHAIN' AND Place='0' AND Protocol LIKE '%-$IPVersion';
				 COMMIT;"
			CHAIND=${CHAIN^^}
			$IPTABLES -D $CHAIND $PLACE
			$(RunSQL "$sql")
			Q="SELECT Place,IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PlaceN' AND Offline='0'"
			R=$(RunSQL "$Q")
			for Port in $R; do
				Place=$(Field 1 "$Port")
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
				#echo IPTables "$IPVersion" "insert" "$PLACE" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
				IPTables "$IPVersion" "insert" "$Place" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
			done
		fi
	else
		$IP6TABLES --policy $CHAIN $RPOLICY
	fi
  exit 0
  fi
  if [[ "$ACTION" = "Del" ]]; then
	echo "Delete rule"
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	PROTOCOLv="$(echo $PROTOCOL | cut -d- -f1)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		if [[ "$TABLE" = "nat" ]]; then
			#Get the data to find the matching forward rule to delete
			Q="SELECT SourcePort, DestinationPort FROM Firewall WHERE RuleType='nat-$CHAIN' AND PROTOCOL='$PROTOCOL' AND Place='$PLACE'"
			echo $Q
			R=$(RunSQL "$Q")
			for Port in $R; do
				SOURCEPORT=$(Field 1 "$Port")
				DESTINATIONPORT=$(Field 2 "$Port")
				echo $DESTINATIONPORT $SOURCEPORT
			done
			Q="SELECT Place FROM Firewall WHERE RuleType='FORWARD' AND PROTOCOL='$PROTOCOL' AND SourcePort='$SOURCEPORT' AND DestinationPort='$DESTINATIONPORT'"
			echo $Q
			R=$(RunSQL "$Q")
			for Port in $R; do
				PLACEFOREWARD=$(Field 1 "$Port")
				echo $PLACEFOREWARD
				echo "1"
			done
			#Delete the nat rule
			sql="DELETE FROM Firewall WHERE RuleType='nat-$CHAIN' AND Protocol='$PROTOCOL' AND Place=$PLACE"
			echo $sql
			$(RunSQL "$sql")
			sql="BEGIN;
				UPDATE Firewall SET Place = Place - 1 WHERE RuleType='nat-$CHAIN' AND Place > '$PLACE' AND Protocol='$PROTOCOL' order by Place ASC;
				COMMIT;"
			$(RunSQL "$sql")
			$IPTABLES -t nat -D $CHAIN $PLACE
			#Delete the forward rule matched by the nat rule.
			sql="DELETE FROM Firewall WHERE RuleType='FORWARD' AND Protocol='$PROTOCOL' AND Place=$PLACEFOREWARD"
			echo $sql
			$(RunSQL "$sql")
			sql="BEGIN;
				UPDATE Firewall SET Place = Place - 1 WHERE RuleType='FORWARD' AND Place > '$PLACEFOREWARD' AND Protocol='$PROTOCOL' order by Place ASC;
				COMMIT;"
			$(RunSQL "$sql")
			$IPTABLES -D FORWARD $PLACEFOREWARD
		else
			sql="DELETE FROM Firewall WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'"
			$(RunSQL "$sql")
			sql="BEGIN;
				UPDATE Firewall SET Place = Place - 1 WHERE RuleType='$CHAIN' AND Place > '$PLACE' AND Protocol='$PROTOCOL' order by Place ASC;
				COMMIT;"
			$(RunSQL "$sql")
			echo "IPTABLES -D $CHAIN $PLACE"
			CHAIN=${CHAIN^^}
			$IPTABLES -D $CHAIN $PLACE
		fi
	else
		sql="DELETE FROM Firewall WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'"
		echo "$sql"
		$(RunSQL "$sql")
		sql="BEGIN;
			UPDATE Firewall SET Place = Place - 1 WHERE RuleType='$CHAIN' AND Place > '$PLACE' AND Protocol='$PROTOCOL' order by Place ASC;
			COMMIT;"
		$(RunSQL "$sql")
		CHAIN=${CHAIN^^}
		$IP6TABLES -D $CHAIN $PLACE
	fi
	exit 0
  fi
  if [[ "$ACTION" = "Disable" ]]; then
    echo "Disable rule"
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="UPDATE Firewall SET Disabled='1' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'";
		$(RunSQL "$sql")
		CHAIN=${CHAIN^^}
		$IPTABLES -D $CHAIN $PLACE
	else
		sql="UPDATE Firewall SET Disabled='1' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'";
		$(RunSQL "$sql")
		CHAIN=${CHAIN^^}
		$IP6TABLES -D $CHAIN $PLACE
	fi
  exit 0
  fi
  if [[ "$ACTION" = "Enable" ]]; then
	echo "Enable rule"
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	echo "$IPVersion"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="UPDATE Firewall SET Disabled='0' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'";
		$(RunSQL "$sql")
		Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE' AND Offline='0'"
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
            DestIP=$(Field 8 "$Port")
            DestPort=$(Field 9 "$Port")
            RPolicy=$(Field 10 "$Port")
            Description=$(Field 11 "$Port")
            #echo IPTables "$IPVersion" "insert" "$PLACE" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
            IPTables "$IPVersion" "insert" "$PLACE" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
        done		
	else
		$IP6TABLES -D $CHAIN $PLACE
	fi
  exit 0
  fi
  if [[ "$ACTION" = "EnableVPN" ]]; then
		#add the chain 
		Q="INSERT INTO Firewall (Protocol, Matchname) VALUES('chain-ipv4', 'VPN');"
        $(RunSQL "$Q")
		#Iptables
		$IPTABLES -N VPN
		
		#add the links to the chain VPN
		#input
		AddRuleFromExt "insert" "2" "filter" "INPUT" "NULL" "NULL" "NULL" "ip-ipv4" "NULL" "NULL" "NULL" "NULL" "NULL" "NULL" "VPN" "Allow VPN"
		#forward
		AddRuleFromExt "insert" "2" "filter" "FORWARD" "NULL" "NULL" "NULL" "ip-ipv4" "NULL" "NULL" "NULL" "NULL" "NULL" "NULL" "VPN" "Allow VPN"		
		#add the rules
		#500
		AddRuleFromExt "insert" "1" "filter" "VPN" "NULL" "NULL" "-m policy --dir in --pol none" "udp-ipv4" "NULL" "NULL" "NULL" "NULL" "500" "NULL" "ACCEPT" "Allow VPN"
		#4500
		AddRuleFromExt "insert" "2" "filter" "VPN" "NULL" "NULL" "-m policy --dir in --pol none" "udp-ipv4" "NULL" "NULL" "NULL" "NULL" "4500" "NULL" "ACCEPT" "Allow VPN"
        $(RunSQL "$Q")
		#1701 nee to be in ipsec.
		AddRuleFromExt "insert" "3" "filter" "VPN" "NULL" "NULL" "-m policy --dir in --pol ipsec" "udp-ipv4" "NULL" "NULL" "NULL" "NULL" "1701" "NULL" "ACCEPT" "Allow VPN"
        $(RunSQL "$Q")
  exit 0
  fi
  if [[ "$ACTION" = "DisableVPN" ]]; then
		#Get the link id's to remove (TODO)
		sql="SELECT Place FROM Firewall WHERE" 
		#delete chain
		sql="DELETE FROM Firewall WHERE Matchname='VPN'"
		$(RunSQL "$sql")
		$IPTABLES -F VPN
		$IPTABLES -X VPN
		#rules
  exit 0
  fi
  if [[ "$ACTION" = "Offline" ]]; then
   echo "set rule offline"
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="UPDATE Firewall SET Offline='1' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL'";
		$(RunSQL "$sql")
		#CHAIN=${CHAIN^^}
		$IPTABLES -D $CHAIN $PLACE
	else
		sql="UPDATE Firewall SET Offline='1' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE'";
		$(RunSQL "$sql")
		#CHAIN=${CHAIN^^}
		$IP6TABLES -D $CHAIN $PLACE
	fi
  exit 0
  fi
  if [[ "$ACTION" = "Online" ]]; then
		echo "Set rule Online"
	IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
	echo "$IPVersion"
	if [[ "$IPVersion" = "ipv4" ]]; then
		sql="SELECT Protocol FROM Firewall WHERE RuleType='$CHAIN' AND SourceIP='$SOURCEIP' AND Protocol='$PROTOCOL'"
		$(RunSQL "$sql")
		if ! [ "$R" ]; then
			echo "1"
		else
			sql="UPDATE Firewall SET Offline='0' WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL'";
			$(RunSQL "$sql")
		fi
		Q="SELECT IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType='$CHAIN' AND Protocol='$PROTOCOL' AND Place='$PLACE' AND Disabled='0'"
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
            DestIP=$(Field 8 "$Port")
            DestPort=$(Field 9 "$Port")
            RPolicy=$(Field 10 "$Port")
            Description=$(Field 11 "$Port")
            #echo IPTables "$IPVersion" "insert" "$PLACE" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
            IPTables "$IPVersion" "insert" "$PLACE" "" "$CHAIN" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
        done		
	else
		$IP6TABLES -D $CHAIN $PLACE
	fi
  exit 0
  fi
  if [[ "$ACTION" == "Blocklist" ]]; then
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
  exit 0
  fi

  echo "add/insert rule"
  echo "$HOST" "$TABLE" "$ACTION" "$CHAIN" "$PLACE" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "$RPOLICY" "$DESCRIPTION"
  if [[ "$CHAIN" = "AUTO" ]]; then
	re='^[0-9]+$'
	if ! [[ $PLACE =~ $re ]]; then
		IPVersion="$(echo $PROTOCOL | cut -d- -f2)"
		highest="SELECT MAX( Place ) AS max FROM Firewall WHERE RuleType='$CHAIN' AND Protocol LIKE '%-$IPVersion'"
		R=$(RunSQL "$highest")
		for max in $R; do
			max=$(Field 1 "$max")
		done
		PLACE=$[max+1]
	fi

	if [[ "$ExtIP" = "dhcp" ]]; then
		ExtIP=$(ip addr show $ExtIf | awk '/inet / {print $2}')
		ExtIP="$(echo $ExtIP | cut -d/ -f1)"
	fi
	echo "$DESTINATIONIP $ExtIP $DESTINATIONPORT"
	if [[ "$DESTINATIONIP" = "$ExtIP" ]] || [[ "$DESTINATIONIP" = "NULL" ]]; then
		if [[ "$SOURCEPORT" = "0" ]] || [[ "$SOURCEPORT" = "NULL" ]]; then
			echo "input or other chain rule"
			AddRuleFromExt "$ACTION" "$PLACE" "filter" "INPUT" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "$RPOLICY" "$DESCRIPTION"
		else
			echo "PREROUTING REDIRECT (1)"
			AddRuleFromExt "$ACTION" "$PLACE" "nat" "PREROUTING" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "REDIRECT" "$DESCRIPTION"
			AddRuleFromExt "$ACTION" "$PLACE" "filter" "FORWARD" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "ACCEPT" "$DESCRIPTION"
		fi
	elif [[ "$DESTINATIONIP" = "127.0.0.1" ]] || [[ "$DESTINATIONIP" = "$IntIP" ]]; then
		echo "PREROUTING REDIRECT (2)"
		AddRuleFromExt "$ACTION" "$PLACE" "nat" "PREROUTING" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "REDIRECT" "$DESCRIPTION"
		AddRuleFromExt "$ACTION" "$PLACE" "filter" "FORWARD" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "ACCEPT" "$DESCRIPTION"
	else
		echo "PREROUTING DNAT"
		AddRuleFromExt "$ACTION" "$PLACE" "nat" "PREROUTING" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "DNAT" "$DESCRIPTION"
		AddRuleFromExt "$ACTION" "$PLACE" "filter" "FORWARD" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "ACCEPT" "$DESCRIPTION"
	fi
else 
	AddRuleFromExt "$ACTION" "$PLACE" "$TABLE" "$CHAIN" "$INIF" "$OUTIF" "$MATCHNAME" "$PROTOCOL" "$SOURCEIP" "$SOURCEPORT" "$SOURCEPORTEND" "$DESTINATIONIP" "$DESTINATIONPORT" "$DESTINATIONPORTEND" "$RPOLICY" "$DESCRIPTION"
fi
else	

#####
# Main working spaghetti
#####
trap 'Unlock "Firewall" "Firewall"' EXIT
WaitLock "Firewall" "Firewall"

if [[ -n "$IntIP" ]]; then
	#TODO: use 4 byte netmask in these calculations
	IntNet="$(echo "$IntIP" | cut -d. -f-3).0"
	IntBitmask=24

	Intv6Net="$(echo "$Intv6IP" | cut -d":" -f-4):"
fi

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

#if [[ "$ACTION" = "RESET" ]]; then
	
	#set rules to the db what are not set and need to work for the network,
#set the rules only as advanced firewall is not set as advanced firewall is set user can use the default or set his own rules.
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
		        Q="INSERT INTO Firewall (Place, Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'ip-ipv4', 'input', 'ACCEPT')"
	                $(RunSQL "$Q")
	      	  Q="INSERT INTO Firewall (Place, Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'ip-ipv4', 'output', 'ACCEPT')"
	                $(RunSQL "$Q")
	      	  Q="INSERT INTO Firewall (Place, Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'lo', 'ip-ipv4', 'forward', 'ACCEPT')"
	                $(RunSQL "$Q")
	        fi
	
	        # Set stateful inspection
	        Q="SELECT Matchname, Protocol FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv4' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv4', 'output', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	        fi
	
	        #DHCP/BOOTP
	        Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv4' AND SourcePort='67' AND RuleType='input' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                Q="INSERT INTO Firewall (Place, IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('3', '$IntIf', 'udp-ipv4', '67', 'input', 'ACCEPT', 'Allow DHCP')"
	                $(RunSQL "$Q")
		                Q="INSERT INTO Firewall (Place, ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('3', '$IntIf', 'udp-ipv4', '68', 'output', 'ACCEPT', 'Allow DHCP')"
	                $(RunSQL "$Q")
	                if [[ "$ExtIP" == "DHCP" ]] || [[ "$ExtIP" == "dhcp" ]]; then
	                        Q="INSERT INTO Firewall (Place, IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('4', '$ExtIf', 'udp-ipv4', '68', 'input', 'ACCEPT', 'Allow DHCP')"
	                        $(RunSQL "$Q")
	                        Q="INSERT INTO Firewall (Place, ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('4', '$ExtIf', 'udp-ipv4', '67', 'output', 'ACCEPT', 'Allow DHCP')"
	                        $(RunSQL "$Q")
	                fi
	        fi
	
	        #Local Network
	        Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$IntNet/$IntBitmask' AND RuleType='input' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                Q="INSERT INTO Firewall (Place, IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('5', '$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'input', 'ACCEPT', 'Allow Local Network')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('4', '$IntIf', '$IntNet/$IntBitmask', 'ip-ipv4', 'forward', 'ACCEPT', 'Allow Local Network')"
	                $(RunSQL "$Q")
	        fi
			
			#Local network postrouting
			Q="SELECT ExtIF, SourceIP, Ruletype FROM Firewall WHERE ExtIF='$ExtIf' AND SourceIP='$IntNet/$IntBitmask' AND DestinationIP='!$IntNet/$IntBitmask' AND RuleType='nat-POSTROUTING' AND RPOLICY='MASQUERADE' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
			if ! [ "$R" ]; then
				echo "insert postrouting"
				Q="INSERT INTO Firewall (Place, ExtIF, Protocol, SourceIP, DestinationIP, RuleType, RPolicy, Disabled, Offline, Description, Bantime) VALUES ('1','$ExtIf','ip-ipv4', '$IntNet/$IntBitmask', '!$IntNet/$IntBitmask', 'nat-POSTROUTING', 'MASQUERADE', '0', '0', 'Masquerade Local network', '0')"
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
		        Q="INSERT INTO Firewall (Place, Description, IntIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'ip-ipv6', 'input', 'ACCEPT')"
	                $(RunSQL "$Q")
		        Q="INSERT INTO Firewall (Place, Description, ExtIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'ip-ipv6', 'output', 'ACCEPT')"
	                $(RunSQL "$Q")
		        Q="INSERT INTO Firewall (Place, Description, IntIf, ExtIf, Protocol, RuleType, RPolicy) VALUES ('1', 'Allow Loopback', 'lo', 'lo', 'ip-ipv6', 'forward', 'ACCEPT')"
	                $(RunSQL "$Q")
	        fi
	
	        # Set stateful inspection
	        Q="SELECT Matchname FROM Firewall WHERE Matchname='-m state --state ESTABLISHED,RELATED' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('2', '-m state --state ESTABLISHED,RELATED', 'ip-ipv6', 'output', 'ACCEPT', 'Allow Established')"
	                $(RunSQL "$Q")
	        fi

	        #DHCP/BOOTP
	                Q="SELECT Matchname, IntIF, Protocol, SourcePort, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND Protocol='udp-ipv6' AND SourcePort='546' AND RuleType='input' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                Q="INSERT INTO Firewall (Place, IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('3', '$IntIf', 'udp-ipv6', '546', 'input', 'ACCEPT', 'Allow DHCP')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('3', '$IntIf', 'udp-ipv6', '547', 'output', 'ACCEPT', 'Allow DHCP')"
	                $(RunSQL "$Q")
	                if [[ "$Extv6IP" == "DHCP" ]] || [[ "$Extv6IP" == "dhcp" ]]; then
	                        Q="INSERT INTO Firewall (Place, IntIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('4', '$ExtIf', 'udp-ipv6', '547', 'input', 'ACCEPT', 'Allow DHCP')"
	                        $(RunSQL "$Q")
	                        Q="INSERT INTO Firewall (Place, ExtIF, Protocol, SourcePort, RuleType, RPolicy, Description) VALUES ('4', '$ExtIf', 'udp-ipv6', '546', 'output', 'ACCEPT', 'Allow DHCP')"
	                        $(RunSQL "$Q")
	                fi
	        fi

	        #Local Network
	        Q="SELECT Matchname, IntIF, SourceIP, Ruletype FROM Firewall WHERE IntIf='$IntIf' AND SourceIP='$Intv6Net/$Intv6Netmask' AND RuleType='input' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                [[ ! "$Intv6Net" == "disabled::/" ]] && Q="INSERT INTO Firewall (Place, IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('5', '$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'input', 'ACCEPT', 'Allow Local Network')" && $(RunSQL "$Q")
	                [[ ! "$Intv6Net" == "disabled::/" ]] && Q="INSERT INTO Firewall (Place, IntIF, SourceIP, Protocol, RuleType, RPolicy, Description) VALUES ('3', '$IntIf', '$Intv6Net/$Intv6Netmask', 'ip-ipv6', 'forward', 'ACCEPT', 'Allow Local Network')" && $(RunSQL "$Q")
	        fi
	
	        # Allow certain ICMP traffic
	        Q="SELECT Description FROM Firewall WHERE Description='Allow PING requests' AND Protocol LIKE '%-ipv6' ORDER BY PK_Firewall"
	        R=$(RunSQL "$Q")
	        if ! [ "$R" ]; then
	                echo "ipv6 icmp"
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('6', '-m icmp6  --icmpv6-type 128', 'icmp-ipv6', 'input', 'ACCEPT', 'Allow PING requests')"
	                $(RunSQL "$Q")
	                echo $Q
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('7','-m icmp6 --icmpv6-type 129', 'icmp-ipv6', 'input', 'ACCEPT', 'Allow PING replys')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('8', '-m icmp6 --icmpv6-type 133 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('9','-m icmp6 --icmpv6-type 134 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Router solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('10','-m icmp6 --icmpv6-type 135 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('11', '-m icmp6 --icmpv6-type 136 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Neighbor advertisement
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('12', '-m icmp6 --icmpv6-type 137 -m hl --hl-eq 255', 'icmp-ipv6', 'input', 'ACCEPT', '')" #Allow Redirect message
	                $(RunSQL "$Q")
	
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('5', '-m icmp6 --icmpv6-type 128', 'icmp-ipv6', 'output', 'ACCEPT', 'Allow PING requests')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('6', '-m icmp6 --icmpv6-type 129', 'icmp-ipv6', 'output', 'ACCEPT', 'Allow PING replys')"
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('7', '-m icmp6 --icmpv6-type 133 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('8', '-m icmp6 --icmpv6-type 134 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Router solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('9', '-m icmp6 --icmpv6-type 135 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor solicitation
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('10', '-m icmp6 --icmpv6-type 136 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Neighbor advertisement
	                $(RunSQL "$Q")
	                Q="INSERT INTO Firewall (Place, Matchname, Protocol, RuleType, RPolicy, Description) VALUES ('11', '-m icmp6 --icmpv6-type 137 -m hl --hl-eq 255', 'icmp-ipv6', 'output', 'ACCEPT', '')" #Allow Redirect message
	                $(RunSQL "$Q")
	        fi
#fi

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
		
	echo "1"
#If DisableIPv4Firewall = true then add postrouting masquarading to let internal client's connect to the internet.
	if [[ "$DisableIPv4Firewall" == 1 ]]; then
		echo "2"
		echo $IPTABLES -t nat -A POSTROUTING -o $ExtIf -s $IntNet/$IntBitmask ! -d $IntNet/$IntBitmask -j MASQUERADE
		$IPTABLES -t nat -A POSTROUTING -o $ExtIf -s $IntNet/$IntBitmask ! -d $IntNet/$IntBitmask -j MASQUERADE
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
                                DestIP=$(Field 8 "$Port")
                                DestPort=$(Field 9 "$Port")
                                RPolicy=$(Field 10 "$Port")
                                Description=$(Field 11 "$Port")

                                #echo IPTables "$IPVersion" "-A INPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                                IPTables "$IPVersion" "add" "0" "" "INPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
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
                                DestIP=$(Field 8 "$Port")
                                DestPort=$(Field 9 "$Port")
                                RPolicy=$(Field 10 "$Port")
                                Description=$(Field 11 "$Port")

                                #echo IPTables "$IPVersion" "-A FORWARD" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                                IPTables "$IPVersion" "add" "0" "" "FORWARD" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                        done
done

# Configuring all port forwards (NAT)

echo "Setting up Port_forwarding (NAT)"
ForwardTypeArr=('port_forward (NAT)' 'PREROUTING' 'POSTROUTING' 'OUTPUT' 'MASQUERADE' 'REDIRECT')
for ForwardType in "${ForwardTypeArr[@]}"; do
        echo $ForwardType
        if [[ $ForwardType == 'port_forward (NAT)' ]]; then
                dbForwardType='port_forward (NAT)'
        elif [[ $ForwardType == 'REDIRECT'  ]]; then
                dbForwardType='REDIRECT';
        else
                dbForwardType='%-'$ForwardType
        fi
        Q="SELECT RuleType,IntIf,ExtIf,Matchname,Protocol,SourceIP,SourcePort,SourcePortEnd,DestinationIP,DestinationPort,RPolicy,Description FROM Firewall WHERE RuleType LIKE '$dbForwardType' AND Protocol LIKE '%ipv4' AND Disabled='0' AND Offline='0' ORDER BY Place, PK_Firewall"
        R=$(RunSQL "$Q")
                        for Port in $R; do
                                IPVersion="ipv4"
                                RuleType=$ForwardType
                                if [[ "$RuleType" == "port_forward (NAT)" ]]; then
                                        RuleType="PREROUTING"
                                fi
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
								Table="nat"
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

                                #change RPolicy to what the rule needs
                                if [[ "$RuleType" == "POSTROUTING" ]] && [[ "$RPolicy" != "MASQUERADE"  ]]; then
                                        RPolicy="SNAT $parmDest"
                                elif [[ "$RuleType" == "PREROUTING" ]]; then
                                        RPolicy="DNAT $parmDest"
                                elif [[ "$RuleType" == "OUTPUT" ]]; then
                                        RPolicy="REDIRECT $parmDest"
                                fi

                                if [[ "$Ruletype" == "REDIRECT" ]]; then
                                        RuleType="PREROUTING"
                                        RPolicy="REDIRECT $parmDest"
                                fi

                                #echo IPTables "$IPVersion" "-t nat -A $RuleType" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                                         IPTables "$IPVersion" "add" "0" "$Table" "$RuleType" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
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
                                DestIP=$(Field 8 "$Port")
                                DestPort=$(Field 9 "$Port")
                                RPolicy=$(Field 10 "$Port")
                                Description=$(Field 11 "$Port")

                                #echo IPTables "$IPVersion" "-A OUTPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
                                IPTables "$IPVersion" "add" "0" "" "OUTPUT" "$IntIf" "$ExtIf" "$Matchname" "$Protocol" "$SrcIP" "$SrcPort1:$SrcPort2" "$DestIP" "$DestPort" "$RPolicy" "$Description"
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
		 
fi
