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

if (!isset($_REQUEST['id']) || empty($_REQUEST['id']))
{
	header("Location: index.php");
	exit();
}
$id = $_REQUEST['id'];

if (!isset($_REQUEST['PK_File']) || empty($_REQUEST['PK_File']))
{
	header("Location: index.php");
	exit();
}
$PK_File = $_REQUEST['PK_File'];

$Filename = $_REQUEST['Filename'];
?>
<b>Now operating on <font color="red"><i><?=htmlentities($Filename)?></i></font></b><br>
<?

require_once("tmdb.inc.php");

if (isset($_REQUEST['CoverArt']))
	$CoverArt = $_REQUEST['CoverArt'];
else
	$CoverArt = "replace";

function GetDetails($id, &$details)
{
	global $tmdb;

	$result = $tmdb->getMovie($id);
	$details = json_decode($result, TRUE);
}

function hidden($name, $value)
{
	echo "<input type=\"hidden\" name=\"$name\" value=\"" . htmlentities($value) . "\">\n";
}

function HtmlGen($details)
{
	global $CoverArt, $PK_File;

	echo "<form method=\"POST\" action=\"save.php\">";

	$posters = $details[0]['posters'];
	$had = array('cover' => FALSE, 'mid' => FALSE);
	for ($i = 0; $i < count($posters); $i++)
	{
		$poster = $posters[$i]['image'];
		if ($poster['size'] === 'cover' && !$had['cover'])
		{
			echo "<img src=\"{$poster['url']}\"><br>\n";
			$had['cover'] = TRUE;
		}
		if ($poster['size'] === 'mid' && !$had['mid'])
		{
			hidden("cover", $poster['url']);
			$had['mid'] = TRUE;
		}
		if ($had['cover'] && $had['mid'])
			break;
	}

	echo "Name: <input type=\"text\" name=\"name\" size=\"100\" value=\"" . htmlentities($details[0]['name']) . "\"><br>\n";
	echo "Alt. Name: {$details[0]['alternative_name']}<br>\n";
	echo "Summary: {$details[0]['overview']}<br>\n";
	echo "Length: {$details[0]['runtime']} minutes<br>\n";
	echo "Release date: {$details[0]['released']}<br>\n";

	//hidden('name', $details[0]['name']);
	hidden('alt_name', $details[0]['alternative_name']);
	hidden('overview', $details[0]['overview']);
	hidden('runtime', $details[0]['runtime']);
	hidden('released', $details[0]['released']);

	echo "Cast:<br>\n";
	$cast = $details[0]['cast'];
	for ($i = 0; $i < count($cast); $i++)
	{
		$as_character = $cast[$i]['character'];
		if (!empty($as_character))
			$as_character = "as $as_character";
		echo "&nbsp;&nbsp;&nbsp;&nbsp;{$cast[$i]['job']}: {$cast[$i]['name']} {$as_character}<br>\n";
		hidden("cast[{$cast[$i]['job']}][]", $cast[$i]['name']);
	}

	$genres = $details[0]['genres'];
	echo "Genres: ";
	for ($i = 0; $i < count($genres); $i++)
	{
		$genre = $genres[$i]['name'];
		if ($i > 0)
			echo ", ";
		echo "$genre";
		hidden('genre[]', $genre);
	}
	echo "<br>\n";
	switch ($CoverArt)
	{
	case "none": break;
	case "replace": echo "<input type=\"checkbox\" name=\"CoverArtAction\" value=\"keep\"> Keep existing cover art<br>\n"; break;
	case "keep": echo "<input type=\"checkbox\" name=\"CoverArtAction\" value=\"keep\" checked> Keep existing cover art<br>\n"; break;
	}
	echo "<input type=\"hidden\" name=\"PK_File\" value=\"$PK_File\">\n";
	echo "<input type=\"submit\" value=\"Save\">\n";
	echo "</form>\n";
}

$details = array();
GetDetails($id, $details);

if (0)
{
	echo "<pre>";
	var_dump($details);
	echo "</pre>";
}

HtmlGen($details);
?>
