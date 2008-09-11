<?
	session_start();
	header("Content-type: text/html;charset=UTF-8");

	if (!($site = $_PUSH['site'])) $site = $_GET['site'];
	if ($site == "") $site="home";

	require_once "inc/config.inc";
	require_once "content/content.inc";

	// internationalization
	if ($_POST['locale']!="") {
		$locale = $_POST['locale'];
	} else {
		$locale=$default_locale;
		$locale=$_SESSION['locale'];
	}
	setlocale(LC_MESSAGES, $locale);
	bindtextdomain('sqlcvsweb', '/var/www/locale');
	textdomain('sqlcvsweb');

	// expire sessions after 30 mins
	db_select("MasterUsers");
	db_query("delete from WebSession where UNIX_TIMESTAMP(LastActive)<(UNIX_TIMESTAMP(NOW())-3600)");
	$sessionresult = db_query("select SessionId,Username from WebSession where SessionId = '".$_SESSION['sok'] ."'");
	if (db_num_rows($sessionresult)) {
		$sessionrow = db_fetch_row($sessionresult);
		if ($sessionrow[0]==$_SESSION['sok']) {
			$sessionuser = $sessionrow[1];
			db_query("update WebSession set LastActive = 'NOW()' where SessionId='".$_SESSION['sok'] ."'");
		}


	}
	db_select("pluto_main");

	$appname = _("sqlCVSweb");
	$appdescr = _("Management GUI");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title><?  echo $appname; ?> - <? echo $appdescr; ?>
</title>
<link rel="stylesheet" type="text/css" href="style/standard.css">
<link rel="shortcut icon" href="images/icon/favicon.ico">
</head>
<body>
<div id="head">
<div id="logo">
<IMG SRC="images/linuxmcelogo.gif" ALT="LMCE Logo">
</div>
<div id="apptitle">
<?  echo $appname; ?> - <? echo $appdescr; ?>
</div>
<div id="username">
<? if (($sessionuser!="") && ($site!="logout")) print _("User").": ".$sessionuser; ?>
</div>
</div>
<div id="menu">
<? include "content/menuleft.inc"; ?>
</div>
<div id="content">
<?  @include $content[$site]; ?>
</div>
<div id="copyright">
<? echo $copyright; ?>
</div>
</body>
</html>
<? 
?>
