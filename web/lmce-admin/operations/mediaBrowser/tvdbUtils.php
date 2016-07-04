<?php
/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com

	GPLv2 Licensed
	TVDB Data Grabber
	TvDB.com data grabber for television shows from the web admin, both for single and multiple files
	This file contains common functions for the metadata grabbing proccess

	checktvdb.php		- 	initialization
	tvdbUtils.php 		-	common include
	tvdbseries.php		-	Episode confirmation page
	tvdbAttributes.php	- 	Displays inserted attributes and screenshot
	tvdbbatch.php		-	Common functions for batch processing of files. Should be merged into tvdbUtils.php

	Technical Overview
	File name is passed to this plugin via a modification to _ANOTHER_ page. This reference is then parsed
	using regular expressions to try and extrapolate Show Name, Season Number, Episode Number. With these specific
	pieces of information, the exact episode can be referenced in the tvdb.com api and inserted into the linuxMCE
	database. So, after parsing, a search occurs. At the same time, the type of file is examined to determine if its
	a directory, which is processed in a different manner, or if its a tv special, which also has its own handling.
	After user selects the proper show, they are presented a confirmation screen, then taken to a results page
	showing what was added.

*/

include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
$apiKey = "4C6CEBDFB4558279" ;


function grabEp ($sID, $seasonNo, $episodeNo)
/* fetches relevant information to spider out from zip file from input
 * $sID = showID, unique show identifying number used by other sources as a unique identifier
 */
{			$xmlPath='/var/tmp';
			$lang= $_POST['lang'];
			$epInfArray['aImg']=actorImages($sID);
			$seriesBase = new DOMDocument();
			$seriesBase->load($xmlPath.'/'.$sID.'/'.$lang.'.xml');

			$episodes = $seriesBase->getElementsByTagName('Episode');

			foreach ($episodes as $episode)
				{
			$sNumBlock = $episode->getElementsByTagName('SeasonNumber');
			$sNum = $sNumBlock->item(0)->nodeValue;

			$epNumBlock = $episode->getElementsByTagName('EpisodeNumber');
			$epNum = $epNumBlock->item(0)->nodeValue;

			if ($epNum == $episodeNo && $seasonNo == $sNum)
			{
			$titleBlock= $episode->getElementsByTagName('EpisodeName');
			$epInfArray['title']= $titleBlock->item(0)->nodeValue;

			$epID= $episode->getElementsByTagName('id');
			$epInfArray['epid']= $epID->item(0)->nodeValue;

			$epWrite= $episode->getElementsByTagName('Writer');
			$epInfArray['writer']= $epWrite->item(0)->nodeValue;

			$epDir= $episode->getElementsByTagName('Director');
			$epInfArray['director']= $epDir->item(0)->nodeValue;

			$dvdChap= $episode->getElementsByTagName('DVD_CHAPTER');
			$epInfArray['dvdChap']= $dvdChap->item(0)->nodeValue;

			$dvdEpNo= $episode->getElementsByTagName('DVD_episodenumber');
			$epInfArray['dvd_epNo']= $dvdEpNo->item(0)->nodeValue;

			$dvdSeason= $episode->getElementsByTagName('DVD_season');
			$epInfArray['dvd_season']= $dvdSeason->item(0)->nodeValue;

			$epImgFlag= $episode->getElementsByTagName('EpImgFlag');
			$epInfArray['epImgFlag']= $epImgFlag->item(0)->nodeValue;

			$airEpNo= $episode->getElementsByTagName('EpisodeNumber');
			$epInfArray['air_epNo']= $airEpNo->item(0)->nodeValue;

			$firstAir= $episode->getElementsByTagName('FirstAired');
			$epInfArray['firstAir']= $firstAir->item(0)->nodeValue;

			$guestStars= $episode->getElementsByTagName('GuestStars');
			$epInfArray['guestStars']= $guestStars->item(0)->nodeValue;

			$imdb= $episode->getElementsByTagName('IMDB');
			$epInfArray['imdb']= $imdb->item(0)->nodeValue;

			$epLang= $episode->getElementsByTagName('Language');
			$epInfArray['lang']= $epLang->item(0)->nodeValue;

			$synopsis= $episode->getElementsByTagName('Overview');
			$epInfArray['synopsis']= $synopsis->item(0)->nodeValue;

			$epImg= $episode->getElementsByTagName('filename');
			$epInfArray['epImg']= $epImg->item(0)->nodeValue;

			$lastUpdate= $episode->getElementsByTagName('lastupdated');
			$epInfArray['lastupdate']= $lastUpdate->item(0)->nodeValue;

			$seasonId= $episode->getElementsByTagName('seasonid');
			$epInfArray['seasonID']= $seasonId->item(0)->nodeValue;

			$seriesID= $episode->getElementsByTagName('seriesid');
			$epInfArray['seriesID']= $seriesID->item(0)->nodeValue;
			echo "Series ID::".$epInfArray['seriesID'];
			$epInfArray['seasonNo'] = $sNum;
					}
					else
			{ 
			$epInfArray['status']=  'match fail for Season: '.$seasonNo.',Episode: '.$episodeNo;}		//	print_r($epInfArray);
			};
return $epInfArray;
};

function processImages($sID)
/*
 * Spiders out relevant images to the episode and series. returns an array of thumbnails, banners and other images
 * Note, all images need to be hosted and diplayed locally, TVDB.com does not allow hot linking.
 * Data loaded from xml file into XMLDom for parsing.
 */
{
$xmlPath='/var/tmp';
$apiKey = "4C6CEBDFB4558279";
$showName = $_POST['showName'];
//$sID = $_POST['showID'];
$fileID = $_POST['fileID'];
$showData =$_POST['seriesData'];
$lang = $_POST['lang'];

$processArray=array();
$vArray=array();
$seasonImgArry=array('seasons'=>array('1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20'));

			$bannerXml= new DOMDocument();
			$bannerXml->load($xmlPath.'/'.$sID.'/'.'banners.xml') or die ('file load error');
			$bannerArray = $bannerXml->getElementsByTagName('Banner');
			
			foreach ($bannerArray as $banner)
		{
				$testObj =$banner->getElementsByTagName('Season');				
				$seaNo = $testObj->item(0)-nodeValue;				
				$len = count_chars($seaNo);
				$i = 0;

				if ($seaNo == 0)
			{
				$btype = $banner->getElementsByTagName('BannerType');
				$bannerType = $btype->item(0)->nodeValue ;

				$bID = $banner->getElementsByTagName('id');
				$bannerID =$bID->item(0)->nodeValue ;

				$tbpath = $banner->getElementsByTagName('ThumbnailPath');
				$thumbBannerpath = $tbpath->item(0)->nodeValue ;
				$processArray[$bannerType]['thumbnailpath'] = $thumbBannerpath ;

				$bpath = $banner->getElementsByTagName('BannerPath');
				$Bannerpath = $bpath->item(0)->nodeValue ;
				$processArray[$bannerType]['bannerpath'] = $Bannerpath ;

				$Vpath = $banner->getElementsByTagName('VignettePath');
				$vBannerpath = $Vpath->item(0)->nodeValue ;
				$processArray[$bannerType]['VignettePath'] = $vBannerpath ;
			}
				else
			{
				$bID = $banner->getElementsByTagName('id');
				$bannerID =$bID->item(0)->nodeValue ;

				$sID = $banner->getElementsByTagName('Season');
				$seasonID =$sID->item(0)->nodeValue ;


				$btype = $banner->getElementsByTagName('BannerType');
				$bannerType = $btype->item(0)->nodeValue ;
				$seasonArray['Season'.$seasonID][$bannerID]['type'] = $bannerType;

				 $btype2 = $banner->getElementsByTagName('BannerType2');
				$bannerType2 = $btype2->item(0)->nodeValue ;
				$seasonArray['Season'.$seasonID][$bannerID]['bannertype2'] = $bannerType2;

				$tbpath = $banner->getElementsByTagName('ThumbnailPath');
				$thumbBannerpath = $tbpath->item(0)->nodeValue ;
				$seasonArray['Season'.$seasonID][$bannerID]['thumbnailpath'] = $thumbBannerpath ;

				$bpath = $banner->getElementsByTagName('BannerPath');
				$Bannerpath = $bpath->item(0)->nodeValue ;
				$seasonArray['Season'.$seasonID][$bannerID]['bannerpath'] = $Bannerpath ;

				$Vpath = $banner->getElementsByTagName('VignettePath');
				$vBannerpath = $Vpath->item(0)->nodeValue ;
				$processArray[$bannerType]['Vignettepath'] = $vBannerpath ;
			};

				$i++;
		};

            foreach ($bannerArray as $banner)
		{
				$testObj =$banner->getElementsByTagName('VignettePath');
				$seriesText = $testObj->item(0)-nodeValue;
				$testPath = $banner->getElementsByTagName('SeriesName');


				if ($testObj != NULL && $testPath = 'true' )
			{
				$btype = $banner->getElementsByTagName('BannerType');
				$bannerType = $btype->item(0)->nodeValue ;

				$bID = $banner->getElementsByTagName('id');
				$bannerID =$bID->item(0)->nodeValue ;

				$tbpath = $banner->getElementsByTagName('ThumbnailPath');
				$thumbBannerpath = $tbpath->item(0)->nodeValue ;
				$vArray[$bannerType]['thumbnailpath'] = $thumbBannerpath ;

				$bpath = $banner->getElementsByTagName('BannerPath');
				$Bannerpath = $bpath->item(0)->nodeValue ;
				$vArray[$bannerType]['bannerpath'] = $Bannerpath ;

				$Vpath = $banner->getElementsByTagName('VignettePath');
				$vBannerpath = $Vpath->item(0)->nodeValue ;
				$vArray[$bannerType]['VignettePath'] = $vBannerpath ;
			}
		}

			 	$imgArray=array($processArray, $seasonArray, 'vg'=>$vArray);
				return $imgArray;
} ;

