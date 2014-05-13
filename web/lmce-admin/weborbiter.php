<?php
require_once('include/config/config.inc.php');
require_once('include/utils.inc.php');
require_once('include/weborbiter.inc.php');

require_once("weborbiter/IT.php");

function GetVar($VarName, $Default = "")
{
	if (isset($_REQUEST[$VarName]))
		$Value = $_REQUEST[$VarName];
	else
		$Value = $Default;
	return $Value;
}

$installationID = GetVar('installation', 0);
$deviceID = GetVar('device_id', 0);
$action = GetVar('action');

session_start();

$IT = new HTML_Template_IT("./weborbiter");

switch ($action)
{
case 'login':
	$username = GetVar('username');
	$password = GetVar('password');
	$userID = user_exists($username, $password, $dbADO);
	if ($userID !== false)
	{
		$_SESSION['userID'] = $userID;
		$_SESSION['passMD5'] = md5($password);
		header("Location: weborbiter.php");
	}
	else
	{
		ShowLoginPage(true);
	}
break;
case 'logout':
	Logout();
	header("Location: weborbiter.php");
break;
case 'anynews':
	if (!isset($_SESSION['userID']) || !isset($_SESSION['passMD5']) || empty($_SESSION['userID']) || empty($_SESSION['passMD5']))
		ShowNews("no");
	elseif (!is_valid_user($_SESSION['userID'], $_SESSION['passMD5'], $dbADO))
		ShowNews("no");
	elseif ($deviceID == 0)
		ShowNews("no");
	else
		ShowNews("yes");
break;
default:
	if (!isset($_SESSION['userID']) || !isset($_SESSION['passMD5']) || empty($_SESSION['userID']) || empty($_SESSION['passMD5']))
		ShowLoginPage();
	elseif (!is_valid_user($_SESSION['userID'], $_SESSION['passMD5'], $dbADO))
		ShowLoginPage();
	elseif ($deviceID == 0)
		ShowOrbiterSelectionPage();
	else
		ShowOrbiter();
}

function Logout()
{
	unset($_SESSION['userID']);
	unset($_SESSION['passMD5']);
}

function ShowLoginPage($error = false)
{
	global $IT;

	Logout();

	$IT->loadTemplatefile("login.html");
	$IT->touchBlock("__global__");
	if ($error !== false)
	{
		$IT->setCurrentBlock("ErrorMessage");
		$IT->setVariable("ErrorMessage", "Login failed");
		$IT->parseCurrentBlock();
	}
	$IT->show();
}

function ShowOrbiterSelectionPage()
{
	global $IT;
	global $deviceID;
	global $installationID;
	global $dbADO;

	$IT->loadTemplatefile("weborbiter_select.html");
	$IT->touchBlock("__global__");

	$installations = get_installation_array($_SESSION['userID'], $dbADO, $installationID);

	if (count($installations) == 0)
	{
		$IT->touchBlock("HasNoInstallation");
	}
	else
	{
		if (count($installations) == 1)
		{
			$instID = array_keys($installations);
			$instID = $instID[0];
			$installationID = $instID;
			$IT->setCurrentBlock("OneInstallation");
			$IT->setVariable("InstallationNumber", $instID);
			$IT->parseCurrentBlock();
		}
		else
		{
			foreach ($installations as $instID => $instDescription)
			{
				$IT->setCurrentBlock("InstallationOption");
				$IT->setVariable("InstallationNumber", $instID);
				$IT->parseCurrentBlock();
			}
		}

		$devices = get_devices_array($installationID, $_SESSION['userID'], $dbADO);
		if (count($devices) == 0)
		{
			$IT->touchBlock("HasNoDevice");
		}
		else
		{
			foreach ($devices as $devID => $Description)
			{
				$IT->setCurrentBlock("DeviceOption");
				$IT->setVariable("DeviceNumber", $devID);
				$ProxyOrbiterInfo = get_web_orbiter_info($devID, $dbADO);
				$IT->setVariable("DeviceDescription", $ProxyOrbiterInfo['PK_Device'][0] . " " . $ProxyOrbiterInfo['Description'][0]);
				$IT->parseCurrentBlock();
			}
		}
	}

	$IT->show();
}

function ShowOrbiter()
{
	global $IT;
	global $deviceID;
	global $installationID;
	global $dbADO;

	$IT->loadTemplatefile("weborbiter.html");
	$IT->touchBlock("__global__");

	$ProxyOrbiterInfo = get_web_orbiter_info($deviceID, $dbADO);

	if (count($ProxyOrbiterInfo) == 0)
	{
		$IT->touchBlock("NotFound");
	}
	else
	{
		$IT->setCurrentBlock("Orbiter");
		$IT->setVariable("ImgRandom", rand(10000, 99000));
		$IT->setVariable("DeviceNumber", $deviceID);
		$IT->parseCurrentBlock();

		$IT->setCurrentBlock("__global__");
		$IT->setVariable("DeviceNumber", $deviceID);
		$IT->parseCurrentBlock();
	}

	$IT->show();
}

function ShowNews($news)
{
	global $IT;
	global $deviceID;
	global $installationID;
	global $dbADO;

	$ProxyOrbiterInfo = get_web_orbiter_info($deviceID, $dbADO);

	header("Content-type: text/plain; charset=utf-8");

	if (count($ProxyOrbiterInfo) == 0)
	{
		echo "no";
		return;
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
		echo "no";
		return;
	}
	$News = getNews($socket);
	closeSocket($socket);

	echo "$News";
}
?>
