<?php
/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com	
	GPLv2 Licensed
	TVDB.com search and selection.
*/

require_once(APPROOT.'operations/mediaBrowser/tvdbUtils.php');

function searchTVDB($output,$mediadbADO,$dbADO)
{
	$scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"></script> ';
	$showName1 = $_POST['showName1'];
	$season = $_POST['sNo'];
	$episode =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$lang = $_POST['lang'];
	$fileName=$_POST['fileName'];
	$showName2=$_POST['showName2'];
	$rez = $_POST['rez'];
	$completed="";
	$fileString="";
	$externalAttributesBtn2='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
	$showName = urlEncode($showName1.$showName2);

		if (!$showName)
			{
			searchSeries($fileID,$mediadbADO,$dbADO, $output);
			}

		if(isset($_POST['isDir']))
			{
			batchProcess($output,$mediadbADO,$dbADO, $showName);
			}
		elseif (isset($_POST['special']) )
			{
			specials($output,$mediadbADO,$dbADO, $showName);
			}
		else
			{
			$searchUrl='http://www.thetvdb.com/api/GetSeries.php?seriesname='.$showName ;
			//echo urlEncode($showName);
			$searchResp = file_get_contents($searchUrl);
			
		if(!$searchResp) 
			{
			 searchSeries($fileID,$mediadbADO,$dbADO, $output);
			}
		else
			{
				$xmlResp = New DOMDocument() ;
				$xmlResp->load($searchUrl);
				$xmlResp->savexml();
				$series = $xmlResp->getElementsByTagName("Series");

//getting the image paths seperately//
		foreach ($series as $sKey)
			{

				$bPath = $sKey->getElementsByTagName("banner");
				$bnrPath =$bPath->item(0)->nodeValue;
				$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;
				$showValueNode = $sKey->getElementsByTagName('seriesid');
				$showID = $showValueNode->item(0)->nodeValue;		
				$fileString.= ','.'searchImg'.$showID.'.jpg';		

// main parsing of series information
		foreach ($series as $sKey)
				{
					$sName = $sKey->getElementsByTagName("SeriesName");
					$fsName =$sName->item(0)->nodeValue;

					$bPath = $sKey->getElementsByTagName("banner");
					$bnrPath =$bPath->item(0)->nodeValue;
					$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;

					$sDesc = $sKey->getElementsByTagName("Overview");
					$synop =$sDesc->item(0)->nodeValue;

					$showValueNode = $sKey->getElementsByTagName('seriesid');
					$showID = $showValueNode->item(0)->nodeValue;

					$img = file_get_contents($cbPath);		
					$tempfile = fopen('coverartscan/searchImg'.$showID.'.jpg', 'w');
					fwrite($tempfile, $img);
					fclose($tempfile);		
			
					$imgPath = 'coverartscan/searchImg'.$showID.'.jpg';	
		//$fileString.= 'searchImg'.$showID.'.jpg'.',';	
			
				$out.='
				<table width="85%" border="0" cellpadding="0" align="center">
				<form action="index.php" method="post" name="tvdbSeries">
				<input name="showName" type="hidden" value="'.$fsName.'" />
 				<tr>
  				<td colspan="2">
				<div id="appearImg"><img src=coverartscan/searchImg'.$showID.'.jpg></div></td></tr>			
				
  				<tr>
   				<td><p id="desc"><b>Show Description<br><b><br>'.$synop.'</p></td></tr>
   			 	<tr align="center">
   			 	<td>
   			 	<input name="submit" type="submit" value="Select This Show" /> Or 	 	
   			 	'.$externalAttributesBtn2.'</td>
				</tr>
				</table>
				<input type="hidden" name="showID" value="'.$showID.'">
 				<input type="hidden" name="section" value="tvdbSeries">
           		<input type="hidden" name="fileID" value="'.$fileID.'">
				<input name="seriesData" type="hidden" value="'.$sSeries.'"/>
				<input name="showBanner" type="hidden" value="'.$cbPath.'"/>
				<input name="epNo" type="hidden" value="'.$episode.'"/>
				<input name="sNo" type="hidden" value="'.$season.'"/>
				<input name="lang" type="hidden" value="'.$lang.'"/>
				<input name="rez" type="hidden" value="'.$rez.'"/>
				<input name ="filename" type="hidden" value="'.$_POST['fileName'].'">
				<input name="unlinkFiles" type="hidden" value="'.$fileString.'">
				</form>';
				};
		}
	};
			
$output->setReloadLeftFrame(false);
$output->setMenuTitle('Search Results ||');
$output->setPageTitle($showName);
//	$output->setScriptInHead($scriptInHead);
$output->setScriptInBody('bgColor="#F0F3F8" ');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();
};
}


