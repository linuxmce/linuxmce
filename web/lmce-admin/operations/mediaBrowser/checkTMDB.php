<?

/*
 Copyright 2010 - Langston Ball
 golgoj4@gmail.com
 GPLv2 Licensed
 TVDB Data Grabber
 TvDB.com data grabber for television shows from the web admin, both for single and multiple files

 checktvdb.php		- 	initialization
 tvdbUtils.php 		-	common include
 tvdbseries.php		-	Episode confirmation page
 tvdbAttributes.php	- 	Displays inserted attributes and screenshot

 */

include ('TMDb.php');
include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/common.lang.php');
include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/editMediaFile.lang.php');

$fileID = $_GET['fileID'];

error_reporting(1);

function initialSetup($fileID, $mediadbADO, $dbADO, $output) {

	$tmdb = new TMDb('6ba51a94858c1748757dc06f7d4afbaa', $GLOBALS['lang'], false);
	$config = $tmdb -> getConfig();
	// $dump = json_decode($config, false, 512);

	search($fileID, $mediadbADO, $dbADO, $output, $config, $tmdb);

}

function search($fileID, $mediadbADO, $dbADO, $output, $config, $tmdb) {
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
	//	$fileData=getFieldsAsArray('Filename',$mediadbADO,'WHERE PK_File='.$fileID) or die ('error!!');
$out.='<table border="1" bordercolor="#FFFFFF" style="background-color:#FFFFFF" width="65%" cellpadding="3" cellspacing="3">
	<tr>
		<td>Guessed Title</td>
		<td>'.$guessedArray['title'].'</td>
	</tr>
	<tr>
		<td>Guessed Year</td>
		<td>'.$guessedArray['year'].'</td>
	</tr>
	<tr>
		<td>Guessed Resolution</td>
		<td>'.$guessedArray['rez'].'</td>
	</tr>
</table>
';
	$extracted = $rowFile['Filename'];
	
	//var_dump($guessedArray);
	
	$out .=$guessedArray['rez'];

	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Search TVDB |');
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
		
		echo "\n";
		if ($test == 'hdtv') {
			echo "std def";
			$guessArray['rez'] = "3";
		} elseif ($test === '720p') {
			$guessArray['rez'] = "4";
		} elseif ($test == '1080p') {
			$guessArray['rez'] = "5";
		}
	} else {
		echo "guess failed \n";
		$guessArray['rez'] = "3";
	}

	return $guessArray;

};
?>