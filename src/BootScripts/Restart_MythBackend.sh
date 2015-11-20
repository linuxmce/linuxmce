#!/bin/bash

# XXX: This script restarts the mythtv backend if it should be running but isn't

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

if Lock "Restart_MythBackend" "Restart_MythBackend"; then
	MythUser=$(grep ^DBUserName /etc/mythtv/mysql.txt | cut -d= -f2)
	MythPass=$(grep ^DBPassword /etc/mythtv/mysql.txt | cut -d= -f2)
	MysqlCommand="mysql -D mythconverg -h $MySqlHost -P $MySqlPort -u $MythUser -p$MythPass";

	# We don't unlock this one, ever, so that way we can run this multiple times in
	# both the myth plugin and myth player and know that it will always be running

	while :; do
		# Lock the MythBackend lock to prevent backend restarting when MythTV Setup is running
		# launchMythSetup holds this lock while MythTV is running
		WaitLock "MythBackend" "MythBackend_Restart" nolog
		Backend=$(pgrep mythbackend)
		MythSetup=$(pgrep mythtv-setup)
		if [[ -z "$MythSetup" && -z "$Backend" && -x $(which mythtv-backend) ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "MythBackend_Restart" "MythBackend not found running; restarting it"
			echo "LOCK TABLE schemalock WRITE;" | $MysqlCommand  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
			service mythtv-backend restart
			Logging "$TYPE" "$SEVERITY_CRITICAL" "MythBackend_Restart" "MythBackend restarted"
		fi
		Unlock "MythBackend" "MythBackend_Restart" nolog
		sleep 5
	done
fi