function batchProcess($output,$mediadbADO,$dbADO, $showName)
{

	//$showName1 = $_POST['showName'];
	$season = $_POST['sNo'];
	$episode =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$lang = $_POST['lang'];
	$fileName=$_POST['fileName'];
	//$showName2=$_POST['showName2'];
	$externalAttributesBtn2='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';

	if (!$showName)
		{
			searchSeries($fileID,$mediadbADO,$dbADO, $output);
		}

	$searchUrl='http://www.thetvdb.com/api/GetSeries.php?seriesname='.$showName ;

	$searchResp = file_get_contents($searchUrl);
	
	if(!strpos($searchResp,'Series',0)) 
		{
			//header('Location: index.php?section=checkTVDB&fileID='.$fileID);
			echo "Not Found";
			searchSeries($fileID,$mediadbADO,$dbADO, $output);
		}
		else
		{
			$xmlResp = New DOMDocument() ;
			$xmlResp->load($searchUrl);
			$xmlResp->savexml();
			$series = $xmlResp->getElementsByTagName("Series");

			//getting the image paths seperately//
				foreach ($series as $sKey)
				{
					$bPath = $sKey->getElementsByTagName("banner");
					$bnrPath =$bPath->item(0)->nodeValue;
					$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;
					
					$showValueNode = $sKey->getElementsByTagName('seriesid');
					$showID = $showValueNode->item(0)->nodeValue;		
					$fileString.= ','.'searchImg'.$showID.'.jpg';
				};

				foreach ($series as $sKey)
				{
					$sName = $sKey->getElementsByTagName("SeriesName");
					$fsName =$sName->item(0)->nodeValue;

					$bPath = $sKey->getElementsByTagName("banner");
					$bnrPath =$bPath->item(0)->nodeValue;
					$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;

					$sDesc = $sKey->getElementsByTagName("Overview");
					$synop =$sDesc->item(0)->nodeValue;

					$showValueNode = $sKey->getElementsByTagName('seriesid');
					$showID = $showValueNode->item(0)->nodeValue;
					$img = file_get_contents($cbPath);		
					$tempfile = fopen('coverartscan/searchImg'.$showID.'.jpg', 'w');
					fwrite($tempfile, $img);
					fclose($tempfile);		
					$imgPath = 'coverartscan/searchImg'.$showID.'.jpg';		
					$fileString.= ','.'searchImg'.$showID.'.jpg';
				
				
$out.='
			<table width="85%" border="0" cellpadding="0" align="center">
			<form action="index.php" method="post" name="tvdbbatchprocess">
			<input name="showName" type="hidden" value="'.$fsName.'" />
 			<tr>
  			<td colspan="2">
			<div id="appear"><img src="coverartscan/searchImg'.$showID.'.jpg"></div></td></tr>
			<tr><td colspan="2"><h2>'.$fsName.'</h2><div width="50%" align="right"><b>Season: '.$season.' || Episode: '.$episode.'<br>Filename: '.$fileName.'</b></td></tr>
  			<tr>
   			<td><p id="desc"><b>Show Description<br><b><br>'.$synop.'</p></td></tr>
   			<tr>
   			<td colspan=2 align=center>
   			<div align = "left"><input name="submit" type="submit" value="Select This Show" /></div><div align="right">'.$externalAttributesBtn2.'</div></td>
			</tr>
			</table>
			<input type="hidden" name="showID" value="'.$showID.'">
 			<input type="hidden" name="section" value="tvdbBatch">
            <input type="hidden" name="fileID" value="'.$fileID.'">
			<input name="seriesData" type="hidden" value="'.$sSeries.'"/>
			<input name="showBanner" type="hidden" value="'.$cbPath.'"/>
			<input name="epNo" type="hidden" value="'.$episode.'"/>
			<input name="sNo" type="hidden" value="'.$season.'"/>
			<input name="lang" type="hidden" value="'.$lang.'"/>
			<input name ="filename" type="hidden" value="'.$fileName.'"> 
			<input name="unlinkFiles" type="hidden" value="'.$fileString.'">
			</form>' ;
		
		};
	};
		
		
$output->setReloadLeftFrame(false);
$output->setMenuTitle('Batch Search Result ||');
$output->setPageTitle($showName);
$output->setScriptInHead($scriptInHead);
$output->setScriptInBody('bgColor="#F0F3F8"');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();
}




