<?
require_once('TMDb.php');
if (file_exists('moviedb.key'))
	$APIkey = rtrim(file_get_contents('moviedb.key'));
else
	die("The Movie Database API key wasn't set up. These scripts cannot be used until you put your key in the file moviedb.key");
$tmdb = new TMDb($APIkey);
?>
