#!/bin/bash

#Do something for webfilter or proxy perhaps
#Set default when both are not active or set.
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

