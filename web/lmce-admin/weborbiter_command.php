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

$socket = openSocket($address, $port);
if ($socket === false)
{
	header("HTTP/1.0 404 Not Found");
	die;
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
header("Content-type: text/plain");

/* For IMAGE please use weborbiter_image.php instead */
if (isset($_REQUEST['cmd']) && !empty($_REQUEST['cmd']) && $_REQUEST['cmd'] !== 'IMAGE')
{
	$cmd = $_REQUEST['cmd'];
	echo sendCommand($deviceID, $socket, $cmd);
}
?>