function saveData($dataFile)
{
/*
 * Saves the requested file into the tmp directory. Its a zip containing the series information in XML files.
 * Shows are organized by show ID($sID) and $lang as the XML files in them share similar names.
 */
			$cbPath = $_POST['showBanner'];
			$sID = $_POST['showID'];
			$xmlPath='/var/tmp';

			$lang= $_POST['lang'];
			if(is_dir($xmlPath.'/'.$sID.'/'))
			{
			$xmlGrab = file_put_contents($xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip' ,$dataFile) ;
			//echo 'wrote'.$xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip';
			unlink('/var/tmp/'.$sID.'/actors.xml');
			unlink('/var/tmp/'.$sID.'/'.$lang.'.xml');
			unlink('/var/tmp/'.$sID.'/banners.xml');
			}
			else
                {
			mkdir($xmlPath.'/'.$sID.'/', 0777, true);
			$xmlGrab = file_put_contents($xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip' ,$dataFile)or die ('cant save! inside of new directory!');
			//echo 'wrote'.$xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip';
                }
}

function getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo)
{
/*
 * Retrieves all the series data from the xmlfile, filtering out what isnt needed. Returns various elements
 * outlined as it traverses the XMLDom
 */
			$cbPath = $_POST['showBanner'];
			$lang	= $_POST['lang'];
			$xmlPath='/var/tmp';

		   	$Dzip = $xmlPath.'/'.$sID.'/'.$sID.'.'.$lang.'.zip';
			$zip = new ZipArchive;
			if ($zip->open($Dzip) === TRUE)
			{
		    $zip->extractTo($xmlPath.'/'.$sID.'/');
  			  $zip->close();

			$seriesImg = processImages($sID);
			$aImage= actorImages($sID);

			$seriesBase = new DOMDocument();
			$seriesBase->load($xmlPath.'/'.$sID.'/'.$lang.'.xml');

			$seriesName = $seriesBase->getElementsByTagName('SeriesName');
			$sName =$seriesName->item(0)->nodeValue;

			$overview = $seriesBase->getElementsByTagName('Overview');
			$textDesc= $overview->item(0)->nodeValue;

			$actors = $seriesBase->getElementsByTagName('Actors');
			$talent= $actors->item(0)->nodeValue;

			$genre = $seriesBase->getElementsByTagName('Genre');
			$oGenre= $genre->item(0)->nodeValue;

			error_reporting(0);
			$episodes = $seriesBase->getElementsByTagName('Episode');

			foreach ($episodes as $episode)
				{
					$sNumBlock = $episode->getElementsByTagName('SeasonNumber');
					$sNum = $sNumBlock->item(0)->nodeValue;
				if ($seasonNo == $sNum)
						{
					$oepCount = $episode->getElementsByTagName('Episode');
					$EpCount[]= $oepCount->item(0)->nodeValue;
						}
				}
				$numEp = count($EpCount);
			$sp = array_values($seriesImg[1]['Season'.$sesasonNo]);
			$Ph= $sp[0]['bannerpath'];
			$showPoster = $Ph;


			$cleanActors = str_replace("|", " |", $talent) ;
	//===================================episodegrab===============================================
			$seriesBase->savexml();
			$episodeInfo = grabEp($sID, $seasonNo, $episodeNo);

			$episodeInfo['series']=array('series'=>$sName, 'desc'=>$textDesc, 'seriesactors'=>$cleanActors,
			 'genre'=>$oGenre, 'showPoster'=>$showPoster, 'seriesImg'=>$seriesImg, 'showID'=>$sID, 'lang'=>$lang , 'episodeCount'=>$numEp, 'aImg'=>$aImg);
			$episodeInfo['fileID']=$fileID;
			return $episodeInfo; error_reporting(1);
	//================================output=======================================================
		}
			 else
			{
 			   echo 'failed opening file'.$xmlPath.'/'.$sID.'/'.$lang.'.xml';
			}
		}



function makeDirs($fileID,$mediadbADO,$dbADO, $output)
{
/*
 * Function that is semi-obsolete and needs to be re-written.
 */
	$fileList;
	$fileList['mirrors'] = 'mirrors.xml' ;						//as documented in the TVDB api
	$fileList['seriesData'] = 'seriesData.xml';
	$fileList['baseConfig'] = 'baseConfig.xml';
	$fileList['lang'] = 'language.xml';
	$fileList['seriesID']='seriesID.xml' ;

	foreach($fileList as $fName =>$fileName)
	{
	$newFilename = '/var/tmp'.$fileName;
	$fileHandle = fopen($newFilename, w) or die('cant create!') ;
	fclose($fileHandle)or die('file creation error');
	$out.='Created:'.$newFilename.'<br>';
 	}
	$mirrorDoc = new DOMdocument();
	$mirrorDoc -> load('http://www.thetvdb.com/api/4C6CEBDFB4558279/mirrors.xml');
	$mirrorDoc ->save('/var/tmp/mirrors.xml') or die ('error');
	$out.='<br><b>Mirrors - Check!</b>' ;

	$langDoc= new DOMdocument();
	$langDoc ->load('http://www.thetvdb.com/api/4C6CEBDFB4558279/languages.xml');
	//$langDoc->save('../tmp/languages.xml') or die ('error');
	$out.='<br><b>Languages - Check!</b>' ;
	$out.='<br><h2>File Creation Done</h2>';
	//searchSeries($fileID,$mediadbADO,$dbADO, $output);


};

function searchSeries($fileID,$mediadbADO,$dbADO, $output)
 {
 /*
  * Initial search function that attempts to parse out Name of the show, season and episode.
  * Also attempts to detect video resolution ONLY based on text, not actual inspection of video.
  */
 	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');

 $externalAttributesBtn3='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'">';
 $scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>

 				<script>

 				function clear()
 				{
 				this.value="";
 				}

 				function box()
 				{
 				document.getElementById("specials").innerHTML = "<div id=\"searchBox\" align=\"right\" stlye=\"display: none\"><input type=\"text\" id=\"seriesName\" value=\"Enter Special Name\" onclick=\"clear()\"><br><input type=\"hidden\" value=\"0\" name=\"specSeas\"><br><input type=\"submit\" onclick=\"getSpecials()\"/></div>";
 				}
 					 </script>';
 		$output->setScriptInHead($scriptInHead);
		$resFile=$mediadbADO->Execute('SELECT * FROM File WHERE PK_File=?',$fileID);
		$rowFile=$resFile->FetchRow();

		//$fileData=getFieldsAsArray('Filename',$mediadbADO,'WHERE PK_File='.$fileID) or die ('error!!');

		$extracted = $rowFile['Filename'];
		$guessedArray = stringParse($rowFile['Filename']) ;
		$langArray = array();

		$langXML = New DOMDocument();
		$langXML ->load('http://www.thetvdb.com/api/4C6CEBDFB4558279/languages.xml');
		//$langDoc->save('var/tmp/languages.xml') or die ('error');
		$langXML->saveXML;

		$langSel = $langXML->getElementsByTagName('Language');
		foreach ($langSel as $language)
	{
		$langBlock = $language->getElementsByTagName("name");
		$langName = $langBlock->item(0)->nodeValue;

		$langBlock = $language->getElementsByTagName("abbreviation");
		$langAbbrev = $langBlock->item(0)->nodeValue;

		$langBlock = $language->getElementsByTagName("id");
		$langID = $langBlock->item(0)->nodeValue;

		$langArray[$langID]['name']=$langName;
		$langArray[$langID]['abbrev']=$langAbbrev;
	};

		$out='
				<table width="75%" border="0" cellpadding="0" align="center" id="searchTable"><form action="index.php" method="post" id="searchForm" name="searchtvDB">
 				<div id="wrapper"><tr>';
  				if ($rowFile['IsDirectory']==1)
				{
				$out.='<th colspan="3" height="150"><div id="batch"><img src=operations/mediaBrowser/images/batch.png height="110" width="130" align="center"><b>Batch Mode, Directory Detected</b><input name="isDir" type="hidden" value="1"/></div></th>';
				$output->setPageTitle('|Batch Search');
				}else
				{
				$out.='<th colspan="3" height="150"><div id="single"><img src=operations/mediaBrowser/images/single.png height="110" width="130" align="center"><b>Single File Mode</b>
				</div></th>';$output->setPageTitle('|File Search');
				};
   		 		$out.='
   		 		</tr>
 		 		<tr>
				<tr id="metaData"><td width="150" id="filename"><p><b>'.$TEXT_FILENAME_CONST.': </b>'.$extracted.'</p>Manual Show Entry: <input type=text id="showName2" name="showName2" size="15"><br>
				<b>Guessed Season Number: </b><input name="sNo" type="text" value="'.$guessedArray['season'][0].'" size="2" maxlength="50"><br>
				<b>Guessed Episode:       </b><input name="epNo" type="text" value="'.$guessedArray['ep'][0].'" size="2" maxlength="50">';

   		 		$out.='<p><input name="submit" type="submit" value="Search!"></p></td><td rowspan="2" width="200">
    		  	<input type="hidden" name="section" value="searchtvDB">
                <input type="hidden" id="fileID" name="fileID" value="'.$fileID.' ">
                <input type="hidden" id="filename" name="fileName" value="'.$extracted.' ">
                <input type="hidden" id="rez" name="rez" value="'.$guessedArray['rez'].' ">
                <b>Guessed Show Name</b><div id="appear_test" style="display:none">' ;
				foreach ($guessedArray['title'] as $title =>$key)
				{
				$clean = str_replace( ".", " ", $key)	;
				$out.='<p><input type=radio id="showName1" name="showName1" value="'.$clean.'">'.$clean.'</p>';
				};
				$out.='</div></td></tr>
    		    <tr><td><p><input name="special" type=checkbox>Special Episode?</p>
				';
				$out.='<br><b>Language</b><p><br>Saved for future.<div><select length="2" name=lang><option value="'.$GLOBALS['lang'].'">'.$GLOBALS['lang'].'</option>' ;
				foreach ($langArray as $lang => $val)
					{
				$out.='<option value="'.$val['abbrev'].'">'.$val['abbrev'];
					};
   		   		$out.='</option></div><br>
    			</td>'.$externalAttributesBtn3.'
  				</tr><tr><td colspan="3" height="100" {table-layout:fixed}></td></tr>
  				<tr {valign:bottom}><td align ="center" colspan="3">Television Metadata generously provided by: <a href="http://thetvdb.com">TheTVDB.com</a> </tr></td>
				</form></table></div>';
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('Search TVDB |');
				$output->setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);
				$output->output();
};

	function stringParse($extracted)
{
/*
 * Parses filename input to try and extrace Show name, season and episode based on a regular expression brute force
 * deal.
 */
	$guessArray = array() ;

	$patterns = array();
	$patterns[0]= array('s0','-s0','so', '[sx', '-', '[','.s0', ' -', ' - ', ' s', 'season ',' season ', 'season ','.season.', '1x ', ' 1x', '-1x'
	, '2x', '3x', '4x' , '5x', '6x', '7x', '8x', '9x' ,' 0', '_', ' ', 'pt', 'ep', 'EP', 'Ep');

	$patterns[1]= array('ep', 'ex','-') ;
	$smatch= 1;
	$lower=htmlentities(strtolower($extracted));

	if (preg_match("/(?<=s[0])\d/", $lower, $smatch) )	//pattern match season s0(digit)
	{	$guessArray['season'] = $smatch;
	}
	elseif
	(preg_match("/(?<=s[0])\d/", $lower, $smatch) )		//pattern match season s0(double digit)
	{
	$guessArray['season'] = $smatch;
	}
	elseif
	(preg_match("/(?<=s)[1-9]/", $lower, $smatch) )		//pattern match season s1-9
	{
	$guessArray['season'] = $smatch;
	}
	elseif
	(preg_match("/(?<=sx)[1-9]\d/", $lower, $smatch) )		//pattern match season sx(double digits)
	{
	$guessArray['season'] = $smatch;
	}
	elseif
	(preg_match("/(?<=season)[1-9]\d/", $lower, $smatch) )		//pattern match season sxdd
	{
	$guessArray['season'] = $smatch;
	}
	elseif
	(preg_match("/(?<=season.[0])\d/", $lower, $smatch) )		//pattern match season sxdd
	{
	$guessArray['season'] = $smatch;
	}
elseif
	(preg_match("/(?<=season.)\d/", $lower, $smatch) )		//pattern match season sxdd
	{
	$guessArray['season'] = $smatch;
	}
elseif
	(preg_match("/(?<=season.])\d\d/", $lower, $smatch) )		//pattern match season sdd
	{
	$guessArray['season'] = $smatch;
	}
	elseif
	 (preg_match("/[1-9](?=x\d\d)/", $lower, $smatch) )	//pattern match season with digit notation dxdd
	{
	$guessArray['season'] =  $smatch;
	}
	elseif
	 (preg_match("/(?<=[0])\d(?=\d\d)/", $lower, $smatch) )	//					dDdd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/[1-9](?=\d[0-9])/", $lower, $smatch) )	//					dDdd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/(?<=\ss)\d/", $lower, $smatch) )		//pattern matching season _sd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/(?<=s\s)[1-9]\d/", $lower, $smatch) )		//pattern matching s_dd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/(?<=\season)[1-9]\d/", $lower, $smatch) )		//pattern matching seasondd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/(?<=\season\s)[1-9]\d/", $lower, $smatch) )		//pattern matching season _dd
	{
	$guessArray['season'] = $smatch ;
	}
	elseif
	 (preg_match("/(?<=\season\s)[1-9]/", $lower, $smatch) )		//pattern matching season _dd
	{
	$guessArray['season'] = $smatch ;
	}
elseif
	 (preg_match("/(?<=\season.)[1-9]\d/", $lower, $smatch) )		//pattern matching season _dd
	{
	$guessArray['season'] = $smatch ;
	}
elseif
	 (preg_match("/(?<=-s)\d(?=-e\d)/", $lower, $smatch) )		//pattern matching season _dd
	{
	$guessArray['season'] = $smatch ;
	}
		;
//============================================end season preg_match============================================
if	(preg_match("/(?<=e[0])\d/", $lower, $ematch) )		//pattern matching episode edd
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\s\d\d[0])\d/", $lower, $ematch) )					//dddd
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\s\d\d)[1-9]\d/", $lower, $ematch) )					//dddd
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\s\d)[1-9]\d/", $lower, $ematch) )					//dddd
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\s\d[0])[1-9]/", $lower, $ematch) )					//dddd
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=e)[1-9]\d/", $lower, $ematch) )		//pattern matching episode ed_
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=e)\d/", $lower, $ematch) )		//pattern matching episode ed_
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=-e)\d/", $lower, $ematch) )		//pattern matching episode ed_
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=ep)\d\d/", $lower, $ematch) )
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=ep)\d/", $lower, $ematch) )
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\dx)[1-9]\d/", $lower, $ematch) )
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\dx[0])\d/", $lower, $ematch) )
	{	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/\S(?<=\d\d)[1-9][1-9]/", $lower, $ematch) )  	// " 0018"
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=\s\d\d)[1-9][1-9]/", $lower, $ematch) )	// " 0028"
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=-s\d-e)\d\d-/", $lower, $ematch) )		// "- 0-e19-"
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/(?<=-s\d-e)\d(?=-)/", $lower, $ematch) )		// "- 0-e4-"
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	(preg_match("/\d(?=of\d)/", $lower, $ematch) )			// "0of9"
	{
	$guessArray['ep'] = $ematch ;
	}
