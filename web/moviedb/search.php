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

require_once("tmdb.inc.php");

if (!isset($_REQUEST['PK_File']) || empty($_REQUEST['PK_File']))
{
	header("Location: index.php");
	exit();
}
$PK_File = $_REQUEST['PK_File'];

if (!isset($_REQUEST['q']) || empty($_REQUEST['q']))
{
	die('ERROR: Search field was empty');
}
$title = $_REQUEST['q'];

if (isset($_REQUEST['CoverArt']))
	$CoverArt = $_REQUEST['CoverArt'];
else
	$CoverArt = "replace";

$Filename = $_REQUEST['Filename'];
?>
<b>Now operating on <font color="red"><i><?=htmlentities($Filename)?></i></font></b><br>
<?

function Search($title, &$result)
{
	global $tmdb;

	$movie_result = $tmdb->searchMovie($title);
	$result = json_decode($movie_result, TRUE);
}

function HtmlGen($result)
{
	global $tmdb;
	global $CoverArt, $PK_File;
	global $Filename;

	$encodedFilename = urlencode($Filename);

	if ($result[0] === "Nothing found.")
	{
		echo "The search returned no results";
		return;
	}

	for ($i = 0; $i < count($result); $i++)
	{
		$entry = $result[$i];
		echo "Name: {$entry['name']} <a href=\"details.php?id={$entry['id']}&CoverArt=$CoverArt&PK_File=$PK_File&Filename=$encodedFilename\">Open</a><br>\n";
		echo "Alt. name: {$entry['alternative_name']}<br>\n";
		echo "Summary: {$entry['overview']}<br>\n";
		echo "Cover art:<br>\n";
		$posters = $entry['posters'];
		echo "<div style=\"white-space:nowrap\">";
		for ($j = 0; $j < count($posters); $j++)
		{
			$poster = $posters[$j]['image'];
			if ($poster['size'] !== 'cover')
				continue;
			echo "<img src=\"{$poster['url']}\"><br>\n";
		}
		echo "</div><br>\n";
		echo "<hr>\n";
	}
}

$result=array();
Search($title, $result);

if (0)
{
	echo "<pre>";
	var_dump($result);
	echo "</pre><hr>";
}

HtmlGen($result);
?>
