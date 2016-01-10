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
if (!isset($_REQUEST['Filename']) || empty($_REQUEST['Filename']))
{
	header("Location: index.php");
	exit();
}

$PK_File = $_REQUEST['PK_File'];
$Filename = $_REQUEST['Filename'];

$Title = $Filename;
$ExtPos = strrpos($Title, '.');
if ($ExtPos !== FALSE)
	$Title = substr($Title, 0, $ExtPos);
?>
<b>Now operating on <font color="red"><i><?=htmlentities($Filename)?></i></font></b><br>
<form method="POST" action="save.php">
<input type="hidden" name="PK_File" value="<?=htmlentities($PK_File)?>">
Title: <input type="text" name="name" size="100" value="<?=htmlentities($Title)?>">
<input type="submit" value="Set title">
</form>
<b>Will set title only</b><br>
