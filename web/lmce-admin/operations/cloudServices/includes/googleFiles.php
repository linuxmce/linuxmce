<?php

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
include 'googleApiFunctions.php';
function showGoogleDriveFiles($mediadbADO, $dbADO, $output){
	
$client = getClient(array());		
$out=testMetadata($client);


finishOutput($out, $output);	
}

function finishOutput($out, $output){
	$scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"></script> ';
	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Google Drive Files');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
}
	

?>