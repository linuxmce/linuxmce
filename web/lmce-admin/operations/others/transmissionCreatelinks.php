<?php
function transmissionCreatelinks($path, $Transmission_Root, $transmissionADO, $CLpage, $transmission_files_per_page) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');
	include(APPROOT.'/operations/others/transmissionEditLinks.php');

	$operation = (isset($_REQUEST['operation']) && $_REQUEST['operation']!='')?cleanString($_REQUEST['operation']):'';
	$ID = (isset($_REQUEST['ID']) && $_REQUEST['ID']!='')?cleanString($_REQUEST['ID']):'';

	$noPagesArray=array(
	10=>'10',
	20=>'20',
	50=>'50',
	100=>'100',
	500=>'500',
	1000=>'1000',
	2000=>'2000',
	10000=>'10000',
	1000000=>'All *'
	);

	$out='
		<script>
		function transmissionPath(path)
		{
			var dirSep = document.getElementById("dirSep").value;

			if ( path != document.transmission.Transmission_Root.value ){
				document.transmission.parentdir.value= path.substr(0,path.lastIndexOf(dirSep));
			};

			document.transmission.path.value=path;
			document.transmission.CLpage.value=1;
			document.transmission.submit()
		}

		function windowOpen(locationA,attributes) {
			window.open(locationA,"",attributes);
		}

		function ChangeLink(ID, operation)
		{
			document.getElementById("operation").value=operation
			document.getElementById("ID").value=ID
			document.getElementById("transmission").submit()
			//document.getElementById("create_links").click();
		}

		</script>
                <input type="hidden" name="create_link" value="form">
                ';

	transmissionEdit($transmissionADO, $operation, $ID);

	$operation="";
	$ID="";

	$out.='
		<input type="hidden" name="operation" id="operation" value="'.$operation.'">
		<input type="hidden" name="ID" id="ID" value="'.$ID.'">
		';

	//Build table from links in database
	$TorrentContent=dirToArray(stripslashes($path));

	$fileCount=0;
	foreach ($TorrentContent as $content){
		if ($content[1]==0){
			$fileCount++;
		};
	}

	$fileFrom=(($CLpage-1)*$transmission_files_per_page) + 1;

	if (($CLpage*$transmission_files_per_page) > $fileCount){
		$toFile=$fileCount;
	}else{
		$toFile=$CLpage*$transmission_files_per_page;
	};

	$itemCounter=$fileFrom ." to ".$toFile;

	$noPages=ceil($fileCount/$transmission_files_per_page);

	$navBarArray=array();

	for($i=1;$i<$noPages+1;$i++){
		$navBarArray[]=($i==$CLpage)?$i:'<a href="javascript:{}" onclick="document.getElementById(\'CLpage\').value='.$i.';document.transmission.submit();" class="white_link">'.$i.'</a>';
	}

	$navBar=join(' | ',$navBarArray);

	$out.='
		<a href="javascript:transmissionPath(\''. $_POST['parentdir'] . '\')">Up...</a><br>
		<table>';

	if (is_array($TorrentContent)){
		$out.='<tr><td class="tablehead">'.$TEXT_TRANSMISSION_LINK_STATUS.'</td><td class="tablehead">'.$TEXT_TRANSMISSION_TORRENT_LOCATION.'</td><td class="tablehead">'.$TEXT_TRANSMISSION_CREATE_DELETE.'</td></tr>';
		$out.=DisplayContents($path, $TorrentContent, $transmissionADO, $fileFrom, $toFile);
	}else{
		$out.='<tr><td>'.$TEXT_TRANSMISSION_NO_FILES.'</td></tr>';
	}

	$out.='
			<tr class="tablehead">
				<td>&nbsp;</td>
				<td><B>Files per page:</B> '.pulldownFromArray($noPagesArray,'transmission_files_per_page',$transmission_files_per_page,'onChange="document.transmission.CLpage.value=1;document.transmission.files_per_page.value=this.value;document.transmission.submit();"','key','').', Displaying '.$itemCounter.' Items</td>
				<td align="right">'.$navBar.'</td>
			</tr>
		</table>';

	return $out;

}

