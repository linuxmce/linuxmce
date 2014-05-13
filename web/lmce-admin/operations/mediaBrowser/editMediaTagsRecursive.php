<?php
//ADDED BY PAUL MUMBY FOR RECURSIVE TAGGING CODE
function editMediaTagsRecursive($output,$mediadbADO,$dbADO)
{
    include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaTagsRecursive.lang.php');
    include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
    include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
    include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaFilesSync.lang.php');    

    $scriptInHead = 
<<<SCRIPT
    <SCRIPT>
	function addField()
	{
	    var fields = document.getElementById('striplist');
	    if (typeof fields == "object")
	    {
		var new_field = fields.firstChild.cloneNode(true);
		new_field.value = '';
		fields.appendChild(new_field, fields);
	    }
	}
    </SCRIPT>
SCRIPT;

    $currentpath = $_GET['path'];
    
    $out = "";
    
    if($_POST['submit']=="List Affected Files" || $_POST['submit']=="Confirm And Execute")
    {
	if($_POST['submit']=="Confirm And Execute")
	{
	    $filelisting = array();
	    $folderlisting = array();
	    if(isset($_REQUEST['list_files']))
		$filelisting = MediaFileRecursiveList($currentpath,$dbADO,"file");
	    if(isset($_REQUEST['list_folders']))
		$folderlisting = MediaFileRecursiveList($currentpath,$dbADO,"folder");
	    $dirlisting = array_merge($filelisting,$folderlisting);
	    $currentattribs = GetCurrentFolderAttributes($currentpath,$mediadbADO);
	    $out .= "<input class='button_fixed' type=button value='Return To Browser' onClick='history.go(-3)'>";
	    $out .= "<H2>Conversion Log:</H2>";
	    $out .= "<div style='width:80%; height:200px; border:thin solid blue; overflow:auto;'>";
	    foreach($dirlisting as $file)
	    {
		$out .= "Setting Attributes For: ".$file['file']."... ";
		SetFileAttributes($file['file'],$mediadbADO,$currentattribs);
		if(isset($_REQUEST['tvmode']) && $file['type']=="file")
		{
		    $out .= "<BR>&nbsp;&nbsp;&nbsp;&nbsp;Tagging TV Episode Info: ";
		    $thisfilearray = explode("/",$file['file']);
		    $thisfilename = $thisfilearray[count($thisfilearray)-1];
		    $parsed_tags = ParseFileName($thisfilename,$_REQUEST['tvmode_seasonoffset'],$_REQUEST['tvmode_seasonlength'],$_REQUEST['tvmode_episodeoffset'],$_REQUEST['tvmode_episodelength'],$_REQUEST['tvmode_titleoffset'],$_REQUEST['tvmode_striptext']);
		    $season = $parsed_tags['season'];
		    $episode_num = $parsed_tags['episode'];
		    $title = $parsed_tags['title'];
		    $episode = "$season" . "x" . "$episode_num $title";
		    SetTVAttributes($file['file'],$mediadbADO,$season,$episode,$title);
		    $out .= "Season:<B>$season</B>, Episode:<B>$episode</B>, Title:<B>$title</B><BR>";
		}
		$out .= "Done!<BR>";
	    }
	    $out .= "</div><BR>";
	    $updatecommand = "sudo -u root /usr/pluto/bin/UpdateMedia -d '$currentpath'";
	    $out .= "Running UpdateMedia to resync media tags...";
	    $out .= "<H2>UpdateMedia Output:</H2>";	    
	    $response=exec_batch_command($updatecommand,1);
	    $out .= "<div style='width:80%; height:200px; border:thin solid blue; overflow:auto;'>";
	    $out .= "<pre style='background:black;color:lightgray;'>$response</pre>";
	    $out .= "</div>";
	    $out .= "<BR><H2>Recursive Tagging Complete!</H2>";
	}else{
	    $currentpath = $_GET['path'];
	    $currebtattribs = GetCurrentFolderAttributes($currentpath,$mediadbADO);
	    $filelisting = array();
	    $folderlisting = array();
	    if(isset($_REQUEST['list_files']))
	        $filelisting = MediaFileRecursiveList($currentpath,$dbADO,"file");
	    if(isset($_REQUEST['list_folders']))
		$folderlisting = MediaFileRecursiveList($currentpath,$dbADO,"folder");
	    $dirlisting = array_merge($filelisting,$folderlisting);
	    $getstring = "";
	    if(isset($_POST['list_files']))
		$getstring .= "&list_files=true";
	    if(isset($_POST['list_folders']))
		$getstring .= "&list_folders=true";
	    $fl_out = "<ul>";
	    $fileid_error = false;
	    foreach($dirlisting as $file)
	    {
		$fileid = GetFileID($file['file'],$mediadbADO);
		if($fileid>0)
		{
		    if(isset($_REQUEST['tvmode']))
		    {
			if($file['type']=="file")
			{
			    $thisfilearray = explode("/",$file['file']);
			    $thisfilename = $thisfilearray[count($thisfilearray)-1];
			    $parsed_tags = ParseFileName($thisfilename,$_REQUEST['tvmode_seasonoffset'],$_REQUEST['tvmode_seasonlength'],$_REQUEST['tvmode_episodeoffset'],$_REQUEST['tvmode_episodelength'],$_REQUEST['tvmode_titleoffset'],$_REQUEST['tvmode_striptext']);
			    $season = $parsed_tags['season'];
			    $episode = $parsed_tags['episode'];
			    $title = $parsed_tags['title'];
			    $fl_out .= "<li>".$file['file']." <FONT COLOR='green'><B>In Database!</B></FONT><BR>Parsed TV Episode Tags: [<FONT COLOR='blue'>S<B>$season</B> E<B>$episode</B> - <B>$title</B></FONT>]</FONT></li>";
			}else{
			    $fl_out .= "<li>".$file['file']." <FONT COLOR='green'><B>In Database!</B></FONT><BR>Not Doing TV Tag Parsing On Directory.</li>";			
			}
		    }else{
			$fl_out .= "<li>".$file['file']." <FONT COLOR='green'><B>In Database!</B></FONT></li>";
		    }
		}else{
		    $fl_out .= "<li>".$file['file']." <FONT COLOR='red'><B>NOT IN DATABASE!</B></FONT></li>";		
		    $fileid_error = true;
		}
	    }
	    $fl_out .= "</ul>";    	    
	    if($fileid_error)
	    {
		$out .= "ERROR: Unfortunately not all of the files you have selected are in the media database, please first resync and confirm that all your media is in the database first, then we can run the recursive tagger again.<BR>";
		$out .= "I am respectfully refusing to allow you to continue (because got knows what kind of errors we would generate if we ran this on media not in the db already!)<BR>";
		$out .= "Please see the media list below to see which items were not in the database.<BR>";
		$out .= "<input class='button_fixed' type=button value='Back' onClick='history.go(-1)'>";
	    }else{
		$out .= "Are you sure you want to continue? please review the list of affected files below before hitting the Confirm button:<BR>";
		$out .= "<FORM NAME='CONFIRM' ACTION='index.php?section=editMediaTagsRecursive&path=".urlencode($_GET['path']).$getstring."' METHOD='POST'>";
		$out .= "<input class='button_fixed' type=button value='Back' onClick='history.go(-1)'> <INPUT TYPE='submit' NAME='submit' VALUE='Confirm And Execute' CLASS='button_fixed'>";
		if(isset($_REQUEST['tvmode']))
		{
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode' VALUE='true'>";
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode_seasonoffset' VALUE='".$_REQUEST['tvmode_seasonoffset']."'>";
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode_seasonlength' VALUE='".$_REQUEST['tvmode_seasonlength']."'>";
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode_episodeoffset' VALUE='".$_REQUEST['tvmode_episodeoffset']."'>";
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode_episodelength' VALUE='".$_REQUEST['tvmode_episodelength']."'>";
		    $out .= "<INPUT TYPE='hidden' NAME='tvmode_titleoffset' VALUE='".$_REQUEST['tvmode_titleoffset']."'>";
		    $striptext = $_REQUEST['tvmode_striptext'];
		    if(is_array($striptext))
		    {
			foreach($striptext as $entry)
			{
			    $out .= "<INPUT TYPE='hidden' NAME='tvmode_striptext[]' VALUE='$entry'>";
			}
		    }
		}		
		$out .= "</FORM>";
	    }
	    $out .= "<H3>Affected Files:</H3>";
	    $out .= $fl_out;
	}
    }else{
    
	$out .= "<H2>Applying Media Tags Recursively For: $currentpath</H2>";
	$out .= "This tool will read out all the attributes, images, and so on associated with the current path (shown above) and then recursively hunt down all files and/or folders (depending on the settings below) ";
	$out .= "It will then walk through all those files and folders, applying this folder's attributes onto all of them.<BR>";
	$out .= "It is designed to greatly simplify the tagging of large volumes of media. It also has some features targeted specifically at making the tagging of TV Episodes much simpler.<BR><BR>";
	$out .= "Are you absolutely sure you want to do this? this process CAN NOT be reversed, and it WILL completely overwrite any and all tags already existing within this folder on the filesystem.<BR><BR>";
	$out .= "Not to mention I don't want to hear any complaining if it deletes your files, makes you braindead, gives you hives, turns your cat into a wombat, or makes all your media read 'Silly Sams Great Dancing Penguin Parade!'... Because really people... I can't be held responsible...<BR><BR>";
	$out .= "If you are sure, then you can choose below if you want it to apply recursively to folders only, or files only, or even both. It will duplicate all tags/attributes/images applied to this current folder, and propogate it down the filesystem to all children.<BR><BR>";
	$out .= "<FORM NAME='CONFIRM' ACTION='index.php?section=editMediaTagsRecursive&path=".urlencode($_GET['path'])."' METHOD='POST'>";
	$out .= "<H2>Recursive File List Filters:</H2>";
	$out .= "<INPUT TYPE='checkbox' NAME='list_files' CHECKED> List Files<BR>";
	$out .= "<INPUT TYPE='checkbox' NAME='list_folders' CHECKED> List Folders<BR>";
	$out .= "<BR>";
	$out .= "<H2>TV Episode Tagging Mode:</H2>";
	$out .= "The TV Episode Tagging Mode will auto-tag media files based on the filenames. You can fine tune the filename format with the fields below.<BR>";
	$out .= "It currently sets the 'Program' attribute to be the Season Number. Future LMCE Development may change this, but this is the solution for now.<BR>";
	$out .= "The default settings are designed for a file with the following format:<BR><BR>";
	$out .= "4 digit number for season/episode (2 digits each) followed by a space, then hyphen, then space, then the title. With no trailing text.<BR>";
	$out .= "Example: <B>0109 - The Title of My Episode.avi</B><BR><BR>";
	$out .= "<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0 WIDTH=650 STYLE='border:thin solid black;'>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>Enable TV Episode Tagging Mode:</TD><TD><INPUT TYPE='checkbox' NAME='tvmode'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>How Many Characters Into The Filename Does The Season Number Begin:</TD><TD><INPUT TYPE='textbox' NAME='tvmode_seasonoffset' VALUE='1'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>How Many Digits Long The Season Number:</TD><TD><INPUT TYPE='textbox' NAME='tvmode_seasonlength' VALUE='2'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>How Many Characters Into The Filename Does The Episode Number Begin:</TD><TD><INPUT TYPE='textbox' NAME='tvmode_episodeoffset' VALUE='3'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>How Many Digits Long The Episode Number:</TD><TD><INPUT TYPE='textbox' NAME='tvmode_episodelength' VALUE='2'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>How Many Characters Into The Filename Does The Title Begin:</TD><TD><INPUT TYPE='textbox' NAME='tvmode_titleoffset' VALUE='8'></TD></TR>";
	$out .= "<TR><TD ALIGN='right' STYLE='padding-right:10px;'>Text To Strip From Filename (Before Parsing):</TD>";
	$out .= "<TD><DIV ID='striplist'><div><input type='text' name='tvmode_striptext[]' value='' /><BR></div></DIV><INPUT TYPE='button' VALUE='Add' NAME='addstrip' onClick='return addField();' class='button_fixed' /></TD></TR>";
	$out .= "</TABLE><BR>";
	$out .= "<BR><input class='button_fixed' type=button value='Back' onClick='history.go(-1)'> <INPUT TYPE='submit' NAME='submit' VALUE='List Affected Files' CLASS='button_fixed'> (and see sample filename parsing results, if applicable)";
	$out .= "</FORM>";
	$filelisting = MediaFileRecursiveList($currentpath,$dbADO,"file");
	$out .= "<BR><H2>Sample Media Files From Listing (for adjusting name parser)</H2>";
	$out .= "<UL>";
	foreach($filelisting as $entry)
	{
	    $out .= "<LI>".$entry['file']."</LI>";
	}
	$out .= "</UL>";
    }
    
    $output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
    $output->setPageTitle($TEXT_RECURSIVE);
    $output->setReloadLeftFrame(false);
    $output->setScriptInHead($scriptInHead);
    $output->setScriptCalendar('null');
    $output->setBody($out);
    $output->setTitle(APPLICATION_NAME);
    $output->output();								    
}

