#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

ruletype="$2"
name="$3"
Check()
{
	ruletype=$1
	name=$2
	if [[ $ruletype == "chain" ]]; then
                        Q="SELECT * FROM Firewall WHERE Matchname='$name'"
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

                        Q="SELECT * FROM Firewall WHERE Place='1' AND Protocol='$protocol-ipv4' AND SourceIP='$ip' AND DestinationPort='$port' AND RuleType='fail2ban-$name' AND RPolicy='DROP' AND Description='fail2ban-$name'"
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
			Q="DELETE FROM Firewall WHERE Place='2' AND Protocol='ip-ipv4' AND RuleType='fail2ban-$name' AND RPolicy='RETURN'"
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
			Q="INSERT INTO Firewall (Matchname, Protocol) VALUES ('fail2ban-$name','chain-ipv4')"
                        $(RunSQL "$Q")
			Q="INSERT INTO Firewall (Place, Protocol, RuleType, RPolicy) VALUES ('2', 'ip-ipv4','fail2ban-$name','RETURN')"
                        $(RunSQL "$Q")
			Q="INSERT INTO Firewall (Place, Protocol, RuleType, RPolicy) VALUES ('0', 'ip-ipv4','input','fail2ban-$name')"
			$(RunSQL "$Q")
		elif [[ $ruletype == "rule" ]]; then
			protocol="$4"
			port="$5"
			ip="$6"
			bantime="$7"
	
			Check "$ruletype" "$name" "$protocol" "$port" "$ip"
			if [[ $returnCheck == "false" ]]; then
			
				Q="INSERT INTO Firewall (Place, Protocol, SourceIP,DestinationPort, RuleType, RPolicy, Description, Bantime) VALUES ('1', '$protocol-ipv4', '$ip','$port', 'fail2ban-$name', 'DROP', 'fail2ban-$name', '$bantime')"
	        		$(RunSQL "$Q")
			fi

		fi
	;;
 	
	*)
		echo "usage: $0 {check|delete|add}"
		exit 1
	;;
esac
exit 0
