<?
function matchCoverArt($output,$mediadbADO) {
	global $mediaTypes;
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/coverArt.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	
	if($action=='form'){
		$scannedArray=array();
		$rs=$mediadbADO->Execute('
			SELECT 
			Keyword1Type,Keyword1Search,Keyword2Type,Keyword2Search,Keyword3Type,Keyword3Search,Attributes,
			FK_CoverArtScan,PK_CoverArtScanEntry,FK_File,FK_Disc,FK_Attribute,File.Path,File.Filename,Attribute.Name,Engine 
			FROM CoverArtScanEntry 
			INNER JOIN CoverArtScan ON FK_CoverArtScan=PK_CoverArtScan
			LEFT JOIN File ON FK_File=PK_File
			LEFT JOIN Attribute ON FK_Attribute=PK_Attribute
			ORDER BY PK_CoverArtScanEntry ASC');
		while($row=$rs->FetchRow()){
			$scannedArray[$row['FK_CoverArtScan']]['FK_File']=$row['FK_File'];
			$scannedArray[$row['FK_CoverArtScan']]['FK_Disc']=$row['FK_Disc'];
			$scannedArray[$row['FK_CoverArtScan']]['FK_Attribute']=$row['FK_Attribute'];
			$scannedArray[$row['FK_CoverArtScan']]['Path']=$row['Path'];
			$scannedArray[$row['FK_CoverArtScan']]['Filename']=$row['Filename'];
			$scannedArray[$row['FK_CoverArtScan']]['Name']=$row['Name'];
			$scannedArray[$row['FK_CoverArtScan']]['Engine']=$row['Engine'];
			$scannedArray[$row['FK_CoverArtScan']]['Entries'][]=$row['PK_CoverArtScanEntry'];
			$scannedArray[$row['FK_CoverArtScan']]['Filters']=(!is_null($row['Keyword1Type']))?$row['Keyword1Type'].': '.urldecode($row['Keyword1Search']):'';
			$scannedArray[$row['FK_CoverArtScan']]['Filters'].=(!is_null($row['Keyword2Type']))?$row['Keyword2Type'].': '.urldecode($row['Keyword2Search']):'';
			$scannedArray[$row['FK_CoverArtScan']]['Filters'].=(!is_null($row['Keyword3Type']))?$row['Keyword3Type'].': '.urldecode($row['Keyword3Search']):'';
			$scannedArray[$row['FK_CoverArtScan']]['Attributes']=$row['Attributes'];
		}
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="matchCoverArt">
			<input type="hidden" name="section" value="matchCoverArt">
			<input type="hidden" name="action" value="update">
		<a href="index.php?section=coverArt">'.$TEXT_COVERART_CONST.'</a><br><br>
		'.formatScannedItems($scannedArray).'
		
		</form>';
	}else{
		$ids=explode(',',$_POST['ids']);
		foreach ($ids AS $id){
			$selPic=$_POST['pic_'.$id];
			if($selPic==0){
				deleteScans($id,$mediadbADO);
			}else{
				$itemType=$_POST['itemType_'.$id];
				$itemValue=$_POST['itemValue_'.$id];

				if((int)@$_POST['remove_old']==1 && $itemType=='File'){
					remove_cover_arts($itemValue,$mediadbADO);
				}

				if((int)@$_POST['remove_old_attributes']==1){
					$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=?',array($itemValue));
					$mediadbADO->Execute('DELETE FROM LongAttribute WHERE FK_File=?',array($itemValue));
				}
			
							
				$urls=getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','URL',$mediadbADO,'WHERE FK_CoverArtScan='.$id);
				
				$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array('jpg',$urls[$selPic]));
				$pictureID=$mediadbADO->Insert_ID();
				
				$cmd='sudo -u root cp /home/coverartscan/'.$selPic.'.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'.jpg';
				exec_batch_command($cmd);

				$cmd='sudo -u root cp /home/coverartscan/'.$selPic.'_tn.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'_tn.jpg';
				exec_batch_command($cmd);
				
				$mediadbADO->Execute('INSERT INTO Picture_'.$itemType.' (FK_'.$itemType.',FK_Picture) VALUES (?,?)',array($itemValue,$pictureID));
				if(($itemType=='File' || $itemType=='Disc') && (int)@$_POST['includeAttributes']==1){
					syncAttributes($itemType,$itemValue,$selPic,$mediadbADO);
				}
				deleteScans($id,$mediadbADO);
			}
		}
		
		header('Location: index.php?section=matchCoverArt&msg='.urlencode($TEXT_COVER_ARTS_UPDATED_CONST));
		exit();
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MATCHCOVERART_CONST);
	$output->setNavigationMenu(array($TEXT_MATCHCOVERART_CONST=>'index.php?section=matchCoverArt'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MATCHCOVERART_CONST);
	$output->output();
}

function formatScannedItems($scannedArray){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/coverArt.lang.php');
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$records_per_page=10;
	
	$out='<table>';
	if(count($scannedArray)==0){
		return 'No scanned items.';
	}
	
	$totalRecords=count($scannedArray);
	$maxRecords=$page*$records_per_page;
	$noPages=ceil($totalRecords/$records_per_page);

	$pageLinks='';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i.' ':' <a href="index.php?section=matchCoverArt&page='.$i.'">'.$i.'</a>';
	} 
	$out.='
		<tr>
			<td align="right" class="alternate_back">'.$pageLinks.'</td>
		</tr>	
	';
	$item=0;
	$displayedArray=array();
	foreach ($scannedArray AS $id=>$data){

		if($item>=$records_per_page*($page-1) && $item< $records_per_page*$page){
			$displayedArray[]=$id;
			if(count($data['Entries'])==0){
				$picsTable='No covert arts found';
			}else{
				$picsTable='<table>
					<tr>
						<td valign="top" align="center"><input type="radio" name="pic_'.$id.'" value="0" checked><br>None of this</td>';
				$pos=0;
				foreach ($data['Entries'] AS $pic){
					$pos++;
					if(file_exists('/home/coverartscan/'.$pic.'_tn.jpg')){
						$picsTable.='<td align="center"><input type="radio" name="pic_'.$id.'" value="'.$pic.'"><br><a href="coverartscan/'.$pic.'.jpg" target="_blank" title="'.$data['Attributes'].'"><img src="coverartscan/'.$pic.'_tn.jpg" border="0"></a></td>';
					}
					if($pos==6){
						$picsTable.='</tr><tr>';
						$pos=0;
					}
				}
				$picsTable.='</tr></table>';
			}
			
			if($data['FK_File']!=''){
				$itemType='File';
				$itemValue=$data['FK_File'];
			}
			if($data['FK_Disc']!=''){
				$itemType='Disc';
				$itemValue=$data['FK_Disc'];
			}
			if($data['FK_Attribute']!=''){
				$itemType='Attribute';
				$itemValue=$data['FK_Attribute'];
			}
			
			$itemLabel=str_replace(array('Amazon | '),'',$data['Engine']);
			
			
			$out.='
			<input type="hidden" name="itemType_'.$id.'" value="'.$itemType.'">
			<input type="hidden" name="itemValue_'.$id.'" value="'.$itemValue.'">
			<tr>
				<td class="alternate_back">'.$itemLabel.': '.$data['Path'].$data['Filename'].$data['Name'].' ('.$data['Filters'].')</td>
			</tr>
			<tr>
				<td>'.$picsTable.'</td>
			</tr>
			';
		}
		$item++;
	}
	$out.='
		<tr>
			<td align="center" class="alternate_back"> 
				<table>
					<tr>
						<td><input type="checkbox" name="remove_old" value="1"></td>
						<td>'.$TEXT_REMOVE_EXISTING_COVERARTS_CONST.'</td>
					</tr>
					<tr>
						<td><input type="checkbox" name="remove_old_attributes" value="1"></td>
						<td>'.$TEXT_REMOVE_EXISTING_ATTRIBUTES_CONST.'</td>
					</tr>					
					<tr>
						<td><input type="checkbox" name="includeAttributes" value="1" checked></td>
						<td>'.$TEXT_INCLUDE_AMAZON_ATTRIBUTES_CONST.'</td>
					</tr>
				</table>
			<input type="submit" class="button" name="match" value="'.$TEXT_MATCHCOVERART_CONST.'"></td>
		</tr>	
	</table>
	<input type="hidden" name="ids" value="'.join(',',$displayedArray).'">';
	
	return $out;
}
?>