function ParseFileName($filename,$seasonoffset,$seasonlength,$episodeoffset,$episodelength,$titleoffset,$striptext)
{
    //Strip off the extension:
    $filearray = explode(".",$filename);
    $ext = $filearray[count($filearray)-1];
    $extoffset = strpos($filename,$ext);
    $stripped_filename = substr($filename,0,$extoffset-1);
    
    //Strip off other garbage specified by the user:
    if(is_array($striptext))
    {
	foreach($striptext as $stripentry)
	{
	    $stripped_filename = str_replace($stripentry,"",$stripped_filename);
	}
    }
    
    //Parse the cleaned up filename
    $result['season'] = substr($stripped_filename,$seasonoffset-1,$seasonlength);
    $result['episode'] = substr($stripped_filename,$episodeoffset-1,$episodelength);
    $result['title'] = substr($stripped_filename,$titleoffset-1);
    
    return $result;
}

function GetFileID($path,$mediadbADO)
{
    $filearray = explode("/",$path);
    $filename = $filearray[count($filearray)-1];
    $filepathoffset = strpos($path,$filename);
    $filepath = substr($path,0,$filepathoffset-1);
    $filepath = mysql_escape_string($filepath);
    $filename = mysql_escape_string($filename);
    $resfileID = $mediadbADO->Execute("SELECT PK_File FROM File WHERE Path = '$filepath' AND Filename = '$filename';");
    $rowfileID = $resfileID->FetchRow();
    $fileID = $rowfileID['PK_File'];
    return $fileID;    
}

