<?php
function setTimezone($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/installationSettings.lang.php');
	
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$countriesFileArray=file('/usr/share/zoneinfo/iso3166.tab');
	$zonesFileArray=file('/usr/share/zoneinfo/zone.tab');
	$continentsArray=array('Africa', 'America', 'Antarctica', 'Arctic [Ocean]', 'Asia', 'Atlantic [Ocean]', 'Australia', 'Europe', 'Indian [Ocean]', 'Pacific [Ocean]');
	//$currentTimeZone=trim(implode('',file('/etc/timezone')));
	$currentTimeZone=str_replace('/usr/share/zoneinfo/','',exec('readlink /etc/localtime'));
	
	$countriesArray=array();
	foreach ($countriesFileArray AS $line){
		if($line[0]!='#'){
			$parts=explode("\t",$line);
			$countriesArray[$parts[0]]=trim($parts[1]);
		}
	}
	
	$zonesArray=array();
	foreach ($zonesFileArray AS $line){
		if($line[0]!='#'){
			$parts=explode("\t",$line);
			$tz=explode('/',$parts[2]);
			$zonesArray[$tz[0]][$countriesArray[$parts[0]]][]=array($parts[2],@$parts[3]);
			if(trim($parts[2])==$currentTimeZone){
				$selContinent=$tz[0];
				$selCountry=$countriesArray[$parts[0]];
				$oldTimeZone=array($currentTimeZone,(isset($parts[3])?$parts[3]:$currentTimeZone));
			}
		}
	}
	$selContinent=(isset($_POST['continent']))?$_POST['continent']:'';	
	$selCountry=(isset($_POST['country']))?$_POST['country']:'';
	
	if ($action=='form') {		
		$out.=setLeftMenu($dbADO).'
		
		
		<form action="index.php" method="post" name="setTimezone">
		<input type="hidden" name="section" value="setTimezone">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="timeZoneText" value="update">
		<input type="hidden" name="oldTimeZone" value="'.$oldTimeZone.'">
		
		<h3>'.$TEXT_SET_TIMEZONE_CONST.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<table cellpadding="3" align="center" border="0">
			<tr>
				<td colspan="2" align="center" class="tablehead">Current timezone <B>'.$oldTimeZone[1].'</B></td>
			</tr>
			<tr>
				<td><B>'.$TEXT_CONTINENT_CONST.'</B></td>
				<td><select name="continent" onChange="document.setTimezone.action.value=\'form\';document.setTimezone.submit();">
					<option value="">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';

		foreach ($continentsArray AS $continent){
			$cleanContinent=preg_replace('/ \[(\w+)\]/','',$continent);
			$out.='<option value="'.$cleanContinent.'" '.(($cleanContinent==$selContinent)?'selected':'').'>'.$continent.'</option>';
		}
		$out.='
				</select></td>
			</tr>';
		if($selContinent!=''){
			$out.='
			<tr>
				<td><B>'.$TEXT_COUNTRY_CONST.'</B></td>
				<td>';
			if(@count(array_keys($zonesArray[$selContinent]))==1){
				$tmpArray=array_keys($zonesArray[$selContinent]);
				$out.=$tmpArray[0];
				$singleCountry=1;
				$selCountry=$tmpArray[0];
			}else{
				$out.='
				<select name="country" onChange="document.setTimezone.action.value=\'form\';document.setTimezone.submit();">
					<option value="">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
				foreach (@array_keys($zonesArray[$selContinent]) AS $country){
					$out.='<option value="'.$country.'" '.(($country==@$selCountry)?'selected':'').'>'.$country.'</option>';
				}
				$out.='
					</select>';
			}
			$out.='
				</td>
			</tr>';
			if($selCountry!='' && isset($zonesArray[$selContinent][$selCountry])){
				$out.='
				<tr>
					<td><B>'.$TEXT_ZONE_CONST.'</B></td>
					<td>';
				if(count($zonesArray[$selContinent][$selCountry])==1){
					$tmpArray=$zonesArray[$selContinent][$selCountry][0];
					$timeZoneLabel=((@$tmpArray[1]!='')?$tmpArray[1]:$tmpArray[0]);
					$out.=$timeZoneLabel;
					$timeZone=array($tmpArray[0],$timeZoneLabel);
				}else{
					$out.='
						<select name="zone" onChange="document.setTimezone.timeZoneText.value=escape(document.setTimezone.zone[document.setTimezone.zone.selectedIndex].text);document.setTimezone.action.value=\'form\';document.setTimezone.submit();">
							<option value="">- Please select -</option>';
					$orderedZonesArray=$zonesArray[$selContinent][$selCountry];
					asort($orderedZonesArray);
					foreach ($orderedZonesArray AS $zone){
						$out.='<option value="'.$zone[0].'" '.(($zone[0]==@$_POST['zone'])?'selected':'').'>'.str_replace($selContinent.'/','',$zone[0]).' - '.(($zone[1]!='')?$zone[1]:$zone[0]).'</option>';
					}
					$out.='
						</select>';
				}
				$out.='	
					</td>
				</tr>';
			}
			if(isset($_POST['zone']) && $_POST['zone']!=''){
				$timeZone=array($_POST['zone'],urldecode($_POST['timeZoneText']));
			} 
			if(isset($timeZone)){
				
				$out.='
				<tr>
					<td colspan="2" align="center">'.$TEXT_TIMEZONE_SELECTED_CONST.': <B>'.$timeZone[1].'</B></td>
				</tr>
				<input type="hidden" name="newTimeZone" value="'.$timeZone[0].'"> 
				';
			}
			
		}
		$out.='
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  > <input type="reset" class="button" name="closeBtn" value="'.$TEXT_CLOSE_CONST.'" onClick="self.close();"></td>
			</tr>
		</table>
		</form>
		
		<script>
		 	var frmvalidator = new formValidator("setTimezone");
		</script>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setTimezone&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		

		
		$newTimeZone=$_POST['newTimeZone'];
		$oldTimeZone=$_POST['oldTimeZone'];
		if($newTimeZone!=$oldTimeZone){
			$cmdToSend='sudo -u root /usr/pluto/bin/SetTimeZone.sh '.trim($newTimeZone);
			//echo $cmdToSend;
			exec_batch_command($cmdToSend);	
		}
			
		$suffix=(isset($err))?'&error='.$err:'';
		$out='
		<script>
				opener.location.reload();
			 	self.location="index.php?section=setTimezone&from='.$from.'&msg='.$TEXT_TIMEZONE_UPDATED_CONST.$suffix.'";
		</script>';
		die($out);
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SET_TIMEZONE_CONST);			
	$output->output();
}
?>
