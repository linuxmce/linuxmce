echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the RomTitles table exists. This also takes care
# of installs where the old schema is used, because this table does not exist there.
# in addition, if RomTitles isn't there, but the psc_ tables are there, then we need to drop the db, before import.
mysql $MYSQL_DB_CRED lmce_game -e "SELECT * from RomTitles LIMIT 1"||PROCESS="install"
mysql $MYSQL_DB_CRED lmce_game -e "SELECT * from psc_game_repset LIMIT 1"||DONOTDROP="donotdrop"
if [ $PROCESS = "install" ]; then
	(
	if [ -z $DONOTDROP ]; then 
			mysql $MYSQL_DB_CRED lmce_game -e "DROP DATABASE lmce_game"
	fi
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D lmce_game -r game import
	) || exit $?
	
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
	
	Q="GRANT ALL PRIVILEGES ON lmce_game.* TO 'plutomedia'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON lmce_game.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi

