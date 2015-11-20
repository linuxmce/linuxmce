#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DBHostName="localhost"
DBUserName="mythtv"
DBName="mythconverg"
DBPassword="mythtv"
DBPort="3306"

if [ -f /etc/mythtv/mysql.txt ]; then
	. /etc/mythtv/mysql.txt
fi

mysql_command="mysql --skip-column-names -B -h $DBHostName -P $DBPort -u $DBUserName -p$DBPassword $DBName"

# Lock the MythBackend lock to prevent backend restart by another script...
WaitLock "MythBackend" "Restart_Backend_With_SchemaLock.sh" nolog

# Make sure we're not in the middle of a schema upgrade -- myth doesn't check this
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $mysql_command

# Actually perform the restart..
service mythtv-backend restart &> /dev/null

# Restart slave backends as well
SLAVES=`echo "SELECT hostname from capturecard WHERE hostname IS NOT NULL GROUP BY hostname;" | $mysql_command | grep -v dcerouter`
if [ x"$SLAVES" != x"" ] ; then
        for SLAVE in $SLAVES ; do
                ssh $SLAVE "service mythtv-backend restart" &> /dev/null
        done
fi

# Unlock the MythBackend lock so auto restart will work elsewhere
Unlock "MythBackend" "Restart_Backend_With_SchemaLock.sh" nolog
