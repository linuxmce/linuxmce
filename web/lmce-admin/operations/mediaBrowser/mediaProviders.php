<?
function mediaProviders($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaProviders.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$mtArray=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');
	$mtArray=array_map('cleanMediaType',$mtArray);
	
	if($action=='form'){
		
		$mpArray=getFieldsAsArray('MediaProvider','PK_MediaProvider,Description,EK_MediaType',$mediadbADO,'','ORDER BY Description ASC');
		$mpRows='';
		for($i=0;$i<count(@$mpArray['PK_MediaProvider']);$i++){
			$color=($i%2!=0)?'#F0F3F8':'#FFFFFF';
			$mpRows.='
			<tr bgcolor="'.$color.'">
				<td>'.$mpArray['Description'][$i].'</td>
				<td>'.@$mtArray[$mpArray['EK_MediaType'][$i]].'</td>
				<td><a href="javascript:if(confirm(\'Are you sure you want to delete this media provider?\'))self.location=\'index.php?section=mediaProviders&action=del&dmp='.$mpArray['PK_MediaProvider'][$i].'\'">Delete</a></td>
			</tr>		
			';
		}
		if(count(@$mpArray['PK_MediaProvider'][0])==0){
			$mpRows.='
			<tr>
				<td colspan="3" align="center">'.$TEXT_NO_MEDIA_PROVIDERS_CONST.'</td>
			</tr>		
			';
		}
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="mediaProviders">
			<input type="hidden" name="section" value="mediaProviders">
			<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" cellspacing="0" align="center">
			<tr class="tablehead">
				<td colspan="3" align="center"><B>'.$TEXT_MEDIA_PROVIDERS_CONST.'</B></td>
			</tr>		
			<tr bgcolor="#F0F3F8">
				<td align="center"><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_MEDIA_TYPE_CONST.'</B></td>
				<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>		
			'.$mpRows.'
			<tr>
				<td colspan="3" align="center">&nbsp;</td>
			</tr>		

			<tr class="tablehead">
				<td colspan="3" align="center"><B>'.$TEXT_ADD_MEDIA_PROVIDER_CONST.'</B></td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td colspan="2"><input type="text" name="description" value=""></td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_MEDIA_TYPE_CONST.'</B></td>
				<td colspan="2">'.pulldownFromArray($mtArray,'type',0).'</td>
			</tr>		
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_MEDIA_PROVIDER_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>		
		</table>
		
		</form>';
	}else{
		if((int)@$_REQUEST['dmp']>0){
			$mediadbADO->Execute('DELETE FROM MediaProvider WHERE PK_MediaProvider=?',(int)$_REQUEST['dmp']);
			header('Location: index.php?section=mediaProviders&msg='.$TEXT_MEDIA_PROVIDER_WAS_DELETED_CONST);
			exit();
		}
		
		$description=stripslashes($_POST['description']);
		$type=(int)$_POST['type'];
		if($description!='' && $type>0){
			$mediadbADO->Execute('INSERT INTO MediaProvider (Description,EK_MediaType) VALUES (?,?)',array($description,$type));
			if($mediadbADO->Affected_Rows()>0)
				$msg=$TEXT_THE_MEDIA_PROVIDER_WAS_ADDED_CONST;
		}
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=mediaProviders'.$suffix);
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MEDIA_PROVIDERS_CONST);
	$output->setNavigationMenu(array($TEXT_MEDIA_PROVIDERS_CONST=>'index.php?section=mediaProviders'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MEDIA_PROVIDERS_CONST);
	$output->output();
}
?>