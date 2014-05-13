<?php
	 
	//depreciated and not used. functions moved into tvdb utils.
function  tvdbSeries($output,$mediadbADO,$dbADO)	 
{	
	$xmlPath='/var/tmp';
	$apiKey = "4C6CEBDFB4558279";
	$showName = $_POST['showName']; 
	$sID = $_POST['showID'];
	$seasonNo =$_POST['sNo'];
	$episodeNo =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$showData =$_POST['seriesData'];
	$rez=$_POST['rez'];
	//$unlinkAction = $_POST['unlinkFileArray'];
	
	//echo $rez;
	
	//$baseXml =APPROOT."operations/mediaBrowser/tvDBxml/baseconfig.xml";
//	$baseFh = fOpen($baseXml, 'r') or die ('cant open file');
	//fclose($baseXml);
	
	$flag='new';
	$externalAttributesBtn4='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
	$lang = $_POST['lang'];
	$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	$dataFile = file_get_contents($seriesDataRequest);
	saveData($dataFile, $flag);
	$episodeInfo= getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
	
	$out='<table align="center" width="85%" border="0" cellpadding="5">
    <tr>
        <th colspan=3><form action="index.php" method="post" name="tvdbAttributes"><b>Episode Information</b> <img src=operations/mediaBrowser/images/single.png height="50" width="70" align="center">'.$_POST['filename'].' 
        </th>
       
    </tr>
    <tr><td rowspan="3" >
	<img src="http://www.thetvdb.com/banners/_cache/'.$episodeInfo['epImg'].'" align=center><br><input name="submit" type="submit" value="Confirm"></td>
		<td><h1>'.$episodeInfo['series']['series'].'</h1><b>Episode Title: </b>'.$episodeInfo['title'].'<br>
		<b>Season:</b> '.$seasonNo.' <b> Episode:</b> '.$episodeNo.'<br>
		 <p><i>Overview</i><br>'.$episodeInfo['synopsis'].'</p></td><tr>
        <td>		
	<b>Guest Stars: </b>'.$episodeInfo['guestStars'].'
        </td></tr>
        <tr><td>
       <b> Genre: </b>'.$episodeInfo['series']['genre'].'
        </td></tr>        
    <tr>
        <td colspan=3" align="center" bgcolor="#FFFFFF">'.$externalAttributesBtn4.'
        </td>
    <tr>
        <td rowspan="2" width="100" ><img src=http://www.thetvdb.com/banners/_cache/'.$episodeInfo['series']['showPoster'].'  border="1" height="216" width="150" align="left"></td>
        <td width="75%">Series Actors: <br>'.$episodeInfo['series']['seriesactors'].'
       <p> 	Series Description<br><br>'.$episodeInfo['series']['desc'].'</p></td>
    </tr>
    
</table>	
    		  	<input type="hidden" name="section" value="tvdbAttributes">						
                <input type="hidden" name="fileID" value="'.$fileID.' ">
                <input type="hidden" name="sName" value='.$episodeInfo['series']['series'].'> 
                <input type="hidden" name="seasonNo" value='.$seasonNo.'>
                <input type="hidden" name="episodeNo" value='.$episodeNo.'> 
                 <input type="hidden" name="seriesID" value='.$episodeInfo['series']['showID'].'> 
                 <input type="hidden" name="lang" value='.$lang.'>            
             
	</table>' ;
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('Check TVDB');
				$output->setPageTitle('Series and Episode');
				$output->setScriptInHead($scriptInHead);	
				$output->setScriptInBody('bgColor="#F0F3F8"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);			
				$output->output();
};
?>