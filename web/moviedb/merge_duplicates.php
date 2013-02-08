<?
mysql_connect("localhost", "root", "");
mysql_select_db("pluto_media");

$Q = "SELECT PK_Attribute, Name, FK_AttributeType, COUNT(*) AS Count FROM Attribute GROUP BY Name, FK_AttributeType HAVING Count > 1";
$R_having_dupes = mysql_query($Q);

$Duplicate_IDs = array();
while ($Row = mysql_fetch_assoc($R_having_dupes))
{
	$Name = mysql_real_escape_string($Row['Name']);
	$PK_Attribute = $Row['PK_Attribute'];
	$Q = "SELECT PK_Attribute FROM Attribute
		WHERE Name = '$Name'
		AND FK_AttributeType = {$Row['FK_AttributeType']}
		AND PK_Attribute != $PK_Attribute
	";
	$R_to_delete = mysql_query($Q);
	while ($row = mysql_fetch_assoc($R_to_delete))
	{
		$dupe_PK_Attribute = $row['PK_Attribute'];
		mysql_query("UPDATE File_Attribute SET FK_Attribute=$PK_Attribute WHERE FK_Attribute=$dupe_PK_Attribute");
		mysql_query("DELETE FROM Attribute WHERE PK_Attribute=$dupe_PK_Attribute");
	}
}
?>
