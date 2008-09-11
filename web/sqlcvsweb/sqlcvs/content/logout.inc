<?
	db_select("MasterUsers");
	$result = db_query ("delete from WebSession where SessionId = '".$_SESSION['sok'] ."'");
	db_select("pluto_main");
	$_SESSION['sok']="goodbye";

?>
<div id="loginprompt">
<H3><? echo _("Logged out"); ?></H3>
<? echo _("Good bye..."); ?>
<p>
</div>

