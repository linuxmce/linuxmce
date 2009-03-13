<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/weborbiter.inc.php');

// retrieve variables
$username=$_REQUEST['username'];
$pass=$_REQUEST['password'];

$userID=user_exists($username,$pass,$dbADO);
if($userID!==false){
	header("Location: weborbiter.php?userID=$userID&pass=".md5($pass));
	exit();
}else{
	header("Location: weborbiter.php?err=1");
	exit();
}
?>