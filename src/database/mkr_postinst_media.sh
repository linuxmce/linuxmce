
echo
. /usr/pluto/bin/Config_Ops.sh || :
. /usr/pluto/bin/SQL_Ops.sh 
. /usr/pluto/bin/pluto.func
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
	RunSQL "$Q"

	Q="GRANT ALL PRIVILEGES ON pluto_media.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
	RunSQL "$Q"
	
	Q="FLUSH PRIVILEGES;"
	RunSQL "$Q"

	#temporary code -- we got rid of the 
	Q="UPDATE Attribute SET FK_AttributeType=13 WHERE FK_AttributeType=4;"
	RunSQL "$Q"
fi

Q="SELECT Count(*) AS Entries FROM information_schema.STATISTICS S WHERE TABLE_NAME = 'File' AND INDEX_NAME='FatGroupBy'"

Entries=$(RunSQL "$Q" )
if [[ "$Entries" -eq "0" ]]; then
	Q="ALTER TABLE pluto_media.File ADD INDEX FatGroupBy(PK_File,Path,Filename,INode,AttrDate,AttrCount,ModificationDate);"
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

# add some TV related media attributes. this should end up in SqlCVS some day. see trac ticket #1022
Q="insert into pluto_media.AttributeType(PK_AttributeType,Description,Define,PicPriority) values(50,'Season Number',NULL,NULL) on duplicate key update psc_id=psc_id;"
RunSQL "$Q"

Q="insert into pluto_media.AttributeType(PK_AttributeType,Description,Define,PicPriority) values(51,'Episode Number',NULL,NULL) on duplicate key update psc_id=psc_id;"
RunSQL "$Q"

Q="insert into pluto_media.AttributeType(PK_AttributeType,Description,Define,PicPriority) values(52,'TV Season ID',NULL,NULL) on duplicate key update psc_id=psc_id;"
RunSQL "$Q"

Q="update pluto_media.MediaType_AttributeType set MediaSortOption=5, CombineAsOne=0 where EK_MediaType=5 and FK_AttributeType=12;"
RunSQL "$Q"

exit 0
