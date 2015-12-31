<?
$dbUser = 'root';
$dbPass = '';
$dbDatabase = 'pluto_main';
$dbServer = 'localhost';

//-you must not modify anything bellow this line
$conn = mysql_connect($dbServer, $dbUser, $dbPass) or die('could not connect to database');
$db = mysql_selectdb($dbDatabase, $conn) or die("could not select $dbDatabase");
?>
