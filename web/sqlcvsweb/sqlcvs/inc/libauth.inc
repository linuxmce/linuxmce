<?

function authUserPass($username, $password) {
	db_select("MasterUsers");
	$tmpresult = db_query("select PK_MasterUsers from MasterUsers where Username = '".$username."' and Password = '".md5($password)."'");
	db_select("pluto_main");
	if (mysql_num_rows($tmpresult)) {
		return 1;
	} else {
		return 0;
	}
}

?>
