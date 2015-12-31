<?php
// include language files
includeLangFile('common.lang.php');
includeLangFile('sipConf.lang.php');

function sipConf($output,$astADO,$dbADO,$telecomADO) {
	global $wikiHost, $editedID, $editdata;

	// check if database asterisk exists
	$databases=$dbADO->MetaDatabases();
	if(!in_array('asterisk',$databases)){
		error_redirect(translate('TEXT_ASTERISK_DB_NOT_FOUND_CONST'),'index.php?section=sipConf');
	}

	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out=setLeftMenu($dbADO);

	$out .= sipConfForm($astADO);

	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$editedID=@$_REQUEST['eid'];

	$output->setMenuTitle(translate('TEXT_TELECOM_CONST').' |');
	$output->setPageTitle(translate('TEXT_SIP_CONF_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_SIP_CONF_CONST')=>'index.php?section=sipConf'));

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_SIP_CONF_CONST'));
	$output->output();

}

function is_digits($element) {
	return !preg_match("/[^0-9]/", $element);
}

function sipConfForm($astADO){
	global $editedID;
	global $editdata;
	$GLOBALS['count']=0;

	$checkboxes = array("videosupport", "notifyringing", "notifyhold", "limitonpeers", "rtcachefriends", "rtupdate", "tcpenable", "alwaysauthreject");
	$initText = array("bindport", "registerattempts", "registertimeout");
	$ipText = array("bindaddr", "tcpbindaddr");
	$noValidate = array("tos", "callerid", "language");

	$availableFields = array_merge($checkboxes, $initText, $ipText, $noValidate);

	$out = '<script type="text/javascript" src="javascript/wz_tooltip/wz_tooltip.js"></script>';

	// WRITE FORM
	if (!empty($_POST)) {

		//   HANDLE CHECKBOXES
		foreach($checkboxes as &$value) {
			if (!isset($_POST['sip'][$value])) {
				$astADO->Execute("UPDATE `asterisk`.`ast_config` SET `var_val` = 'no' WHERE `ast_config`.`var_name` = '" . $value . "';");
			} else {
				$astADO->Execute("UPDATE `asterisk`.`ast_config` SET `var_val` = 'yes' WHERE `ast_config`.`var_name` = '" . $value . "';");
			}
		}

		//   HANDLE INIT
		foreach($initText as &$value) {

			if(is_digits($_POST['sip'][$value])) {
				$astADO->Execute("UPDATE `asterisk`.`ast_config` SET `var_val` = '" . $_POST['sip'][$value] . "' WHERE `ast_config`.`var_name` = '" . $value . "';");
			} else {
				$out .= '<div style="color: #ff0000; font-weight: bold;">' . translate('TEXT_VALID_INT_CONST') . ' '. $value .'</div>';
			}

		}

		//   HANDLE IP
		foreach($ipText as &$value) {

			if (ip2long($_POST['sip'][$value]) !== false) {
				$astADO->Execute("UPDATE `asterisk`.`ast_config` SET `var_val` = '" . $_POST['sip'][$value] . "' WHERE `ast_config`.`var_name` = '" . $value . "';");
			} else {
				$out .= '<div style="color: #ff0000; font-weight: bold;">' . translate('TEXT_VALID_IP_CONST') . ' '. $value .': '. $_POST['sip'][$value] .'</div>';
			}
		}

		//   HANDLE NO VALIDATION
		foreach($noValidate as &$value) {
			$astADO->Execute("UPDATE `asterisk`.`ast_config` SET `var_val` = '" . $_POST['sip'][$value] . "' WHERE `ast_config`.`var_name` = '" . $value . "';");
		}

	}

	// READ FORM
	$res=$astADO->Execute("SELECT * FROM `ast_config` WHERE `filename` LIKE 'sip.conf' AND `var_name` IN ('".implode("', '", $availableFields)."')");

	$out .='
		<form action="./index.php?section=sipConf" method="post">
		<table cellpadding="3" cellspacing="0">';

	while($row=$res->FetchRow()) {
		if($row['var_name'] != 'allow' && $row['var_name'] !== 'disallow' && $row['var_name'] !== 'register' && $row['var_name'] !== 'context') {
			$out .= '<tr><td width="200">
				<label style="font-weight: bold">'. translate('TEXT_' . strtoupper($row['var_name']) .'_CONST') . '</label></td><td>';

			if($row['var_val'] == 'yes' || $row['var_val'] == 'on' && $row['var_val'] != '0') {
				$out .= '<input name="sip['. $row["var_name"] . ']" type="checkbox" checked="checked">';
			} elseif($row['var_val'] == 'no' || $row['var_val'] == '0') {
				$out .= '<input name="sip['. $row["var_name"] . ']" type="checkbox">';
			} else {
				$out .= '<input name="sip['. $row["var_name"] . ']" type="text" value="' . $row['var_val'] .'">';
			}

			$out .= '</td><td><img onmouseout="UnTip()" onmouseover="Tip(\'' . translate('TEXT_' . strtoupper($row['var_name']) .'_TIP') . ' '. '\',ABOVE,true,WIDTH,350,FADEIN,250,FADEOUT,250,BGCOLOR,\'#C0FFC0\')" src="img/question.gif"></td></tr>';
		}
	}

	$out.='</table>
			<input type="submit" value="save values">
			</form>';
	return $out;
}
?>