function GetCurrentFolderAttributes($path,$mediadbADO)
{
    $attribs = array();
    $attribs['Attributes'] = array();
    $attribs['LongAttributes'] = array();
    $attribs['Pictures'] = array();
    $fileID = GetFileID($path,$mediadbADO);
    $resAttributes = $mediadbADO->Execute("SELECT * FROM File_Attribute WHERE FK_File = '$fileID';");
    while($rowAttributes = $resAttributes->FetchRow())
    {
	$attribs['Attributes'][] = $rowAttributes;
    }
    $resLongAttributes = $mediadbADO->Execute("SELECT * FROM LongAttribute WHERE FK_File = '$fileID';");
    while($rowLongAttributes = $resLongAttributes->FetchRow())
    {
	$attribs['LongAttributes'][] = $rowLongAttributes;
    }    
    $resPictures = $mediadbADO->Execute("SELECT * FROM Picture_File WHERE FK_File = '$fileID';");
    while($rowPictures = $resPictures->FetchRow())
    {
	$attribs['Pictures'][] = $rowPictures;
    }
    return $attribs;
}

function SetTVAttributes($path,$mediadbADO,$season,$episode,$title)
{
    $fileID = GetFileID($path,$mediadbADO);
    
    $SeasonAttributeType = GetAttributeType("Program",$mediadbADO);
    $EpisodeAttributeType = GetAttributeType("Episode",$mediadbADO);
    $TitleAttributeType = GetAttributeType("Title",$mediadbADO);
    
    $SeasonAttribute = GetAttribute($season,$SeasonAttributeType,$mediadbADO);
    $EpisodeAttribute = GetAttribute($episode,$EpisodeAttributeType,$mediadbADO);
    $TitleAttribute = GetAttribute($title,$TitleAttributeType,$mediadbADO);
    
    SetAttributeWithOverride($fileID,$SeasonAttribute,$mediadbADO);
    SetAttributeWithOverride($fileID,$EpisodeAttribute,$mediadbADO);
    # Don't over-write the title.  All episodes of the same show should have
    # the same title (e.g. The Simpsons).
    #SetAttributeWithOverride($fileID,$TitleAttribute,$mediadbADO);    
}

