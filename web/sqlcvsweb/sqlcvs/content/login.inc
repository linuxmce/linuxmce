<? if ($_POST['username']!="" && $_POST['password']!="") {

	 if (authUserPass($_POST['username'],$_POST['password']))  {
		$sessionkey=md5 (uniqid (rand()));
		// print "KEY: ".$sessionkey."<br>";
		db_select("MasterUsers");
		$result = db_query("insert into WebSession set SessionId='". $sessionkey."', Username='".$_POST['username']."'");
		db_select("pluto_main");
		$_SESSION['sok']=$sessionkey; 
		$_SESSION['locale']=$_POST['locale']; 
?>
		<div id="loginprompt"><h3><? echo _("Login successful"); ?></h3><? echo _("You are logged in"); ?>.<p></div>
	<?  } else { ?>
		print "failed";
		<div id="loginprompt"><h3>
			<? echo _("Login failed"); ?>
			</h3>
			<? echo _("Please verify your credentials"); ?><p>
			<a href="/"><? echo _("Try again");?></A>
		</div>
<?	} 
} else { ?>
<div id="loginprompt">
<h3><? echo _("Login"); ?></h3>
<FORM METHOD="POST" ACTION="?site=login">
<? echo _("Username");?>: <INPUT TYPE="TEXT" NAME="username"><br>
<? echo _("Password");?>: <INPUT TYPE="password" NAME="password"><br>
<? echo _("Language");?>: <SELECT NAME="locale">
<OPTION VALUE="en_US"><? echo _("English"); ?>
<OPTION VALUE="de_DE"><? echo _("Deutsch"); ?>
</SELECT>
<INPUT TYPE="submit" NAME="login" VALUE="<? echo _("Login");?>">
</FORM>
</div>
<? } ?>
