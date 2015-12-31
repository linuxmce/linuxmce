<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
require_once("include/imdb/imdb.class.php");
require_once("include/imdb/imdbsearch.class.php");

/* @var $mediadbADO ADOConnection */
/* @var $rs ADORecordSet */

$start_time=getmicrotime();

$coverArtPath='/home/coverartscan/';


$searchIndex=$_REQUEST['searchIndex'];
$fileID=isset($_REQUEST['fileID'])?(int)$_REQUEST['fileID']:NULL;
$discID=isset($_REQUEST['discID'])?(int)$_REQUEST['discID']:NULL;
$attributeID=isset($_REQUEST['attributeID'])?(int)$_REQUEST['attributeID']:NULL;

$Keyword1Type=@$_REQUEST['Keyword1Type'];
$Keyword2Type=@$_REQUEST['Keyword2Type'];
$Keyword3Type=@$_REQUEST['Keyword3Type'];

$Keyword1Search=cleanString(@$_REQUEST['Keyword1Search']);
$Keyword2Search=urlencode(cleanString(@$_REQUEST['Keyword2Search']));
$Keyword3Search=urlencode(cleanString(@$_REQUEST['Keyword3Search']));


// get matching IMDB IDs
$myImdbIdSearch = new imdbsearch();
$myImdbIdSearch->setsearchname($Keyword1Search);
$myImdbIdResults = $myImdbIdSearch->results();

// add the record in CoverArtScan table
// depending on parameters, FK_File, FK_Disc or FK_Attribute

$mediadbADO->Execute('
	INSERT INTO CoverArtScan 
	(FK_File,FK_Disc,FK_Attribute,Engine,Keyword1Type,Keyword1Search,Keyword2Type,Keyword2Search,Keyword3Type,Keyword3Search) 
	VALUES 
	(?,?,?,?,?,?,?,?,?,?)',array($fileID,$discID,$attributeID,ENGINE.' | '.$searchIndex,$Keyword1Type,$Keyword1Search,$Keyword2Type,$Keyword2Search,$Keyword3Type,$Keyword3Search));
$casID=$mediadbADO->Insert_ID();

// If music, the Title should be imported as an Album attribute
$isMusic = $searchIndex == "Music";
$titleAttrName = $isMusic ? "Album" : "Title";

$found=0;
foreach ($myImdbIdResults as $item) { 
	$found++;
	// get IMDB details
	$myImdb = new imdb($item->imdbid());
	
	$attributes = "";
	
	$attributes .= "IMDBID\t".$item->imdbid()."\n";
	$attributes .= $titleAttrName."\t".$myImdb->title()."\n";
	$attributes .= "Release Date\t".$myImdb->year()."\n";
	$attributes .= "Run Time\t".$myImdb->runtime()."\n";
	$plot = $myImdb->plot();
	$attributes .= "Synopsis\t";
	foreach($plot as $part) {
		$attributes .= nl2br($part)."<br>";
	}
	$attributes .= "\n";
	$cast = $myImdb->cast();
	foreach($cast as $person) {
		$attributes .= "Performer\t".$person['name']."\n";
	}
	$director = $myImdb->director();
	foreach($director as $person) {
		$attributes .= "Director\t".$person['name']."\n";
	}
	$writers = $myImdb->writing();
	foreach($writers as $person) {
		$attributes .= "Composer/Writer\t".$person['name']."\n";
	}
	$genres = $myImdb->genres();
	foreach($genres as $genre) {
		$attributes .= "Genre\t".$genre."\n";
	}
		
	$photoUrl = $myImdb->photo(false);

			
	// grab the cover art if exist, download it and save the record in database
	$mediadbADO->Execute('INSERT INTO CoverArtScanEntry (FK_CoverArtScan,ID,URL,Attributes) VALUES (?,?,?,?)',array($casID,$item->imdbid(),$photoUrl,$attributes));
	$entryID=$mediadbADO->Insert_ID();
	savePic($photoUrl,$coverArtPath.$entryID.'.jpg');
	$resizeFlag=resizeImage($coverArtPath.$entryID.'.jpg', $coverArtPath.$entryID.'_tn.jpg', 256, 256);	
	}

$end_time=getmicrotime();
writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tFinished in ".round(($end_time-$start_time),3)."s, found: ".$found." cover arts.",'a+');

?>