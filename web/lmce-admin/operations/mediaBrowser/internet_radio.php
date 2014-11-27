<?PHP
function internet_radio($output,$mediadbADO,$dbADO) {
		// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('internetradio.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	#define vars what later will be overwritten
	$out='';
	$continet='';
	$city='';
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
				$data.='<input type="button" class="button" value="add" onclick="add()" />';
				foreach( $lines as $line) {
					if (preg_match('/^<outline/', $line)) {
						preg_match('@^(?:<outline type="audio" text=")?([^"]+)@i', $line, $text);
						//preg_match('@^(?:<outline type="audio" text="'.$text['1'].'" URL=")?([^"]+)@i', $line, $URL);
						$data.=$text['1'].'<br />';
					}
					$data.='</p>';
				}
				$data.='<input type="button" class="button" value="add" onclick="add()" />';
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
		if (!isset($_POST['city']) && isset($_POST['country'])) {
			$city=grep_internet_radio_data($_POST['country'],'Dropdown_list','city','');
		} elseif (!isset($_POST['city'])) {
			$stations=grep_internet_radio_data('http://opml.radiotime.com/Browse.ashx?c=local','list','','');
		} elseif (isset($_POST['city'])) {
			$city=grep_internet_radio_data($_POST['country'],'Dropdown_list','city',$_POST['city']);
			$stations=grep_internet_radio_data($_POST['city'],'list','','');
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

			function IRcity()
			{
				document.getElementById(\'internet_radio\').appendChild(input);
				document.getElementById(\'internet_radio\').continent.value;
				document.getElementById(\'internet_radio\').country.value;
				document.getElementById(\'internet_radio\').city.value;
				document.getElementById(\'internet_radio\').submit();
			}

			function add()
			{
				document.getElementById(\'internet_radio\').appendChild(input1);
				document.getElementById(\'internet_radio\').stationsUrl.value;
				document.getElementById(\'internet_radio\').submit();
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
		<form id="internet_radio" action="index.php" method="POST" name="internetRadio" >
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		<table>
			<tr>
				<td></td>
				<td><input type="hidden" name="section" value="internetRadio"><td>
			</tr>
			<tr>
			<td colspan="2">Manual add a station:</td>
			</tr>
			<tr>
				<td>Station name:</td><td><input type="text" name="station_name" value="" /></td>
			</tr>
			<tr>
				<td>Station (stream)Url:</td><td><input type="text" name="stream_url" value="" /></td>
			</tr>
			<tr>
				<td>Station Genre:</td><td><span id="Genre">'.$manual_genres.'</span></td>
			</tr>
			<tr>
				<td colspan="2"><input type="button" class="button" value="add (manual)" onclick="add_manual()" /></td>
			</tr>
			<tr>
				<td>continent:</td><td>'.$continent.'</td>
			</tr>
			<tr>
				<td>Country:</td><td>'.$country.'</td>
			</tr>
			<tr>
				<td>City:</td><td>'.$city.'</td>
			</tr>
			<tr>
				<td colspan="2">Stations:</td>
			</tr>
			</table>
			'.$stations.'
		</form>';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=internetRadio&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		//add the radio stations by script.
		if ($_POST['action'] == 'add') {
		exec_batch_command('sudo -u root /usr/pluto/bin/radioscript.sh add '.$_POST['stationsUrl'].'');
		header("Location: index.php?section=internetRadio&msg=".translate('TEXT_INTERNETRADIO_STATIONS_UPDATED_CONST'));
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
	}
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_INTERNETRADIO_STATIONS_CONST')=>'index.php?section=internetRadio'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->output();
}
?>