function specials($output,$mediadbADO,$dbADO, $showName)
{
	$fileID= $_POST['fileID'];
	$externalAttributesBtn2='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
	$lang = $_POST['lang'];
	$season = 0;
	
	//print_r($_POST);


	if (!$showName)
		{
			searchSeries($fileID,$mediadbADO,$dbADO, $output);
		}
	$searchUrl='http://www.thetvdb.com/api/GetSeries.php?seriesname='.$showName ;
	$searchResp = file_get_contents($searchUrl);

	if(!strpos($searchResp,'Series',0)) 
		{
//header('Location: index.php?section=checkTVDB&fileID='.$fileID);
	searchSeries($fileID,$mediadbADO,$dbADO, $output);
		}
	else
		{

		$xmlResp = New DOMDocument() ;
		$xmlResp->load($searchUrl);
		$xmlResp->savexml();
		$series = $xmlResp->getElementsByTagName("Series");

//getting the image paths seperately//
			foreach ($series as $sKey)
			{

				$bPath = $sKey->getElementsByTagName("banner");
				$bnrPath =$bPath->item(0)->nodeValue;
				$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;

				$showValueNode = $sKey->getElementsByTagName('seriesid');
				$showID = $showValueNode->item(0)->nodeValue;
		
				$fileString.= ','.'searchImg'.$showID.'.jpg';
			

			foreach ($series as $sKey)
			{
				$sName = $sKey->getElementsByTagName("SeriesName");
				$fsName =$sName->item(0)->nodeValue;

				$bPath = $sKey->getElementsByTagName("banner");
				$bnrPath =$bPath->item(0)->nodeValue;
				$cbPath = 'http://thetvdb.com/banners/'.$bnrPath ;
				$img = file_get_contents($cbPath);		
				$tempfile = fopen('coverartscan/searchImg'.$showID.'.jpg', 'w');
				fwrite($tempfile, $img);
				fclose($tempfile);		
				$imgPath = 'coverartscan/searchImg'.$showID.'.jpg';

				$sDesc = $sKey->getElementsByTagName("Overview");
				$synop =$sDesc->item(0)->nodeValue;

				$showValueNode = $sKey->getElementsByTagName('seriesid');
				$showID = $showValueNode->item(0)->nodeValue;
	
		$out.='
				<table width="85%" border="0" cellpadding="0" align="center">
			<form action="index.php" method="post" name="tvdbSpecSave">
			<input name="showName" type="hidden" value="'.$fsName.'" />
 			<tr>
  			<td colspan="2">
			<div id="appearImg"><img src="'.$imgPath.' "></div></td></tr>
			<tr><td colspan="2"><h2>'.$fsName.'</h2><div width="50%" align="right"><b>Season: '.$season.' || Episode: '.$episode.'<br>Filename: '.$_POST['fileName'].'</b></td></tr>
  			<tr>
   			 <td><p id="desc"><b>Show Description<br><b><br>'.$synop.'</p></td></tr>
   			 <tr>
   			 <td colspan=2 align=center>
   			 <div align = "left"><input name="submit" type="submit" value="Select This Show" /></div><div align="right">'.$externalAttributesBtn2.'</div></td>
			 </tr>
			</table>
			<input type="hidden" name="showID" value="'.$showID.'">
 			<input type="hidden" name="section" value="tvdbSpecSave">
            <input type="hidden" name="fileID" value="'.$fileID.'">
			<input name="seriesData" type="hidden" value="'.$sSeries.'"/>
			<input name="showBanner" type="hidden" value="'.$cbPath.'"/>
			<input name="epNo" type="hidden" value="'.$episode.'"/>
			<input name="sNo" type="hidden" value="'.$season.'"/>
			<input name="lang" type="hidden" value="'.$lang.'"/>
			<input name ="fileName" type="hidden" value="'.$_POST['fileName'].'">
			<input name="banner" type="hidden" value="'.$cbPath.'">
			<input name="lang" type="hidden" value="'.$lang.'"/>
			<input name="unlinkFiles" type="hidden" value="'.$fileString.'">
			</form>';
			}
		}
	};
$output->setReloadLeftFrame(false);
$output->setMenuTitle('Search Result ||');
$output->setPageTitle( $showName.' - TV Specials');
$output->setScriptInHead($scriptInHead);
$output->setScriptInBody('bgColor="#F0F3F8"');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();
	}



	?>