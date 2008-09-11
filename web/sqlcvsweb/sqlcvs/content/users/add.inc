<h1><? echo _("Add user") ?></h1>
<? if ($_POST['Username']!="" && $_POST['Password']!="" && $_POST['Email']!="" && $_POST['sqlCvsAdmin']!="") {
	db_select("MasterUsers");
	$queryEmail=db_query("SELECT * from MasterUsers WHERE Email='".$_POST['Email']."'");
	$queryUsername=db_query("SELECT * from MasterUsers WHERE Username='".$username."'");
	if(db_num_rows($queryEmail)!=0){
		print _("ERROR").": "._('MasterUsers Email already exists');
	}elseif(db_num_rows($queryUsername)!=0){
		print _("ERROR").": "._('MasterUsers Username already exists');
	}else{
		// add user to MasterUsers table
		$prefix="PV";
		$pin=calculatePIN();
		$signupDate=date("YmdHis");
		$addquery = "INSERT INTO MasterUsers set PK_MasterUsers='',PlutoId='".$prefix."',Pin='".$pin."',FirstAccount='Users'," .
			"Username='".$_POST['Username']."',Password='".$_POST['Password']."',PwdIsTemp=0,Email='".$_POST['Email']."'," .
			"SignupDate='".$signupDate."',RecordVersion=1";
		print "<br><pre>".$addquery."</pre><br>";
		$insertMasterUsers=mysql_query($addquery);
		$PK_MasterUsers=mysql_insert_id($insertMasterUsers);
		print "PK_MU:".$PK_MasterUsers."<BR>";
		$plutoId=calculatePlutoID($PK_MasterUsers);
		$updateMasterUsers=db_query("UPDATE MasterUsers SET PlutoId='".$plutoId."' WHERE PK_MasterUsers='".$PK_MasterUsers."'");
		echo _("User added");
	}
}


?>
