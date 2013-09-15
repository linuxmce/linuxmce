<?php
function transmissionLink($output,$dbADO,$transmissionADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');

	$out='';

	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	$linkName = (isset($_REQUEST['linkName']) && $_REQUEST['linkName']!='')?cleanString($_REQUEST['linkName']):substr($path,strrpos($path, DIRECTORY_SEPARATOR)+1);
	$linkTo = (isset($_REQUEST['linkTo']) && $_REQUEST['linkTo']!='')?cleanString($_REQUEST['linkTo']):'/home/public/data';
	$linkType = (isset($_REQUEST['linkType']) && $_REQUEST['linkType']!='')?cleanString($_REQUEST['linkType']):'';
	$rarcount = (isset($_REQUEST['rarcount']) && $_REQUEST['rarcount']!='')?cleanString($_REQUEST['rarcount']):'0';

	if ($action=='form' && $linkType!='') {
		$out.='
			<form action="index.php" method="POST" name="transmissionLink" id="transmissionLink">
			<input type="hidden" name="section" value="transmissionLink">
			<input type="hidden" name="action" id="action" value="'.$action.'">
			<input type="hidden" name="path" value="'.$path.'">
			<input type="hidden" name="linkTo" id="linkTo" value="'.$linkTo.'">
			<input type="hidden" name="linkType" id="linkType" value="'.$linkType.'">

			<script>
				function breadcrumb(linkTo, method) {
					document.getElementById("linkTo").value=linkTo;
					document.getElementById("action").value=method;
					document.getElementById("transmissionLink").submit();
				}

				function showHint(str) {
					var xmlhttp;

					if (str.length==0)
					{ 
						document.getElementById("txtHint").innerHTML="";
						return;
					}

					if (window.XMLHttpRequest)
						{// code for IE7+, Firefox, Chrome, Opera, Safari
						xmlhttp=new XMLHttpRequest();
					}
					else
					{// code for IE6, IE5
						xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
					}

					xmlhttp.onreadystatechange=function() {
						if (xmlhttp.readyState==4 && xmlhttp.status==200)
						{
							document.getElementById("txtHint").innerHTML=xmlhttp.responseText;
						}
					}
					xmlhttp.open("GET","./operations/others/transmissionCheckfilename.php?q="+str,true);
					xmlhttp.send();
				}

				function CheckFilename() {
					var filepath;
					var response;

					filepath=addslashes(document.getElementById("linkTo").value+"'.DIRECTORY_SEPARATOR.'"+document.getElementById("linkName").value);

					showHint(filepath);
				}

				function addslashes(string){
					return string.replace(/\\\\/g, \'\\\\\\\\\').
					replace(/\u0008/g, \'\\\\b\').
					replace(/\t/g, \'\\\\t\').
					replace(/\n/g, \'\\\\n\').
					replace(/\f/g, \'\\\\f\').
					replace(/\r/g, \'\\\\r\').
					replace(/\'/g, \'\\\\\\\'\').
					replace(/"/g, \'\\\\"\');
				}

				function disableAll() {
					for (item in document.transmissionLink.elements){
						if (item.name.substring(0,8) == "extract_" ){
							if ( document.transmissionLink.extractAll.checked ){
								item.checked = false;
								item.enabled = false;
							}else{
								item.enabled = true;
							}
						}else if (item.name.substring(0,8) == "rarname_"){
							if ( document.transmissionLink.extractAll.checked ){
								item.enabled = false;
							}else{
								item.enabled = true;
							}
						}
					}
				}
			</script>';

			$dirContent=dirToArray($linkTo);

			if (file_exists(stripslashes($linkTo.DIRECTORY_SEPARATOR.$linkName))){
				$safename="File Exists";
			}else{
				$safename="";
			};

			$out.='
				<h3>'.$TEXT_TRANSMISSION_CREATE_LINK_CONST.'</h3>
				<table>	
					<tr valign="top" class="tablehead">
						<td colspan="3">';
			if ($linkType=='rar'){
				$out.=$TEXT_TRANSMISSION_RARARCHIVE;
			}else{
				$out.=$TEXT_TRANSMISSION_FILENAME;
			};
						$out.='</td>
					</tr>
					<tr valign="top">
						<td colspan="3">'.stripslashes($path).'</td>
					</tr>';

			if ($linkType=='rar'){
				$out.='
					<tr valign="top" class="tablehead">
						<td><input type="checkbox" name="extractAll" onchange="disableAll()">'.$TEXT_TRANSMISSION_EXTRACTALL.'</td><td colspan="2">'.$TEXT_TRANSMISSION_RARCONTENTS.'</td>
					</tr>
					<tr valign="top" class="tablehead">
						<td>Extract</td>
						<td>Stored as</td>
						<td>Extract Name</td>
					</tr>';

				exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getRarVolume \''.stripslashes($path).'\'', $volumeInfo);

				$i_readCount=0;
				$i_rarContent=0;

				foreach ($volumeInfo as $line) {
					if (substr(trim($line), 0, 6)=="------"){
						$i_readCount++;
					}elseif ($i_readCount > 0 && $i_readCount < 2){
						$line=trim(strstr(trim($line), " -->", true));
						$line=trim(substr($line,0,strrpos($line, " ")));
						$line=trim(substr($line,0,strrpos($line, " ")));

						$i_rarContent++;

						$newname=$_REQUEST['rarname_'.$i_rarContent];

						if ( $newname=="" ){
							$newname=$line;
						};

						if ( $_REQUEST["extract_".$i_rarContent]){
							$checked="checked";
						}else{
							$checked="";
						};

						$out.='
							<tr valign="top">
								<td><input type="checkbox" name="extract_'.$i_rarContent.'" '.$checked.'></td>
								<td>'.$line.'<input type="hidden" name="origrarname_'.$i_rarContent.'" value="'.$line.'"></td>
								<td><input type="text" size="50" name="rarname_'.$i_rarContent.'" value="'.$newname.'"></td>';

						if (file_exists(stripslashes($linkTo.DIRECTORY_SEPARATOR.$line))){
							$out.='<td><font color="red">'.$TEXT_TRANSMISSION_FILE_EXISTS.'</font></td>';
						}else{
							$out.='<td>&nbsp;</td>';
						};

						$out.='</tr>';
					};
				}
				$out.='<input type="hidden" name="rarcount" value="'.$i_rarContent.'">';
			}else{
				$out.='
					<tr valign="top">
						<td class="tablehead" colspan="3">'.$TEXT_TRANSMISSION_LINK_NAME.'
							<input type="text" name="linkName" id="linkName" size="50" value="'.$linkName.'" onkeyup="CheckFilename()">
						</td>
					</tr>					
					<tr valign="top">
						<td colspan="3">
							<span id="txtHint">'.$safename.'</span>
						</td>
					</tr>';
			};

			$out.='
					<tr valign="top">
						<td class="tablehead" colspan="3">'.$TEXT_TRANSMISSION_LINK_LOCATION.'</td>
					</tr>					
					<tr valign="top">
						<td colspan="3">
							'.breadcrumb($linkTo).'
						</td>
					</tr>					
					<tr>
						<td colspan="3" align="center">
						<input type="button" class="button" name="submitX" id="submitX" value="'.$TEXT_SAVE_CONST.'" onClick="breadcrumb(\''.addslashes($linkTo) .'\', \'commit\')" >

						<input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
					</tr>
					</table>

					<table>
					<tr>
						<td class="tablehead">'.$TEXT_TRANSMISSION_BROWSE.'</td>
					</tr>';

		if (is_array($dirContent)){
			$out.=DisplayContents($linkTo, $dirContent);
		}else{
			$out.='<tr><td>'.$TEXT_TRANSMISSION_NO_FILES.'</td></tr>';
		};

		$out.='
			</table>
			</form>';
	} elseif ($action=='commit' && $linkType=="link") {
		//Database insert and Link insert
		//The bit that closes the form
		//setTimeout(function(){self.close();},5000);
		$queryLinks='INSERT INTO Torrent_Links (Torrent_Location, Link_Location, Link_Name, Link_Type) VALUES (?,?,?,?)';

		$out.='<table>';

		if (! file_exists(stripslashes($linkTo.DIRECTORY_SEPARATOR.$linkName))){
			$transmissionADO->Execute($queryLinks,array(stripslashes($path),stripslashes($linkTo),stripslashes($linkName),'link'));
			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh createLink "'.stripslashes($path).'" "'.stripslashes($linkTo.DIRECTORY_SEPARATOR.$linkName).'"');

			$out.='Link Created
				<tr>
					<td colspan="2" align="center"><input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
				</tr>';
		}else{
			$out.='Link Failed, File already exists!
                                <tr>
                                        <td colspan="2" align="center"><input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
                                </tr>';	
		};

		$out.='</table>';
	} elseif ($action=='commit' && $linkType=="rar") {
		//This is a rar file to expand
		if ($_REQUEST['extractAll']){
			$queryLinks='INSERT INTO Torrent_Links (Torrent_Location, Link_Location, Link_Name, Link_Type, Link_Status) VALUES (?,?,?,?,?)';
			$transmissionADO->Execute($queryLinks,array(stripslashes($path),stripslashes($linkTo),'','rar','extracting'));
			$ID=$transmissionADO->Insert_ID();

			exec_batch_command('sudo -u root /usr/pluto/bin/transmission-settings.sh extractRar "'.stripslashes($path).'" "'.stripslashes($linkTo.DIRECTORY_SEPARATOR).'" "'.$ID.'"');
		}else{
			$queryLinks='INSERT INTO Torrent_Links (Torrent_Location, Rar_File, Link_Location, Link_Name, Link_Type, Link_Status) VALUES (?,?,?,?,?,?)';
			$ID=array();

			for ($i_rarcount=1; $i_rarcount<=$rarcount; $i_rarcount++){
				if ( $_REQUEST['rarname_'.$i_rarcount]!="" && $_REQUEST['extract_'.$i_rarcount]){
					$transmissionADO->Execute($queryLinks,array(stripslashes($path),$_REQUEST['origrarname_'.$i_rarcount],stripslashes($linkTo),$_REQUEST['rarname_'.$i_rarcount],'rar','extracting'));
					$ID[]=$transmissionADO->Insert_ID();
				};
			}

			exec('sudo -u root /usr/pluto/bin/transmission-settings.sh extractRarfiles "'.stripslashes($path).'" "'.stripslashes($linkTo.DIRECTORY_SEPARATOR).'" "'.implode('" "',$ID).'" >> /dev/null &');

		};

		$out.='RAR archive:'.stripslashes($path).' is being extracted, this may take some time!
			<table>
			<tr>
				<td colspan="2" align="center"><input type="button" class="button" name="update" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
			</table>';
	} else {
		die('<script>self.location=\'index.php\'</script>');
                exit();
	};

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_TRANSMISSION_LINK);			
	$output->output();
}

function dirToArray($dir) {
	$result = array();

	$cdir = scandir($dir);
	foreach ($cdir as $key => $value)
	{
		if (!in_array($value,array(".","..")))
		{
			if (is_dir($dir . DIRECTORY_SEPARATOR . $value))
			{
				$result[]=$value;
			};
		};
	}

	return $result;
}

function breadcrumb($Link) {
	$LinkArr=explode(DIRECTORY_SEPARATOR, $Link);
	$result='';
	$crumbtrail='';

	foreach ($LinkArr as $crumb)
	{
		$crumbtrail=substr($Link,0,strpos($Link,$crumb)).$crumb;
		$result.='/<a href="javascript:breadcrumb(\''.addslashes($crumbtrail).'\',\'form\');">'.$crumb.'</a>';
	}

	return $result;
}

function DisplayContents($linkTo, $content, $Name) {

	$result='';

	foreach ($content as $FolderList){
		$result.='<tr><td><a href="javascript:breadcrumb(\''.addslashes($linkTo.DIRECTORY_SEPARATOR.$FolderList) .'\',\'form\')">'.$FolderList.'</a></td></tr>';
	}

	return $result;
}
?>
