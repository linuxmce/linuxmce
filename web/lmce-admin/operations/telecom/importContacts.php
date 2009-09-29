<?php

function importContacts($output,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	include('include/vcard.inc.php');
		
	$title = $TEXT_IMPORT_CONTACTS_CONST;
	
	$action = isset($_REQUEST['action']) ? cleanString($_REQUEST['action']) : '';
	
	$check_duplicates = isset($_REQUEST['check_dupl']) ? true : false;
	$overwrite_existing = isset($_REQUEST['overwrite_ex']) ? true : false;

	if($action == 'import') {
		
		$file = $_FILES['vcard_file']['tmp_name'];
	
		if (!$file) {
			error_log("File vCards is not specified!");
			exit('Required $file parameter not specified.');
		}

		$lines = file($file);
		if (!$lines) {
			error_log("Can't read the vCard file: $file");
			exit("Can't read the vCard file: $file");
		}
		$cards = parse_vcard_file($lines);
		$result = process_vcards($cards);
		$ret = insert_data($result, $telecomADO, $check_duplicates, $overwrite_existing);
		header("Location: index.php?section=phoneBook&msg=".@$ret);
		exit();
	} else {
		
		$out.='
		
		<div class="err" align="center">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm" align="center">'.@$_GET['msg'].'</div>
			
		<form action="index.php" method="POST" name="vcard"  enctype="multipart/form-data">
		<input type="hidden" name="section" value="importContacts">
		<input type="hidden" name="action" value="import">
		
		<table width="40%">
		<tr>
			<td colspan="2"><b>
				'.$TEXT_IMPORT_CONTACTS_NOTE_CONST.'	
			</b></td>
		</tr>
		<tr><td colspan="2" hight="15px">&nbsp;</td></tr>
		<tr>
			<td><b>'.$TEXT_IMPORT_CONTACTS_OPT_CONST.'</b></td>
			<td>
				<input type="checkbox" name="check_dupl" value=""> '.$TEXT_IMPORT_CONTACTS_DUP_CONST.' 
				<input type="checkbox" name="overwrite_ex" value=""> '.$TEXT_IMPORT_CONTACTS_OVER_CONST.' 
			</td>
		</tr>
		<tr>
			<td><b>'.$TEXT_UPLOAD_VCARD_FILE_CONST.'</b></td>
			<td>
				<input type="file" name="vcard_file" value="">
			</td>
		</tr>
		<tr>
			<td colspan="2" align="center">
				<input type="submit" name="import" class="button" value="Import">
			</td>
		</tr>
		<tr>
			<td colspan="2">'.$TEXT_IMPORT_CONTACTS_OPT_NOTE_CONST.'</td>
		</tr>
		</table>
		</form>
		';
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_BOOK_CONST=>'index.php?section=phoneBook',$title=>'index.php?section=addPhoneNumber&addContact='.$addContact));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$title);
	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($title);
	$output->output();

}

function parse_vcard_file(&$lines)
{
    $cards = array();
    $card = new VCard();
    while ($card->parse($lines)) {
        $property = $card->getProperty('N');
        if (!$property) {
            return "";
        }
        $n = $property->getComponents();

        $tmp = array();
        if ($n[3]) $tmp[] = $n[3];      // Mr.
        if ($n[1]) $tmp[] = $n[1];      // John
        if ($n[2]) $tmp[] = $n[2];      // Quinlan
        if ($n[4]) $tmp[] = $n[4];      // Esq.
        $ret = array();
        if ($n[0]) $ret[] = $n[0];
        $tmp = join(" ", $tmp);
        if ($tmp) $ret[] = $tmp;
        $key = join(", ", $ret);
        $cards[$key] = $card;
        $card = new VCard();
    }
    ksort($cards);
    return $cards;
}

function process_vcards ($cards) {
    // Mapping between vCard phone types and LMCE
	$phone_types = array(
		'HOME' => 1,
		'WORK' => 2,
		'CELL' => 3,
	);
	
	// VCard property types
	$property_types = array('FN', 'TITLE', 'ORG', 'TEL', 'EMAIL', 'URL', 'ADR', 'BDAY', 'NOTE');

	$userID = (int)@$_SESSION['userID'];
	
	$result = array();
	$count = 0;
	foreach ($cards as $card_name => $card) {
		
		$phones = array();

    	foreach ($property_types as $type) {
        	$properties = $card->getProperties($type);
        	if ($properties) {
            	foreach ($properties as $property) {
            		$components = $property->getComponents();
					$name = $property->name;
					$value = $property->value;
					
					error_log("[$card_name] $name => $value: $phone_type");   		
					
					switch($type) {
						case 'FN':
							$result[$card_name]['Name'] = $value;
						break;
						case 'TEL':
    						$phone_type = $property->params['TYPE'][0];
							array_push($phones, array('Number' => $value, 'Type' => $phone_types[$phone_type]));
						break;
						case 'TITLE':
							$result[$card_name]['Title'] = $value;
						break;
						case 'ORG':
							$result[$card_name]['Company'] = $value;
						break;	
					}

					$result[$card_name]['Tel'] = $phones;
				}
    		}

		}

	}	
	
	return $result;
}

function insert_data($data, $telecomADO, $check_duplicates=false, $overwrite_existing=false) {
	
	$userID = (int)@$_SESSION['userID'];
	
	$cont_count = 0;
	$phone_count = 0;
	foreach($data as $name => $item) {
		$ret = $telecomADO->Execute('
				INSERT INTO Contact
					(`Name`, `Company`, `Title`, `EK_Users`)
				VALUES
					(?,?,?,?)',
			array($item['Name'],$item['Company'],$item['Title'],$UserID));
		
		$cont_count++;
		
		$contact = $telecomADO->Insert_ID();

		foreach($item['Tel'] as $phone) {
			$telecomADO->Execute('
				INSERT INTO PhoneNumber
					(`FK_Contact`, `FK_PhoneType`, `CountryCode`, `AreaCode`, `PhoneNumber`, `Extension`, `DialAs`)
				VALUES
					(?,?,?,?,?,?,?)',
				array($contact,$phone['Type'], '', '', $phone['Number'], '', ''));
			$phone_count++;	
		}
	}

	$result = "Added $cont_count contacts and $phone_count phone numbers";

	return $result;
}

?>
