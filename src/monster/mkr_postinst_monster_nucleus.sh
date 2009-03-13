#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

#Q="DELETE FROM Device_DeviceData WHERE FK_Device=1 AND FK_DeviceData=32"
#RunSQL "$Q"
#Q="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(1,32,'eth1,192.168.80.98,255.255.255.0,192.168.80.99,192.168.80.99|eth1:1,192.168.80.1,255.255.255.0')"
#RunSQL "$Q"

Q="GRANT ALL PRIVILEGES ON * TO 'root'@'192.168.%.%'"
RunSQL "$Q"

#sed 's/X -ac -allowMouseOpenFail/X -dpi 75 -ac -allowMouseOpenFail/g' /usr/pluto/bin/Start_X.sh > /tmp/Start_X.sh
#mv /tmp/Start_X.sh /usr/pluto/bin/Start_X.sh
#chmod +x /usr/pluto/bin/Start_X.sh

#sed -i 's/#log..= \/var\/log\/mysql\/mysql.log/log = \/var\/log\/mysql\/mysql.log/g' /etc/mysql/my.cnf
#sed -i 's/#log.= \/var\/log\/mysql\/mysql.log/log = \/var\/log\/mysql\/mysql.log/g' /etc/mysql/my.cnf
#sed -i 's/--daemon --logfile/--daemon --verbose all --logfile/g' /etc/init.d/mythtv-backend

set +e  # we want the exit code
#grep 'deb http://www.yttron.as.ro/ realextra main' /etc/apt/sources.list > /dev/null
#if [[ $? -ne 0 ]] ; then
#	echo "deb http://www.yttron.as.ro/ realextra main" >> /etc/apt/sources.list
#fi
#set -e  # change it back
#echo "apt-get update; apt-get install -y libdvdcss id-my-disc; rm /etc/rc2.d/S20dvd" > /etc/rc2.d/S20dvd
#chmod 777 /etc/rc2.d/S20dvd
#chmod +x /etc/rc2.d/S20dvd

echo "## Please wait, installing cisco firmware"
#wget -P /tmp http://www.plutohome.com/cisco7970firmware_7.0.2-1_i386.deb || :

#rm -rf /tmp/cisco
#mkdir -p /tmp/cisco

#dpkg -x /tmp/cisco7970firmware_7.0.2-1_i386.deb /tmp/cisco
#pushd /tmp/cisco
#	cp -Ruf * /
#popd

#rm -f /tmp/cisco7970firmware_7.0.2-1_i386.deb || :
#rm -rf /tmp/cisco

# monster web skin
sed -i "s/_SESSION\\['skin'\\]=.;/_SESSION\\['skin'\\]=1;/" /var/www/lmce-admin/include/config/config.inc.php

# Append the vendor label in pluto.conf
Vendor="Monster"
ConfSet "Vendor" "$Vendor"
