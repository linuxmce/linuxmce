<?
session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$irgcID=(int)@$_REQUEST['irgcID'];

if($irgcID>0){
	$res=$dbADO->Execute('SELECT IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irgcID);
	if($res->RecordCount()!=0){
		$row=$res->FetchRow();
		$code=$row['IRData'];
	}else{
		$code='ERROR: code does not exist';
	}
}else{
	$code='ERROR: code ID not specified';
}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
	<title>Syntax Highlighting Code Editor JavaScript</title>
	<link type="text/css" rel="stylesheet" href="scripts/codepress/codepress-php.css" />
	<script type="text/javascript" src="scripts/codepress/codepress.js"></script>
	<script type="text/javascript">
	CodePress.language = 'php';
	</script>
</head>

<!-- no line break between body and pre -->
<body id="ffedt"><pre id="ieedt"><?=$code?>
</pre>
</body>
</html>
