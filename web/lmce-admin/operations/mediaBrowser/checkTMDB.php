<?

/*
 Copyright 2010 - Langston Ball
 golgoj4@gmail.com
 GPLv2 Licensed
 TMDB Data Grabber
 TMDB.com data grabber for television shows from the web admin, both for single files.



 */

include ('TMDb.php');
include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/common.lang.php');
include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/editMediaFile.lang.php');

$fileID = $_GET['fileID'];



error_reporting(1);

function initialSetup($fileID, $mediadbADO, $dbADO, $output) {	
	// $dump = json_decode($config, false, 512);
	search($fileID, $mediadbADO, $dbADO, $output, $config);
}

function search($fileID, $mediadbADO, $dbADO, $output, $config) {
	include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/common.lang.php');
	include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/editMediaFile.lang.php');
	//var_dump($opt);

	$externalAttributesBtn3 = '<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=editMediaFile&fileID=' . $fileID . '\'">';
	$scriptInHead = '<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>

 				<script>
 				function clear()
 				{this.value="";}

function box()
{document.getElementById("specials").innerHTML = "<div id=\"searchBox\" align=\"right\" stlye=\"display: none\"><input type=\"text\" id=\"seriesName\" value=\"Enter Special Name\" onclick=\"clear()\"><br><input type=\"hidden\" value=\"0\" name=\"specSeas\"><br><input type=\"submit\" onclick=\"getSpecials()\"/></div>";
 				}
 				</script>';
	$resFile = $mediadbADO -> Execute('SELECT * FROM File WHERE PK_File=?', $fileID);
	$rowFile = $resFile -> FetchRow();
	$guessedArray = stringParse($rowFile['Filename']);
	$extracted = $rowFile['Filename'];
	//	$fileData=getFieldsAsArray('Filename',$mediadbADO,'WHERE PK_File='.$fileID) or die ('error!!');
	/* opening the form */
	$out.='<form action="index.php" method="post" id="TMDBform" name="tmdbResults">';
    $out.='<table border="1" bordercolor="#FFFFFF" style="background-color:#FFFFFF" width="65%" cellpadding="3" cellspacing="3">
	<tr>
	<td>'.$TEXT_FILENAME_CONST.'</td>
	<td>'.$extracted.'</td>
	</tr>
	<tr>
		<td>Guessed Title</td>
		<td><input name="movie_title" type="text" value="'.$guessedArray['title'].'" size="20" maxlength="50"></td>
	</tr>
	<tr>
		<td>Guessed Year</td>
		<td><input name="movie_year" type="text" value="'.$guessedArray['year'].'" size="4" maxlength="4"></td>
	</tr>
	<tr>
		<td>Guessed Resolution</td>
		<td>'.$guessedArray['rez'].'</td>
	</tr>
</table>
				<input type="hidden" name="section" value="tmdbResults">
                <input type="hidden" id="fileID" name="fileID" value="'.$fileID.' ">
                <input type="hidden" id="name" name="fileName" value="'.$extracted.' ">
              
                <input type="hidden" id="rez" name="rez" value="'.$guessedArray['rez'].' ">
               ';
	$out.='<p><input name="submit" type="submit" value="Search!"></p></td><td rowspan="2" width="200">';
	
	//var_dump($guessedArray);
	
$out.=' </form>'.$externalAttributesBtn3;

	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Search TMDB | The open Movie Database');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
}

function stringParse($extracted) {
	/*
	 * Parses filename input to try and extrace Show name, season and episode based on a regular expression brute force
	 * deal.
	 */
	$guessArray = array();

	$patterns[0] = ("/\d\d\d\d(?!p)/");
	/*Year filter */
	$patterns[1] = ("/dvd-rip|dvdrip\\S+|720p|1080p|\\d\\d\\d\\d|mp4|avi|flv|m2ts|avi|wmv|ogg|mkv|\(|\)|\./");
	/*Resolution search*/
	$patterns[2] = ("/(HDTV|hdtv|720p|1080p|extended.edition|directors.cut|special.edition|DVD|BluRay|H264|BrRip)/");
	/*Resolution search*/

	$smatch = 1;
	$titleSearch = "/(\d\d\d\d)|\[\]|\(|\)|\s\s|\s\s\s\s|\[\S+|\(\S+|DVD|rip|HDTV|x264|ac3|(720|1080)p|CD\d|DISC\d|extended.edition|directors.cut|special.edition|xvid|BluRay|H264|BrRip)/";
	$lower = htmlentities(strtolower($extracted));

	$guessArray['title'] = preg_replace($patterns[1], " ", $lower);
	preg_match($patterns[0], $lower, $sed);	
	$guessArray['year'] = $sed[0];

	//============================================end season preg_match============================================

	$formatEnum = array("Standard Def" => 3, "HD 720" => 4, "HD 1080" => 5);
	if (preg_match("/(HDTV|hdtv|720p|1080p|extended.edition|directors.cut|special.edition|BluRay|H264|BrRip)/", $lower, $rez)) {$test = $rez[0];
		
		
		if ($test == 'hdtv') {
			
			$guessArray['rez'] = "3";
		} elseif ($test === '720p') {
			$guessArray['rez'] = "4";
		} elseif ($test == '1080p') {
			$guessArray['rez'] = "5";
		}
	} else {
		
		$guessArray['rez'] = "3";
	}

	return $guessArray;

};

function tmdbResults($output,$mediadbADO,$dbADO){
	$scriptInHead = '<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>
 				<script>
 				function clear()
 				{this.value="";}

function box()
{document.getElementById("specials").innerHTML = "<div id=\"searchBox\" align=\"right\" stlye=\"display: none\"><input type=\"text\" id=\"seriesName\" value=\"Enter Special Name\" onclick=\"clear()\"><br><input type=\"hidden\" value=\"0\" name=\"specSeas\"><br><input type=\"submit\" onclick=\"getSpecials()\"/></div>";}
 </script>';
	
	$title = $_POST['movie_title'];
		if(!$tmdb){
$tmdb = new TMDb('6ba51a94858c1748757dc06f7d4afbaa', $GLOBALS['lang'], false);
$config = $tmdb -> getConfig();
}
		$raw = $tmdb->searchMovie($title);
print_r($raw['results']);
	
	foreach ($raw['results'] as $k => $val) {
		
		foreach ($val as $key => $value) {
			$out.=$key."\n".$value."<br>";
		}
	}
	
	$out.=$raw;
    $output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle(' TMDB  Results | The open Movie Database');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
}
?>