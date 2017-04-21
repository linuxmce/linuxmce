echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
DL_DB="lmce_datalog"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED $DL_DB -e "Select 1" > /dev/null 2>&1||PROCESS="install"
if [ $PROCESS = "install" ]; then
	(
		cd /usr/pluto/database
		Q="CREATE DATABASE $DL_DB;"
		mysql $MYSQL_DB_CRED -e "$Q"
		mysql $MYSQL_DB_CRED "$DL_DB" < lmce_datalog.sql
	) || exit $?

	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	
	Q="GRANT ALL PRIVILEGES ON $DL_DB.* TO 'plutomedia'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON $DL_DB.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON $DL_DB.* TO 'plutomedia'@'localhost';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON $DL_DB.* TO '$MySqlUser'@'localhost' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi

