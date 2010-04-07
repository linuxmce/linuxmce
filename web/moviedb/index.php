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
	echo "<a href=\"start.php?Filename=$Filename&PK_File={$row['PK_File']}&CoverArt=$CovertArt\">{$row['Filename']}</a><br>\n";
}
?>
