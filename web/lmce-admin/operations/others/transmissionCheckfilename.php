<?php
//Check the file name entered against the directory pointed to, if it already exists, warn accordingly.  Only 2 possible outputs, nothing, and "Filename already exists!".
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');

//get the q parameter from URL
$filename=$_GET["q"];

//lookup all hints from array if length of q>0
if (file_exists(stripslashes($filename))){
	$response=$TEXT_TRANSMISSION_FILE_EXISTS;
}else{
	$response="";
};

echo $response
?>
