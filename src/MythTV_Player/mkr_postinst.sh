#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# START: Hack to get mythtv working
#ln -s /usr/lib/libXmu.so.6.2.0 /usr/lib/libXmu.so || :
# END  : Hack to get mythtv working

# If this is not the hybrid, copy the mysql.txt file from hybrid
if [[ "$PK_Device" != "1" ]] ;then
	scp -o 'StrictHostKeyChecking no' -o 'PasswordAuthentication no' root@dcerouter:/etc/mythtv/config.xml /etc/mythtv/ || :
	scp -o 'StrictHostKeyChecking no' -o 'PasswordAuthentication no' root@dcerouter:/etc/mythtv/mysql.txt /etc/mythtv/ || :
	touch /etc/mythtv/mysql.txt
fi

# In case we have a later MythTV, we need to transpose the information
# from the config.xml file.
# If this is the hybrid, create the mysql.txt
if [[ -f /etc/mythtv/config.xml ]] ; then
	HOST='Host'
	USERNAME='UserName'
	PASSWORD='Password'
	DATABASE='DatabaseName'

	WITHDB=`grep DBHostName -i /etc/mythtv/config.xml` || :

	if [ -n "$WITHDB" ] ; then
		HOST="DBHostName"
		USERNAME="DBUserName"
		DATABASE="DBName"
		PASSWORD="DBPassword"
	fi
	# START: Hack to transpose xml
	echo $(grep -e "<$HOST>" /etc/mythtv/config.xml | sed -e "s#<$HOST>#DBHostName=#" -e "s#</$HOST>#\n#") > /etc/mythtv/mysql.txt
	echo $(grep -e "<$USERNAME>" /etc/mythtv/config.xml | sed -e "s#<$USERNAME>#DBUserName=#" -e "s#</$USERNAME>#\n#") >> /etc/mythtv/mysql.txt
	echo $(grep -e "<$DATABASE>" /etc/mythtv/config.xml | sed -e "s#<$DATABASE>#DBName=#" -e "s#</$DATABASE>#\n#") >> /etc/mythtv/mysql.txt
	echo $(grep -e "<$PASSWORD>" /etc/mythtv/config.xml | sed -e "s#<$PASSWORD>#DBPassword=#" -e "s#</$PASSWORD>#\n#") >> /etc/mythtv/mysql.txt
	# END: Hack to transpose xml
fi

# If this is not the hybrid, fix the DBHostName
if [[ "$PK_Device" != "1" ]] ;then
	sed -i "s/^DBHostName.*/DBHostName=$MySqlHost/g" /etc/mythtv/mysql.txt || :
fi

# make the proper ownership's because the backend can't read it otherwise
chown mythtv /etc/mythtv/config.xml || :
chown mythtv /etc/mythtv/mysql.txt || :

## Get a valid mysql connection string to mythconvert
eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

if ( [[ -z "$DBUserName" ]] || [[ -z "$DBHostName" ]] || [[ -z "$DBPassword" ]] || [[ -z "$DBName" ]] ) ; then
	echo "********";
	echo "There is an error in the mythtv backend setup. It looks like /etc/mythtv/mysql.txt didn't create properly";
	echo "********";
	exit 0;
fi

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";

Q="select IK_DeviceData from Device_DeviceData where FK_DeviceData=206"
AutoConf=$(echo "$Q" | /usr/bin/mysql -h $MySqlHost $MySqlDBName | tail -n +2)

if [[ "$AutoConf" == "1" ]]; then
    echo "Auto Configure is set"
	exit 0
fi

function addEntries
{
	if [ "$3" = "" ]; then
		echo "insert into settings(value, data) values('$1', '$2')" | $mysql_command;
	else
		echo "insert into settings(value, data, hostname) values('$1', '$2', '$3')" | $mysql_command;
	fi;
}

function addRootUser
{
	is_in_group="false"
	for group in `id -nG root` ;do
		if [[ "$group" == "mythtv" ]] ;then
			is_in_group="true"
		fi
	done

	if [[ "$is_in_group" == "false" ]] ;then
		group_line=$(grep "^mythtv:" /etc/group)
		if [[ "$(echo $group_line | cut -d':' -f4)" == "" ]] ;then
			group_line="${group_line}root"
		else
		    group_line="${group_line},root"
		fi
		grep -v "^mythtv:" /etc/group > /etc/group.$$
		echo $group_line >> /etc/group.$$
		mv /etc/group.$$ /etc/group
	fi
}

## Force the backend to make the database structure
echo "LOCK TABLE schemalock WRITE;" | $mysql_command  || :
invoke-rc.d mythtv-backend force-reload || /bin/true

echo "Waiting 3 seconds so that myth backed is able to actually create the schema"
FOUND=0;
TRIES=20;
SLEEP=0.5;
while [ $TRIES != 0 ]; do
	RESULT=`echo show tables | $mysql_command | grep settings | wc -l`;
	if [ "$RESULT" != "0" ]; then
		TRIES=0;
		FOUND=1;
	else
		sleep $SLEEP;
		TRIES=$[$TRIES - 1];
	fi;
done;

if [ "$FOUND" == "0" ]; then 
	echo "********";
	echo "There is an error in the mythtv backend setup. It looks like the database schema wasn't created properly";
	echo "********";
	exit 0;
fi

PID=`pidof /usr/bin/mythbackend` || /bin/true;
echo "LOCK TABLE schemalock WRITE;" | $mysql_command  || :
if [ "$PID" != "" ]; then
	invoke-rc.d mythtv-backend start || /bin/true
else
	invoke-rc.d mythtv-backend restart || /bin/true
fi

#Create a shortcut to MythTV Setup
#Find the media director -- it could be a child if this is a hybrid
Q="SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=8 AND (PK_Device=$PK_Device OR FK_Device_ControlledVia=$PK_Device) LIMIT 1"
PK_Device_MD="$(RunSQL "$Q")"


#Do we have a quick launch already?
Q="SELECT PK_Device_QuickStart FROM Device_QuickStart where FK_Device=$PK_Device_MD AND Description='MythTV Setup'"
PK_Device_QuickStart="$(RunSQL "$Q")"

if [ "$PK_Device_QuickStart" = "" ]; then

	Q="INSERT INTO Device_QuickStart(FK_Device,Description,SortOrder,FK_QuickStartTemplate) SELECT $PK_Device_MD,Description,1,PK_QuickStartTemplate FROM QuickStartTemplate WHERE PK_QuickStartTemplate=8"
	RunSQL "$Q"
fi

## Add the myth user to the public group so it will be able to write intro /public/data dir
adduser --quiet mythtv public

## Set root user to mythtv group
if grep -q 'mythtv' /etc/group ;then
	addRootUser
else
	groupadd mythtv || :
	addRootUser
fi

mkdir -p /root/.mythtv/ || :
touch /root/.mythtv/ignoregroup || :
