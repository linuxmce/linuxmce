<?
function checkAmazon($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	require_once("include/amazonTools.inc.php");
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=(int)@$_REQUEST['fileID'];
	
	$fileData=getFieldsAsArray('File','Filename,Path,PK_File,EK_MediaType',$mediadbADO,'WHERE PK_File='.$fileID);
	if(count($fileData)==0){
		error_redirect('Invalid file ID','');
	}
	$extension=strtolower(str_replace('.','',strrchr($fileData['Filename'][0],".")));
	$filename=str_replace('.'.$extension,'',$fileData['Filename'][0]);
	$_SESSION['lastFileID']=$fileID;
	$searchIndex=(in_array($fileData['EK_MediaType'][0],array(3,5)))?'DVD':'Music';
	
	if($action=='form'){
		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="checkAmazon">
			<input type="hidden" name="section" value="checkAmazon">
			<input type="hidden" name="action" value="process">
			<input type="hidden" name="fileID" value="'.$fileID.'">
		
		'.amazonSearch($fileData,$searchIndex,$mediadbADO).'
		</form>
		
		';
	}elseif($action=='process'){
	// process area
		if(isset($_POST['doSearch'])){
			processDoSearch($fileData,$mediadbADO);
		
			header('Location: index.php?section=checkAmazon&fileID='.$fileID.'&scanned=1');
			exit();
		}

		if(isset($_POST['syncAttributes'])){
			processScan($fileData,$mediadbADO);
		
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.urlencode('Operation successfull'));
			exit();
		}

		
		header('Location: index.php?section=checkAmazon&fileID='.$fileID);

	}
	
	$output->setReloadLeftFrame(false);
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_GET_INFO_ON_AMAZON_CONST);	
	$output->setNavigationMenu(array($TEXT_EDIT_MEDIA_FILE_CONST=>'index.php?section=editMediaFile&fileID='.$fileID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_GET_INFO_ON_AMAZON_CONST);
	$output->output();
}
function amazonSearch($fileData,$searchIndex,$mediadbADO){
	if(isset($_REQUEST['scanned'])){
		// get records from coverArtScan and display for validation
		return getScanResults($fileData['PK_File'][0],$mediadbADO);
	}else{
		return amazonSearchBox($fileData,$searchIndex,$mediadbADO);
	}
}

function amazonSearchBox($fileData,$searchIndex,$mediadbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/coverArt.lang.php');
	
	if($searchIndex=='Music'){
		// music options allowed as parameters for Amazon search index music
		$filterOptions=array(
			'Artist'=>'Artist',
			'Title'=>'Album'
		);
	}else{
		// music options allowed as parameters for Amazon search index DVD
		$filterOptions=array(
			'Actor'=>'Actor',
			'Director'=>'Director',
			'Publisher'=>'Publisher',
			'Title'=>'Title'
		);
	}	
	$id=$fileData['PK_File'][0];
	$mediaType=$fileData['EK_MediaType'][0];
	
	$filters=array();
	$rs=$mediadbADO->Execute('SELECT * FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType WHERE FK_AttributeType IN (2,3) AND FK_File=?',array($fileData['PK_File'][0]));
	while($row=$rs->FetchRow()){
		$filters[$row['FK_File']][$row['FK_AttributeType']]=$row['Name'];
	}

	
	$toSearch=substr($fileData['Filename'][0],0,strrpos($fileData['Filename'][0],'.'));
	$toSearch=str_replace(array('-','(',')'),' ',$toSearch);
			
	$filter2='';
	$filterSelected2='';
	$filter3='';
	$filterSelected3='';
			
	if(isset($filters[$id][2]) && isset($filters[$id][3]) && $mediaType==4){
				
		if(@trim($filters[$id][2])!=''){
			$filter2=@$filters[$id][2];	
			$filterSelected2='Artist';
		}
		if(@trim($filters[$id][3])!=''){
			$filter3=@$filters[$id][3];				
			$filterSelected3='Title';
		}
		$toSearch='';
	}
	
	$out='
	<table cellpadding="2" cellspacing="0">
		<tr class="alternate_back">
			<td align="center"><B>'.$TEXT_FILENAME_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_AMAZON_KEYWORD_CONST.'</B></td>
			<td colspan="4" align="center"><B>'.$TEXT_FILTERS_CONST.'</B></td>
		</tr>
	
		<tr>
			<td align="left">'.$fileData['Path'][0].'/'.$fileData['Filename'][0].'</td>
			<td align="left"><input type="text" name="Keyword1Search_'.$id.'" value="'.$toSearch.'"></td>
			<td align="left">'.pulldownFromArray($filterOptions,'Keyword2Type_'.$id,$filterSelected2).'</td>
			<td align="left"><input type="text" name="Keyword2Search_'.$id.'" value="'.@$filter2.'"></td>
			<td align="left">'.pulldownFromArray($filterOptions,'Keyword3Type_'.$id,$filterSelected3).'</td>
			<td align="left"><input type="text" name="Keyword3Search_'.$id.'" value="'.@$filter3.'"></td>
		</tr>
		<tr>
			<td align="center" colspan="6"><input type="submit" class="button" name="doSearch" value="'.$TEXT_GET_AMAZON_ATTRIBUTES_CONST.'"> <input type="button" class="button" name="syncAttributes" value="Back to file" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileData['PK_File'][0].'\'"></td>
		</tr>		
	</table>';
	return $out;
}