function GetAttributeType($desc,$mediadbADO)
{
    $safedesc = mysql_escape_string($desc);
    $resTypeID = $mediadbADO->Execute("SELECT PK_AttributeType FROM AttributeType WHERE Description = '$safedesc';");
    $rowTypeID = $resTypeID->FetchRow();
    return $rowTypeID['PK_AttributeType'];
}

function GetTypeFromAttribute($attribute,$mediadbADO)
{
    $resTypeID = $mediadbADO->Execute("SELECT FK_AttributeType FROM Attribute WHERE PK_Attribute = '$attribute';");
    $rowTypeID = $resTypeID->FetchRow();
    return $rowTypeID['FK_AttributeType'];    
}

function GetAttribute($name,$typeid,$mediadbADO)
{
    $safename = mysql_escape_string($name);
    $resAttribute = $mediadbADO->Execute("SELECT PK_Attribute FROM Attribute WHERE Name = '$safename' AND FK_AttributeType = '$typeid';");
    if($rowAttribute = $resAttribute->FetchRow())
    {
	return $rowAttribute['PK_Attribute'];
    }else{
	$mediadbADO->Execute("INSERT INTO Attribute (FK_AttributeType,Name) VALUES ('$typeid','$safename');");
	$resAttribute = $mediadbADO->Execute("SELECT PK_Attribute FROM Attribute WHERE Name = '$safename' AND FK_AttributeType = '$typeid';");
	$rowAttribute = $resAttribute->FetchRow();
	return $rowAttribute['PK_Attribute'];	
    }
}