function dirToArray($dir) { 
	$info=array();
	$result=array(); 
	$rarfiles=array();
	$israr=false;

	$cdir = scandir($dir); 
	foreach ($cdir as $key => $value) 
	{ 
		if (!in_array(addslashes($value),array(".",".."))) 
		{ 
			$israr=false;

			if (is_dir($dir . DIRECTORY_SEPARATOR . $value)) 
			{ 
				$info[0]="dir";
				$info[1]=0;
			}else{ 
				if (pathinfo($value, PATHINFO_EXTENSION)=="rar"){
					if (isset($rarfiles[$dir . DIRECTORY_SEPARATOR . $value])){
						$info[0]="rarpart";
						$info[1]=1;
					}else{
						$info[0]="rar";
						$info[1]=0;
						$israr=true;
					};
				}else{
					if (isset($rarfiles[$dir . DIRECTORY_SEPARATOR . $value])){
						$info[0]="rarpart";
						$info[1]=1;
					}else{
						$info[0]="file";
						$info[1]=0;
					};
				};
			} 

			$info[2]=addslashes($value);
			$result[]=$info;

			if ($israr){
				$rarpart=getrarinfo($dir, $value, $result);

				$result=$rarpart[0];

				if ( count($rarfiles)>0 && $rarfiles[0]!=""){
					$rarfiles=array_merge($rarfiles,$rarpart[1]);
				}else{
					$rarfiles=$rarpart[1];
				};
			};
		}; 
	} 

	return $result; 
}

function getrarinfo($dir, $file, $filearray){
	$result=array();
	$i_count=0;

	exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getRar \''.$dir.DIRECTORY_SEPARATOR.$file.'\'', $rarparts);
	$rarparts=array_flip($rarparts);

	if (is_array($rarparts)){
		for ($i_count=0; $i_count<=count($filearray); $i_count++){
			if (isset($rarparts[stripslashes($dir.DIRECTORY_SEPARATOR.$filearray[$i_count][2])]) && $rarparts[stripslashes($dir.DIRECTORY_SEPARATOR.$filearray[$i_count][2])]>0){
				$filearray[$i_count][1]=1;
				$filearray[$i_count][0]="rarpart";
			};
		}
	};

	$result[0]=$filearray;
	$result[1]=$rarparts;
	return $result;
}

