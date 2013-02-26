<?php

$pass=""; $user="root"; $db="pluto_media"; $con= mysql_connect("localhost", $user, $pass); 	 // connection
	if (!$con) { die('Could not Connect'.mysql_error()); //error messaging
	$connMessage="Fail";
	};
	if ($con) {mysql_select_db("pluto_media") or die(mysql_error()); //connect to media db or error out
	$connMessage="Conn Good";
	};

$file =$_GET['imdb'];
$filemarker = substr_replace($file, "", 0, 2);

# $ref is the Picture_Attribute.FK_Picture value
$ref="";

#magic sql to determine ref go here

	
	$img =file_get_contents("mediapics/".$ref.".jpg");
echo $img;

?>