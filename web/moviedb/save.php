<?
header("Content-Type: text/html; charset=utf-8");
if (get_magic_quotes_gpc()) {
    $process = array(&$_GET, &$_POST, &$_COOKIE, &$_REQUEST);
    while (list($key, $val) = each($process)) {
        foreach ($val as $k => $v) {
            unset($process[$key][$k]);
            if (is_array($v)) {
                $process[$key][stripslashes($k)] = $v;
                $process[] = &$process[$key][stripslashes($k)];
            } else {
                $process[$key][stripslashes($k)] = stripslashes($v);
            }
        }
    }
    unset($process);
}

$map['job'] = array(
	'Director' => 1,
	'Actor' => 2,
	'Author' => 16,
	'Screenplay' => 16,
	'Producer' => 38,
	'Executive Producer' => 39,
);
$map['runtime'] = 28;
$map['overview'] = 37;
$map['name'] = 13;
$map['genre'] = 8;
$map['released'] = 19;

if (0)
{
	echo "<pre>";
	var_dump($_REQUEST);
	echo "</pre>";
}

mysql_connect("localhost", "root", "");
mysql_select_db("pluto_media");

if (!isset($_REQUEST['name']) || empty($_REQUEST['name']))
{
	header("Location: index.php");
	exit();
}

if (!isset($_REQUEST['PK_File']) || empty($_REQUEST['PK_File']))
{
	header("Location: index.php");
	exit();
}
$PK_File = mysql_escape_string($_REQUEST['PK_File']);

$CoverArtAction = "";
if (isset($_REQUEST['CoverArtAction']))
	$CoverArtAction = $_REQUEST['CoverArtAction'];

function insert_attribute($PK_File, $PK_AttributeType, &$Value)
{
	if (!isset($Value) || empty($Value) || empty($PK_AttributeType))
		return;
	$ValueEsq = mysql_escape_string($Value);
	$R = mysql_query("SELECT PK_Attribute FROM Attribute WHERE FK_AttributeType=$PK_AttributeType AND Name='$ValueEsq' LIMIT 1");
	if (mysql_num_rows($R) === 1)
	{
		$row = mysql_fetch_assoc($R);
		$PK_Attribute = $row['PK_Attribute'];
	}
	else
	{
		mysql_query("INSERT INTO Attribute(FK_AttributeType, Name) VALUES($PK_AttributeType, '$ValueEsq');");
		$PK_Attribute = mysql_insert_id();
	}
	mysql_query("INSERT INTO File_Attribute(FK_File, FK_Attribute) VALUES($PK_File, $PK_Attribute);");
}

function insert_long_attribute($PK_File, $PK_AttributeType, &$Value)
{
	if (!isset($Value) || empty($Value) || empty($PK_AttributeType))
		return;
	$ValueEsq = mysql_escape_string($Value);
	mysql_query("INSERT INTO LongAttribute(FK_AttributeType, FK_File, Text) VALUES($PK_AttributeType, $PK_File, '$ValueEsq');");
}

mysql_query("DELETE FROM File_Attribute WHERE FK_File=$PK_File;");
mysql_query("DELETE FROM LongAttribute WHERE FK_File=$PK_File;");
insert_attribute($PK_File, $map['name'], $_REQUEST['name']);
insert_attribute($PK_File, $map['released'], $_REQUEST['released']);
insert_long_attribute($PK_File, $map['overview'], $_REQUEST['overview']);
insert_attribute($PK_File, $map['runtime'], $_REQUEST['runtime']);
if (isset($_REQUEST['genre']))
{
	foreach (@$_REQUEST['genre'] as $genre)
		insert_attribute($PK_File, $map['genre'], $genre);
}
if (isset($_REQUEST['cast']))
{
	foreach (@$_REQUEST['cast'] as $job => $persons)
	{
		foreach(@$persons as $person)
			insert_attribute($PK_File, $map['job'][$job], $person);
	}
}
//echo "-- Cover art<br>\n";
if (isset($_REQUEST['cover']) && !empty($_REQUEST['cover']))
{
	$cover = $_REQUEST['cover'];
	if ($CoverArtAction !== "keep")
	{
		$R = mysql_query("SELECT PK_Picture,Extension FROM Picture WHERE PK_Picture IN (SELECT FK_Picture FROM Picture_File WHERE FK_File=$PK_File);");
		while ($row = mysql_fetch_assoc($R))
		{
			unlink("/home/mediapics/{$row['PK_Picture']}.{$row['Extension']}");
			unlink("/home/mediapics/{$row['PK_Picture']}_tn.{$row['Extension']}");
		}
		mysql_query("DELETE FROM Picture WHERE PK_Picture IN (SELECT FK_Picture FROM Picture_File WHERE FK_File=$PK_File);");
		mysql_query("DELETE FROM Picture_File WHERE FK_File=$PK_File;");
		mysql_query("INSERT INTO Picture(Extension) VALUES('jpg');");
		$id = mysql_insert_id();
		mysql_query("INSERT INTO Picture_File(FK_Picture, FK_File) VALUES($id, $PK_File);");
		$filename = "/home/mediapics/$id.jpg";
		$thumb = "/home/mediapics/{$id}_tn.jpg";
		system("rm -f '$filename'; wget -O '/tmp/$id.tmpimg' '$cover' && convert '/tmp/$id.tmpimg' '$filename' && convert -sample 256x256 '$filename' '$thumb' && rm -f '/tmp/$id.tmpimg'");
	}
}
//echo "-- Done<br>\n";
//echo "<a href=\"index.php\">Go to the beninning</a>";
header("Location: index.php");
?>
