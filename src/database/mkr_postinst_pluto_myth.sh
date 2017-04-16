echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED pluto_myth -e "Select 1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	
	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_myth -r myth import
	) || exit $?
	
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	
	Q="GRANT ALL PRIVILEGES ON pluto_myth.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi

