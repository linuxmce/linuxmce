<?PHP

error_reporting(E_ALL);
ini_set('display_errors', 0);

function internet_radio($output,$mediadbADO,$dbADO) {
		// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('internetradio.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$accessFile=$GLOBALS['pluto.conf'];
	
	// check pluto.conf
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}		
	
	#define vars what later will be overwritten
	$out='';
	$continet='';
	$region='';
	$stations='';
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	if ($action == 'form') {
		
			$query='SELECT Name FROM Attribute WHERE FK_AttributeType=8';
			$res=$mediadbADO->Execute($query);
			if ($res->RecordCount() == 0) {
				$manual_genres='<input type="text" name="station_genre" value="" />';
			}
			else {
				$manual_genres='<select onchange="Genre(this.value);" name="station_genre">';
				while ($row=$res->FetchRow()) {
					$manual_genres.='<option value="'.$row['Name'].'">'.$row['Name'].'</option>';
				}
				$manual_genres.='<option value="no_genre">Genre not in list.</option></select>';
			}

		function grep_internet_radio_data( $url, $list, $name, $selected ) {
			$ch = curl_init();
				$timeout = 5; // set to zero for no timeout
				curl_setopt ($ch, CURLOPT_URL, $url);
				curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
				curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
				$file_contents = curl_exec($ch);
			curl_close($ch);
			$lines = array();
			$lines = explode("\n", $file_contents);
			if ($list == 'Dropdown_list') {
				$data='<select name="'.$name.'" onChange="IR'.$name.'()"><option value="">Select</option>';
				foreach( $lines as $line) {
					if (preg_match('/^<outline/', $line)) {
						//$line=htmlspecialchars($line);
						//echo $line.'<br />';
						preg_match('@^(?:<outline type="link" text=")?([^"]+)@i', $line, $text);
						preg_match('@^(?:<outline type="link" text="'.$text['1'].'" URL=")?([^"]+)@i', $line, $URL);
						//echo $URL['1'];
						if ($selected == $URL['1']) {
							$data.='<option value="'.$URL['1'].'" selected>'.$text['1'].'</option>';
						} else {
							$data.='<option value="'.$URL['1'].'">'.$text['1'].'</option>';
						}
					}
				}
				$data.='</select>';
			} else {
				$data='<input type="hidden" name="stationsUrl" value="'.$url.'" />';
				if ($url == 'http://opml.radiotime.com/Browse.ashx?c=local'){
				$data.='<p class="err" >stations external found on your location (can be wrong).<br />
				if stations already on the system they are not being installed again.</p>';
				}
				$data.='';
				foreach( $lines as $line) {
					if (preg_match('/^<outline/', $line)) {
						preg_match('@^(?:<outline type="audio" text=")?([^"]+)@i', $line, $text);
						//preg_match('@^(?:<outline type="audio" text="'.$text['1'].'" URL=")?([^"]+)@i', $line, $URL);
						$data.=$text['1'].'<br />';
					}
					$data.='</p>';
				}
				$data.='<input type="button" class="button" value="add (all)" onclick="add()" />';
			}
			return $data;
		}
		
		if (!isset($_POST['continent'])) {
			$continent=grep_internet_radio_data('http://opml.radiotime.com/Browse.ashx?id=r0','Dropdown_list','continent','');
		} else {
			$continent=grep_internet_radio_data('http://opml.radiotime.com/Browse.ashx?id=r0','Dropdown_list','continent',$_POST['continent']);
		}
		if (!isset($_POST['country']) && isset($_POST['continent'])) {
			$country=grep_internet_radio_data($_POST['continent'],'Dropdown_list','country','');
		} elseif (isset($_POST['country'])) {
			$country=grep_internet_radio_data($_POST['continent'],'Dropdown_list','country',$_POST['country']);
		}
 
		if (!isset($_POST['region']) && isset($_POST['country'])) {
			$region=grep_internet_radio_data($_POST['country'],'Dropdown_list','region','');
		} elseif (!isset($_POST['region'])) {
			$stations=grep_internet_radio_data('http://opml.radiotime.com/Browse.ashx?c=local','list','','');
		} elseif (isset($_POST['region'])) {
			$region=grep_internet_radio_data($_POST['country'],'Dropdown_list','region',$_POST['region']);
			$stations=grep_internet_radio_data($_POST['region'],'list','','');
		} 
		if (($_POST['country'] == "http://opml.radiotime.com/Browse.ashx?id=r100436" || $_POST['country'] == "http://opml.radiotime.com/Browse.ashx?id=r101227") && $_POST['region'] !="" ) {
		   if (!isset($_POST['city']) && isset($_POST['region'])) {
		   	  $city=grep_internet_radio_data($_POST['region'],'Dropdown_list','city','');
		   } elseif (!isset($_POST['city'])) {
		   	  $stations=grep_internet_radio_data('http://opml.radiotime.com/Browse.ashx?c=local','list','','');
		   } elseif (isset($_POST['city'])) {
			  $city=grep_internet_radio_data($_POST['region'],'Dropdown_list','city',$_POST['city']);
			  if ($_POST['city'] != "") {	
		      	 $stations=grep_internet_radio_data($_POST['city'],'list','','');
			  }																						   
		   }
		}
		 $out.='<script>
			function Genre(value){
				var x=document.getElementById("Genre");
				if (value == \'no_genre\') {
					x.innerHTML = "<input type=\"text\" name=\"station_genre\" value=\"\" />";
				}
			}

			var input = document.createElement("input");
			input.setAttribute("type", "hidden");
			input.setAttribute("name", "action");
			input.setAttribute("value", "form");

			var input1 = document.createElement("input");
			input1.setAttribute("type", "hidden");
			input1.setAttribute("name", "action");
			input1.setAttribute("value", "add");

			var input2 = document.createElement("input");
			input2.setAttribute("type", "hidden");
			input2.setAttribute("name", "action");
			input2.setAttribute("value", "add_manual");
			
			var input3 = document.createElement("input");
			input3.setAttribute("type", "hidden");
			input3.setAttribute("name", "action");
			input3.setAttribute("value", "del");
			
			var input4 = document.createElement("input");
			input4.setAttribute("type", "hidden");
			input4.setAttribute("name", "action");
			input4.setAttribute("value", "sort");

			function IRcontinent()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').submit();
			}

			function IRcountry()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').country.value;
				document.getElementById(\'internet_radio\').submit();
			}

			function IRregio()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').country.value;
				document.getElementById(\'internet_radio\').regio.value;
				document.getElementById(\'internet_radio\').submit();
			}

			function IRregion()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').country.value;
				document.getElementById(\'internet_radio\').region.value;
				document.getElementById(\'internet_radio\').submit();
			}
			function IRcity()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').country.value;
				document.getElementById(\'internet_radio\').region.value;
				document.getElementById(\'internet_radio\').city.value;
				document.getElementById(\'internet_radio\').submit();
			}

			function add()
			{
				document.getElementById(\'internet_radio\').appendChild(input1);
				document.getElementById(\'internet_radio\').stationsUrl.value;
				document.getElementById(\'internet_radio\').submit();
			}
			function sort()
			{
				document.getElementById(\'internet_radio\').appendChild(input4);
				document.getElementById(\'internet_radio\').setDefaultSortRadio.value = 1;
				document.getElementById(\'internet_radio\').submit();
			}
			function del()
			{
			    document.getElementById(\'internet_radio\').appendChild(input3);
				document.getElementById(\'internet_radio\').station_name.value;
				document.getElementById(\'internet_radio\').stream_url.value;
				document.getElementById(\'internet_radio\').station_genre.value;
				document.getElementById(\'internet_radio\').submit();
			}
			var formblock;
			var forminputs;
 
 			function prepare()
			{
  			    formblock= document.getElementById(\'internet_radio\');
  				forminputs = formblock.getElementsByTagName(\'input\');
			}
 
 			function selall(value)
			{
  			    for (i = 0; i < forminputs.length; i++) {
    			    if (forminputs[i].type === \'checkbox\') {
						if (value == \'1\') {
        				   forminputs[i].checked = true;
      					} else {
        				   forminputs[i].checked = false;
						}
					}
					
  				}
			}
 		
			
			function confirmAddInternetRadio()
        	{
                        document.getElementById(\'internet_radio\').AddInternetRadio.value;
						document.getElementById(\'internet_radio\').submit();
            }
 
 			if (window.addEventListener) {
     		    window.addEventListener("load", prepare, false);
			} else if (window.attachEvent) {
  			    window.attachEvent("onload", prepare)
			} else if (document.getElementById) {
  			    window.onload = prepare;
			}
			
			
			function add_manual()
			{
				document.getElementById(\'internet_radio\').appendChild(input2);
				document.getElementById(\'internet_radio\').station_name.value;
				document.getElementById(\'internet_radio\').stream_url.value;
				document.getElementById(\'internet_radio\').station_genre.value;
				document.getElementById(\'internet_radio\').submit();
			}

				
		</script>
		
		
		<div '.(isset($_GET['error']).''?strip_tags('class="err"'):'').'>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div  '.(isset($_GET['msg']).''?strip_tags('class="success"'):'').' ><h2><B >'.(isset($_GET['msg']).''?strip_tags($_GET['msg']):'').'</B></h2></div>
		
		<div '.(isset($_GET['msg2']).''?strip_tags('class="err"'):'').' ><h2><B >'.(isset($_GET['msg2']).''?strip_tags($_GET['msg2']):'').'</B></h2></div>

		<center><table><tr><td width="800">';
		
		// Media Sort Option
		$rArray=array("Recently Used" => -1, "Filename" => "0", "Genre"=>8, "Channel"=>10);
		$sortHandle=$dbADO->Execute('select EK_AttributeType_DefaultSort from MediaType WHERE PK_MediaType=43');
		if ($sortHandle) {
			$sortRow=$sortHandle->FetchRow();
			$curOption=$sortRow['EK_AttributeType_DefaultSort'];
		}
		
		$out.='<form id="internet_radio" action="index.php" method="POST" name="internetRadio" >
		<table border="1">
			   <input type="hidden" name="section" value="internetRadio">
			   <tr>
			   <td colspan="4"><b>Add Internet Radio Stations:</b><b>Set Default Sorting for Radio</b></td>
			   </tr>
			   <tr>
			   <td></td>
			   <td>Station Name</td>
			   <td>Genre</td>
			   <td>URL</td>
			   <td align="right" width="200"><select name ="RadiosortOption">';
			   foreach ($rArray as $Option => $val )
			   {
			   		   $out.='<option value="'.$val.'"';
               		   if ($val == $curOption) {
               		   	  $out .= " selected";
               		   }
               		   $out .= '>'.$Option.'</option>';
			   };
		       $out.='</td>
			   </tr>
			   <tr>
			   <td><input type="button" class="button" value="add (manual)" onclick="add_manual()" /></td>
			   <td><input type="text" name="station_name" value="" /></td>
			   <td><span id="Genre">'.$manual_genres.'</span></td>
			   <td><input size="30" type="text" name="stream_url" value="" /></td>
			   <td align="right"><input type="hidden" name="setDefaultSortRadio" value="none" />
			   <input type="button" class="button" value="'.translate('TEXT_DEFAULT_SORT_CONST').'" onclick="sort()"></td>
			   </tr>
			   <tr>
			   <td colspan="5"><input type="button" class="button" value="Auto add based on location" onclick="location.href = \'index.php?section=internetRadioAdd\'" /></td>
			   </tr>
		</table>
		<br>
		  
		<input type="hidden" name="deletepkfile" value="none" />';
		$out.='<br><form title="delete">
		<table border="1">
		<tr>
			   <td colspan="5"  class="button"><b >Current Internet Radio Stations that are in the <img src="http://forum.linuxmce.org/Themes/LinuxMce/images/LinuxMCE_home_automation.png" height="10px"/> Database</b></td>
			   </tr>

			   <tr>
			   <td class="button">Logo</td>
			   <td class="button">Station Name</td>
			   <td class="button">Genre</td>
			   <td class="button">URL</td>
			   <td class="button">Delete</td>
			  
			   </tr>';
		$res=$mediadbADO->Execute('SELECT PK_File, Filename FROM File where EK_Mediatype = 43');
		
		while($row = $res->FetchRow()){
				   $out.='<tr >';
				   
				   $res2=$mediadbADO->Execute("select FK_Attribute from File_Attribute where FK_File = '".$row["PK_File"]."'");
				   while($row2 = $res2->FetchRow()){
							  
							  //Station Name
							  $res3=$mediadbADO->Execute("select Name from Attribute where PK_Attribute = '".$row2["FK_Attribute"]."' and FK_AttributeType=10");
				   			  while($row3 = $res3->FetchRow(0)){
				   			  									 $LName=$row3["Name"];
							  }
							  
							  //Genre
							  $res4=$mediadbADO->query("select Name from Attribute where PK_Attribute = '".$row2["FK_Attribute"]."' and FK_AttributeType=8");
				   			  while($row4 = $res4->FetchRow(0)){
				   			  									 $LGenre=$row4["Name"];
							  }
				   
				   			  //Logo
							  $res5=$mediadbADO->query("select FK_Picture from Picture_Attribute where FK_Attribute = '".$row2["FK_Attribute"]."'");
				   			  while($row5 = $res5->FetchRow()){
				   			  			  					   	 $LLogoN=$row5["FK_Picture"];
							  }
							  
							  //$res6=$mediadbADO->query("select URL from Picture where PK_Picture = '".$LLogoN."'");
				   			  //while($row6 = $res6->FetchRow()){
				   			  //	 			 				     $LLogo=$row6["URL"];
							  //}
							  
							  
							  
								 
				   
				   
				   
				   			  //if (LLogoN != "") {
							  $queryPictures='
							  SELECT * FROM Picture_Attribute
							  INNER JOIN Picture ON Picture_Attribute.FK_Picture=PK_Picture
							  WHERE FK_Attribute=?';
							  $resPictures=$mediadbADO->Execute($queryPictures,$row2["FK_Attribute"]);
							  while($rowPictures=$resPictures->FetchRow()){
							    $LURL='"mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'"';
							  }
							  //}
				   }
				   //URL       		   
				   $out.='<td><IMG class="img-rounded box-shadow--16dp" HEIGHT=50 SRC='.$LURL.'></td>';
				   
				   $out.='<td>'.$LName.'</td>';
				   $out.='<td>'.$LGenre.'</td>';
				   $out.='<td>'.$row["Filename"].'</td>';
				   $out.='<td><input type="checkbox" name="check[]" value="'.$row["PK_File"].'"><input type="button" class="button" value="Delete" onclick="del()" /></td>';
				  
				   $out.='</tr>';
				   $LLogoN='';
				   $LURL='';
		}
		$out.='</form><tr>
		<td colspan="4"> <input type="button" class="button"  value="Select All" onclick="selall(1)" />&nbsp<input type="button" class="button" value="Unselect All" onclick="selall(0)" />&nbsp;
		<input type="button" class="button" value="Delete Selected" onclick="del()" /></td></table></form>
		
		</td></tr></table></center>';

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=internetRadio&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		// Update pluto.conf
 		if(isset($_POST['AddInternetRadio'])){
		//if(@$_POST['AddInternetRadio']=='1'){
						writeConf($accessFile, 'AddInternetRadio',@$AddInternetRadio,0);
					}else{
						writeConf($accessFile, 'AddInternetRadio',@$AddInternetRadio,0);
					}
		
		//add the radio stations by script.
		if ($_POST['action'] == 'add') {
		exec_batch_command('sudo -u root /usr/pluto/bin/radioscript.sh add '.$_POST['stationsUrl'].'');
		header("Location: index.php?section=internetRadio&msg=".translate('TEXT_INTERNETRADIO_STATIONS_UPDATED_CONST'));
		}
		
		//delete radio station. Check if we have ticked the box Govo 23/12/2015 16:47pm

		if ($_POST['action'] == 'del' && isset($_POST['check'])) {
		
				
        //show success if we have ticked and updated database Govo 23/12/2015 16:47pm
		$err  = 'TEXT_INTERNETRADIO_STATIONS_DEL_CONST';
 
 
        #Delete Url from Database  UPDATED delete URL stations when one or more boxes are selected for deletion Govo 23/12/2015 16:45pm 
        $ids = array();
        foreach ($_POST['check'] as $PK_File2) {
        $ids[] = (int)$PK_File2;
        }
        $ids = implode(',',$ids);
        $mediadbADO->Execute("delete from File where PK_File IN ( $ids )");
		
		#Find Hook for Station Name
		$StationName=$mediadbADO->Execute("select b.PK_Attribute from File_Attribute a, Attribute b where a.FK_Attribute = b.PK_Attribute and a.FK_File IN ( $ids ) and b.FK_AttributeType=10");
		while($Sta1 = $StationName->FetchRow()){
				$StationID=$Sta1["PK_Attribute"];
		} 
        #Delete Station Name
		
		
        $mediadbADO->Execute("delete from Attribute where PK_Attribute='".$StationID."' and FK_AttributeType = 10");

        #Delete Hooks
        $mediadbADO->Execute("delete from File_Attribute where FK_File IN ( $ids )");
		
		#Find Picture Hook
        $Pic1=$mediadbADO->Execute("select FK_Picture from Picture_Attribute where FK_Attribute='".$StationID."'");
		while($Pic2 = $Pic1->FetchRow()){
				$PicID=$Pic2["FK_Picture"];
		} 
		#Delete Placeholder for Picture
        $mediadbADO->Execute("delete from Picture where PK_Picture='".$PicID."'");

		#Delete Picture Hooks
        $mediadbADO->Execute("delete from Picture_Attribute where FK_Picture='".$PicID."'");

		header("Location: index.php?section=internetRadio&msg=".translate($err));
		}
		 else if(empty($_POST['check'])){
	
	///else we show error check box not ticked Govo 23/12/2015 16:49pm
	      $err  = 'TEXT_INTERNETRADIO_STATIONS_ERROR_CONST';
		  header("Location: index.php?section=internetRadio&msg2=".translate($err));
   
}
		//add the station maually.
		if ($_POST['action'] == 'add_manual') {
			$station_name=mysql_real_escape_string($_POST['station_name']);
			$stream_url=mysql_real_escape_string($_POST['stream_url']);
			$station_genre=mysql_real_escape_string($_POST['station_genre']);
			$date=date('Y-m-d H:i:s');
			//check of the url exists
			$res=$mediadbADO->Execute("SELECT PK_File FROM File WHERE Filename='".$stream_url."'");
			if ($res->RecordCount() == 0) {
				#Add Url to Database
				$mediadbADO->Execute("INSERT INTO File (EK_MediaType,DateAdded,Filename,Missing,IsDirectory,IsNew) VALUES('43','".$date."','".$stream_url."','0','0','1')");
				$PK_File=$mediadbADO->Insert_ID();
				#Add Station Name to Database
				$mediadbADO->Execute("INSERT INTO Attribute (FK_AttributeType,Name) VALUES(10,'".$station_name."')");
				$FK_Attribute=$mediadbADO->Insert_ID();
				#Hook Station Name to Url
				$mediadbADO->Execute("INSERT INTO File_Attribute (FK_File,FK_Attribute,Track,Section) VALUES('".$PK_File."','".$FK_Attribute."',0,0)");
				#Check If Genre Exists in Database
				$res=$mediadbADO->Execute("SELECT PK_Attribute FROM Attribute WHERE FK_AttributeType=8 and Name='".$station_genre."'");
				if ($res->RecordCount() ==0) {
					#Add Genre to Database
					$mediadbADO->Execute("INSERT INTO Attribute (FK_AttributeType,Name) VALUES(8,'".$station_genre."')");
					$GenreID=$mediadbADO->Insert_ID();
				} else {
					while ($row=$res->FetchRow()){
						$GenreID=$row['PK_Attribute'];
					}
				}
				#Hook Genre to Url
				$mediadbADO->Execute("INSERT INTO File_Attribute (FK_File,FK_Attribute,Track,Section) VALUES('".$PK_File."','".$GenreID."',0,0)");
				header("Location: index.php?section=internetRadio&msg=".translate('TEXT_INTERNETRADIO_STATIONS_UPDATED_CONST'));
			} else {
				header("Location: index.php?section=internetRadio&msg=".translate('TEXT_INTERNETRADIO_STATIONS_NOT_UPDATED_STATION_AL_READY_EXIST_CONST'));
			}
		}
		
		if ($_POST['action'] == 'sort') {
		// Added in query to change default sort
			if ($_POST['RadiosortOption'] == 0)
			{
				$sortVal = 'null';;
			}
			else
			{
				$sortVal = $_POST['RadiosortOption'];
			}
			$dbADO->Execute("UPDATE MediaType SET EK_AttributeType_DefaultSort=? WHERE PK_MediaType=43",$sortVal);
			header("Location: index.php?section=internetRadio&msg='Please Regen and Reload for changes to take effect'");
		}
		
	}
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_INTERNETRADIO_STATIONS_CONST')=>'index.php?section=internetRadio'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->output();
}

function writeConf($accessFile, $variable,$oldValue,$newValue)
{
		//header("Location: index.php?section=internetRadio&error=".$variable.$newValue." $accessFile");
		//exit();

	// include language files
 	includeLangFile('common.lang.php');
	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=internetRadio&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
		exit();
	}
	$oldFile=implode('',$oldFileArray);
	$stringToReplace=$variable.'='.$oldValue;
	if(ereg($stringToReplace,$oldFile)){
		$newFile=str_replace($stringToReplace,$variable.'='.$newValue,$oldFile);
	}
	else
		$newFile=$oldFile.$variable.'='.$newValue."\n";
	if(!is_writable($accessFile)){
		header("Location: index.php?section=internetRadio&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=internetRadio&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}
?>
