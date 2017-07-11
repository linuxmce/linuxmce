<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);


function initAuth($mediadbADO, $dbADO, $output){

$client_id="252010262386-d8djlkh01ag0v0r514jf1lgcflpodb1c.apps.googleusercontent.com";
$client_secret="ibgv0ECx-N5bK_Abj4cXR1rj";
$driveScope="https://www.googleapis.com/auth/drive%20https://www.googleapis.com/auth/drive.metadata";
$photosScope="%20https://picasaweb.google.com/data/";
$youTubeScopes="https://www.googleapis.com/auth/youtube.readonly";
$authUrl="https://accounts.google.com/o/oauth2/auth";
$tokenUrl='token_uri":"https://accounts.google.com/o/oauth2/token';
$out='';
$url="https://accounts.google.com/o/oauth2/auth?scope=email%20profile%20".$driveScope."%20".$youTubeScopes.$photosScope."&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&client_id=".$client_id."";
$out.="<body align=center>";
$out.='Please click authorize. This will allow you to recieve the authorization code to paste below <br>';
$out.='<button onclick="myFunction()">Authorize</button> <br><br>';

$out.="<form action='index.php' method='GET'>";
$out.="Please Enter Authorization code: <input type='text' name='code'>";
$out.="<input type='hidden' name='section' value='cloudServices'>";
$out.="<input type='hidden' name='service' value='gmail'>";
$out.="<input type='hidden' name='action' value='complete'> ";
$out.="<button type='submit' value='submit'>Authorize LinuxMCE </button>";
$out.="</form>";

$out.='<script>
function myFunction() {
    
    window.open("'.$url.'");
}
</script>';
	$out.="</body>";
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services | Authorize Gmail');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	}
?>