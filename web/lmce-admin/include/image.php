<?
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
require('utils.inc.php');

$imagepath=cleanString($_REQUEST['imagepath']);
$imageinfo = getimagesize( $imagepath );
switch ($imageinfo[2]){
	case 2:
		$imagetype = "jpg";
	break;
	case 3:
		$imagetype = "png";
	break;
	case 6:
		$imagetype = "bmp";
	break;	
	case 15:
		$imagetype = "wbmp";
	break;	
	default:
		header( "HTTP/1.0 404 Not Found" );
		exit ;
	break;
}

header( "Content-type: image/$imagetype" );
readfile( $imagepath );
?>