function DisplayContents($path, $content, $transmissionADO, $fromFile, $toFile) {
	$result='';
	$i_fileCount=0;
	$i_button_count=0;

	foreach ($content as $FolderList){
		if ( $FolderList[1]==0){
			$i_fileCount++;

			if ( $i_fileCount >= $fromFile && $i_fileCount <= $toFile ){
				$queryLinks='SELECT * FROM Torrent_Links WHERE Torrent_Location=\''.$path.DIRECTORY_SEPARATOR.$FolderList[2].'\';'; 
				$Linkresults=$transmissionADO->Execute($queryLinks);

				$i_button_count ++;

				$result.='<tr>';

				if ($Linkresults->RecordCount()==0){
					$result.='<td><img src=include/images/disk.gif align=middle border=0></td>';
				}else{
					if ( $FolderList[0]=="rar"){
						$result.='<td>';
						$lineCount=0;

						foreach ($Linkresults as $link){
							if ($lineCount>0){
								$result.='<br>';
							};

							if ($link["Link_Status"]=="extracting"){
								$result.='Archive Extracting';
							}else{
								$result.='<img src=include/images/sync.gif align=middle border=0>';
							};
						}

						$Linkresults->Move(0);

						$result.='</td>';
					}else{
						$result.='<td><img src=include/images/sync.gif align=middle border=0></td>';
					};
				};

				if ($FolderList[0]=="dir"){
					$result.='<td><a href="javascript:transmissionPath(\''.addslashes($path.DIRECTORY_SEPARATOR.$FolderList[2]).'\')">'.stripslashes($FolderList[2]).'</a></td>';
				}else{
					$result.='<td>'.stripslashes($FolderList[2]).'</td>';
				};

				if ( $FolderList[0]=="rar"){
					if ($Linkresults->RecordCount()>0){
						$Linkresults->MoveFirst();
						$link=$Linkresults->FetchRow();

						if ($link['Rar_File']==''){
							//Complete Archive was unpacked
							if ($link["Link_Status"]=="extracting"){
								$result.='<td>Archive Extracting';
							}else{
								$result.='<td><input type="button" name="'.$i_button_count.'" value="Delete Extract" onclick="ChangeLink('.$Link["Link_ID"].', \'delete\')">';
							};
						}else{
							$volumeInfo = GetRarVolume(stripslashes($path.DIRECTORY_SEPARATOR.$FolderList[2]));
							$Linkresults->Move(0);

							foreach ($Linkresults as $link){
								if (isset($volumeInfo[$link["Rar_File"]])){
								    	unset($volumeInfo[$link["Rar_File"]]);
								};
							}

							if (count($volumeInfo)==0){
								//Not all the array was unpacked
								//Needs to be improved for where there are multiple files extraced.
								$Linkresults->Move(0);
								$result.='<td>';
								$lineCount=0;

								foreach ($Linkresults as $link){
									if ($lineCount>0){
										$result.='<br>';
									};

									if ($link["Link_Status"]=="extracting"){
										$result.='Archive Extracting';
									}else{
										$result.='<input type="button" name="'.$i_button_count.'" value="Delete:'.$link["Link_Name"].'" onclick="ChangeLink('.$Link["Link_ID"].', \'delete\')">';
									};
									$lineCount++;
								}
							}else{
								//All of the array has been unpacked
								$result.='<td><input type="button" name="'.$i_button_count.'" value="Unpack Archive" onclick="windowOpen(\'index.php?section=transmissionLink&path='.addslashes($path.DIRECTORY_SEPARATOR.$FolderList[2]).'&linkType=rar\',\'width=600,height=600,toolbar=1,resizable=1,scrollbars=1\');">';
							};
						};
					}else{
						$result.='<td><input type="button" name="'.$i_button_count.'" value="Unpack Archive" onclick="windowOpen(\'index.php?section=transmissionLink&path='.addslashes($path.DIRECTORY_SEPARATOR.$FolderList[2]).'&linkType=rar\',\'width=600,height=600,toolbar=1,resizable=1,scrollbars=1\');">';
					};
				}else{
					if ($Linkresults->RecordCount()==0){
						$result.='<td><input type="button" name="'.$i_button_count.'" value="Add Link" onclick="windowOpen(\'index.php?section=transmissionLink&path='.addslashes($path.DIRECTORY_SEPARATOR.$FolderList[2]).'&linkType=link\',\'width=600,height=600,toolbar=1,resizable=1,scrollbars=1\');">';
					}else{
						foreach ( $Linkresults As $Link){
							$result.='<td><input type="button" name="'.$i_button_count.'" value="Delete Link" onclick="ChangeLink('.$Link["Link_ID"].', \'delete\')">';
						}
					};
				};

				$result.='</td></tr>';
			};
		};
	}

	return $result;
}

function GetRarVolume($path){
	$i_readCount=0;
	$return=array();

	exec('sudo -u root /usr/pluto/bin/transmission-settings.sh getRarVolume \''.$path.'\'', $volumeInfo);

	foreach ($volumeInfo as $line) {
		if (substr(trim($line), 0, 6)=="------"){
			$i_readCount++;
		}elseif ($i_readCount > 0 && $i_readCount < 2){
			$line=trim(strstr(trim($line), " -->", true));
			$line=trim(substr($line,0,strrpos($line, " ")));
			$line=trim(substr($line,0,strrpos($line, " ")));

			$return[$line]=$line;
		};
	}

	return $return;
}
?>
