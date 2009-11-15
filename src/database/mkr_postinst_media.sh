
echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED pluto_media -e "Select * From File Limit 0,1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_media -r media import
	) || exit $?

	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost';"
	mysql $MYSQL_DB_CRED -e "$Q"

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	mysql $MYSQL_DB_CRED -e "$Q"
	
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"

	#temporary code -- we got rid of the 
	Q="UPDATE Attribute SET FK_AttributeType=13 WHERE FK_AttributeType=4;"
	mysql $MYSQL_DB_CRED pluto_media -e "$Q"
fi

Q="ALTER TABLE pluto_media.File ADD INDEX FatGroupBy(PK_File,Path,Filename,INode,AttrDate,AttrCount,ModificationDate);"
mysql $MYSQL_DB_CRED pluto_media -e "$Q"

Q="ALTER TABLE pluto_media.Bookmark ADD INDEX FK_File(FK_File);"
mysql $MYSQL_DB_CRED pluto_media -e "$Q"

Q="ALTER TABLE pluto_media.LongAttribute ADD INDEX FK_File(FK_File);"
mysql $MYSQL_DB_CRED pluto_media -e "$Q"

exit 0