elseif
	 (preg_match("/(?<=[1-9]x)[1-9]\d/", $lower, $ematch) )	//pattern match season with digit notation dxdd
	{
	$guessArray['ep'] =  $ematch;
	}
elseif
	 (preg_match("/(?<=[1-9]x)[1-9]/", $lower, $ematch) )	//pattern match season with digit notation dxdd
	{
	$guessArray['ep'] =  $ematch;
	}
	elseif
	 (preg_match("/(?<=\s)[1-9]\d/", $lower, $ematch) )	//pattern match season with digit notation dxdd
	{
	$guessArray['ep'] =  $ematch;
	}
	elseif
	 (preg_match("/(?<=episode\s0)[1-9]/", $lower, $ematch) )	//pattern match season with digit notation
	{
	$guessArray['ep'] =  $ematch;
	}
	elseif( preg_match("/(?<=Episode.0)[1-9]/", $lower, $ematch) ) {
	$guessArray['ep'] =  $ematch;
	}
	elseif
	(preg_match("/(?<=_|_0)([1-9]|\d\d)/", $lower, $ematch) )
	{
	$guessArray['ep'] = $ematch ;
	}
	elseif
	 (preg_match("/[0-9](?<=of\d)/", $lower, $ematch) )		//guessing 1 off specials
	{
	$guessArray['ep'] = $ematch ;
	}