function DeleteConflictingAttributes($fileid,$attributetype,$mediadbADO)
{    
    $conflictquery  = "SELECT Attribute.PK_Attribute,AttributeType.PK_AttributeType,File_Attribute.FK_File ";
    $conflictquery .= "FROM ";
    $conflictquery .= "(File_Attribute LEFT JOIN Attribute ON File_Attribute.FK_Attribute = Attribute.PK_Attribute) ";
    $conflictquery .= "LEFT JOIN AttributeType ON Attribute.FK_AttributeType = AttributeType.PK_AttributeType ";
    $conflictquery .= "WHERE AttributeType.PK_AttributeType = '$attributetype' AND File_Attribute.FK_File = '$fileid';";    
    $resConflicts = $mediadbADO->Execute($conflictquery);
    while($rowConflicts = $resConflicts->FetchRow())
    {
	$attribID = $rowConflicts['PK_Attribute'];
	$mediadbADO->Execute("DELETE FROM File_Attribute WHERE FK_File = '$fileid' AND FK_Attribute = '$attribID';");
    }
}

function SetAttributeWithOverride($fileid,$attributeid,$mediadbADO)
{
    DeleteConflictingAttributes($fileid,GetTypeFromAttribute($attributeid,$mediadbADO),$mediadbADO);
    $mediadbADO->Execute("INSERT INTO File_Attribute (FK_File,FK_Attribute) VALUES ('$fileid','$attributeid');");    
}

function SetFileAttributes($path,$mediadbADO,$attribs)
{
    $fileID = GetFileID($path,$mediadbADO);
    
    $drop = "DELETE FROM File_Attribute WHERE FK_File = $fileID;";
    $mediadbADO->Execute($drop);
    foreach($attribs['Attributes'] as $record)
    {
	$q = "INSERT INTO File_Attribute ";
	$q .= "(";
	$q .= "FK_File,";
	$q .= "FK_Attribute,";
	//$q .= "Track,";
	//$q .= "Section,";
	//$q .= "psc_id,";
	//$q .= "psc_batch,";
	//$q .= "psc_user,";
	//$q .= "psc_frozen,";
	//$q .= "psc_mod,";
	$q .= "psc_restrict";
	$q .= ") VALUES (";
	$q .= "'".$fileID."',";
	$q .= "'".$record['FK_Attribute']."',";
	//$q .= "'".$record['Track']."',";
	//$q .= "'".$record['Section']."',";
	//$q .= "'".$record['psc_id']."',";
	//$q .= "'".$record['psc_batch']."',";
	//$q .= "'".$record['psc_user']."',";
	//$q .= "'".$record['psc_frozen']."',";
	//$q .= "'".$record['psc_mod']."',";
	$q .= "'".$record['psc_restrict']."'";
	$q .= ");";
	$mediadbADO->Execute($q);
    }
    
    $drop = "DELETE FROM LongAttribute WHERE FK_File = $fileID;";
    $mediadbADO->Execute($drop);
    foreach($attribs['LongAttributes'] as $record)
    {
	$q = "INSERT INTO LongAttribute ";
	$q .= "(";
	$q .= "FK_File,";
	$q .= "FK_AttributeType,";
	$q .= "FK_Disc,";
	$q .= "FK_Attribute,";
	$q .= "Text,";
	//$q .= "psc_id,";
	//$q .= "psc_batch,";
	//$q .= "psc_user,";
	//$q .= "psc_frozen,";
	//$q .= "psc_mod,";
	$q .= "psc_restrict";
	$q .= ") VALUES (";
	$q .= "'".$fileID."',";
	$q .= "'".$record['FK_AttributeType']."',";
	$q .= "'".$record['FK_Disc']."',";
	$q .= "'".$record['FK_Attribute']."',";
	$q .= "'".mysql_escape_string($record['Text'])."',";
	//$q .= "'".$record['psc_id']."',";
	//$q .= "'".$record['psc_batch']."',";
	//$q .= "'".$record['psc_user']."',";
	//$q .= "'".$record['psc_frozen']."',";
	//$q .= "'".$record['psc_mod']."',";
	$q .= "'".$record['psc_restrict']."'";
	$q .= ");";
	$mediadbADO->Execute($q);
    }
    
    $drop = "DELETE FROM Picture_File WHERE FK_File = $fileID;";
    $mediadbADO->Execute($drop);
    foreach($attribs['Pictures'] as $record)
    {
	$q = "INSERT INTO Picture_File ";
	$q .= "(";
	$q .= "FK_File,";
	$q .= "FK_Picture,";
	//$q .= "psc_id,";
	//$q .= "psc_batch,";
	//$q .= "psc_user,";
	//$q .= "psc_frozen,";
	//$q .= "psc_mod,";
	$q .= "psc_restrict";
	$q .= ") VALUES (";
	$q .= "'".$fileID."',";
	$q .= "'".$record['FK_Picture']."',";
	//$q .= "'".$record['psc_id']."',";
	//$q .= "'".$record['psc_batch']."',";
	//$q .= "'".$record['psc_user']."',";
	//$q .= "'".$record['psc_frozen']."',";
	//$q .= "'".$record['psc_mod']."',";
	$q .= "'".$record['psc_restrict']."'";
	$q .= ");";
	$mediadbADO->Execute($q);
    }
}

