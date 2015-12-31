<?php

/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com	
	GPLv2 Licensed
	GPLv2 Licensed
	TVDB Data Grabber
	TvDB.com data grabber for television shows from the web admin, both for single and multiple files 
	
	This file contains functions for batch processing.
	
*/
	require_once(APPROOT.'operations/mediaBrowser/tvdbUtils.php'); 
	
function  tvdbBatch($output,$mediadbADO,$dbADO)	 
{	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	

$scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 			<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"></script>
 			
 		<style>	
 			#container{
width:600px;
height: 300px;
border: 8px solid #FFF;
margin: 0 auto;
overflow-x: hidden;
}

.slider{
width: 600%;
height: 260px;
padding: 25px;
background: #CCCCCC;
overflow-x: hidden;
}

.section{
margin:0;
width:250px;
margin-right:5px;
float:left;

}
</style>' ;
	$fileFormatEnum = array (1 => "Low Res",2 => "DVD",3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");
	$xmlPath='/var/tmp';
	$lang = $_POST['lang'];
	$apiKey = "4C6CEBDFB4558279";
	$showName = $_POST['showName']; 
	$sID = $_POST['showID'];
	$cSno =$_POST['sNo'];
	$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	$flag='new';
	$dataFile = file_get_contents($seriesDataRequest, $flag);
	saveData($dataFile);
	//$episodeNo =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$showData =$_POST['seriesData'];
	$seasonNo = $cSno;
	
	//$tempTrashArray = explode("," , $_POST['unlinkFiles']);
	//$unlinkArray =array_slice( $tempTrashArray, "1");
	//print_r($unlinkArray);	
	
	
	$externalAttributesBtn4='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
		
	$episodeInfo= getSeriesData($sID, $lang, $data, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
	//print_r($episodeInfo);
	$combine = 'Season'.$seasonNo;
	$sImgP = key($episodeInfo['series']['seriesImg'][1][$combine]);
	$sImg = $episodeInfo['series']['seriesImg'][1][$combine][$sImgP]['bannerpath'];
	
	//--------------special case for image processing on the fly because the cant be hotlinked from thetvb.com-------------------|
				$picPath= 'http://www.thetvdb.com/banners/_cache/'.$epinfo2['seriesImg'].$sImg;				
				$img = file_get_contents($picPath);		
				$tempfile = fopen('coverartscan/batchImg'.$showID.$seasonNo.'.jpg', 'w');
				fwrite($tempfile, $img);
				fclose($tempfile);		
				$SimgPath = 'coverartscan/batchImg'.$showID.$seasonNo.'.jpg';		
								
				//---------------------------------------------------------------------------------------------------------------------------|
	
	//echo $seasonNo;
	//print_r($sImg);
	$out='<form action="index.php" method="post" name="batchFiles" id="fileList"><table align="center" width="85%" border="0" cellpadding="5" id="MainTable"> 
    <tr>
        <th colspan="3"><b>Series Information</b>
        </th>        
    </tr>
    <tr>
        <td  width="20%" rowspan="3"><img src='.$SimgPath.' id="f_in" style="display:none;" border="1" height="216" width="150" align="left">
       	<div align="center"><input type="submit" action="Submit"  value="Go"  /></div> </td></tr>
        <tr>
         <td ><h2><b>'.$episodeInfo['series']['series'].'<b/> - Season: '.$seasonNo.'</h2>
         <p> Series Description<br>'.$episodeInfo['series']['desc'].'</p>         
         </td><tr> 
        <td align="Left">Genre(s): <b>'.$episodeInfo['series']['genre'].'
        </td>  
         <tr/>     	         
        <tr bgcolor="#FFFFFF"><td colspan="2" align="left" >	Files to Be Batch Processed ---------Override Resolution: <select name="rezMaster" maxlength=15>
						<option value="" selected="selected"></option>
						<option value="3">Standard Def</option>
						<option value="4">HD 720</option>
						<option value="5">HD 1080</option> </select>					    
       <span align="right">-----------------Not the Right season? '.$externalAttributesBtn4.'</span></td>      
    </tr>
    <tr><td align="center" colspan="3" width="100%" style=" overflow-x: hidden; overflow-y: hidden; height: 300px;   ">
    <div style="overflow-x: scroll; width: 800px; height: 275px; " >    
  	<div style="overflow-x: hidden ; height: 240px ; width: 6750px;" >
   ';

		$pathQuery= 'SELECT * FROM File WHERE PK_File=?';
		$pth=$mediadbADO->Execute($pathQuery,$fileID);
		$pathFetch=$pth->FetchRow();
		$path=$pathFetch['Path'].'/'.$pathFetch['Filename'];
	//	echo $path;
		$dirQuery = 'SELECT *  FROM File WHERE Path = ? AND EK_MediaType=5 ';
		$fileList=$mediadbADO->Execute($dirQuery,$path);
		
		$arrayFiles = array();
		$fileCount=0;
		
		while($rowFiles=$fileList->FetchRow())
			{	$seasonNo =$cSno;
				$guessedArray = stringParse($rowFiles['Filename']) ;
				$episodeNo = $guessedArray['ep'][0];
				$rez = $guessedArray['rez'];
				$fileID = $rowFiles['PK_File'];
				
				$epinfo2 = getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
											
				//--------------special case for image processing on the fly because the cant be hotlinked from thetvb.com-------------------|
				$picPath= 'http://www.thetvdb.com/banners/_cache/'.$epinfo2['epImg'];
				$img = file_get_contents($picPath);		
				//echo $picPath;
				$tempfile = fopen('coverartscan/batchImg'.$showID.$episodeNo.'.jpg', 'w');
				fwrite($tempfile, $img);
				fclose($tempfile);		
				$imgPath = 'coverartscan/batchImg'.$showID.$episodeNo.'.jpg';		
								
				//---------------------------------------------------------------------------------------------------------------------------|
				
				$out.='	<div style=" float:left;"></nobr>						
						<div class="section"><p>
					'.$TEXT_FILENAME_CONST.': '.$rowFiles['Filename'].'<br>
						<img src="'.$imgPath.'" height="100" width"300" ><br>
						<b>Title - <i>'.$epinfo2['title'].'</i>-----------Include</b><input type="checkbox" name="file[]" value="'.$rowFiles['PK_File'].'"  id="chk"  checked="true" />
						<br><select name="rezOver[]" maxlength=15>
						<option value="'.$rez.'" selected="selected">'.$fileFormatEnum[$rez].'</option>
						<option value="3">Standard Def</option>
						<option value="4">HD 720</option>
						<option value="5">HD 1080</option><br> 													
							<input type="hidden" id="fileID" name="fileID[]" value="'.$rowFiles['PK_File'].' ">
                			<input type="hidden" id="sName" name="sName" value='.$epinfo2['series']['series'].'>  
                			<input type="hidden" id="seriesID" name="seriesID" value='.$epinfo2['series']['showID'].'> 
                 			<input type="hidden" id="lang" name="lang" value='.$lang.'>
                 			<input type="hidden" id="res" name="res[]" value='.$rez.'><br>
							<b>Guessed Season Number: </b><input name="sNo[]" type="text" value="'.$cSno.'" size="2" maxlength="2"><br>
							<b>Guessed Episode:       </b><input name="epNo[]" type="text" value="'.$guessedArray['ep'][0].'" size="2" maxlength="2"><br> </p>
							</div></div>';			
					$fileCount++;
			}
				
				$out.='</div></div></td></tr></table>
				<input type="hidden" name="batchFiles" value="'.$arrayFiles.'"> <input type="hidden" name="section" value="tvdbBatchResult"></form>';    		  
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('File List ||');
				$output->setPageTitle('Confirm Selection');
				$output->setScriptInHead($scriptInHead);	
				$output->setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'f_in\').appear(); return false;"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);			
				$output->output();
				//$batchArray; 
				//print_r($arrayFiles);
};

function tvdbBatchResult ($output,$mediadbADO,$dbADO)
{	
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	$pass=""; $user="root"; $db="pluto_media"; $con= mysql_connect("localhost", $user, $pass) or die(mysql_error()); 	 // connection 
	if (!$con) { die('Could not Connect'.mysql_error()); //error messaging
	$connMessage="Fail";
	};
	if ($con) {mysql_select_db("pluto_media") or die(mysql_error()); //connect to media db or error out
	$connMessage="Conn Good"; 	}; 	
	
	$batchArray= array();
	$i = 0;
	$updateCount = count($_POST['file']);	
	//add data to folder
	while ($i < $updateCount) {		
	$batchArray[$_POST['file'][$i]] = array('seriesID'=>$_POST['seriesID'], 'episodeNo'=>$_POST['epNo'][$i], 'seasonNo'=>$_POST['sNo'][$i], 'rez'=>$_POST['rezOver'][$i] ); 
	$i++;
	}
		
	$out.='<table width="%85" align="center"><td>';	
	foreach ($batchArray as $key =>$val)
	{ 
	$out.='<tr><td>';
	
	$fileIdent = $key;
	
	$out.='<td bg="#ffffff">Starting Process for file: '.$fileIdent.'<br></td></tr><td>';	
	$fileID=$fileIdent;
	$seasonNo = $val['seasonNo'];
	$episodeNo = $val['episodeNo'];
	if (isset($_POST['override']))
	{$rez=$_POST['override'];}
	else
	{$rez = $val['rez'];}
	//echo $seasonNo.$episodeNo;
	$out.= $fileIdent.' -Season No- '.$seasonNo.' -Episode No- '.$episodeNo.' -Resolution-'.$rez;
	$sID = $val['seriesID'];
	$xmlPath='/var/tmp';
	//echo 'series data quest';
	unlink('coverartscan/batchImg'.$showID.$seasonNo.'.jpg');
	$episodeData = getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);  
	
//begin ugly code======================================================

$attributeType=Array();												//dynamic array to deal with changes to attribute types
$query = "SELECT PK_AttributeType, Description from AttributeType";
$result = mysql_query($query);

while ($row=mysql_fetch_array($result)) {
        $attributeType[$row['PK_AttributeType']]=$row['Description'];
}

$row=mysql_fetch_array($result);
for ($i = 0; $i < count($row); $i++) {
        $attributeType[$row['PK_AttributeType']]=$row['Description'];
}

$mediaSubTypeEnum = array (1 => "TV Shows",2 => "Movies",3 => "Home Videos", 4 => "Sports Events", 5 => "Music Videos", 6 => "Alternative", 7 => "Popular Music", 8 => "Classical Music");
$fileFormatEnum = array (1 => "Low Res",2 => "DVD",3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");
//$attributeType = array (1=>"Director", 2=>"Performer", 8=>"Genre", 10=>"Channel", 11=>"Episode", 12=>"Program", 13=>"Title", 14=>"Disc ID", 16=>"Composer/Writer", 17=>"Studio", 18=>"Rating", 19=>"Release Date", 21=>"Chapter", 26=>"Rated", 31=>"Format", 32=>"Region", 33=>"Language", 34=>"Aspect Ratio", 36=>"IMDB", 37=>"Synopsis", 38=>"Producer", 39=>"Executive Producer", 41=>"TV Channel ID", 42=>"TV Program ID", 43=>"TV Series ID" );
$aTypeProper= array_flip($attributeType);
$directors = explode("|", $episodeData['director']);
$seriesActors = explode("|", $episodeData['series']['seriesactors']);
$genreArr = explode("|", $episodeData['series']['genre']);
//print_r($episodeData);
if (strpos($episodeData['guestStars'], ','))
{
$procGuestActors = explode(",", $episodeData['guestStars']);
}
else
{
$guestStars = explode("|", $episodeData['guestStars']);
foreach($guestStars as $val =>$key)
{ if (!(preg_match('/[a-z0-9]/', $key))) {unset($guestStars[$val]); } };
$procGuestActors = array_values($guestStars);
}
foreach($directors as $val =>$key)
{ if (!(preg_match('/[a-z0-9]/', $key))) {unset($directors[$val]); } };
$directorArray = array_values($directors);

foreach($seriesActors as $val =>$key)
{ if (!(preg_match('/[a-z0-9]/', $key))) {unset($seriesActors[$val]); } };
$procSeriesActors = array_values($seriesActors);
$actorArray = array_merge($procSeriesActors, $procGuestActors);

foreach($genreArr as $val =>$key)
{ if (!(preg_match('/[a-z0-9]/', $key))) {unset($genreArr[$val]); } };
$genreArray = array_values($genreArr);


//subtypes==========================================================================
	if ($_POST['rezMaster'] !="")
	{
	$rez = $_POST['rezMaster'];
	}
	
	$out.="<br><b>Starting Subtypes<br>";
	mysql_query("UPDATE File SET FK_MediaSubType=1  WHERE `PK_File`=\"$fileIdent\" ")or die (mysql_error());
	$out.="Tv Shows - Set<br>";
	mysql_query("UPDATE File SET FK_FileFormat=\"$rez\" WHERE `PK_File`=\"$fileIdent\" ")or die (mysql_error());
	$out.= $fileFormatEnum[$rez].' Set<br>';
	$out.="<b>End Subtypes</b><br>";
	
//=========================series title, mapped to Program===========================================		
	$program=mysql_real_escape_string($episodeData['series']['series']);
        $sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$program\" AND FK_AttributeType = 12";
	$result = mysql_query($sql);		 
	$row = mysql_fetch_assoc($result);	
	$attrib = $row['PK_Attribute'];		
	$count = mysql_num_rows($result);
	$attrib = $row['PK_Attribute'];
//	$fileIdent = $_POST['fileID'];
	if ($count < 1)
	{		$out.= 'Series title not found...' ;	
                        $iQuery = "INSERT INTO Attribute VALUES (\"\" , 12 , \"$program\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysql_query($iQuery) or die (mysql_error());	
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			 mysql_query($idQuery)  or die (mysql_error());
	}
	else
	{$out.= 'Series title in database, associating .....<br>' ;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileIdent";
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult); 
	if ($chkCount < 1)
	{		$out.='...Inserting Series title: <b>'.$program.'</b> into '.$fileID ;
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
	mysql_query($idQuery2)  or die (mysql_error());
	} 
	};$out.='Series title <b>'.$program.'</b> complete. <br>';

	//====================================actors=======================================
$ATotal = count($actorArray);
	//print_r($directorArray);
	//echo '<b>Total Attributes: </b>'.$dTotal.'<br>' ; 
	$i=0;
	$attribType = 2;
	while ($i < $ATotal)
	{	
	$content=mysql_real_escape_string($actorArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType"; 
	$result = mysql_query($sql);
	$count = mysql_num_rows($result);	 
	$row = mysql_fetch_assoc($result);
	//print_r($row);	
	$attrib = $row['PK_Attribute'];
	if ($count < 1)
	{			
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;  	
			mysql_query($iQuery) or die (mysql_error());	
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysql_query($idQuery)  or die (mysql_error());
	}
	else
	{
	//echo $content.' in database...<br>'; echo $attrib;
	
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult); 
	if ( $chkCount < 1)
	{		
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysql_query($idQuery2)  or die (mysql_error());
	} 
	};
	$i++;} $out.='<b>Actor success</b><br></td></tr><td>';
//==============================eptitle mapped to episode field==================================================================
	$epTitle=mysql_real_escape_string($episodeData['title']);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$epTitle\" AND FK_AttributeType = 11"; //test to see if its in the db already
	$result = mysql_query($sql);
	$count = mysql_num_rows($result);	 
	$row = mysql_fetch_assoc($result);	
	$attrib = $row['PK_Attribute'];
	
	if ($count < 1)
	{
	$out.= 'Episode title not found...' ;
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , 11 , \"$epTitle\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;  	
			mysql_query($iQuery) or die (mysql_error());	
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			 mysql_query($idQuery)  or die (mysql_error());
	}
	else
	{$out.= 'Episode title in database, associating .....<br>' ;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileIdent";
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult); 
	if ($chkCount < 1)
	{		
		$out.='Inserting episode title: '.$epTitle.' into '.$fileID ;
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
	mysql_query($idQuery2)  or die (mysql_error());
	} else
	{ $out.='Episode Title: <b>'.$epTitle.'</b> already in ep, skipping <br>';}
	};$out.='Episode: '.$epTitle.' Complete<br></tr></td><tr><td>';
//===================================================================================director
	$dTotal = count($directorArray);
	//print_r($directorArray);
	//echo '<b>Total Attributes: </b>'.$dTotal.'<br>' ; 
	$i=0;
	$attribType = 1;
	while ($i < $dTotal)
	{	
	$content=mysql_real_escape_string($directorArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType"; 
	$result = mysql_query($sql);
	$count = mysql_num_rows($result);	 
	$row = mysql_fetch_assoc($result);
	//print_r($row);	
	$attrib = $row['PK_Attribute'];
	if ($count < 1)
	{			$out.= 'Director(s) not in db---------inserting' ;
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;  	
			mysql_query($iQuery) or die (mysql_error());	
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysql_query($idQuery)  or die (mysql_error());
	}
	else
	{
	
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult); 
	if ( $chkCount < 1)
	{		
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysql_query($idQuery2)  or die (mysql_error());
	} else
	{ 
	}	
	};
	$i++;}$out.='Director Complete<br></td></tr><tr><td>';	

	//=========================================epidsode img=====================================
unlink('coverartscan/batchImg'.$showID.$episodeNo.'.jpg');
$test=mysql_query("SELECT * FROM Picture_File WHERE FK_File=\"$fileIdent\" ") or die (mysql_error());
$fileCount = mysql_num_rows($test); 
//echo $fileCount; 
if ($fileCount < 1)
{ $out.= 'No episode picture, adding' ;
$fullUrl ='http://www.thetvdb.com/banners/'.$episodeData['epImg'];
$img = file_get_contents($fullUrl);
$fName1 = strrchr($episodeData['epImg'], '/');
$fName = str_replace (".jpg", "", $fName1);
$savefile = fopen(APPROOT.'operations/mediaBrowser/tvDBxml/'.$fName.'.jpg', 'w');
fwrite($savefile, $fullUrl);
fclose($savefile);
$import_cover_art='http://www.thetvdb.com/banners/'.$episodeData['epImg'];
$extension=strtolower(str_replace('.','',strrchr($import_cover_art,".")));	

	if($import_cover_art!=''){
		mysql_query("INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
		$picID=(int)mysql_insert_id()  or die (mysql_error());
		mysql_query("INSERT INTO Picture_File (FK_Picture,FK_File) VALUES (\"$picID\",\"$fileIdent\")");

		// create the file and the thumbnail
		$extension=($extension=='jpeg')?'jpg':$extension;
		$newPicName=$picID.'.'.$extension;
			
			$error='';
			if($extension!='jpg'){
				$error=$TEXT_ERROR_NOT_JPEG_CONST;
			}
			else{
				// create thumbnail
				savePic($import_cover_art,$GLOBALS['mediaPicsPath'].$newPicName);
				if(file_exists($GLOBALS['mediaPicsPath'].$newPicName)){
					// try to delete exisitng thumbnail if user messed the files and database
					if(file_exists($GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension)){
						$cmd='sudo -u root rm -f "'.$GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension.'"';
						exec_batch_command($cmd);
					}
					 unlink(APPROOT.'operations/mediaBrowser/tvDBxml/'.$fName.'.jpg');	
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension, 256, 256);
					if(!$resizeFlag){
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
					}
				}
			}		
	}
}else
{ $out.= 'Episode has image, skipping'; }; $out.='Image Complete<br></td></tr><tr><td>';
//==========================================genre===========================================================================

	$gTotal = count($genreArray);	
	$i=0;
	$attribType = 8;	
	while ($i < $gTotal)
	{	
	$content=mysql_real_escape_string($genreArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType"; 
	$result = mysql_query($sql);
	$count = mysql_num_rows($result);	 
	$row = mysql_fetch_assoc($result);	
	$attrib = $row['PK_Attribute'];		
	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;  	
			mysql_query($iQuery) or die (mysql_error());	
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysql_query($idQuery)  or die (mysql_error());
		}
	else
	{
	
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult); 
	if ( $chkCount < 1)
	{	
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysql_query($idQuery2)  or die (mysql_error());
	} 
	}; $out.='Genre Added <br></td></tr><tr><td>';
	$i++;}

