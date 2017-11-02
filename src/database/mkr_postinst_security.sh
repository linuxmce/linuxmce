echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED pluto_security -e "Select * From AlertType Limit 0,1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_security -r security import
	) || exit $?
	
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	
	Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'plutosecurity'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'plutosecurity'@'localhost';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON pluto_security.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi
if [ $PROCESS = "upgrade" ]; then
        echo
        echo Updating system database using sqlCVS
        echo Please be patient...
        ## FIXME - schema.linuxmce.org is hard coded
        /usr/pluto/bin/sqlCVS -R 6999 -H schema.linuxmce.org $PLUTO_DB_CRED -n -d anonymous -U anonymous~nopass -D pluto_security -r security -A -e update || exit $?
fi


