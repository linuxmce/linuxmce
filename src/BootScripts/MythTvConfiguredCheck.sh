#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

chown mythtv.mythtv /etc/mythtv/mysql.txt
chmod 660 /etc/mythtv/mysql.txt

QueryIsConfigured="SELECT count(data) as a FROM settings WHERE value like '%BackendServerIP%' AND hostname='$(hostname)'";
#MysqlCommand="mysql -D mythconverg -u mythtv -pArkhNKpr";
MythUser=$(grep ^DBUserName /etc/mythtv/mysql.txt | cut -d= -f2)
MythPass=$(grep ^DBPassword /etc/mythtv/mysql.txt | cut -d= -f2)
MysqlCommand="mysql -D mythconverg -h $MySqlHost -P $MySqlPort -u $MythUser -p$MythPass";

QueryResult=`echo $QueryIsConfigured | $MysqlCommand | tail -n 1`;

export DISPLAY=:$Display;
if [ $QueryResult -eq 0 ]; then
        echo "Need to launch the setup";
        echo -e "\n\n" | mythtv-setup;
        chvt 1;
        echo "Please wait until MythTv backend server completes updating the channel database";
        /etc/cron.daily/mythtv-backend;
        echo "MythTV Channel list update is complete.";
fi;

echo "LOCK TABLE schemalock WRITE;" | $MysqlCommand  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
service mythtv-backend restart

# looking for XServer pid

# XServerPid=`lsof | grep /tmp/.X11-unix/X0 | head -n 1 | sed "s/^[^ ]* *//" | cut -f 1 -d ' '`;
# echo "XServer pid: $XServerPid";

chvt 7;

