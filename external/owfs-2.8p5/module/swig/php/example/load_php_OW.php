<?php

$extensionFile = "/libowphp.so";

//make sure that we are ABLE to load libraries
if( !(bool)ini_get( "enable_dl" ) || (bool)ini_get( "safe_mode" ) ) 
{

  die( "dh_local(): Loading extensions is not permitted.\n" );
}


//check to make sure the file exists
if( !file_exists( $extensionFile ) ) 
{
  die( "dl_local(): File '$extensionFile' does not exist.\n" );
}


//check the file permissions
if( !is_executable( $extensionFile ) ) 
{
  die( "dl_local(): File '$extensionFile' is not executable.\n" );
}


global $OWPHP_LOADED__;
if ($OWPHP_LOADED__) return;
$OWPHP_LOADED__ = true;

/* if our extension has not been loaded, do what we can */
if (!extension_loaded("libowphp")) {
	if (!dl("libowphp.so")) return;
}



?>
