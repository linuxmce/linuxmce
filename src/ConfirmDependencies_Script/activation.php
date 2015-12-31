<?
/* Based on the "code" GET parameter, return a message and a one-per-row list of packages following it
 * if code is ok: return "OK\npackage1\npacket2\n..."
 * else: return "ERROR. Invalid code.\n"
 */


require('include/config/config.inc.php');
require('include/utils.inc.php');
header("Content-type: text/plain");

$signup_url = "http://www.plutohome.com/signup.html";

$code = $_GET["code"];

// sanity checks
if (! isset($code) || $code === "" || preg_match("/^\d+-\d+$/", $code) != 1 || ! CheckValidCode($code,$dbADO))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = GetActivationSh($code);
}

echo "$message";

// return true if code is valid, false if not
?>

