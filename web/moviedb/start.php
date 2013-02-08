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

if (!isset($_REQUEST['PK_File']) || empty($_REQUEST['PK_File']))
{
	header("Location: index.php");
	exit();
}
$PK_File = $_REQUEST['PK_File'];
if (isset($_REQUEST['CoverArt']))
	$CoverArt = $_REQUEST['CoverArt'];
else
	$CoverArt = "replace";

$Filename = $_REQUEST['Filename'];
$QueryString = $Filename;
$ExtPos = strrpos($QueryString, '.');
if ($ExtPos !== FALSE)
	$QueryString = substr($QueryString, 0, $ExtPos);
$QueryString = preg_replace("/[\.\-,]/", " ", $QueryString);
$QueryString = preg_replace("/ +/", " ", $QueryString);
?>
<b>Now operating on <font color="red"><i><?=htmlentities($Filename)?></i></font></b><br>
<form method="GET" action="search.php">
<input type="text" size="100" name="q" value="<?=htmlentities($QueryString)?>"><input type="submit" value="Search">
<input type="hidden" name="CoverArt" value="<?=$CoverArt?>">
<input type="hidden" name="PK_File" value="<?=$PK_File?>">
<input type="hidden" name="Filename" value="<?=htmlentities($Filename)?>">
</form>
<b>Will search the Movie Database</b>