//==============================eptitle mapped to title==================================================================
        $epTitle=mysql_real_escape_string($episodeData['title']);
        $sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$epTitle\" AND FK_AttributeType = 13"; //test to see if its in the db already
        $result = mysql_query($sql);
        $count = mysql_num_rows($result);
        $row = mysql_fetch_assoc($result);
        $attrib = $row['PK_Attribute'];

        if ($count < 1)
        {
        $out.= 'Episode title not found...' ;
                        $iQuery = "INSERT INTO Attribute VALUES (\"\" , 13 , \"$epTitle\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
                        mysql_query($iQuery) or die (mysql_error());
                        $insertRes= (int)mysql_insert_id()  or die (mysql_error());
                        //echo $insertRes;
                        $idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
                         mysql_query($idQuery)  or die (mysql_error());
        }
        else
        {$out.= 'Episode title in database, associating .....<br>' ;
        $fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileIdent";
        $chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult);
        if ($chkCount < 1)
        {
                $out.='Inserting episode title: '.$epTitle.' into '.$fileID ;
                $idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
        mysql_query($idQuery2)  or die (mysql_error());
        } else
        { $out.='Episode Title: <b>'.$epTitle.'</b> already in ep, skipping <br>';}
        };$out.='Episode: '.$epTitle.' Complete<br></tr></td><tr><td>';

