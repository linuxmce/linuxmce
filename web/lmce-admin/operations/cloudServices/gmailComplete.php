<?php
/*
 * 
 * {
 * "access_token":"ya29.oAE9CGRIqAq3fXzTmFEo-07eNMSrc7EU9Dw-obfkkEfDwanSAtJPofAtuA7_wx_w_ydEHag-o_TKsQ",
 * "token_type":"Bearer",
 * "expires_in":3600,
 * "id_token":"eyJhbGciOiJSUzI1NiIsImtpZCI6IjEyMzg5MzdjOTBlOGU2MzgzZmYwYzdiYzE0NTlkOGQyNjQzZDFhYzkifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwic3ViIjoiMTEzMjg5MDgxMzkxMjA1NzE3NzAzIiwiYXpwIjoiMjUyMDEwMjYyMzg2LWQ4ZGpsa2gwMWFnMHYwcjUxNGpmMWxnY2ZscG9kYjFjLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiZW1haWwiOiJnb2xnb2o0QGdtYWlsLmNvbSIsImF0X2hhc2giOiJpWWd6Tll5TFBrM183LVB1SG13dEdRIiwiZW1haWxfdmVyaWZpZWQiOnRydWUsImF1ZCI6IjI1MjAxMDI2MjM4Ni1kOGRqbGtoMDFhZzB2MHI1MTRqZjFsZ2NmbHBvZGIxYy5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlhdCI6MTQzNTQ2NTc2NCwiZXhwIjoxNDM1NDY5MzY0fQ.Otlb8fz6fICa9Ncn8OTVbOaORuwkhjPec6DSVgAG-4k3Qob4PzqtlxaRt1qvRSFGXMrdiJ1UgMsVkBKWROvBjnhhI6ABtFpVUbTizx-hbhznapIXbj8MpeqrR0o1nb5WWP1yXmw33bQXz35ZSMjJctbfwNm1ZGnd5VtazhsP76K0N93DAGhY-fQ6YngSY2eb9dznP0TMxxwiD3ywjUXh2oiXj9lfsxM29j454DBEliW8vzrcg81YNkg-8ZbhALlFexyLlL985Ch6SJ1xRaJF9FG_IlavpkWgkgetDWOkmRvKZHWvK5Zf5VQYAohobFv09edOtrYCQfl9qw04R06Xqg","refresh_token":"1\/h6Y5eJqvELDHsv7eLg5HvelYrsZ-FMRa-jS_xxSs929IgOrJDtdun6zK6XiATCKT",
 * "created":1435465764
 * }
 * 
 */
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
set_include_path('operations/cloudServices/google-api-php-client/src');	
require_once('Google/autoload.php');

  function completeAuth($mediadbADO, $dbADO, $output){

$client = new Google_Client();
$client->setAuthConfigFile('/tmp/client_secret.json');
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
	
	$out.="<br>";
	$out.="Device:".$ddForDevice['PK_Device'];
	$oldData= $ddForDevice['IK_DeviceData'];
	$newDataString= array( $_SESSION['userID']=>$_SESSION['access_token'] );
	
	$parsedOldData= json_decode($oldData, true);
	$parsedOldData[$_SESSION['userID']] = $_SESSION['access_token'];
	$newDataString = json_encode($parsedOldData);
		
	print_r($newDataString);	
		
	$updateQuery="UPDATE Device_DeviceData SET IK_DeviceData='".$newDataString."'where FK_DEVICE=".$ddForDevice['PK_Device']." AND FK_DeviceData=59";
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
	
	print_r($parsedOldData);
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