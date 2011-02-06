#!/bin/bash -x

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
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData where FK_Device=$CoreID AND FK_DeviceData=$DD_NetworkInterfaces")
Internal_IP=$(Field 1 "$R")
Internal_IP=$(echo $Internal_IP | cut -d',' -f3)
Address_1=$(echo "$Internal_IP" | cut -d'.' -f1)
Address_2=$(echo "$Internal_IP" | cut -d'.' -f2)
Address_3=$(echo "$Internal_IP" | cut -d'.' -f3)
Address_4=$(echo "$Internal_IP" | cut -d'.' -f4)
Short_IP="$Address_1.$Address_2.$Address_3"
Reverse_IP="$Address_3.$Address_2.$Address_1"



# Add dynamic zone files for bind
# Bind doesnt like it when its zone files are altered while it is running
invoke-rc.d bind9 stop

if [[ -e /var/cache/bind/db.linuxmce.local ]] ;then
	# Determine old ip address
	Old_IP=$(grep ^dcerouter /var/cache/bind/db.linuxmce.local | awk '{print $3}')
	if [[ "$Internal_IP" != "$Old_IP" ]] ;then
	Address_1=$(echo "$Internal_IP" | cut -d'.' -f1)
	Address_2=$(echo "$Internal_IP" | cut -d'.' -f2)
	Address_3=$(echo "$Internal_IP" | cut -d'.' -f3)
	Address_4=$(echo "$Internal_IP" | cut -d'.' -f4)
	Old_Short_IP="$Address_1.$Address_2.$Address_3"
	Old_Reverse_IP="$Address_3.$Address_2.$Address_1"
	fi
fi

if [[ $Old_IP != $Internal_IP &&  -e /var/cache/bind/db.linuxmce.local && -e /var/cache/bind/db.linuxmce.rev ]] ;then
	# Zone files exist and IP has changed; update zone files

	# Forward
	sed -i "s,$Old_Short_IP,$Short_IP,g" /var/cache/bind/db.linuxmce.local
	# Reverse
	sed -i "s,$Old_Reverse_IP,$Reverse_IP,g" /var/cache/bind/db.linuxmce.rev
else
	# Zone files do not exist; create new ones

# Use current date as the serial
Serial=$( date +%Y%m%d00 )

#Forward
  echo "\
\$ORIGIN .
\$TTL 259200     ; 3 days
linuxmce.local          IN SOA  dcerouter.linuxmce.local. postmaster.linuxmce.local. (
                                $Serial  ; serial
                                28800         ; refresh (8 hours)
                                7200          ; retry (2 hours)
                                2419200       ; expire (4 weeks)
                                86400         ; minimum (1 day)
                                )
                        NS      dcerouter.linuxmce.local.
\$ORIGIN linuxmce.local.
dcerouter               A       $Internal_IP" > /var/cache/bind/db.linuxmce.local

# Reverse
echo "\
\$ORIGIN .
\$TTL 259200     ; 3 days
$Reverse_IP.in-addr.arpa IN SOA  linuxmce.local. postmaster.linuxmce.local. (
                                $Serial  ; serial
                                28800         ; refresh (8 hours)
                                7200          ; retry (2 hours)
                                2419200       ; expire (4 weeks)
                                86400         ; minimum (1 day)
                                )
                        NS      dcerouter.linuxmce.local.
                        PTR     linuxmce.local.
\$ORIGIN $Reverse_IP.in-addr.arpa.
1                       PTR     dcerouter.linuxmce.local." > /var/cache/bind/db.linuxmce.rev
fi

# Check for and update named.conf files
if [[ $( grep named.conf.linuxmce /etc/bind/named.conf ) == "" ]] ;then
	echo 'include "/etc/bind/named.conf.linuxmce";' >> /etc/bind/named.conf
fi
cp /usr/pluto/templates/named.conf.linuxmce.tmpl /etc/bind/named.conf.linuxmce
sed -i "s,%DYNAMIC_REVERSE_RANGE%,$Reverse_IP,g" /etc/bind/named.conf.linuxmce

chown bind: /var/cache/bind/db.*
chown bind:dhcpd /etc/bind/rndc.key
chmod 664 /etc/bind/rndc.key

# Apparmor prevents dhcpd from reading bind conf files
if [[ -e /etc/apparmor.d/usr.sbin.dhcpd3 && $( grep '/etc/bind' /etc/apparmor.d/usr.sbin.dhcpd3 ) == "" ]] ;then
	sed -i "s,\},\n  # Let dhcpd read bind's config files\n  /etc/bind/\*\* r\,\n\},g" /etc/apparmor.d/usr.sbin.dhcpd3
	invoke-rc.d apparmor restart
	invoke-rc.d dhcp3-server restart
fi
invoke-rc.d bind9 start 
