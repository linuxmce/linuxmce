#!/bin/bash

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

