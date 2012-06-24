#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

# Update bind zone files
DD_NetworkInterfaces=32
ConfGet "PK_Installation"

R=$(RunSQL "SELECT Device.* FROM Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=7 AND Device.FK_Installation=$PK_Installation")
CoreID=$(Field 1 "$R")
if [[ "$CoreID" == "" ]] ;then
	# No database entries yet, probably still installing
	exit 0
fi

# Read the new core internal ip address
. /usr/pluto/bin/Network_Parameters.sh
Internal_IP=$IntIP
echo "Internal IPv4: $Internal_IP"
Address_1=$(echo "$Internal_IP" | cut -d'.' -f1)
Address_2=$(echo "$Internal_IP" | cut -d'.' -f2)
Address_3=$(echo "$Internal_IP" | cut -d'.' -f3)
Address_4=$(echo "$Internal_IP" | cut -d'.' -f4)
Short_IP="$Address_1.$Address_2.$Address_3"
Reverse_IP="$Address_3.$Address_2.$Address_1"

# If IPv6 is enabled, see if we have a global IPv6 address on internal NIC
if [[ "$IPv6Active" == "on" || "$Extv6IP" != "disabled" ]]; then
	Internal_IPv6=$(ip -f inet6 addr show $IntIf| awk '/scope global/ {print $2}' | cut -d'/' -f1)
	echo "Internal IPv6: $Internal_IPv6"
fi

# Add dynamic zone files for bind
# Bind doesnt like it when its zone files are altered while it is running
service bind9 stop

if [[ -e /var/cache/bind/db.linuxmce.local ]] ;then
	# Determine old IPv4 address
	Old_IP=$(grep ^$Hostname /var/cache/bind/db.linuxmce.local | grep -w A | awk '{print $3}')	
	if [[ "$Internal_IP" != "$Old_IP" ]] ;then
		Address_1=$(echo "$Internal_IP" | cut -d'.' -f1)
		Address_2=$(echo "$Internal_IP" | cut -d'.' -f2)
		Address_3=$(echo "$Internal_IP" | cut -d'.' -f3)
		Address_4=$(echo "$Internal_IP" | cut -d'.' -f4)
		Old_Short_IP="$Address_1.$Address_2.$Address_3"
		Old_Reverse_IP="$Address_3.$Address_2.$Address_1"
	fi

	# Determine old IPv6 address
	Old_IPv6=$(grep ^$Hostname /var/cache/bind/db.linuxmce.local | grep -w AAAA | awk '{print $3}')
	
	# If IPv6 is enabled, update record if necessary
	if [[ "$IPv6Active" == "on" || "$Extv6IP" != "disabled" ]]; then
		# TODO: Nothing yet
		:
	else
		# If IPv6 is disabled, remove eventual existing AAAA record
		if [[ "$Old_IPv6" != "" ]]; then
			sed -i "/$Old_IPv6/d" /var/cache/bind/db.linuxmce.local
			Old_IPv6=""
		fi
	fi
	
fi

# If Zone cache file exists, modify it if necessary
if [[ -e /var/cache/bind/db.linuxmce.local && -e /var/cache/bind/db.linuxmce.rev ]] ;then
	
	# Zone files exist. If IP4 has changed; update zone files
	if [[ $Old_IP != $Internal_IP ]]; then
		# Forward
		sed -i "s,$Old_Short_IP,$Short_IP,g" /var/cache/bind/db.linuxmce.local
		# Reverse
		sed -i "s,$Old_Reverse_IP,$Reverse_IP,g" /var/cache/bind/db.linuxmce.rev
	fi
	
	# If IPv6 is activated, check if AAAA record is still correct
	if [[ "$IPv6Active" == "on" || "$Extv6IP" != "disabled" ]]; then
		# If IPv6 has changed or has been added, modify zone files
		if [[ $Old_IPv6 != $Internal_IPv6 ]]; then
			# forward
			# if AAAA record already there, modify it
			if [[ "$Old_IPv6" != "" ]]; then
				sed -i "s,$Old_IPv6,$Internal_IPv6,g" /var/cache/bind/db.linuxmce.local
			# else insert it
			else
				echo "Inserting after $Internal_IP"
				sed -i "/\<$Internal_IP\>/a\\$Hostname               AAAA    $Internal_IPv6" /var/cache/bind/db.linuxmce.local
			fi
			# TODO: reverse
		fi	
	fi
