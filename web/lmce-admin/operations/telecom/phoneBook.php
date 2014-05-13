<?php
function phoneBook($output,$telecomADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	if ($action == 'form') {
		
		$out.='
		
	<div class="err">'.@$_GET['error'].'</div>
	<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="phoneBook">
	<input type="hidden" name="section" value="phoneBook">
	<input type="hidden" name="action" value="form">	
		'.phonesTable($telecomADO,$userID,$page).'
	</form>
	';
	} else {

		// process delete
		$did=(int)@$_REQUEST['did'];
		if($did>0){
			$telecomADO->Execute('DELETE FROM PhoneNumber WHERE PK_PhoneNumber=?',$did);
			$msg=$TEXT_PHONE_NUMBER_DELETED_CONST;
		}
			
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($TEXT_PHONE_BOOK_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_BOOK_CONST=>'index.php?section=phoneBook'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PHONE_BOOK_CONST);
	$output->output();
}

// multipage display for phone numbers
function phonesTable($telecomADO,$userID,$page){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/telecom.lang.php');
	
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	$records_per_page=10;
	$PhoneTypes=getAssocArray('PhoneType','PK_PhoneType','Description',$telecomADO);

	if(isset($_REQUEST['search']) && $_REQUEST['search']!=''){
		$filter=' AND (1=1 ';
		if(!isset($_REQUEST['byContact']) || $_REQUEST['byContact']==1){
			$byContactChecked='checked';
			$filter.=(@$_REQUEST['search']!='')?" AND Contact.Name LIKE '%".$_REQUEST['search']."%'":"";
		}
		if(!isset($_REQUEST['byCompany']) || $_REQUEST['byCompany']==1){
			$byCompanyChecked='checked';
			$filter.=(@$_REQUEST['search']!='')?" OR Contact.Company LIKE '%".$_REQUEST['search']."%'":"";
		}
		if(!isset($_REQUEST['byNumber']) || $_REQUEST['byNumber']==1){
			$byNumberChecked='checked';
			$filter.=(@$_REQUEST['search']!='')?" OR DialAs LIKE '%".$_REQUEST['search']."%'":"";
		}
		$filter.=') ';	
		if(isset($_REQUEST['byPhoneType']) && $_REQUEST['byPhoneType']!=0){
			$filter.=(@$_REQUEST['search']!='')?" AND FK_PhoneType = '".$_REQUEST['byPhoneType']."'":"";
		}	
	}else{
		$byContactChecked='checked';
		$byCompanyChecked='checked';
		$byNumberChecked='checked';
	}

	$res=$telecomADO->Execute('
		SELECT * 
		FROM PhoneNumber
		INNER JOIN PhoneType ON FK_PhoneType = PK_PhoneType
		INNER JOIN Contact ON FK_Contact=PK_Contact
		WHERE (EK_Users IS NULL OR EK_Users=?) '.@$filter.'
		ORDER BY Name ASC,PhoneNumber ASC
		',$userID);
	$totalRecords=$res->RecordCount();
	
	$noPages=ceil($totalRecords/$records_per_page);
	$start=($page-1)*$records_per_page+1;
	$end=$page*$records_per_page;
	$end=($end>$totalRecords)?$totalRecords:$end;

	$pageLinks=$TEXT_PAGES_CONST.': ';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="index.php?section=phoneBook&page='.$i.'">'.$i.'</a>';
	}
	
	
	$out='
		<table celspacing="0" cellpadding="3" align="center">
			<tr>
				<td colspan="11" align="center"><fieldset><legend><B>'.$TEXT_SEARCH_PHONE_BOOK_CONST.'</B></legend>
					<input type="checkbox" name="byContact" value="1" '.@$byContactChecked.'>'.$TEXT_CONTACT_NAME_CONST.' 
					<input type="checkbox" name="byCompany" value="1" '.@$byCompanyChecked.'>'.$TEXT_COMPANY_NAME_CONST.' 
					<input type="checkbox" name="byNumber" value="1" '.@$byNumberChecked.'>'.$TEXT_PHONE_NUMBER_CONST.' 
					'.pulldownFromArray($PhoneTypes,'byPhoneType',(int)@$_REQUEST['byPhoneType'],'','key','All').' 
					<input type="text" name="search" value="'.@$_REQUEST['search'].'"> 
					<input type="submit" class="button" name="seatch_btn" value="'.$TEXT_SEARCH_CONST.'">
					<input type="button" class="button" name="show_all" value="'.$TEXT_SHOW_ALL_CONST.'" onClick="self.location=\'index.php?section=phoneBook\'">
				</fieldset></td>
			</tr>	
			<tr>
				<td colspan="2">Records '.$start.'-'.$end.' of '.$totalRecords.'</td>
				<td colspan="10" align="right">'.$pageLinks.'</td>
			</tr>
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_CONTACT_NAME_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_COMPANY_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_TITLE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_EMAIL_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_PHONE_TYPE_CONST.'</B></td>	
				<td align="center"><B>CC*</B></td>	
				<td align="center"><B>AC**</B></td>
				<td align="center"><B>'.$TEXT_NUMBER_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_EXTENSION_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_DIAL_AS_CONST.'</B></td>	
				<td align="center"><B>'.$TEXT_PRIVATE_CONTACT_CONST.'</B></td>	
				<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>	
			</tr>';
	if($totalRecords==0){
		$out.='
		<tr>
			<td colspan="11">'.$TEXT_NO_PHONE_NUMBERS_AVAILABLE_CONST.'</td>
		</tr>';
	}else{	
		$pos=0;
		while($row=$res->FetchRow()){
			$pos++;
			
			if($pos>=$start && $pos<=$end){
				$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';		
				
				$out.='
				<tr bgcolor="'.$color.'">
					<td align="center"><a href="index.php?section=editContact&cid='.$row['FK_Contact'].'">'.$row['Name'].'</a></td>
					<td align="center">'.$row['Company'].'</td>
					<td align="center">'.$row['Title'].'</td>
					<td align="center">'.$row['Email'].'</td>
					<td align="center">'.$row['Description'].'</td>	
					<td align="center">'.$row['CountryCode'].'</td>	
					<td align="center">'.$row['AreaCode'].'</td>
					<td align="center">'.$row['PhoneNumber'].'</td>
					<td align="center">'.$row['Extension'].'</td>
					<td align="center">'.$row['DialAs'].'</td>	
					<td align="center">'.((!is_null($row['EK_Users']))?'Y':'N').'</td>	
					<td align="center">
						<input type="button" class="button" name="edit" value="'.$TEXT_EDIT_CONST.'" onClick="self.location=\'index.php?section=editPhoneNumber&id='.$row['PK_PhoneNumber'].'\'"> 
						<input type="button" class="button" name="delete" value="'.$TEXT_DELETE_CONST.'" onclick="if(confirm(\''.$TEXT_DELETE_NUMBER_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=phoneBook&action=del&did='.$row['PK_PhoneNumber'].'\'"></td>	
				</tr>';
			}
	
		}
	$out.='
		<tr>
			<td colspan="2">'.$TEXT_RECORDS_CONST.' '.$start.'-'.$end.': '.$totalRecords.'</td>
			<td colspan="10" align="right">'.$pageLinks.'</td>
		</tr>';
	}
	$out.='	
		<tr>
			<td colspan="11" align="center">
				<input type="button" class="button" name="add" value="'.$TEXT_ADD_PHONE_NUMBER_CONST.'" onClick="self.location=\'index.php?section=addPhoneNumber\'"> 
				<input type="button" class="button" name="add" value="'.$TEXT_ADD_CONTACT_CONST.'" onClick="self.location=\'index.php?section=addPhoneNumber&addContact=1\'">
				<input type="button" class="button" name="add" value="'.$TEXT_IMPORT_CONTACTS_CONST.'"onClick="self.location=\'index.php?section=importContacts\'">	
			</td>
		</tr>	
		<tr>
			<td colspan="11" align="left">	* CC='.$TEXT_COUNTRY_CODE_CONST.'<br>
	** AC='.$TEXT_AREA_CODE_CONST.'</em></td>
		</tr>	
	</table>
	<em>';
			
	return $out;
}
?>
