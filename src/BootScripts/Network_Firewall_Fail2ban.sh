#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

ruletype="$2"
name="$3"
Check()
{
	ruletype=$1
	name=$2
	if [[ $ruletype == "chain" ]]; then
                        Q="SELECT * FROM Firewall WHERE Matchname='fail2ban-$name'"
			R=$(RunSQL "$Q")
                        if ! [ "$R" ]; then
                                returnCheck='false'
                        else
                                returnCheck='true'
                        fi
                elif [[ $ruletype == "rule" ]]; then
                        protocol="$3"
                        port="$4"
                        ip="$5"

                        Q="SELECT * FROM Firewall WHERE Place='1' AND Protocol='$protocol-ipv4' AND Matchname='-m multiport' AND SourceIP='$ip' AND DestinationPort='$port' AND RuleType='fail2ban-$name' AND RPolicy='DROP' AND Description='fail2ban-$name'"
                        R=$(RunSQL "$Q")
                        if ! [ "$R" ]; then
				returnCheck='false'
			else
				returnCheck='true'
                        fi
                fi
}

case $1 in
	check)
		 protocol="$4"
                 port="$5"
                 ip="$6"

		Check "$ruletype" "$name" "$protocol" "$port" "$ip"
		echo $returnCheck
	;;
	delete)
		if [[ $ruletype == "chain" ]]; then
			Q="DELETE FROM Firewall WHERE Place='0' AND Protocol='ip-ipv4' AND RuleType='input' AND RPolicy='fail2ban-$name'"
                        $(RunSQL "$Q")
			Q="DELETE FROM Firewall WHERE Place='3' AND Protocol='ip-ipv4' AND RuleType='fail2ban-$name' AND RPolicy='RETURN'"
			$(RunSQL "$Q")
			Q="DELETE FROM Firewall WHERE Matchname='fail2ban-$name' AND Protocol='chain-ipv4'"
			$(RunSQL "$Q")
		elif [[ $ruletype == "rule" ]]; then
			protocol="$4"
			port="$5"
			ip="$6"

	        	Q="DELETE FROM Firewall WHERE Place='1' AND Protocol='$protocol-ipv4' AND SourceIP='$ip' AND DestinationPort='$port' AND RuleType='fail2ban-$name' AND RPolicy='DROP' AND Description='fail2ban-$name'"
	        	$(RunSQL "$Q")

		fi
	;;

	add)
		if [[ $ruletype == "chain" ]];  then
			Check "chain" "$name"
			echo "$returnCheck"
			if [[ $returnCheck == "false" ]]; then
				Q="INSERT INTO Firewall (Matchname, Protocol) VALUES ('fail2ban-$name','chain-ipv4')"
                        	$(RunSQL "$Q")
				Q="INSERT INTO Firewall (Place, Protocol, RuleType, RPolicy) VALUES ('3', 'ip-ipv4','fail2ban-$name','RETURN')"
                        	$(RunSQL "$Q")
				Q="INSERT INTO Firewall (Place, Protocol, RuleType, RPolicy) VALUES ('0', 'ip-ipv4','input','fail2ban-$name')"
				$(RunSQL "$Q")
			fi
		elif [[ $ruletype == "rule" ]]; then
			protocol="$4"
			port="$5"
			ip="$6"
			bantime="$7"
	
			Check "$ruletype" "$name" "$protocol" "$port" "$ip"
			if [[ $returnCheck == "false" ]]; then
			
				Q="INSERT INTO Firewall (Place, Protocol, Matchname, SourceIP,DestinationPort, RuleType, RPolicy, Description, Bantime) VALUES ('1', '$protocol-ipv4', '-m multiport', '$ip','$port', 'fail2ban-$name', 'DROP', 'fail2ban-$name', '$bantime')"
	        		$(RunSQL "$Q")
			fi

		fi
	;;
 
	removeold)
	#this part is called by cron (Hourly) to cleanup old rules what fail2ban for whatever reasen was forgortten out of our DB.	
		dateDiff() {
    			d1=$(date -d "$1" +%s)
    			d2=$(date -d "$2" +%s)
   			 echo $(( (d1 - d2)  )) 
		}

		Q="SELECT Bantime, psc_mod,SourceIP,RuleType  FROM Firewall WHERE Bantime >'0'"
		R=$(RunSQL "$Q")
		for oldrule in $R; do
			datetimenow=$(date +"%F %T")
			bantime=$(Field 1 "$oldrule")
			datetimedb=$(Field 2 "$oldrule")
			SourceIP=$(Field 3 "$oldrule")
			RuleType=$(Field 4 "$oldrule")
			dateexpire=$(date --date "$datetimedb $bantime seconds " +'%F %T')
			diff=$(dateDiff "$datetimenow" "$dateexpire" )

			#remove the rule after the bantime + 30 seconds is reacht:
			if [[ $diff -gt $(($bantime+30)) ]]; then 	
				Q="DELETE FROM Firewall WHERE Bantime='$bantime' AND SourceIP='$SourceIP' AND RuleType='$RuleType'"
				$(RunSQL "$Q")
			fi
		done
	;;
	
	*)
		echo "usage: $0 {check|delete|add|removeold}"
		exit 1
	;;
esac
exit 0