else
	# Zone files do not exist; create new ones

# Use current date as the serial
Serial=$( date +%Y%m%d00 )

#Forward
  echo "\
\$ORIGIN .
\$TTL 259200     ; 3 days
$Domainname          IN SOA  $Hostname.$Domainname. postmaster.$Domainname. (
                                $Serial  ; serial
                                28800         ; refresh (8 hours)
                                7200          ; retry (2 hours)
                                2419200       ; expire (4 weeks)
                                86400         ; minimum (1 day)
                                )
                        NS      $Hostname.$Domainname.
\$ORIGIN $Domainname.
$Hostname               A       $Internal_IP" > /var/cache/bind/db.linuxmce.local

# If IPv6 is activated, insert AAAA record
if [[ "$IPv6Active" == "on" || "$Extv6IP" != "disabled" ]]; then
	echo "$Hostname               AAAA    $Internal_IPv6" >> /var/cache/bind/db.linuxmce.local
fi
# Reverse
echo "\
\$ORIGIN .
\$TTL 259200     ; 3 days
$Reverse_IP.in-addr.arpa IN SOA  $Domainname. postmaster.$Domainname. (
                                $Serial  ; serial
                                28800         ; refresh (8 hours)
                                7200          ; retry (2 hours)
                                2419200       ; expire (4 weeks)
                                86400         ; minimum (1 day)
                                )
                        NS      $Hostname.$Domainname.
                        PTR     $Domainname.
\$ORIGIN $Reverse_IP.in-addr.arpa.
1                       PTR     $Hostname.$Domainname." > /var/cache/bind/db.linuxmce.rev
fi

### Check for and update named.conf files
# Listen only on localhost and internal ip addresses
sed -i "/listen-on /d" /etc/bind/named.conf.options
sed -i "/auth-nxdomain/ a\
listen-on { 127.0.0.1;$IntIP;};" /etc/bind/named.conf.options

# Only try IPv6 connecions if IPv6 is enabled on core
sed -i "/OPTIONS/d" /etc/default/bind9
if [[ "$IPv6Active" == "on" || "$Extv6IP" != "disabled" ]]; then
	echo 'OPTIONS="-u bind"' >> /etc/default/bind9
else
	echo 'OPTIONS="-4 -u bind"' >> /etc/default/bind9
fi

if [[ $( grep named.conf.linuxmce /etc/bind/named.conf ) == "" ]] ;then
	echo 'include "/etc/bind/named.conf.linuxmce";' >> /etc/bind/named.conf
fi
cp /usr/pluto/templates/named.conf.linuxmce.tmpl /etc/bind/named.conf.linuxmce
sed -i "s,%DYNAMIC_REVERSE_RANGE%,$Reverse_IP,g" /etc/bind/named.conf.linuxmce
sed -i "s,%DOMAINNAME%,$Domainname,g" /etc/bind/named.conf.linuxmce

# Setting right permissions after our changes
#chown bind: /var/cache/bind/db.*
chown -R bind: /var/cache/bind
chown bind:dhcpd /etc/bind/rndc.key
chmod 664 /etc/bind/rndc.key

# Apparmor prevents dhcpd from reading bind conf files
if [[ -e /etc/apparmor.d/usr.sbin.dhcpd3 && $( grep '/etc/bind' /etc/apparmor.d/usr.sbin.dhcpd3 ) == "" ]] ;then
	sed -i "s,\},\n  # Let dhcpd read bind's config files\n  /etc/bind/\*\* r\,\n\},g" /etc/apparmor.d/usr.sbin.dhcpd3
	service apparmor restart
	service dhcp3-server restart
fi
rm -f /var/cache/bind/*.jnl
service bind9 start 
