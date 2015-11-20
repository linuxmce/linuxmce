#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

if [ ! -e /etc/mythtv/mysql.txt ]; then
	echo "Mythtv doesn't seem to be installed here";
	exit 0;
fi;

FK_DeviceTemplate=$(RunSQL "SELECT FK_DeviceTemplate from Device WHERE PK_Device=$PK_Device")
if [[ "$FK_DeviceTemplate" == "28" ]] ; then # If is 'Generic PC as MD' 
	sed -i "s/^DBHostName.*/DBHostName=$MySqlHost/g" /etc/mythtv/mysql.txt
	sed -i "s/^DBUserName.*/DBUserName=$MySqlUser/g" /etc/mythtv/mysql.txt
	sed -i "s/^DBPassword.*/DBPassword=$MySqlPassword/g" /etc/mythtv/mysql.txt
fi

Q="select IK_DeviceData from Device_DeviceData JOIN Device ON FK_Device=PK_Device where FK_DeviceData=206 AND FK_DeviceTemplate=36"
AutoConf=$(RunSQL "$Q")

if [[ "$AutoConf" == "1" ]]; then 
	echo "Dont Auto Configure is set"
	exit 0
fi

chown -R mythtv.mythtv /etc/mythtv/mysql.txt
chown root.root /etc/mythtv
chmod 751 /etc/mythtv
chmod 760 /etc/mythtv/mysql.txt

eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

amixer set CD playback off
# this sets up default capture on CD, and we need Mic default
# amixer set CD capture1
# don't ask me why 'capture1' turns on cd capture...

#/usr/pluto/bin/sqlCVS -R 4000 -H schema.linuxmce.org -C 3 -n $PLUTO_DB_CRED -D pluto_myth -a -U anonymous~anonymous -r myth -e checkin
if [[  "$DBPassword" != "" ]] ;then
	mysql_command="mysql -s -B -h $DBHostName -P $MySqlPort -u $DBUserName -p$DBPassword $DBName";
else
	mysql_command="mysql -s -B -h $DBHostName -P $MySqlPort -u $DBUserName $DBName";
fi
query="select count(*) from settings where hostname='`hostname`' AND value LIKE 'Backend%'";

hasSchema=`echo show tables | $mysql_command | grep settings | wc -l`;
if [ $hasSchema == 0 ]; then
	echo "There is no schema in the mythconverg database. Exiting";
	exit 1;
fi

echo "Setting up mythtv configuration for this machine";
hostname=`hostname`;
hostip=`gethostip $hostname | cut -f 2 -d ' '`;
routerip=`gethostip dcerouter | cut -f 2 -d ' '`;

isSetup=`echo "$query" | $mysql_command`;

if [ "$isSetup" != 0  ]; then
        echo "Removing old mythtv configuration for this machine";
		echo "delete from settings where value LIkE 'MasterServer%'" | $mysql_command;
fi;

echo -n Creating and/or setting the cache and recorded folder paths ...
#mkdir -p 		/home/mythtv/shows/$hostip
#chown mythtv 	/home/mythtv/shows/$hostip
#chmod 766 	 	/home/mythtv/shows/$hostip

mkdir -p 		/home/mythtv/cache/$hostip
chown mythtv 	/home/mythtv/cache/$hostip
chmod 766 	 	/home/mythtv/cache/$hostip

echo done.

echo "LOCK TABLE schemalock WRITE;" | $mysql_command  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
service mythtv-backend restart