//=================ids================================================================================
      //	print_r($episodeData);
	$idArray = array( array('TV Season ID'=>$episodeData['seasonID']), array('TV Series ID'=>$episodeData['seriesID']), array( 'TV Program ID'=>$episodeData['epid']), array("Release Date" =>$episodeData['firstAir']), array("Episode Number" =>$episodeData['air_epNo']), array("Season Number" =>$episodeData['seasonNo']) );
 	$counter = count($idArray);
 //	echo $counter;	
 	$i=0;
 	$flip = array_flip($attributeType);
	$fileIdent = $fileID;
while ($i < $counter)
	{
	$item = $idArray[$i];
	$attribType = $flip[key($idArray[$i])];
	$content1 = array_values($idArray[$i]);
	$content = $content1[0];
$out.="$content1";
	$sql = "SELECT * FROM `Attribute` WHERE `Name` = \"$content\" AND FK_AttributeType = \"$attribType\" ";
	
	$result = mysql_query($sql) or die (mysql_error($content));
	$count = mysql_num_rows($result);
	$row = mysql_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];
	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysql_query($iQuery) or die (mysql_error());
			$insertRes= (int)mysql_insert_id()  or die (mysql_error());
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"'.$insertRes.'\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysql_query($idQuery)  or die (mysql_error());
		}
	else
	{
	$fileIdent = $fileID;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysql_query($fileChk) or die (mysql_error()); $chkCount = mysql_num_rows($chkResult); $chk = mysql_fetch_assoc($chkResult);
	if ( $chkCount < 1)
	{
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysql_query($idQuery2)  or die (mysql_error());
	} else
	{
	}
	};$out.="$content1 - done";
	$i++;}$out.='IDs complete <br></td></tr><tr><td>';
//=========synopsis===================================================================================================
	
	$synopsis=mysql_real_escape_string($episodeData['synopsis']);
	$Ssql = "SELECT * FROM `LongAttribute` WHERE `Text` = \"$synopsis\" AND FK_File=\"$fileIdent\" "; 
	$Sresult = mysql_query($Ssql)or die (mysql_error());
	$Scount = mysql_num_rows($Sresult);	 
	
	if ($Scount < 1)
	{		$out.= 'Show synopsis not found...' ;
			$SfileIdent = $_POST['fileID'];
			mysql_query("INSERT INTO LongAttribute VALUES (\"\" , 37 ,NULL, \"$fileIdent\", NULL, \"$synopsis\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )")or die (mysql_error());		
			}$out.= 'Synopsis<p><b>'.$synopsis.'</b></p></td></tr><tr><td>';
	
	$out.= $fileID.':Complete<br></td></tr>';	
	}
	$out.='<h1> Batch Complete</h1></td>';
	
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('Search TVDB ||');
				$output->setPageTitle('Batch Confirm');
				$output->setScriptInHead($scriptInHead);	
				$output->setScriptInBody('bgColor="#F0F3F8"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);			
				$output->output();


}


?>
