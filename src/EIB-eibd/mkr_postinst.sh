#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q='CREATE DATABASE IF NOT EXISTS `eib`'
RunSQL "$Q"

UseDB eib
Q='CREATE TABLE IF NOT EXISTS `groupaddresses` (
	`name` varchar(100) default NULL,
	`address` varchar(100) NOT NULL,
	PRIMARY KEY  (`address`)
)'
RunSQL "$Q"

# add eib user
adduser --system --home /var/run/eib eib || true

Q="GRANT ALL PRIVILEGES ON eib.* to 'eib'@'localhost'; FLUSH PRIVILEGES"
RunSQL "$Q"

# start eibd daemon if not running, else restart
if pidof eibd > /dev/null; then
	service eibd restart
else
	service eibd start
fi

# create cronjob to send date and time to KNX bus
ln -fs /usr/pluto/bin/knxtime /etc/cron.hourly/knxtime
