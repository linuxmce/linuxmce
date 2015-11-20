#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Config_Ops.sh

# SETUP may be modified later on if mythtv-setup.real or a local
# mythtv-setup is found on the system.
SETUP=/usr/bin/mythtv-setup
FILLDB=/usr/pluto/bin/MythTvInitialFillDB.sh
FILLDB_LOG=/var/log/mythtv/mythfilldatabase.log
RESTART=/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh

DBHostName="localhost"
DBUserName="mythtv"
DBName="mythconverg"
DBPassword="mythtv"
DBPort="3306"

if [ -f /etc/mythtv/mysql.txt ]; then
	. /etc/mythtv/mysql.txt
fi

mysql_command="mysql --skip-column-names -B -h $DBHostName -P $DBPort -u $DBUserName -p$DBPassword $DBName"

# Use mythtv-setup.real if it exists..
if test -f /usr/bin/mythtv-setup.real ; then
	SETUP=/usr/bin/mythtv-setup.real
fi

# If the user installed their own mythtv-setup, adjust..
if test -f /usr/local/bin/mythtv-setup ; then
	SETUP=/usr/local/bin/mythtv-setup
fi

# Run mythtv-setup on a single core if possible
if test x"" != x'which taskset' ; then
        SETUP="taskset -c 0 "$SETUP
fi

# Lock the MythBackend lock to prevent backend restarting
WaitLock "MythBackend" "launchMythSetup" nolog

MYTH_SETUP_PIDS=`pidof mythtv-setup`;
if [[ -n "$MYTH_SETUP_PIDS" ]]; then 
	killall mythtv-setup
fi

MYTH_SETUP_PIDS=`pidof mythtv-setup.real`;
if [[ -n "$MYTH_SETUP_PIDS" ]]; then 
	killall mythtv-setup.real
fi;

# Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $mysql_command
service mythtv-backend stop
PID=`pidof mythbackend`

if [ x"$PID" != x"" ] ; then 
	kill -9 $PID
fi

export DISPLAY=:$Display
$SETUP

Unlock "MythBackend" "launchMythSetup" nolog

# TODO We need to send command to MythTV_PlugIn to
# run MythTvInitialFillDB.sh
# until then, we at least restart 
ssh dcerouter $FILLDB &

