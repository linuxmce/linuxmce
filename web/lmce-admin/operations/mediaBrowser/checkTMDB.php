<?php
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

error_reporting(1);
$fileID = $_GET['fileID'];

function initialSetup($fileID, $mediadbADO, $dbADO, $output) {
	// $dump = json_decode($config, false, 512);
	search($fileID, $mediadbADO, $dbADO, $output, $config);
}

function search($fileID, $mediadbADO, $dbADO, $output, $config) {
	include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/common.lang.php');
	include (APPROOT . '/languages/' . $GLOBALS['lang'] . '/editMediaFile.lang.php');

	$fileFormatEnum = array(1 => "Low Res", 2 => "DVD", 3 => "Standard Def", 4 => "HD 720", 5 => "HD 1080", 6 => "Low Quality", 7 => "MP3", 8 => "CD Quality", 9 => "High-def audio");
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
	$extracted = $guessedArray['title'];
	$rawfile = $rowFile['Filename'];
	//	$fileData=getFieldsAsArray('Filename',$mediadbADO,'WHERE PK_File='.$fileID) or die ('error!!');
	/* opening the form */
	$out .= '<form action="index.php" method="post" id="TMDBform" name="tmdbResults">';
	$out .= '<table border="1" bordercolor="#FFFFFF" style="background-color:#FFFFFF" width="65%" cellpadding="3" cellspacing="3">
	<tr>
	<td>' . $TEXT_FILENAME_CONST . '</td>
	<td>' . $extracted . '</td>
	</tr>
	<tr>
		<td>Guessed Title</td>
		<td><input name="movie_title" type="text" value="' . $guessedArray['title'] . '" size="20" maxlength="50"></td>
	</tr>
	<tr>
		<td>Guessed Year</td>
		<td><input name="movie_year" type="text" value="' . $guessedArray['year'] . '" size="4" maxlength="4"></td>
	</tr>
	<tr>
		<td>Guessed Resolution</td>
		<td><select name="rez" maxlength=15><option value="' . $guessedArray['rez'] . '" selected="selected">' . $fileFormatEnum[$guessedArray['rez']] . '</option>
			<option value="3">Standard Def</option>
			<option value="4">HD 720</option>
			<option value="5">HD 1080</option>
			</select></td>
	</tr>
</table>
<br>			<input type="hidden" name="section" value="tmdbResults">
                <input type="hidden" id="fileID" name="fileID" value="' . $fileID . ' ">
                <input type="hidden" id="fname" name="file" value="' . $rawfile . ' ">
              
               ';
	$out .= '<p><input name="submit" type="submit" value="Search!"></p></td><td rowspan="2" width="200">';

	$out .= ' </form>' . $externalAttributesBtn3;

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
	$patterns[1] = ("/dvd-rip|dvdrip\\S+|720p|1080p|\\d\\d\\d\\d|mp4|avi|flv|\[|\]|m2ts|avi|wmv|ogg|mkv|\(|\)|\./");
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

function tmdbResults($output, $mediadbADO, $dbADO) {
	$scriptInHead = '<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>
 				<script>
 				function clear()
 				{this.value="";}
 </script>';

	$title = $_POST['movie_title'];
	$filename = $_POST['file'];
	if (!$tmdb) {
		$tmdb = new TMDb('6ba51a94858c1748757dc06f7d4afbaa', $GLOBALS['lang'], false);
		$config = $tmdb -> getConfig();
	}
	$raw = $tmdb -> searchMovie($title);
	$out .= '<body id="results"> ';

	foreach ($raw['results'] as $k => $val) {
		$image_url = $tmdb -> getImageUrl($val['poster_path'], TMDb::IMAGE_POSTER, 'original');
		$backdrop_url = $tmdb -> getImageUrl($val['backdrop_path'], TMDb::IMAGE_BACKDROP, 'original');

		$out .= '<form action="index.php" method="post" id="TMDBform" name="tmdbSaved"><table border="0" bordercolor="" style="background-color:#C0C0C0" width="45%" cellpadding="3" cellspacing="3" align="center">';
		$out .= '';
		$out .= '	<tr>
		<td width="15%"><h2>' . $val['title'] . '</h2></td><td rowspan="2" style="background-color:#FFF8DC" width=180><img src=' . $image_url . ' height="320" width="180" align="center"> </td><td width="320"><img src=' . $backdrop_url . ' height="180" width="320" align="center"></td>
		<input type="hidden" name="section" value="tmdbSaved">
        <input type="hidden" id="fileID" name="fileID" value="' . $_POST['fileID'] . ' ">
         <input type="hidden" id="movie_name" name="movie_name" value="' . $val['title'] . ' ">
        <input type="hidden" id="name" name="fileName" value="' . $filename . ' ">
        <input type="hidden" id="rez" name="rez" value="' . $_POST['rez'] . ' ">
         <input type="hidden" id="movieID" name="movie_id" value="' . $val['id'] . ' ">         
		</tr>
		<tr>
		<td colspan="25%"> ' . $val['release_date'] . '<br><input name="submit" type="submit" value="Confirm!"></p></td>
		</tr>
		<tr>		
	</tr></table>
	</form>';
	}

	$out .= '</body>';

	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle(' TMDB  Results | The open Movie Database');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
}

function tmdbSaveData($output, $mediadbADO, $dbADO) {
	$pass = "";
	$user = "root";
	$db = "pluto_media";
	$con = mysql_connect("localhost", $user, $pass);
	// connection
	if (!$con) { die('Could not Connect' . mysql_error());
		//error messaging
		$connMessage = "Fail";
	};
	if ($con) {mysql_select_db("pluto_media") or die(mysql_error());
		//connect to media db or error out
		$connMessage = "Conn Good";
	};
	if (!$tmdb) {
		$tmdb = new TMDb('6ba51a94858c1748757dc06f7d4afbaa', $GLOBALS['lang'], false);
		//$config = $tmdb -> getConfig();
	}
	$out .= '<table border="0" bordercolor="" style="background-color:#C0C0C0" width="75%" cellpadding="3" cellspacing="3" alignment="center">
	<tr>
		<td>' . $_POST['fileName'] . ' </td>
		<td>' . $_POST['movie_name'] . ' </td>
		<td>' . $_POST['movie_id'] . ' </td>
	</tr>
</table>
	';
	$output -> setBody($out);
	/*getting the attribute array from linuxmce db*/
	$attribute_type_array = array(1 => "Director", 2 => "Performer", 3 => "Album", 4 => "Track", 6 => "ASIN", 7 => "CDDB", 8 => "Genre", 9 => "Ref #", 10 => "Channel", 11 => "Episode", 12 => "Program", 13 => "Title", 14 => "Disc ID", 15 => "Conductor", 16 => "Composer/Writer", 17 => "Studio", 18 => "Rating", 19 => "Release Date", 20 => "Website", 21 => "Chapter", 22 => "Encoding", 23 => "Format", 24 => "Aspect Ratio(s)", 25 => "Audio Encoding", 26 => "Rated", 27 => "DVD Release Date", 28 => "Run Time", 29 => "Keyword", 30 => "Screen Saver For MD", 31 => "Format", 32 => "Region", 33 => "Language", 34 => "Aspect Ratio", 35 => "Number of Discs", 36 => "IMDB", 37 => "Synopsis", 38 => "Producer", 39 => "Executive Producer", 40 => "Purchase Info", 41 => "TV Channel ID", 42 => "TV Program ID", 43 => "TV Series ID", 44 => "Creation Date", 45 => "THX Disk ID", 46 => "Screen saver disabled", 47 => "Manufacturer", 48 => "Year", 49 => "System", 50 => "Season Number", 51 => "Episode Number", 52 => "TV Season ID", 56 => "System Configuration");
	//dynamic array to deal with changes to attribute types

	$query = "SELECT PK_AttributeType, Description from AttributeType";

	$result = mysql_query($query);

	$postersize = "original";
	$backdropsize = "original";

	/*marshalling up relevant data */
	$movieId += $_POST['movie_id'];
	$movie = $tmdb -> getMovie($movieId, $GLOBALS['lang']);
	$fileID = $_POST['fileID'];
	$format = $_POST['rez'];
	$cast = $tmdb -> getMovieCast($movieId);
	$images = $tmdb -> getMovieImages($movieId, $GLOBALS['lang']);
	$imgTypes = array();
	$imgTypes['poster_sizes'] = $tmdb -> getAvailableImageSizes(TMDb::IMAGE_POSTER);
	$imgTypes['backdrop_sizes'] = $tmdb -> getAvailableImageSizes(TMDb::IMAGE_BACKDROP);

	if ($imgTypes["backdrop_sizes"][count($imgtypes["backdrop_sizes"])] == "w1280") {
		$backdropsize = "w1280";
		$out .= "1280 Backdrop!";
	}

	$actors = $cast['cast'];
	$crew = $cast['crew'];

	$synopsis = mysql_real_escape_string($movie['overview']);
	$backdrop_path = $movie['backdrop_path'];
	$backdropUrl = $tmdb -> getImageUrl($val['backdrop_path'], TMDb::IMAGE_BACKDROP, 'original') . $backdrop_path;

	$poster_path = $movie['poster_path'];
	$genres = $movie['genres'];
	$title = $movie['title'];
	$studios = $movie['production_companies'];
	array_flip($attribute_type_array);

	$out .= $title;
	$out .= '<table border="1" bordercolor="" style="background-color:" width="75%" cellpadding="3" cellspacing="3">
	
	<tr>
		<td>Attribute</td>
		<td>Status</td>
	</tr>';

	$typeArray = array_flip($attribute_type_array);

	/*processing attributes and writing result to table*/

	$out .= '<tr>
		<td>File Image</td>
		<td>' . saveFileImage($movie['original_title'], $tmdb -> getImageUrl($poster_path, TMDb::IMAGE_POSTER, 'original')) . '</td>
	</tr>';

	$out .= '<tr>
		<td>Title</td>
		<td>' . insertAttribute($title, $typeArray['Title'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';

	$out .= '<tr>
		<td>Synopsis</td>
		<td>' . insertAttribute($synopsis, $typeArray['Synopsis'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';

	$out .= '<tr>
		<td>Release Date</td>
		<td>' . insertAttribute($movie['release_date'], $typeArray['Release Date'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';

	foreach ($movie['production_companies'] as $name => $studio) {

		$out .= '<tr>
		<td>Studio</td>
		<td>' . insertAttribute($studio['name'], $typeArray['Studio'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
	}

	foreach ($movie['genres'] as $name => $genre) {

		$out .= '<tr>
		<td>Genre</td>
		<td>' . insertAttribute($genre['name'], $typeArray['Genre'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
	}

	foreach ($cast['crew'] as $name => $crew) {

		if ($crew['job'] == "Director") {

			$out .= '<tr><td>Director</td>
	<td>' . insertAttribute($crew['name'], $typeArray['Director'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
			$hasImage = false;
			$imagePath = "";
		} else if ($crew['job'] == "Producer") {
			$out .= '<tr><td>Producer</td>
	<td>' . insertAttribute($crew['name'], $typeArray['Producer'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
		} else if ($crew['job'] == "Author") {
			$out .= '<tr><td>Producer</td>
	<td>' . insertAttribute($crew['name'], $typeArray['Composer/Writer'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
		}
	}
	foreach ($cast['cast'] as $name => $crew) {

		/*		$tImgPath="";
		 if(!empty($crew['profile_path'])){
		 $tImagePath=$tmdb->getImageUrl($tImagePath,TMDb::IMAGE_PROFILE, 'original');
		 $hasImage= "true";
		 $imagePath =$tImagePath;
		 }
		 */
		$out .= '<tr><td>Performer</td>
	<td>' . insertAttribute($crew['name'], $typeArray['Performer'], $hasImage, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';
		$hasImage = false;
		$imagePath = "";

	}
	$backdrop = false;

	if ($movie['backdrop_path'] != "") {
		$backdrop = true;

	} else {
		$backdrop_path = "";
	}

	$out .= '<tr>
		<td>IMDB</td>
		<td>' . insertAttribute($movie['imdb_id'], $typeArray['IMDB'], $backdrop, $backdropUrl, $mediadbADO, $dbADO) . '</td>
	</tr>';

	$out .= '<tr>
		<td>Running Time</td>
		<td>' . insertAttribute($movie['runtime'], $typeArray['Run Time'], false, $imagePath, $mediadbADO, $dbADO) . '</td>
	</tr>';

	mysql_query("UPDATE File SET FK_FileFormat=\"$format\" WHERE `PK_File`=\"$fileID\" ") or die(mysql_error());
	mysql_query("UPDATE File SET FK_MediaSubType=2 WHERE `PK_File`=\"$fileID\" ") or die(mysql_error());

	/*attrbiute insertion notification table*/

	$out .= '</table><br><br><input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=editMediaFile&fileID=' . $_POST['fileID'] . '\'">';
	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle(' TMDB  Saved Attributes | The open Movie Database');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
}

function insertAttribute($attribute, $attributeType, $hasImage, $imagePath, $mediadbADO, $dbADO) {

	$sql = "SELECT * FROM `Attribute` WHERE `Name` = \"$attribute\" AND FK_AttributeType = \"$attributeType\"";
	$result = mysql_query($sql) or die(mysql_error());
	$row = mysql_fetch_assoc($result);
	$attrib = $row['PK_Attribute'];
	$count = mysql_num_rows($result);
	//echo "<br>Rows found for attribute search=".$count;
	//echo "<br>Attribute::".$attribute;
	$attrib = $row['PK_Attribute'];
	//	echo "<br>".$attrib;
	$fileID = $_POST['fileID'];
	if ($count < 1) {
		$iQuery = "INSERT INTO Attribute VALUES (\"\" , \"$attributeType\" , \"$attribute\" , NULL, NULL, NULL, 0 ,CURTIME() ,NULL )";
		mysql_query($iQuery) or die(mysql_error());
		$insertRes = (int)mysql_insert_id() or die(mysql_error());
		//echo $insertRes;
		$idQuery = "INSERT INTO File_Attribute  VALUES (\"$fileID\", \"$insertRes\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
		mysql_query($idQuery) or die(mysql_error());
		if ($imagePath != "") {
				printf("Saving attribute image \n with attribute::" . $chk['FK_Attribute']);
				$imgResponse = saveAttributeImage($insertRes, $attributeType, $imagePath);
			}
		return "New Attribute:" . $attribute . " added to database under" . $attributeType . "<br>" . $imgResponse;
	} else {

		$fileChk = "SELECT * FROM `File_Attribute` WHERE `FK_Attribute`= \"$attrib\" AND `FK_File` = \"$fileID\"";
		$chkResult = mysql_query($fileChk) or die(mysql_error());
		$chkCount = mysql_num_rows($chkResult);
		$chk = mysql_fetch_assoc($chkResult);
		$test = $chk['FK_Attribute'];
	//	echo "test::" . $test ."::".$chkCount."<br>";
		// echo $attrib;
		$imgResponse = "";
		$inserKey="";
		if ($test !== $attrib) {
			$idQuery2 = "";
			switch ($attributeType) {
				case '13' :
					$idQuery2 .= "INSERT INTO File_Attribute  VALUES (\"$fileID\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
					break;
				default :
					$idQuery2 .= "INSERT INTO File_Attribute  VALUES (\"$fileID\", \"$attrib\", 0, 0, NULL, NULL, NULL, 0, CURTIME(), NULL  )";
					break;
			}
			mysql_query($idQuery2) or die(mysql_error());
			
			
			if ($imagePath != "") {
				
				$imgResponse = saveAttributeImage($attrib, $attributeType, $imagePath);
			}
			return "New Attribute:" . $attribute . " added to database for file." . $fileID . " for attribute type " . $attributeType . "\n" . $imgResponse;
		} else {
			if ($imagePath != "") {
			
				$imgResponse = saveAttributeImage($attrib, $attributeType, $imagePath);
			}
			return "Already Associated to" . $test . "\n" . $imgResponse;
		}
	}

}

function saveAttributeImage($attribute, $attributeType, $imagePath) {

	//echo "now saving attribute picture for attribute type::" . $attributeType . " and attribute::" . $attribute;
	$fileIdent = $_POST['fileID'];
	//print_r($fileIdent);
	$test = mysql_query("SELECT * FROM Picture_Attribute WHERE FK_Attribute=\"$attribute\" ") or die(mysql_error("MYsql Error"));
	$fileCount = mysql_num_rows($test);
	$tarray = mysql_fetch_assoc($test);

	
	if ($fileCount < 1) {
		//	printf("No image found, saving.");
		$fullUrl = $imagePath;
		$img = file_get_contents($fullUrl);
		$fName1 = strrchr($attribute, '/');
		$fName = str_replace(".jpg", "", $fName1);
		$savefile = fopen('/var/tmp' . $fName . '.jpg', 'w');
		fwrite($savefile, $fullUrl);
		fclose($savefile);
		$import_cover_art = $imagePath;
		$extension = strtolower(str_replace('.', '', strrchr($import_cover_art, ".")));

		if ($import_cover_art != '') {
			mysql_query("INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
			$picID = (int)mysql_insert_id() or die(mysql_error());
			mysql_query("INSERT INTO Picture_Attribute (FK_Picture, FK_Attribute) VALUES (\"$picID\",\"$attribute\")");

			// create the file and the thumbnail
			$extension = ($extension == 'jpeg') ? 'jpg' : $extension;
			$newPicName = $picID . '.' . $extension;

			$error = '';
			if ($extension != 'jpg') {
				$error = $TEXT_ERROR_NOT_JPEG_CONST;
				printf($TEXT_ERROR_NOT_JPEG_CONST);
				return "Attribute::" . $error;
			} else {
				// create thumbnail
				savePic($import_cover_art, $GLOBALS['mediaPicsPath'] . $newPicName);
				if (file_exists($GLOBALS['mediaPicsPath'] . $newPicName)) {
					// try to delete exisitng thumbnail if user messed the files and database
					if (file_exists($GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension)) {
						$cmd = 'sudo -u root rm -f "' . $GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension . '"';
						exec_batch_command($cmd);
					}
					unlink('/var/tmp/' . $fName . '.jpg');
					$resizeFlag = resizeImage($GLOBALS['mediaPicsPath'] . $newPicName, $GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension, 256, 256);
					if (!$resizeFlag) {
						$error = $TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST . ' ' . $GLOBALS['mediaPicsPath'];
						return "Attribute::" . $error;
					}
					return "Image " . $newPicName . "saved.";
				}
			}
		}
	} else {
		return "Image exist for attribute::" . $attribute;
	}
}

function saveFileImage($title, $remoteImagePath) {
	$fileIdent = $_POST['fileID'];
	//print_r($fileIdent);
	$test = mysql_query("SELECT * FROM Picture_File WHERE FK_File=\"$fileIdent\" ") or die(mysql_error());
	$fileCount = mysql_num_rows($test);

	if ($fileCount < 1) {
		$fullUrl = $remoteImagePath;
		$img = file_get_contents($fullUrl);
		$fName1 = strrchr($title, '/');
		$fName = str_replace(".jpg", "", $fName1);
		$savefile = fopen('/var/tmp' . $fName . '.jpg', 'w');
		fwrite($savefile, $fullUrl);
		fclose($savefile);
		$import_cover_art = $remoteImagePath;
		$extension = strtolower(str_replace('.', '', strrchr($import_cover_art, ".")));

		if ($import_cover_art != '') {
			mysql_query("INSERT INTO Picture (Extension,URL) VALUES (\"$extension\", \"$import_cover_art\")");
			$picID = (int)mysql_insert_id() or die(mysql_error());
			mysql_query("INSERT INTO Picture_File (FK_Picture,FK_File) VALUES (\"$picID\",\"$fileIdent\")");

			// create the file and the thumbnail
			$extension = ($extension == 'jpeg') ? 'jpg' : $extension;
			$newPicName = $picID . '.' . $extension;

			$error = '';
			if ($extension != 'jpg') {
				$error = $TEXT_ERROR_NOT_JPEG_CONST;
				return "File::" . $error;
			} else {
				// create thumbnail
				savePic($import_cover_art, $GLOBALS['mediaPicsPath'] . $newPicName);
				if (file_exists($GLOBALS['mediaPicsPath'] . $newPicName)) {
					// try to delete exisitng thumbnail if user messed the files and database
					if (file_exists($GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension)) {
						$cmd = 'sudo -u root rm -f "' . $GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension . '"';
						exec_batch_command($cmd);
					}
					unlink('/var/tmp/' . $fName . '.jpg');
					$resizeFlag = resizeImage($GLOBALS['mediaPicsPath'] . $newPicName, $GLOBALS['mediaPicsPath'] . $picID . '_tn.' . $extension, 256, 256);
					if (!$resizeFlag) {
						$error = $TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST . ' ' . $GLOBALS['mediaPicsPath'];
						return "File::" . $error;
					}
				}
			}
		}
	} else {
		return "Image already set.";
	}
}
?>