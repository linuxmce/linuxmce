<?php 

/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com	
	GPLv2 Licensed
	Single file confirmation page.
*/
//error_reporting(0);

function tvdbAttributes($output,$mediadbADO,$dbADO) {	
	require_once(APPROOT.'operations/mediaBrowser/tvdbUtils.php');		
	$apiKey = "4C6CEBDFB4558279" ; 
	$xmlPath='/operations/mediaBrowser/tvDBxml/';
	$seriesName =$_POST['sName'];
	$seasonNo = $_POST['seasonNo'];
	$episodeNo = $_POST['episodeNo'];
	$unlinkString=$_POST['actorUnlink'];
	$sID = $_POST['seriesID'];
	$lang= $_POST['lang']; 
	$fileID=$_POST['fileID'];
	$episodeData['rez'] = $_POST['rezOver'];
	//print_r($_POST['rezOver']);
	//$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	//$dataFile = file_get_contents(APPROOT.$xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip');
	$episodeData = getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);  	
	insertAttributes($fileID,$mediadbADO,$dbADO, $output, $episodeData, $xmlPath);		
	
//================================================================page output=========================	
		$externalAttributesBtn5='<input type="button" class="button_fixed" value="Back To File Detail" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'">';
		$fileQuery=('SELECT * FROM File where PK_FILE='.$fileID);
		$fileResult = mysql_query($fileQuery) or die(mysql_error());
		$fileData = mysql_fetch_array($fileResult) or die(mysql_error());		
		$queryAttributes='
				SELECT Attribute.*, AttributeType.Description 
				FROM Attribute
				INNER JOIN File_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_File='.$fileID.'
				ORDER BY Description ASC'; 				
				$out.='
				<table>
				<tr>
				<td class="tablehead" colspan="2">Existing Attributes<br><div align="right">'.$externalAttributesBtn5.'</div>File #'.$fileID.'</td>
				</tr>		
				<tr>
				<td><B>Filename</B></td>
				<td>'.$fileData['Filename'].'</td>
			</tr>
			<tr>
				<td><B>Path</B></td>
				<td>'.$fileData['Path'].'</td>
			</tr>
			<tr>
				<td valign="top"><B>Attributes</B></td>
				<td>	<table width="100%">';
			
			$rowData = mysql_query($queryAttributes);
			$pos=0;
			while($rowAttributes=mysql_fetch_array($rowData))
			{	$pos++;
				$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td><b>'.$rowAttributes['Description'].'</b></td>
					<td>'.$rowAttributes['Name'].'</td>
						</tr>';	} ;
				
				$queryPictures='
				SELECT * FROM Picture_File
				INNER JOIN Picture ON Picture_File.FK_Picture=PK_Picture
				WHERE FK_File=?';
			$resPictures=$mediadbADO->Execute($queryPictures,$fileID);
			$picsCount=0;
			$picsArray=array();
			while($rowPictures=$resPictures->FetchRow())
			{
				if(!in_array($rowPictures['PK_Picture'],$picsArray))
				{
					$picsArray[]=$rowPictures['PK_Picture'];
				}

				$picsCount++;
				$out.='
					<td style="background-color:#EEEEEE;" align="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a> 
						<br>						
						<a href="#" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_PICTURE_CONST.'\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=properties&picID='.$rowPictures['PK_Picture'].'&path='.htmlentities($rowFile['Path']).'\';">'.$TEXT_DELETE_CONST.'</a></td></tr></table>'; 
			}
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('Check TVDB');
				$output->setPageTitle('Insert Attributes');
			//	$output->setScriptInHead($scriptInHead);	
				$output->setScriptInBody('bgColor="#F0F3F8"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);			
				$output->output();				
				cleanup($unlinkString);
}			
?>