function MediaFileRecursiveList($path,$dbADO,$listtype)
{
    $extlist = ListMediaTypes($dbADO);
    if($listtype=="file")
    {
        $tempfilelist = RecursiveFileArray($path,true);	    
	$filearray = array();
	foreach($tempfilelist as $entry)
	{
	    $entryarray = explode(".",$entry['file']);
	    $entryext = $entryarray[count($entryarray)-1];
	    if(array_search($entryext,$extlist))
	    {
		$temp['file'] = $entry['path']."/".$entry['file'];
		$temp['type'] = $listtype;
		$filearray[] = $temp;
	    }
	}
    }else{
        $tempfilelist = RecursiveDirArray($path,true);	    
	$filearray = array();
	foreach($tempfilelist as $entry)
	{
	    $temp['file'] = $entry['path']."/".$entry['file'];
	    $temp['type'] = $listtype;
	    $filearray[] = $temp;
	}    
    }
    asort($filearray);
    return $filearray;
}

function ListMediaTypes($dbADO)
{
    $extensionarray = array();
    $resMT = $dbADO->Execute("SELECT Extensions FROM MediaType WHERE Extensions != '';");
    while($rowMT = $resMT->FetchRow())
    {
	$extensiontemp = explode(",",$rowMT["Extensions"]);
	$extensionarray = array_merge($extensionarray,$extensiontemp);
    }
    return $extensionarray;
}

function RecursiveDirArray($directory, $recursive) 
{
    $array_items = array();
    if ($handle = opendir($directory)) 
    {
	while (false !== ($file = readdir($handle))) 
	{
	    if ($file != "." && $file != "..") 
	    {
		if (is_dir($directory. "/" . $file)) 
		{
		    if($recursive) 
		    {
			$array_items = array_merge($array_items, RecursiveDirArray($directory. "/" . $file, $recursive));
		    }
		    $arrayitem['file'] = preg_replace("/\/\//si", "/", $file);
		    $arrayitem['path'] = preg_replace("/\/\//si", "/", $directory);
		    $array_items[] = $arrayitem;
		}
	    }
	}
	closedir($handle);
    }
    return $array_items;
}

function RecursiveFileArray($directory, $recursive) 
{
    $array_items = array();
    if ($handle = opendir($directory)) 
    {
	while (false !== ($file = readdir($handle))) 
	{
	    if ($file != "." && $file != "..") 
	    {
		if (is_dir($directory. "/" . $file)) 
		{
		    if($recursive) 
		    {
			$array_items = array_merge($array_items, RecursiveFileArray($directory. "/" . $file, $recursive));
		    }
		} else {
		    $arrayitem['file'] = preg_replace("/\/\//si", "/", $file);
		    $arrayitem['path'] = preg_replace("/\/\//si", "/", $directory);
		    $array_items[] = $arrayitem;
		}
	    }
	}
	closedir($handle);
    }
    return $array_items;
}

?>