function processDoSearch($fileData,$mediadbADO){
	$mediaTypes=array(4=>'Music',3=>'DVD',5=>'DVD');
	$id=$fileData['PK_File'][0];
	
	$searchParms='searchIndex='.$mediaTypes[$fileData['EK_MediaType'][0]];
	// hardcoded first search parameter to "Keywords"
	$Keyword1Type='Keywords';
	$Keyword1Search=cleanString($_POST['Keyword1Search_'.$id]);
	$searchParms.=($Keyword1Search!='')?'&Keyword1Type='.$Keyword1Type.'&Keyword1Search='.urlencode($Keyword1Search):'';

	$Keyword2Type=$_POST['Keyword2Type_'.$id];
	$Keyword2Search=cleanString($_POST['Keyword2Search_'.$id]);
	$searchParms.=($Keyword2Type!='0' && $Keyword2Search!='')?'&Keyword2Type='.$Keyword2Type.'&Keyword2Search='.urlencode($Keyword2Search):'';

	$Keyword3Type=$_POST['Keyword3Type_'.$id];
	$Keyword3Search=cleanString($_POST['Keyword3Search_'.$id]);

	$searchParms.=($Keyword3Type!='0' && $Keyword3Search!='')?'&Keyword3Type='.$Keyword3Type.'&Keyword3Search='.urlencode($Keyword3Search):'';
	if($Keyword1Search=='' && $Keyword2Search=='' && $Keyword3Search==''){
		return '<span class="err">Search parameters not specified</span>';
	}
	
	// add the record to the list of items to scan
	$scanUrl='coverArtExtractor.php?fileID='.$id.'&'.$searchParms;


	// todo: remove after Mircea make his scripts
	$cmd='wget \'http://'.$_SERVER['HTTP_HOST'].'/pluto-admin/'.$scanUrl.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	//echo $cmd.'<br>';
	exec_batch_command($cmd);
}

