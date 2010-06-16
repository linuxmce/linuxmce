<?php
function emailSetup($output,$dbADO) {
        // include language files
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
        include(APPROOT.'/languages/'.$GLOBALS['lang'].'/emailSetup.lang.php');

	// If the settings have been selected go ahead and process them
	if(!empty($_POST["emailName"]) && ($_POST["emailAddress"]) && ($_POST["emailPassword"]) && ($_POST[emailservice])) {
		$name = $_POST["emailName"];
		$email = $_POST["emailAddress"]; 
		$password = $_POST["emailPassword"]; 
		$emailservice = $_POST["emailservice"];

		// Lookup the installationID
		$installationID = cleanInteger($_SESSION['installationID']);

		// Build our query for the country's number
		$query = "
			SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version 
			WHERE PK_Installation = ?";

		// Lookup the selected country's number
		$selectInstallation = $dbADO->Execute($query,array($installationID));
	
		if ($selectInstallation) {
			$rowInstallation = $selectInstallation->FetchRow();			
		}

		$selectedCountry=(isset($_POST['countryID']))?(int)$_POST['countryID']:$rowInstallation['FK_Country'];	

		// Now lookup the selected country's number and return the description
		$query="SELECT Country.country_code FROM Country WHERE PK_Country=$selectedCountry";
		$selectedCountry=$dbADO->Execute($query);

		// Quick n Dirty hack to strip off country_code...SQL query seems to be returning the column name from the DB
		list($country_code, $selectedCountry) = explode("country_code", $selectedCountry);

		// Trim off any whitespace just for good measure
		$selectedCountry=trim($selectedCountry);

		// Lookup the state
		$state = ((isset($_REQUEST['State']))?$_REQUEST['State']:$rowInstallation['State']);

		switch ($emailservice) {
			case "gmail":
				$port = "587";
				$mta = "smtp.gmail.com";
				$tls = "yes";
				$org = "LinuxMCE";
			break;
	
			case "hotmail":
				$port = "587";
				$mta = "smtp.live.com";
				$tls = "yes";
				$org = "LinuxMCE";
			break;
	
			case "yahoo":
				$port = "25";
				$mta = "smtp.mobile.mail.yahoo.com";
				$tls = "no";
				$org = "LinuxMCE";
			break;
	
			case "other":
				$port = $_POST[port];
				$mta = $_POST[mta];
				$tls = $_POST[tls];
				$org = "LinuxMCE";
			break;
		}
		
		// Call our script to setup postfix using our settings
		// system("sudo -u root /usr/pluto/bin/Configure_Postfix_interactive.sh '$name' '$email' '$password' '$mta' '$port' '$tls' '$org' '$state' '$selectedCountry' '$emailservice'",$retval);
		exec("sudo -u root /usr/pluto/bin/Configure_Postfix_interactive.sh '$name' '$email' '$password' '$mta' '$port' '$tls' '$org' '$state' '$selectedCountry' '$emailservice'");

		// echo "$retval <br />";

		// Uncomment to print our settings for debugging
		// echo "<br />Name  $name <br />";
		// echo "Email $email <br />";
		// echo "Password $password <br />";
		// echo "Email Service $emailservice <br />";
		// echo "Selected Country $selectedCountry <br />";
		// echo "State $state <br />";
		// echo "Port $port <br />";
		// echo "MTA $mta <br />";
		// echo "TLS $tls <br />";
		// echo "Org $org <br />";
	}

	// If the user has chosen their service provider retain their settings
	if(!empty($_POST[emailservice])) {
		$serviceselected=$_POST['emailservice'];
		switch ($serviceselected) {
			case "gmail":
				$gmail="selected";
			break;
			case "hotmail":
				$hotmail="selected";
			break;
			case "yahoo":
				$yahoo="selected";
			break;
			case "other":
				$other="selected";
			break;
		}
	}

	// Retain the user's settings for TLS
	if(!empty($_POST[tls])) {
		$tlsselected=$_POST['tls'];
		switch ($tlsselected) {
			case "yes":
				$tlsyes="selected";
			break;
			case "no":
				$tlsno="selected";
			break;
		}
	}

	// Display any status messages we have for the user
	if(!empty($_POST["emailName"]) && ($_POST["emailAddress"]) && ($_POST["emailPassword"]) && ($_POST[emailservice])) {
	$out='
	<div class="confirm" align="center"><B>'.$TEXT_SAVED_CONST.'</B></div>.';
	}

	// Display any error messages we have for the user - not in use currently
	// $out.='
	// <div class="err" align="center"></div>

	// Draw the settings page
	$out.='
	<form action="index.php?section=emailSetup" method="post" name="emailSetup">
	<table>
	<tr><td><B>'.$TEXT_EMAILSERVICE_CONST.'</B></td><td><select name="emailservice" onChange="form.submit()";>
		<option value = "gmail" '.$gmail.'>'.$TEXT_GMAIL_CONST.'</option>
		<option value = "hotmail" '.$hotmail.'>'.$TEXT_HOTMAILMSN_CONST.'</option>
		<option value = "yahoo" '.$yahoo.'>'.$TEXT_YAHOO_CONST.'</option>
		<option value = "other" '.$other.'>'.$TEXT_OTHER_CONST.'</option>
		</td></tr>
	<tr><td><B>'.$TEXT_NAMEFIRSTLAST_CONST.'</B></td><td><input type="text" name="emailName" value="'.$_POST["emailName"].'" /></td></tr>
	<tr><td><B>'.$TEXT_EMAILADDRESS_CONST.'</B></td><td><input type="text" name="emailAddress" value="'.$_POST["emailAddress"].'" /></td></tr>
	<tr><td><B>'.$TEXT_EMAILPASSWORD_CONST.'</B></td><td><input type="password" name="emailPassword" /></td></tr>.';

	// Check if other is selected and if so, display some other options
	if($serviceselected=="other") {
	$out.='
        <tr><td><B>'.$TEXT_PORT_CONST.'</B></td><td><input type="text" name="port" value="'.$_POST["port"].'" /></td></tr>
        <tr><td><B>'.$TEXT_MTA_CONST.'</B></td><td><input type="text" name="mta" value="'.$_POST["mta"].'" /></td></tr>
        <tr><td><B>'.$TEXT_TLS_CONST.'</B></td><td><select name="tls">
                <option value = "yes" '.$tlsyes.'>'.$TEXT_YES_CONST.'</option>
                <option value = "no" '.$tlsno.'>'.$TEXT_NO_CONST.'</option>
                </td></tr>.';
	}
	
	$out.='
	<tr><td colspan="2" align="right"><input type="submit" value="'.$TEXT_UPDATE_CONST.'" class="button"/></td></tr>
	</table>
	</form>';

	// Setup our page environment
	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
        $output->setPageTitle($TEXT_EMAILSETUP_CONST);
        $output->setNavigationMenu(array($TEXT_EMAILSETUP_CONST=>"index.php?section=emailSetup"));
        $output->setBody($out);
        $output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EMAILSETUP_CONST);
        $output->output();
}
?>
