
echo
. /usr/pluto/bin/Config_Ops.sh || :
. /usr/pluto/bin/SQL_Ops.sh 
. /usr/pluto/bin/pluto.func
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

PROCESS="upgrade"

# Check if we have an existing install, by verifying the File table exists
mysql $MYSQL_DB_CRED pluto_media -e "Select * From File Limit 0,1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	(
		cd /usr/pluto/database
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_media -r media import
	) || exit $?

	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'127.0.0.1';"
	RunSQL "$Q"

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	RunSQL "$Q"

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost';"
	RunSQL "$Q"

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO '$MySqlUser'@'localhost' IDENTIFIED BY '$MySqlPassword';"
	RunSQL "$Q"

	Q="FLUSH PRIVILEGES;"
	RunSQL "$Q"

	#temporary code -- we got rid of the ?? the what?
	Q="UPDATE Attribute SET FK_AttributeType=13 WHERE FK_AttributeType=4;"
	RunSQL "$Q"
fi

# Drop the FatGroupBy index which is massive and no longer required due to query optimisations.
Q="SELECT Count(*) AS Entries FROM information_schema.STATISTICS S WHERE TABLE_NAME = 'File' AND INDEX_NAME='FatGroupBy'"
Entries=$(RunSQL "$Q" )
if [[ "$Entries" -gt "0" ]]; then
	Q="ALTER TABLE pluto_media.File DROP INDEX FatGroupBy(PK_File,Path,Filename,INode,AttrDate,AttrCount,ModificationDate);"
	RunSQL "$Q"
fi

# FIXME: These need to made a part of schema!
Q="SELECT Count(*) AS Entries FROM information_schema.STATISTICS S WHERE TABLE_NAME = 'Attribute' AND INDEX_NAME='AttributeSort'"
Entries=$(RunSQL "$Q" )
if [[ "$Entries" -eq "0" ]]; then
	Q="ALTER TABLE pluto_media.Attribute ADD INDEX AttributeSort(FK_AttributeType, Name(20));"
	RunSQL "$Q"
fi

Q="SELECT Count(*) AS Entries FROM information_schema.STATISTICS S WHERE TABLE_NAME = 'Bookmark' AND INDEX_NAME='FK_File'"
Entries=$(RunSQL "$Q")
if [[ "$Entries" -eq "0" ]]; then
	Q="ALTER TABLE pluto_media.Bookmark ADD INDEX FK_File(FK_File);"
	RunSQL "$Q"
fi

Q="SELECT Count(*) AS Entries FROM information_schema.STATISTICS S WHERE TABLE_NAME = 'LongAttribute' AND INDEX_NAME='FK_File'"
Entries=$(RunSQL "$Q")
if [[ "$Entries" -eq "0" ]]; then
	Q="ALTER TABLE pluto_media.LongAttribute ADD INDEX FK_File(FK_File);"
	RunSQL "$Q"
fi

if [ $PROCESS = "upgrade" ]; then
	echo
	echo Updating media database using sqlCVS
	echo Please be patient...
	## FIXME - schema.linuxmce.org is hard coded
	/usr/pluto/bin/sqlCVS -R 4999 -H schema.linuxmce.org $PLUTO_DB_CRED -n -d anonymous -U anonymous~nopass -D pluto_media -r media -A -e update || exit $?
fi
exit 0