foreach ($patterns[0] as $pattern=> $key)
	{
		$sPos = strpos($lower, $key);

		if ($showTitle = substr($lower, 0, $sPos))
		{
		$guessArray['title'][] = $showTitle;
		}

	};

	if (!$guessArray['title'])
	{$guessArray['title'][]= $lower;}

 $formatEnum= array ("Standard Def"=>3, "HD 720"=>4, "HD 1080"=>5);
 if (preg_match("/(HDTV|hdtv|720|1080)/", $lower, $rez))
 {
 	if ($rez[0] =="HDTV" || "hdtv")
 	{
 	$rezProper= "3";
 	 $guessArray['rez'] = $rezProper;

 	}
 	elseif ($rez[0]== "720")
 	{
 	$rezProper="4";
 	 $guessArray['rez'] = $rezProper;
 	}
 	elseif ($rez[0]=="1080")
 	{
 	$rezProper="5";
 	 $guessArray['rez'] = $rezProper;
 	}
 }
 	 else
 	  {
 $rezProper ="3";
 $guessArray['rez'] = $rezProper;

 }

return $guessArray ;

};

/*
 * These functions are called when certain information needs to be inserted into the database.
 */

//======================================synopsis============================================================
function insertSynopsis($episodeData, $fileID, $mediadbADO, $dbADO, $output, $con)
{	$SfileIdent = $fileID;
	$synopsis=mysqli_real_escape_string($episodeData['synopsis']);
	$Ssql = "SELECT * FROM `LongAttribute` WHERE `Text` = \"$synopsis\" AND FK_File=\"$SfileIdent\" ";
	$Sresult = mysqli_query($mediadbADO, $Ssql)or die (mysqli_error($mediadbADO));
	$Scount = mysqli_num_rows($Sresult);

	if ($Scount == 0)
	{
			$SfileIdent = $_POST['fileID'];
			mysqli_query($mediadbADO, "INSERT INTO LongAttribute VALUES (\"\" , 37 ,NULL, \"$SfileIdent\", NULL, \"$synopsis\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )")or die (mysqli_error($mediadbADO));
			}
		return ;
}
	//===========================================================actor(s)=================================================

