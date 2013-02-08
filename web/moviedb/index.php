<?
header("Content-Type: text/html; charset=utf-8");
mysql_connect("localhost", "root", "");
mysql_select_db("pluto_media");

$Q = "
	SELECT PK_File, Filename
	FROM File
	LEFT JOIN File_Attribute ON FK_File=PK_File
	WHERE FK_Attribute IS NULL AND EK_MediaType IN (3,5) AND IsDirectory=0 and Missing=0
	ORDER BY Filename;
";
$R = mysql_query($Q);
echo "<table border=\"0\">\n";
while ($row = mysql_fetch_assoc($R))
{
	$Q2 = "
		SELECT COUNT(*) AS Count
		FROM Picture_File
		WHERE FK_File = {$row['PK_File']}
	";
	$R2 = mysql_query($Q2);
	$row2 = mysql_fetch_assoc($R2);
	$CovertArt = $row2['Count'] > 0 ? "keep" : "none";

	$Filename = urlencode($row['Filename']);
	echo "<tr>\n";
	echo "<td><a href=\"start.php?Filename=$Filename&PK_File={$row['PK_File']}&CoverArt=$CovertArt\">Search&nbsp;tMDb</a></td>\n";
	echo "<td><a href=\"title.php?Filename=$Filename&PK_File={$row['PK_File']}\"><i><b>Set&nbsp;title&nbsp;only</b></i></a></td>\n";
	echo "<td>{$row['Filename']}<td>\n";
	echo "</tr>\n";
}
echo "</table>\n";
?>
