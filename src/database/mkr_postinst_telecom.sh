echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED pluto_telecom -e "Select 1"||PROCESS="install"
if [ $PROCESS = "install" ]; then

	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_telecom -r telecom import
	) || exit $?
	
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	
	Q="GRANT ALL PRIVILEGES ON pluto_telecom.* TO 'plutotelecom'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON pluto_telecom.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi
if [ $PROCESS = "upgrade" ]; then
	echo
	echo Updating media database using sqlCVS
	echo Please be patient...
	## FIXME - schema.linuxmce.org is hard coded
	/usr/pluto/bin/sqlCVS -R 7999 -H schema.linuxmce.org $PLUTO_DB_CRED -n -d anonymous -U anonymous~nopass -D pluto_telecom -r telecom -A -e update || exit $?
fi