function addActors($aImgArray, $actorArray, $fileID, $mediadbADO, $dbADO, $output, $con)
{
$ATotal = count($actorArray);

	$i=0;
	$fileIdent = $fileID; $attribType = 2;														//file id for reference
	while ($i < $ATotal)																				//looping through actors in array
	{
	$content=mysqli_real_escape_string($actorArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType";	//checking to see if the performer exists
	$result = mysqli_query($mediadbADO, $sql);
	$count = mysqli_num_rows($result);
	$row = mysqli_fetch_assoc($result);
	//print_r($row);
	$attrib = $row['PK_Attribute'];
																		//if it exists, used later for ref
	if ($count < 1)																						//catch to see if we got a result
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));									//getting the new attribute number for use
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
			//add images in case of new actor
			$currPerf = trim($content);
	$currUrl = $aImgArray[$currPerf];
	$attrNo = $insertRes;
	//addActorImages($currUrl, $attrNo, $currPerf, $con , $mediadbADO, $dbADO, $output);
	}
	else
	{
	$fileIdent = $fileID;
	//add image
	$currPerf = trim($content);
	$currUrl = $aImgArray[$currPerf];
	$attrNo = $attrib;
	//echo $attrib;
	//addActorImages($currUrl, $attrNo, $currPerf, $con , $mediadbADO, $dbADO, $output);
	//associate with this file

	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;	//check to see if associated
	$chkResult = mysqli_query($fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ( $chkCount == 0)
	{
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	} else
	{
	}
	};


	$i++;}

}
//=================================================directors==================================
function addDirectors($directorArray, $fileID, $mediadbADO, $dbADO, $output, $con)
{
	$dTotal = count($directorArray);
	//print_r($directorArray);
	//echo '<b>Total Attributes: </b>'.$dTotal.'<br>' ;
	$i=0;
	$fileIdent = $fileID; $attribType = 1;
	while ($i < $dTotal)
	{
	$content=mysqli_real_escape_string($directorArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType";
	$result = mysqli_query($mediadbADO, $sql);
	$count = mysqli_num_rows($result);
	$row = mysqli_fetch_assoc($result);
	//print_r($row);
	$attrib = $row['PK_Attribute'];
	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
	}
	else
	{
	$fileIdent = $fileID;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysqli_query($mediadbADO, $fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ( $chkCount == 0)
	{
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	} else
	{
	}
	};
	$i++;}
}
//==========================================episode title=========================================
function addEpTitle($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output)
{	
//print_r($episodeData);
	$epTitle=mysqli_real_escape_string($episodeData['title']);
	
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$epTitle\" AND FK_AttributeType = 11"; 
	$result = mysqli_query($mediadbADO, $sql);
	$count = mysqli_num_rows($result);	 
	$row = mysqli_fetch_assoc($result);	
	$attrib = $row['PK_Attribute'];	
	
	$fileIdent = $episodeData['fileID'];
	
	if ($count < 1)
	{
	$out.= 'Show title not found...' ;
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , 11 , \"$epTitle\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;  	
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));	
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			 mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
	}
	else
	{	
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileIdent";
	$chkResult = mysqli_query($mediadbADO, $fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult); 
	if ($chkCount < 1)
	{				
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
	mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	} else
	{ 
	$out.='already in ep, skipping';}
	};
	return ;		
}
//=========================================epidsode img=====================================
function episodeImage ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output)
{
$fileIdent=$episodeData['fileID'];
//print_r($fileIdent);
$test=mysqli_query($mediadbADO, "SELECT * FROM Picture_File WHERE FK_File=\"$fileIdent\" ") or die (mysqli_error($mediadbADO));
$fileCount = mysqli_num_rows($test); 
//echo $fileCount; 
if ($fileCount < 1)
{
$fullUrl ='http://www.thetvdb.com/banners/'.$episodeData['epImg'];
$img = file_get_contents($fullUrl);
$fName1 = strrchr($episodeData['epImg'], '/');
$fName = str_replace (".jpg", "", $fName1);
$savefile = fopen('/var/tmp'.$fName.'.jpg', 'w');
fwrite($savefile, $fullUrl);
fclose($savefile);
$import_cover_art='http://www.thetvdb.com/banners/'.$episodeData['epImg'];
$extension=strtolower(str_replace('.','',strrchr($import_cover_art,".")));	

	if($import_cover_art!=''){
		mysqli_query($mediadbADO, "INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
		$picID=(int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
		mysqli_query($mediadbADO, "INSERT INTO Picture_File (FK_Picture,FK_File) VALUES (\"$picID\",\"$fileIdent\")");

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
					 unlink('/var/tmp/'.$fName.'.jpg');	
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension, 256, 256);
					if(!$resizeFlag){
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
					}
				}
			}		
	}
}
}
//====================================viginette image=========================================================
function addViginetteImg ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output, $attrib)
{
$fileIdent = $fileID;

$test=mysqli_query($mediadbADO, "SELECT * FROM Picture_Attribute WHERE FK_Attribute=\"$attrib\" ") or die (mysqli_error($mediadbADO));
$fileCount = mysqli_num_rows($test);
//echo $fileCount;
if ($fileCount < 1)
{
$fullUrl ='http://wwww.thetvdb.com/banners/'.$episodeData['series']['seriesImg'][0]['Fanart']['VignettePath'];
$img = file_get_contents($fullUrl);
$fName1 = strrchr($episodeData['series']['seriesImg'][0]['Fanart']['VignettePath'], '/');
$fName = str_replace (".jpg", "", $fName1);
$savefile = fopen('/var/tmp'.$fName.'.jpg', 'w');
fwrite($savefile, $fullUrl);
fclose($savefile);
$import_cover_art='http://www.thetvdb.com/banners/'.$episodeData['series']['seriesImg'][0]['Fanart']['VignettePath'];
$extension=strtolower(str_replace('.','',strrchr($import_cover_art,".")));

	if($import_cover_art!=''){
		mysqli_query($mediadbADO, "INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
		$picID=(int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
		mysqli_query($mediadbADO, "INSERT INTO Picture_Attribute (FK_Picture,FK_Attribute) VALUES (\"$picID\",\"$attrib\")");

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
					 unlink('/var/tmp/'.$fName.'.jpg');
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension, 256, 256);
					if(!$resizeFlag){
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
					}
				}
			}
	}
}
}
//============================================================================================================

