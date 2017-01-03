echo 
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED $MySqlDBName -e "Select * From DeviceTemplate Limit 0,1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	echo 
	echo Setting up system database.
	echo ****This may take a long time****
	echo This involves merging all the changes from the
	echo central database back into your local copy.
	echo Please be patient.......
	
	Q="drop table if exists CriteriaParmNesting_D; drop table if exists CriteriaParm_D; drop table if exists Criteria_D"
	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q" || /bin/true
	
	Q="drop table if exists psc_document_batdet; drop table if exists psc_document_bathdr; drop table if exists psc_document_batuser; drop table if exists psc_document_repset; drop table if exists psc_document_schema; drop table if exists psc_document_tables; drop table if exists Document; drop table if exists Document_Comment;"
	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q" || /bin/true
	
	Q="alter table \`Document_Comment\` ,change \`Date\` \`Date\` datetime   NOT NULL "
	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q" || /bin/true
	
	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D $MySqlDBName -r constants,dce,designer,document,ir,website import
	) || exit $?
	mysql $MySqlDBName < /usr/pluto/database/city.dump
	
	Q="GRANT ALL PRIVILEGES ON $MySqlDBName.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="GRANT ALL PRIVILEGES ON $MySqlDBName.* TO 'plutomedia'@'localhost';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"
fi
if [ $PROCESS = "upgrade" ]; then
	echo
	echo Updating system database using sqlCVS
	echo Please be patient...
	## FIXME - schema.linuxmce.org is hard coded
	/usr/pluto/bin/sqlCVS -R 3999 -H schema.linuxmce.org $PLUTO_DB_CRED -n -d anonymous -U anonymous~nopass -D $MySqlDBName -r constants,dce,designer,document,ir,website -A -e update || exit $?
fi
# Make sure Text_LS has the correct character set (UTF-8)
/usr/bin/perl /usr/pluto/bin/update2utf8.pl || :
# update quick start icons
/bin/bash /usr/pluto/bin/UpdateQuickStartIcons.sh || /bin/true


