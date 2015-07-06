<?php

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
set_include_path('operations/cloudServices/google-api-php-client/src');	
require_once('Google/autoload.php');

  function completeAuth($mediadbADO, $dbADO, $output){

$client = new Google_Client();
$client->setAuthConfigFile('/var/www/lmce-admin/operations/cloudServices/includes/client_secret.json');
//$client->setRedirectUri('http://' . $_SERVER['HTTP_HOST'] . '/lmce-amin/index.php?section=cloudServices&app=gmail');
$client->addScope(Google_Service_Drive::DRIVE_METADATA_READONLY);

if (! isset($_GET['code'])) {
  $auth_url = $client->createAuthUrl();
 header('Location: ' . filter_var($auth_url, FILTER_SANITIZE_URL));
} else {
  $client->authenticate($_GET['code']);
  $_SESSION['access_token'] = $client->getAccessToken();
  $redirect_uri = 'http://' . $_SERVER['HTTP_HOST'] . '/lmce-admin/index.php?section=cloudServices';
 
//header('Location: ' . filter_var($redirect_uri, FILTER_SANITIZE_URL));
}
$deviceTemplate=2316;
$deviceQuery = 'SELECT 
				DeviceData.Description AS dd_Description, 
				Device_DeviceData.FK_DeviceData,
                                Device.PK_Device,
				ParameterType.Description AS typeParam, 
				Device_DeviceData.IK_DeviceData
			FROM DeviceData 
			INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
			INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
			INNER JOIN Device ON FK_Device=PK_Device
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE Device.FK_DeviceTemplate=2316 AND Device_DeviceData.FK_DeviceData=59 ';
	$res = $dbADO->Execute($deviceQuery);
	$ddForDevice = $res->FetchRow();
	$out="";
	$out.="<br>";
	$out.="Device:".$ddForDevice['PK_Device'];
	$oldData= json_decode($ddForDevice['IK_DeviceData']);
	$newDataString= array( $_SESSION['userID']=>$_SESSION['access_token'] );
	$accessData = json_decode($_SESSION['access_token']);
	
	$parsedOldData= $oldData;
	$parsedOldData[$_SESSION['userID']] = $accessData;
	$newDataString = json_encode($parsedOldData);
	$updateQuery="UPDATE Device_DeviceData SET IK_DeviceData= ' ".$newDataString." ' where FK_DEVICE=".$ddForDevice['PK_Device']." AND FK_DeviceData=59";
	$dbADO->Execute($updateQuery);
	
	$out='Completed google authorization for '.$_SESSION['hh_username'].'.<br>';
	
	$jsonArr = json_decode($_SESSION['access_token'], true );
	
	/*
	$out.="Access Token:".$jsonArr['access_token']."<br>";
	$out.="Token Type:".$jsonArr['token_type']."<br>";
	$out.="Expires:".$jsonArr['expires_in']."<br>";
	$out.="created:".$jsonArr['created']."<br>";
	$out.="Refresh Token:".$jsonArr['refresh_token']."<br>";
	$out.="ID Token: Not Shown<br><br>";
	 * 
	 */
	
	print_r($newDataString);
	$out.="Writing to device data.<br>";
	
	//$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services | Authorize Gmail Complete');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	}
?>