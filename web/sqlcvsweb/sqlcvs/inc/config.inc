<?
require_once("libhelpers.inc");
require_once("libmysql.inc");
require_once("libauth.inc");

// language_configuration
// $default_locale = "en_US";
$default_locale = "de_DE";

// database configuration
$db_host='localhost';
$db_user='root';
$db_pass='';

db_connect($db_host,$db_user,$db_pass);
?>
