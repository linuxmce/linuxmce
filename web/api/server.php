<?php

//this is the api that grants access to the incoming web app.
//http://bshaffer.github.io/oauth2-server-php-docs/cookbook/
//test db is assumed setup already.

$dsn= 'mysql:dbname=oauth2_db;host=localhost';
$username = "root";
$password = "";
include('oauth2-server-php/src/OAuth2/Autoloader.php');
ini_set('display_errors', 1); error_reporting(E_ALL);

OAuth2\Autoloader::register();

// $dsn is the Data Source Name for your database, for exmaple "mysql:dbname=my_oauth2_db;host=localhost"
$storage = new OAuth2\Storage\Pdo(array('dsn' => $dsn, 'username' => $username, 'password' => $password));


// Pass a storage object or array of storage objects to the OAuth2 server class
$server = new OAuth2\Server($storage, array(
    'allow_implicit' => true,
));

// create the grant type
$grantType = new OAuth2\GrantType\RefreshToken($storage);

// add the grant type to your OAuth server
$server->addGrantType($grantType);

// Add the "Client Credentials" grant type (it is the simplest of the grant types)
$server->addGrantType(new OAuth2\GrantType\ClientCredentials($storage));

// Add the "Authorization Code" grant type (this is where the oauth magic happens)
$server->addGrantType(new OAuth2\GrantType\AuthorizationCode($storage));
$defaultScope = 'basic';
$supportedScopes = array(
  'basic',
  'postonwall',
  'accessphonenumber',
  'www.googleapis.com/auth/calendar'
);
$memory = new OAuth2\Storage\Memory(array(
  'default_scope' => $defaultScope,
  'supported_scopes' => $supportedScopes
));
$scopeUtil = new OAuth2\Scope($memory);

$server->setScopeUtil($scopeUtil);
?>