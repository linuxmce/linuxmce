<?PHP
function internet_radio_add($output,$mediadbADO,$dbADO) {
		// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('internetradio.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
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

		if (($_POST['country'] == "http://opml.radiotime.com/Browse.ashx?id=r100436" || $_POST['country'] == "http://opml.radiotime.com/Browse.ashx?id=r101227") && $_POST['region'] != "" ) {
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
			

				
		</script>
		
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		
		<input type="button" class="button" value="Back" onclick="location.href = \'index.php?section=internetRadio\'" />
		
		<center><table><tr><td width="800">';
		
		
		
		$out.='<form id="internet_radio" action="index.php" method="POST" name="internetRadioAdd" >
		<input type="hidden" name="section" value="internetRadioAdd">
		<table border="1">
		    <tr>
			   <td colspan="4" align="left"><legend><b>Automaticly Add Internet Radio Stations based on location:</b></legend></td>
			</tr>
			<tr>
				<td width="250">Continent:</td>
				<td width="250">Country:</td>
				<td width="250">region:</td>
		    </tr>
			<tr>
				<td>'.$continent.'</td>
				<td>'.$country.'</td>
				<td>'.$region.'<br>'.$city.'</td>
				<td><input type="button" class="button" value="add (all)" onclick="add()" /></td>
			</tr>	
			<tr>
				<td colspan="4">Stations:<br>'.$stations.'</td>
			</tr>
			</table>
				</td></tr></table></center>';

	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=internetRadioAdd&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		//add the radio stations by script.
		if ($_POST['action'] == 'add') {
		exec_batch_command('sudo -u root /usr/pluto/bin/radioscript.sh add '.$_POST['stationsUrl'].'');
		header("Location: index.php?section=internetRadio&msg=".translate('TEXT_INTERNETRADIO_STATIONS_UPDATED_CONST'));
		}
		
				
	}
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_INTERNETRADIO_STATIONS_CONST')=>'index.php?section=internetRadioAdd'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_INTERNETRADIO_STATIONS_CONST'));
	$output->output();
}

?>