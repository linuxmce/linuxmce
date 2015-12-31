<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
set_include_path(get_include_path() . PATH_SEPARATOR .'/var/www/lmce-admin/operations/cloudServices/google-api-php-client/src');
require_once 'Google/Client.php';
require_once 'Google/autoload.php';

$client = new Google_Client();
$client->setAuthConfigFile('/tmp/client_secret.json');
$client->setRedirectUri('http://' . $_SERVER['HTTP_HOST'] . '/lmce-admin/operations/cloudServices/googleOauthCallback.php');
$client->addScope(Google_Service_Drive::DRIVE_METADATA_READONLY);

if (!isset($_GET['code'])) {
  $auth_url = $client->createAuthUrl();
  header('Location: ' . filter_var($auth_url, FILTER_SANITIZE_URL));
} else {	
  $client->authenticate($_GET['code']);
  $_SESSION['access_token'] = $client->getAccessToken();
  $redirect_uri = 'http://' . $_SERVER['HTTP_HOST'] . '/lmce-admin/index.php?section=cloudServices&app=gmail';
echo "code is set? ".$_GET['code'];
  header('Location: ' . filter_var($redirect_uri, FILTER_SANITIZE_URL));
}