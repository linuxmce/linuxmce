<?php
/*!
\page MediaImageFactory Http image api
 This file serves as a simple way to access images for media via qOrbiter or any other devices for that matter.
 It takes a single parameter, the file name and returns it if it exists
*/
$file =$_GET['img'];
$img =file_get_contents("mediapics/".$file);
echo $img
?>