//=======================================genre================================================================
function addGenre($genreArray, $con ,$fileID, $mediadbADO, $dbADO, $output)
{
	$gTotal = count($genreArray);
	$i=0;
	$fileIdent = $fileID;
	$attribType = 8;
	while ($i < $gTotal)
	{
	$content=mysqli_real_escape_string($genreArray[$i]);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` =\"$content\" AND FK_AttributeType = $attribType";
	$result = mysqli_query($mediadbADO, $sql);
	$count = mysqli_num_rows($result);
	$row = mysqli_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];
	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
		}
	else
	{
	$fileIdent = $fileID;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysqli_query($mediadbADO, $fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ( $chkCount == 0)
	{
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	} 
	else
	{
	
	}
};
	$i++;}
}
//=================================================================mediasubtype=========================
function mediaSubTypes ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output)
{

$format=$_POST['rezOver'];
	
$fileIdent = $fileID;
//print_r($format);
mysqli_query($mediadbADO, "UPDATE File SET FK_FileFormat=\"$format\" WHERE `PK_File`=\"$fileIdent\" ")or die (mysqli_error($mediadbADO)); 
mysqli_query($mediadbADO, "UPDATE File SET FK_MediaSubType=1 WHERE `PK_File`=\"$fileIdent\" ")or die (mysqli_error($mediadbADO)); 
return "Subtypes Complete";

}
//=================program title================================================================================
function addProgTitle ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output)
{
	$program=mysqli_real_escape_string($episodeData['series']['series']);
	//print_r($program);
	$sql = "SELECT * FROM `Attribute` WHERE `Name` = \"$program\" AND FK_AttributeType = 12";
	$result = mysqli_query($mediadbADO, $sql) or die(mysqli_error($mediadbADO));
	$row = mysqli_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];
	$count = mysqli_num_rows($result);
	$attrib = $row['PK_Attribute'];
	$fileIdent = $fileID;
	if ($count < 1 )
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , 12 , \"$program\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			 mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
			 return "New Title:".$program." added to database <br>";
	}
	else
	{
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileID";
	$chkResult = mysqli_query($mediadbADO, $fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ($chkCount < 1)
	{
	$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileID\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
	mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	return "Title:".$program." associated to file. <br>";
	}
	else {return "Already Associated <br>";}
	//addViginetteImg ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output, $attrib);
	
	};
	
}

//=================title================================================================================
function addTitle ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output)
{
        $program=mysqli_real_escape_string($episodeData['title']);
	
	$sql = "SELECT * FROM `Attribute` WHERE `Name` = \"$program\" AND FK_AttributeType = 13";
	$result = mysqli_query($mediadbADO, $sql) or die(mysqli_error($mediadbADO));
	$row = mysqli_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];
	$count = mysqli_num_rows($result);
	$attrib = $row['PK_Attribute'];
	$fileIdent = $fileID;
	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , 13 , \"$program\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			 mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
			 return "New Title:".$program." added to database <br>";
	}
	else
	{
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File = $fileID";
	$chkResult = mysqli_query($fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ($chkCount < 1)
	{
	$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileID\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
	mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	return "Title:".$program." associated to file. <br>";
	}
	else {return "Already Associated <br>";}
	//addViginetteImg ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output, $attrib);
	
	};
	
}
//==========================================================================================episodeID and season ID and seriesID
function ids($episodeData, $fileID, $con ,$attributeType, $mediadbADO, $dbADO, $output)
{//	print_r($episodeData);
	$idArray = array( array('TV Season ID'=>$episodeData['seasonID']), array('TV Series ID'=>$episodeData['seriesID']), array( 'TV Program ID'=>$episodeData['epid']), array("Release Date" =>$episodeData['firstAir']), array("Episode Number" =>$episodeData['air_epNo']), array("Season Number" =>$episodeData['seasonNo']) );
 	$counter = count($idArray);

 //	echo $counter;	
// echo $idArray['TV Season ID'];
 	$i=0;
 	$flip = array_flip($attributeType);
	$fileIdent = $fileID;
while ($i < $counter)
	{
	$item = $idArray[$i];
	
	$attribType = $flip[key($idArray[$i])];	
	$content1 = array_values($idArray[$i]);
	$content = $content1[0];
	
	$sql = "SELECT * FROM `Attribute` WHERE `Name` = \"$content\" AND FK_AttributeType = \"$attribType\" ";
		
	$result = mysqli_query($mediadbADO, $sql) or die (mysqli_error($content));
	$count = mysqli_num_rows($result);
	$row = mysqli_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];

	if ($count < 1)
	{
			$iQuery = "INSERT INTO Attribute VALUES (\"\" , $attribType , \"$content\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )" ;
			mysqli_query($mediadbADO, $iQuery) or die (mysqli_error($mediadbADO));
			$insertRes= (int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			//echo $insertRes;
			$idQuery="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"'.$insertRes.'\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
			mysqli_query($mediadbADO, $idQuery)  or die (mysqli_error($mediadbADO));
		}
	else
	{
	$fileIdent = $fileID;
	$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= $attrib AND FK_File=$fileIdent" ;
	$chkResult = mysqli_query($mediadbADO, $fileChk) or die (mysqli_error($mediadbADO)); $chkCount = mysqli_num_rows($chkResult); $chk = mysqli_fetch_assoc($chkResult);
	if ( $chkCount < 1)
	{
		$idQuery2="INSERT INTO File_Attribute  VALUES (\"$fileIdent\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysqli_query($mediadbADO, $idQuery2)  or die (mysqli_error($mediadbADO));
	} else
	{
	}
	};
	$i++;}
}


function batchAttributes ($fileID, $mediadbADO, $dbADO, $output, $episodeInfo)
{
/*
 * This is a batch attribute function that calls the other functions recursively to process multiple files.
 */

	$apiKey = "4C6CEBDFB4558279" ;
	$xmlPath='/var/tmp';
	$seriesName =$_POST['sName'];
	$seasonNo = $_POST['seasonNo'];
	$episodeNo = $_POST['episodeNo'];
	$sID = $_POST['seriesID'];
	$lang= $_POST['lang'];

	$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	$data = file_get_contents($seriesDataRequest);
	$episodeData = getSeriesData($seriesDataRequest, $sID, $lang, $data, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
	insertAttributes($fileID,$mediadbADO,$dbADO, $output, $episodeInfo, $episodeData, $xmlPath);


}

function insertAttributes($fileID, $mediadbADO, $dbADO, $output, $episodeData)
{
/*
 * This function is called when a file is confirmed and attributes need to be added. It then gathers data to then feed
 * the various smaller insert functions.
 */
	$pass=""; $user="root"; $db="pluto_media"; $con= mysqli_connect("localhost", $user, $pass); 	 // connection
	if (!$con) { die('Could not Connect'.mysqli_error($con)); //error messaging
	$connMessage="Fail";
	};
	if ($con) {mysqli_select_db("pluto_media") or die(mysqli_error($con)); //connect to media db or error out
	$connMessage="Conn Good";
	};
$seasonNo = $episodeData['seasonNo'];
$episodeNo = $episodeData['air_epNo'];
$mediaSubTypeEnum = array (1 => "TV Shows",2 => "Movies",3 => "Home Videos", 4 => "Sports Events", 5 => "Music Videos", 6 => "Alternative", 7 => "Popular Music", 8 => "Classical Music");
$fileFormatEnum = array (1 => "Low Res",2 => "DVD",3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");

//dynamic array to deal with changes to attribute types
$query = "SELECT PK_AttributeType, Description from AttributeType";
$result = mysqli_query($con, $query);
$attributeType=array(1=>"Director",
2=>"Performer",
3=>"Album",
4=> "Track",
6=> "ASIN",
7=> "CDDB",
8=> "Genre",
9=> "Ref #",
10=>"Channel",
11=>"Episode",
12=> "Program",
13=> "Title",
14=>  "Disc ID",
15=>"Conductor",
16=>"Composer/Writer",
17=>"Studio",
18=>"Rating",
19=>"Release Date",
20=> "Website",
21=>"Chapter",
22=>"Encoding",23=> "Format", 24=> "Aspect Ratio(s)", 25=> "Audio Encoding", 26=>"Rated", 27=> "DVD Release Date",28=>"Run Time", 29=> "Keyword", 30=> "Screen Saver For MD",
31=>"Format", 32=> "Region", 33=> "Language", 34=> "Aspect Ratio", 35=>"Number of Discs", 36=> "IMDB", 37=>"Synopsis", 38=> "Producer", 39=>"Executive Producer", 40=>"Purchase Info",
41=> "TV Channel ID", 42=> "TV Program ID", 43=>"TV Series ID", 44=>"Creation Date", 45=> "THX Disk ID", 46=>"Screen saver disabled", 47=>"Manufacturer", 48=>"Year",
49=>"System", 50=>"Season Number", 51=> "Episode Number", 52=> "TV Season ID",  56=> "System Configuration"
);




/*!
 * \note Temporary work around to fix broken db issue. 
 * */


 
 
$aTypeProper= array_flip($attributeType);


$directors = explode("|", $episodeData['director']);
$seriesActors = explode("|", $episodeData['series']['seriesactors']);
$genreArr = explode("|", $episodeData['series']['genre']);
$aImgArray = $episodeData['aImg'];

if ($_POST['catchUnlink'])
{ 
unlink ('coverartscan/searchImg'.$sID.'.jpg');
}

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

$fileID = $episodeData['fileID'];

mediaSubTypes ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output);
//print_r($subTypeStatus);

addProgTitle ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output);

addTitle ($episodeData, $con ,$fileID, $mediadbADO, $dbADO, $output);

//print_r($progTitleStatus);

insertSynopsis($episodeData, $fileID, $mediadbADO, $dbADO, $output, $con);

addActors($aImgArray, $actorArray, $fileID, $mediadbADO, $dbADO, $output, $con);

addEpTitle($episodeData, $fileID, $mediadbADO, $dbADO, $output, $con);

addDirectors($directorArray, $fileID, $mediadbADO, $dbADO, $output, $con);

episodeImage($episodeData, $fileID, $mediadbADO, $dbADO, $output, $con);

addGenre($genreArray, $con ,$fileID, $mediadbADO, $dbADO, $output);

ids($episodeData, $fileID, $con ,$attributeType	, $mediadbADO, $dbADO, $output);


}

function  tvdbSeries($output,$mediadbADO,$dbADO)
{
//error_reporting(E_ALL);
/*
 * Displays information about the file selected with a pane for the screen shot and actors. Normally has tv series information with it
 */

	$scriptInHead ='<style>
	#pictureFrame
	{
	align:center;
	position:fixed;
	height:300px;
	width: 150px;
	overflow-y:scroll;

	}
	#container
	{
	height:180px;
	width:75%;
	padding:5px;

	}
	</style>';
	$fileFormatEnum = array (1 => "Low Res",2 => "DVD",3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");
	$xmlPath='/var/tmp';
	//$unlinkAction = $_POST['unlinkFiles'];

	/* foreach ($_POST['unlinkFiles'] as $key ->$val)
		{
			echo $key;
		}
		*/

	//print_r($_POST['unlink print_r($episodeInfo);Files']);
	//$ike = count($_POST['unlinkFiles'][0]);
	//print_r($ike);
	//removing pictures from the search------------------------------------------|
	$tempTrashArray = explode("," , $_POST['unlinkFiles']);
	$unlinkArray =array_slice( $tempTrashArray, "1");
	//print_r($unlinkArray);

	//---------------------------------------------------------------------------|
	//------api key------------------|
	$apiKey = "4C6CEBDFB4558279";
	//-------------------------------|

	//Basic information about the batch we are processing
	$showName = $_POST['showName'];
	$sID = $_POST['showID'];
	$seasonNo =$_POST['sNo'];
	$episodeNo =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$showData =$_POST['seriesData'];	//this references the saved xml file//
	$flag='new';
	$externalAttributesBtn4='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
	$lang = $_POST['lang'];
	$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	$dataFile = file_get_contents($seriesDataRequest);
	saveData($dataFile, $flag);
	$episodeInfo= getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
	//print_r($episodeInfo);
	$imgUrl= 'http://www.thetvdb.com/banners/_cache/'.$episodeInfo['epImg'];

		$img = file_get_contents($imgUrl);
		$tempfile = fopen('coverartscan/showImg'.$showID.'.jpg', 'w');
		fwrite($tempfile, $img);
		fclose($tempfile);
		$imgPath = 'coverartscan/showImg'.$showID.'.jpg';

		if(isset($_POST['rez']))
		{
		$rez=trim($_POST['rez']);
		}
		else 
		{
		$rez = "3";
		}

	$out='<table align="center" width="85%" border="0" cellpadding="5">
    <tr>
        <th colspan=3><form action="index.php" method="post" name="tvdbAttributes"><b>Episode Information</b> <img src=operations/mediaBrowser/images/single.png height="50" width="70" align="center">'.$_POST['filename'].'
        </th>
    </tr>
    <tr><td rowspan="3" width="25%">
	<img src="'.$imgPath.'" align=center><br><input name="submit" type="submit" value="Confirm"></td>
		<td width="35%"><h1>'.$episodeInfo['series']['series'].'</h1><b>Episode Title: </b>'.$episodeInfo['title'].'<br> <b>Season:</b> '.$seasonNo.' <b> Episode:</b> '.$episodeNo.'<br>
		<br><select name="rezOver" maxlength=15>
		<option value="'.$rez.'" selected="selected">'.$fileFormatEnum[$rez].'</option>
<option value="3">Standard Def</option>
<option value="4">HD 720</option>
<option value="5">HD 1080</option>
</select><br> <p><i>Overview</i><br>'.$episodeInfo['synopsis'].'</p></td>';

		 $out.=' </td>';

		 $out.='<td id="pictureFrame" rowspan="3" >';
		foreach ($episodeInfo['aImg'] as $key => $val)
		{
		$img = file_get_contents('http://www.thetvdb.com/banners/_cache/'.$val.'');
		$cleanedNamed =str_replace(" ", "", $key);
		$tempfile = fopen('coverartscan/'.$cleanedNamed.'.jpg' , 'w');
		fwrite($tempfile, $img);
		fclose($tempfile);
		$imgPath = 'coverartscan/'.$cleanedNamed.'.jpg';
		$actorImgUnlink.=','.$imgPath;

		 $out.='<div id="container"> <img src="'.$imgPath.'" height="150" width="100"><br>'.$key.'</div>';
		};
		$out.='</td>
		<tr>
        <td>

	<b>Guest Stars: </b>'.$episodeInfo['guestStars'].'
        </td></tr>
        <tr><td>
       <b> Genre: </b>'.$episodeInfo['series']['genre'].'
        </td>
        </tr>
    <tr>
        <td colspan=3" align="center" bgcolor="#FFFFFF">'.$externalAttributesBtn4.'
        </td>
</table>
    		  	<input type="hidden" name="section" value="tvdbAttributes">
                <input type="hidden" name="fileID" value="'.$fileID.' ">
                <input type="hidden" name="sName" value='.$episodeInfo['series']['series'].'>
                <input type="hidden" name="seasonNo" value='.$seasonNo.'>
                <input type="hidden" name="episodeNo" value='.$episodeNo.'>
                 <input type="hidden" name="seriesID" value='.$episodeInfo['series']['showID'].'>
                 
                 <input type="hidden" name="lang" value='.$lang.'>
                 <input type="hidden" name="actorUnlink" value='.$actorImgUnlink.'>

	</table>' ;
				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('Confirm ||');
				$output->setPageTitle($episodeInfo['series']['series']);
				$output->setScriptInHead($scriptInHead);
				$output->setScriptInBody('bgColor="#F0F3F8"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);
				$output->output();
};

function  tvdbSpecSave($output,$mediadbADO,$dbADO)
{
/*
 * Function for specials that fall outside of a normal tvseries or special events...like the star wars christmast special.
 *
 */
	$scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>

 				<script>
 				function showContents(x)
 				{

 				Effect.BlindDown("synop"+x);
 				}

 				function hideContents(x)
 				{
 				Effect.BlindUp("synop"+x);
 				}
 				</script>';


	$xmlPath='/var/tmp';
	$apiKey = "4C6CEBDFB4558279";
	$showName = $_POST['showName'];
	$sID = $_POST['showID'];
	$seasonNo =0;
	$episodeNo =$_POST['epNo'];
	$fileID = $_POST['fileID'];
	$showData =$_POST['seriesData'];
	$flag='new';
	$externalAttributesBtn2='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=checkTVDB&fileID='.$fileID.'\'">';
	$lang = $_POST['lang'];
	$seriesDataRequest = 'http://www.thetvdb.com/api/'.$apiKey.'/series/'.$sID.'/all/'.$lang.'.zip ';
	$dataFile = file_get_contents($seriesDataRequest);
	saveData($dataFile, $flag);
	$episodeInfo= getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
	//print_r($episodeInfo);

	$out='<table width="85%" border="0" cellpadding="0" align="center" sytle=" overflow-y: hidden;">
			<form action="index.php" method="post" name="tvdbSpecSave">
			<input name="showName" type="hidden" value="'.$fsName.'" />
 			<tr>
  			<th colspan="3">
			<div id="appearImg"><img src="coverartscan/searchImg'.$sID.'.jpg"></div></th></tr>
			<tr><td colspan="3"><h2>Tv Specials</h2><b>Filename: '.$_POST['fileName'].'</b></td></tr>
  			<tr>
   			 <td id="fileList" colspan="3" >
   			 <div style="table-layout: fixed; height: 300px; overflow-y: hidden; overflow-y: scroll;">
   			 <table width="85%" border="0" align="center" style="overflow-y: scroll;">
   			  <tr>
   			 <td width="5" cellspacing="0"> #</td><td align="center">Episode Title</td><td width="15%">Originally Aired</td>
   			 </tr>';
			$epCounter=0;
			$pos=0;
			while ( $episodeInfo['series']['episodeCount'] > $epCounter )
			{
			$pos++;
			$episodeNo = $epCounter+1;
			$seasonNo= 0;
			$current=  getSeriesData($sID, $lang, $xmlPath, $mediadbADO, $dbADO, $output, $fileID, $seasonNo, $episodeNo);
			$out.='<form action="index.php" method="post" id="specialForm" name="'.$current['air_epNo'].'"><tr bgcolor="'.(($pos%2==0)?'#F0F3F8':'#F0FFF0').'"><td>'.$current['air_epNo'].'</td><td>'.$current['title'].'<div align=right id="show'.$epCounter.'"><input type="submit" action="submit" value="This Special"><br><a href="#"  onclick="showContents('.$epCounter.'); return false" >Show Synopsis</a></div><div id="synop'.$epCounter.'" style="display:none">'.$current['synopsis'].'<br><a href="#"  onclick="hideContents('.$epCounter.'); return false" >Hide</a></div></td><td>'.$current['firstAir'].'</td></tr>
			<input type="hidden" name="section" value="tvdbAttributes">
                <input type="hidden" name="fileID" value="'.$fileID.' ">
                <input type="hidden" name="sName" value='.$current['series']['series'].'>
                <input type="hidden" name="seasonNo" value='.$seasonNo.'>
                <input type="hidden" name="episodeNo" value='.$current['air_epNo'].'>
                 <input type="hidden" name="seriesID" value='.$current['series']['showID'].'>
                 <input type="hidden" name="lang" value='.$lang.'>
                 <input type="hidden" name="catchUnlink" value="coverartscan/searchImg'.$sID.'.jpg"> </form>';
			$epCounter++;
			}
   			$out.='</table></div> <tr><td colspan=2 align=center><div align="center">'.$externalAttributesBtn2.'</div></td></tr><tr><td align="center">Meta Data Generously Provided by <a href="http://www.thetvdb.com">TheTvDB.com</a></td></tr></table></table>' ;

				$output->setReloadLeftFrame(false);
				$output->setMenuTitle('TV Specials ||');
				$output->setPageTitle($current['series']['series']);
				$output->setScriptInHead($scriptInHead);
				$output->setScriptInBody('bgColor="#F0F3F8"');
				$output->setBody($out);
				$output->setTitle(APPLICATION_NAME);
				$output->output();
}

function actorImages($sID)
{
/* This function processes the Images related to performers associated with this tv show
 * Returns an array with links to download and the actor image id, which can be associated with a charater.
 */
$xmlPath='/var/tmp';
$apiKey = "4C6CEBDFB4558279";
$showName = $_POST['showName'];
//$sID = $_POST['showID'];
$fileID = $_POST['fileID'];
$showData =$_POST['seriesData'];
$lang = $_POST['lang'];
$actorArray= array();

			$actorXml= new DOMDocument();
			$actorXml->load($xmlPath.'/'.$sID.'/'.'actors.xml') or die ('file load error');
			$aArray = $actorXml->getElementsByTagName('Actor');

			foreach ($aArray as $actor)
		{
				$nID = $actor->getElementsByTagName('Name');
				$nameID =$nID->item(0)->nodeValue ;


				$aID = $actor->getElementsByTagName('id');
				$actorID =$aID->item(0)->nodeValue;
				$actorArray[$nameID]=$actorID;				

				$aImg = $actor->getElementsByTagName('Image');
				$actorImg = $aImg->item(0)->nodeValue ;
				$actorArray[$nameID] = $actorImg;
				
		};
				return $actorArray;

} ;

function addActorImages($currUrl, $attrNo, $currPerf, $con , $mediadbADO, $dbADO, $output)
{
/*
 * Experimental function to add actor images. Will see use if i ever implement a character table.
 */
	//check for image
	$resulta = mysqli_query($mediadbADO, "SELECT * FROM Picture_Attribute WHERE FK_Attribute=\"$attrNo\"");
	$resultb= mysqli_fetch_assoc($resulta);
	$rowsA = mysqli_num_rows($resultb);
	//print_r($resultb);
	$existingPic= $resultb['FK_Picture'];
	//echo $existingPic ;
	if ($rowsA < 1)
	{
	//echo '<br>'.$currUrl.'<br>';
	//echo $attrno;
	//echo $currPerf;
		$fullUrl ='http://www.thetvdb.com/banners/'.$currUrl;
		$img = file_get_contents($fullUrl);
		$fName1 = strrchr($currUrl, '/');
		$fName = str_replace ("/", "", $fName1);
		//echo 'Filename:'.$fName.' for '.$currPerf.'<br>' ;
		$savefile = fopen('/var/tmp'.$fName.'.jpg', 'w');
		fwrite($savefile, $fullUrl);
		fclose($savefile);
		//echo "fclose complete<br>";
		$import_cover_art='http://www.thetvdb.com/banners/actors/'.$fName;
		$extension=strtolower(str_replace('.','',strrchr($import_cover_art,".")));
		//echo 'Import Cover Art: '.$import_cover_art.'<br>';
		//echo 'ext: '.$extension.'<br>';
		if($import_cover_art!='')
			{
			mysqli_query($mediadbADO, "INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
			$picID=(int)mysqli_insert_id()  or die (mysqli_error($mediadbADO));
			mysqli_query($mediadbADO, "INSERT INTO Picture_Attribute (FK_Picture,FK_Attribute) VALUES (\"$picID\",\"$attrNo\")");
			mysqli_query($mediadbADO, "DELETE * FROM Picture_Attribute WHERE FK_Picture =\"$existingPic\" AND FK_Attribute=\"$attrNo\"");
			echo $existingPic.' deleted';

			// create the file and the thumbnail
			$extension=($extension=='jpeg')?'jpg':$extension;
			$newPicName=$picID.'.'.$extension;
			//echo 'New Picture Name: '.$newPicName.'<br>';
			$error='';
			if($extension!='jpg'){
				$error=$TEXT_ERROR_NOT_JPEG_CONST;
				//echo "<b>ERROR!!:".$error.'<br>';
				}
				else
				{//echo "added new pic for ".$currPerf.'<br>';
				 //create thumbnail
				savePic($import_cover_art,$GLOBALS['mediaPicsPath'].$newPicName);
					if(file_exists($GLOBALS['mediaPicsPath'].$newPicName))
					{ //echo 'new file exists<br>';
					// try to delete exisitng thumbnail if user messed the files and database
						if(file_exists($GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension))
						{
						$cmd='sudo -u root rm -f "'.$GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension.'"';
						exec_batch_command($cmd);
						//echo "created thumb for ".$currPerf;
						}
					unlink('/var/tmp/'.$fName.'.jpg');
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension, 256, 256);
					if(!$resizeFlag)
						{
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
						//echo $error.'<b>ERROR!!Upload issues!ENDED<br><br><br>';
						}
					}
				}
			}
	}
	else
	{echo "has image";
	}
}
?>