function getScanResults($fileID,$mediadbADO){
	$attributeTypes=getAssocArray('AttributeType','Description','PK_AttributeType',$mediadbADO);	
	$existingAttributes=getAssocArray('Attribute','AttributeType.Description AS Description','Attribute.Name',$mediadbADO,'INNER JOIN File_Attribute ON FK_Attribute=PK_Attribute INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType WHERE FK_File='.$fileID);
	
	$res=$mediadbADO->Execute('
		SELECT * 
		FROM CoverArtScanEntry
		INNER JOIN CoverArtScan ON FK_CoverArtScan=PK_CoverArtScan
		WHERE FK_File=?',array($fileID));
	if($res->RecordCount()==0){
		return 'No results, please use the filters and try again.<br><a href="index.php?section=checkAmazon&fileID='.$fileID.'">Back</a>';
	}
	$out='
	<table>
		<tr class="alternate_back">
			<td colspan="2"><input type="radio" name="matched" value="0"><B> None of the this</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$id=$row['PK_CoverArtScanEntry'];
		$scanID=$row['FK_CoverArtScan'];
		$pic='&nbsp;';
		if(file_exists('/home/coverartscan/'.$id.'_tn.jpg')){
			$pic='<a href="coverartscan/'.$id.'.jpg" target="_blank"><img src="coverartscan/'.$id.'_tn.jpg" border="0"></a>';
		}
		
		$attributes='';
		
		$pos=0;
		$amazonAttributesArray=explode("\n",$row['Attributes']);
		foreach ($amazonAttributesArray AS $line){
			$parts=explode("\t",trim($line));
		
			if(count($parts)==2){
				$atype=str_replace(array('Average Customer Review', 'Actor','Directors','Artist'),array('Rating', 'Performer','Director','Performer'),$parts[0]);
				$aname=$parts[1];

				if(isset($attributeTypes[$atype])){
					$tagStart=(in_array($atype,array_keys($existingAttributes))?'<b>':'');
					$tagEnd=(in_array($atype,array_keys($existingAttributes))?'</b>':'');
					$attributes.='<b>'.$atype.':</b> '.$tagStart.$aname.$tagEnd.'<br>';
				}
				$pos++;
			}
		}
		
		$out.='
		<tr class="alternate_back">
			<td colspan="2"><input type="radio" name="matched" value="'.$id.'"> <B>ASIN:</B> '.$row['ID'].'</td>
		</tr>
		<tr>
			<td valign="top">'.$pic.'</td>
			<td valign="top">'.$attributes.'</td>
		</tr>';
	}	
	$out.='
		<tr>
			<td valign="top" colspan="2" class="alternate_back" align="center">
				<table>
					<tr>
						<td><input type="checkbox" name="remove_old" value="1"></td>
						<td>Remove existing cover arts</td>
					</tr>
					<tr>
						<td><input type="checkbox" name="remove_old_attributes" value="1"></td>
						<td>Remove existing attributes</td>
					</tr>					
					<tr>
						<td><input type="checkbox" name="includeAttributes" value="1" checked></td>
						<td>Include Amazon attributes</td>
					</tr>
				</table>			
			<input type="submit" class="button" name="syncAttributes" value="Get cover art"> <input type="button" class="button" name="syncAttributes" value="Back to file" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'"></td>
		</tr>	
	</table>
	<input type="hidden" name="scanID" value="'.$scanID.'">';
	
	return $out;
}

function processScan($fileData,$mediadbADO){
	$matched=(int)@$_POST['matched'];
	
	if($matched==0){
		deleteScans((int)@$_POST['scanID'],$mediadbADO);
		return 'No matches';
	}
	if((int)@$_POST['remove_old']==1){
		remove_cover_arts($fileData['PK_File'][0],$mediadbADO);
	}

	if((int)@$_POST['remove_old_attributes']==1){
		$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=?',array($fileData['PK_File'][0]));
		$mediadbADO->Execute('DELETE FROM LongAttribute WHERE FK_File=?',array($fileData['PK_File'][0]));
	}

	$entryArray=getFieldsAsArray('CoverArtScanEntry','PK_CoverArtScanEntry,URL,FK_CoverArtScan',$mediadbADO,'WHERE PK_CoverArtScanEntry='.$matched);
	$scanID=$entryArray['FK_CoverArtScan'][0];
	$url=$entryArray['URL'][0];
	
	$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array('jpg',$url));
	$pictureID=$mediadbADO->Insert_ID();

	$cmd='sudo -u root cp /home/coverartscan/'.$matched.'.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'.jpg';
	exec_batch_command($cmd);

	$cmd='sudo -u root cp /home/coverartscan/'.$matched.'_tn.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'_tn.jpg';
	exec_batch_command($cmd);

	$mediadbADO->Execute('INSERT INTO Picture_File (FK_File,FK_Picture) VALUES (?,?)',array($fileData['PK_File'][0],$pictureID));

	if((int)@$_POST['includeAttributes']==1){
		syncAttributes('File',$fileData['PK_File'][0],$matched,$mediadbADO);
	}

	deleteScans($scanID,$mediadbADO);
}
?>