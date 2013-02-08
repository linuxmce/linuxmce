<?
if (!isset($_REQUEST['device_id']) || empty($_REQUEST['device_id']))
{
	header("HTTP/1.0 404 Not Found");
	die;
}

$deviceID = $_REQUEST['device_id'];

require_once('include/utils.inc.php');
require_once('include/config/config.inc.php');
require_once('include/weborbiter.inc.php');

$ProxyOrbiterInfo = get_web_orbiter_info($deviceID, $dbADO);

if (count($ProxyOrbiterInfo) == 0)
{
	header("HTTP/1.0 404 Not Found");
	die;
}

$address = $ProxyOrbiterInfo['IPAddress'][0];
if (is_null($address) || $address == '')
	$address = getDeviceIP($ProxyOrbiterInfo['FK_Device_ControlledVia'][0], $dbADO);
if (is_null($address))
	$address = getCoreIP($dbADO);
$port = $ProxyOrbiterInfo['Port'][0];

$imageData = false;
if (!isset($_REQUEST['file']))
{
	$socket = openSocket($address, $port);
	if ($socket === false)
	{
		header("HTTP/1.0 404 Not Found");
		die;
	}

	$imageData = getImageToRAM($deviceID, $socket);
	closeSocket($socket);
}
elseif (is_file($_REQUEST['file']))
{
	$FilePath = realpath($_REQUEST['file']);
	$AllowedPaths = array("/usr/pluto/orbiter/", "/home/mediapics/");
	$Allowed = false;
	foreach ($AllowedPaths as $Path)
	{
		if (substr($FilePath, 0, strlen($Path)) === $Path)
		{
			$Allowed = true;
			break;
		}
	}
	if (!$Allowed)
	{
		header("HTTP/1.0 404 Not Found");
		die;
	}
	$imageData = file_get_contents($FilePath);
}

if ($imageData === false)
{
	header("HTTP/1.0 404 Not Found");
	die;
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

if (substr($imageData, 0, 8) === "\x89PNG\r\n\0x1A\n")
	$imagetype = "png";
else
	$imagetype = "jpg";

header("Content-type: image/$imagetype");
echo "$imageData";
?>
