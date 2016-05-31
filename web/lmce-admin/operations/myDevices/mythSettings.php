<?php
function mythSettings($output,$dbADO) {

	error_reporting(E_ALL );

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mythSettings.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $mythADO ADOConnection */
/* @var $rs ADORecordSet */
	global 	$dbMythType, $dbMythUser, $dbMythPass, $dbMythServer, $dbMythDatabase;
	global $externalCommand;
	
	$externalCommand='/usr/pluto/bin/TuneToChannel.sh ';
	
  	$mythADO= &ADONewConnection('mysqli');
  	$mythADO->NConnect($dbMythServer,urlencode($dbMythUser),urlencode($dbMythPass),urlencode($dbMythDatabase));
	$plutomythADO= &ADONewConnection('mysqli');
  	$plutomythADO->NConnect($dbMythServer,urlencode($dbMythUser),urlencode($dbMythPass),urlencode('pluto_myth'));
	
  	
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	
	
	$query = "
		SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version 
		WHERE 
		PK_Installation = ?
	";
	$selectInstallation = $dbADO->Execute($query,array($installationID));
	
	$zipCode = '';
	if ($selectInstallation) {
		$rowInstallation = $selectInstallation->FetchRow();		
		$zipCode = $rowInstallation['Zip'];
	}

	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$mythInputs=getMythInputs($mythADO);
	$sids = getMythQAM($mythADO);
	
	if ($action == 'form') {
		
		
		$out.=setLeftMenu($dbADO).'
		
		<div class="err">'.(isset($_GET['error'])?stripslashes($_GET['error']):'').'</div>
		<div class="confirm"><B>'.(isset($_GET['msg'])?stripslashes($_GET['msg']):'').'</B></div>
			
	<form method="post" action="index.php" name="mythSettings">
		<input type="hidden" name="section" value="mythSettings">
		<input type="hidden" name="action" value="update">';
		/*
	$out.='<table cellspacing="0" cellpadding="3">
		<tr bgcolor="lightblue">
			<td align="center"><B>'.$TEXT_INPUT_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_PICK_THE_TUNING_DEVICE_CONST.'</B></td>
		</tr>';
	
		$tunningDevices=getDevices(array($GLOBALS['rootAVEquipment'],$GLOBALS['TVdevices'],$GLOBALS['SatelliteBoxes'],$GLOBALS['CableBoxes']),$dbADO);
		
		for ($i=0;$i<count($mythInputs);$i++){
			$altColor=($i%2==0)?'#F0F3F8':'#FFFFFF';
			$out.='
				<tr bgcolor="'.$altColor.'">
					<td>'.$mythInputs['name'][$i].'</td>
					<td>'.pulldownFromArray($tunningDevices,'input_'.$mythInputs['cardinputid'][$i],$mythInputs['device'][$i]).'</td>
				</tr>
			';
		}
		$out.='<tr><td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td></tr></table>';
		*/
		if (count($sids)>0)
		{
			$MDArray=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
			reset($MDArray);
			list($mdk, $mdv) = each($MDArray);

			$out.='<br>'.$TEXT_MYTH_PICK_MD_CONST.'&nbsp;'.pulldownFromArray($MDArray,'OutputMD', $mdk).'<br>';	
			
			
			
			for($i=0;$i<count($sids);$i++)
			{
				if ($zipCode == '')
				{
					$out.='<br><b>'.$TEXT_MYTH_INPUT_ZIP_CONST.'</b><br>';		
				}
				else
				{
					$templates = getTemplates($plutomythADO, $zipCode);
					if (count($templates) > 1)
					{
						$out.='<br>'.$TEXT_MYTH_IMPORT_MULTI_LINEUP_CONST.'<br>';
						
						$out.=pulldownFromArray($templates,'templatepd_'.$sids[$i],'').'&nbsp;<input type="submit" class="button" name="import_lineup_'.$sids[$i].'" value="'.$TEXT_MYTH_IMPORT_CONST.'"><br>';					
						$out.='<input type="checkbox" name="remove_unknown_'.$sids[$i].'" checked value="1">'.$TEXT_MYTH_REMOVE_UNKNOWN_CHANNELS.'<br>';

					}
					else if (count($templates) == 1)
					{
						reset($templates);
						list($mdk, $mdv) = each($templates);
						$out.='<br>'.$TEXT_MYTH_IMPORT_ONE_LINEUP_CONST.'<br>';
						$out.=$mdv.'&nbsp;<input type="submit" class="button" name="import_lineup_'.$sids[$i].'" value="'.$TEXT_MYTH_IMPORT_CONST.'"><br>';
						$out.='<input type="checkbox" name="remove_unknown_'.$sids[$i].'" checked value="1">'.$TEXT_MYTH_REMOVE_UNKNOWN_CHANNELS_CONST.'<br>';
						$out.='<input type="hidden" name="providerid_'.$sids[$i].'" value="'.$mdk.'">';
					}
					else
					{
						$out.='<br>'.$TEXT_MYTH_NOIMPORT_CONST.$zipCode.'<br>';
					}				
				}
				$UnassignedChannels = getMythQAMUnassignedChannels($mythADO, $sids[$i]);
				$UnmappedStations = getMythQAMUnmappedEPGChannels($mythADO, $sids[$i]);
				if (count($UnassignedChannels))
				{
					if (count($UnmappedStations))
					{						
						$out.='<br>'.$TEXT_MYTH_UNASSIGNED_CABLE_CONST.'<table cellspacing="0" cellpadding="3"><tr class="tablehead"><td>'.$TEXT_MYTH_CHANNEL_CONST.'</td><td>'.$TEXT_MYTH_STATION_CONST.'</td><td></td>';
						for($j=0;$j<count($UnassignedChannels);$j++)
						{		
							$altColor=($j%2==0)?'#F0F3F8':'#FFFFFF';
							$out.='<tr bgcolor="'.$altColor.'"><td>'.$UnassignedChannels[$j].'</td><td>'.pulldownFromArray($UnmappedStations['name'],'stationpd_'.$UnassignedChannels[$j], '0').'</td>';
							$out.='<td>
							       <input type="submit" class="button" name="chanset_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_MYTH_SET_CONST.'">
							       <input type="submit" class="button" name="mythtune_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_MYTH_TUNE_CONST.'">
							       <input type="submit" class="button" name="remove_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_DELETE_CONST.'"></td>
							       </tr>';
							
						}
						$out.="</table>";				
					}
					else
					{
						$out.='<br>'.$TEXT_MYTH_NOSTATIONS_CONST;
					}
				}
				$AssignedChannels = getMythQAMAssignedChannels($mythADO, $sids[$i]);
				if (count($AssignedChannels))
				{
					$out.='<br>'.$TEXT_MYTH_ASSIGNED_CABLE_CONST.'<table cellspacing="0" cellpadding="3"><tr class="tablehead"><td>'.$TEXT_MYTH_CHANNEL_CONST.'</td><td>'.$TEXT_MYTH_STATION_CONST.'</td><td></td>';
					$j=0;
					foreach($AssignedChannels['channum'] AS $chanID=>$channum)
					{		
						$altColor=($j%2==0)?'#F0F3F8':'#FFFFFF';
						$j++;
						$out.='<tr bgcolor="'.$altColor.'"><td>'.$channum.'</td><td>'.$AssignedChannels['name'][$chanID].'</td>';
						$out.='<td>
						       <input type="submit" class="button" name="mythtune_'.$sids[$i].'_'.$channum.'" value="'.$TEXT_MYTH_TUNE_CONST.'">
						       <input type="submit" class="button" name="remove_'.$sids[$i].'_'.$channum.'" value="'.$TEXT_DELETE_CONST.'"></td>
						       </tr>';						
					}
					$out.='</table>';
					if ($zipCode!='')
					{
						$out.='<br>'.$TEXT_MYTH_PUBLISH_LINEUP_CONST.'<br>';
						$out.='<input type="text" size="40" name="Provider_'.$sids[$i].'" value="">&nbsp;<input type="submit" class="button" name="publish_'.$sids[$i].'" value="'.$TEXT_MYTH_PUBLISH_CONST.'">';
					}
				}
				
			}		
		}
		$out.="</form>";
	} 
	else 
	{
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		if (!$canModifyInstallation) {
			Header('Location: index.php?section=mythSettings&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}
		
		// process update
		for ($i=0;$i<count($mythInputs);$i++){
			$newCommand=((int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]!==0)?$externalCommand.(int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]:NULL;
			$mythADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array($newCommand,@$mythInputs['cardinputid'][$i]));
		}
		if(isset($_POST['reboot_core'])){
			$command='sudo -u root reboot';
			exec($command);
		}
		for($i=0;$i<count($sids);$i++)
		{
			if (isset($_POST['import_lineup_'.$sids[$i]]))
			{
				$templates = getTemplates($plutomythADO, $zipCode);
				$RemoveUnknown = (int)@$_POST['remove_unknown_'.$sids[$i]];

				if (count($templates) > 1)
				{
					$Provider = $_POST['templatepd_'.$sids[$i]];
				}
				else 
				{
					$Provider = $_POST['providerid_'.$sids[$i]];
				}
				$result = importLineup($plutomythADO, $mythADO, $Provider, $zipCode, $RemoveUnknown, $sids[$i]);
				Header('Location: index.php?section=mythSettings&msg='.urlencode($result));
				exit();
			}	
			
			$UnassignedChannels = getMythQAMUnassignedChannels($mythADO, $sids[$i]);
			for($j=0;$j<count($UnassignedChannels);$j++)
			{
				if (isset($_POST['chanset_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					$setto = $_POST['stationpd_'.$UnassignedChannels[$j]];
					assignChannel($mythADO, $UnassignedChannels[$j], $setto);
				} 
				else if (isset($_POST['remove_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					$mythADO->Execute('DELETE from channel WHERE channum=\''.$UnassignedChannels[$j].'\'');
				}
				else if (isset($_POST['mythtune_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					tuneChannel($mythADO, $dbADO, $UnassignedChannels[$j],$sids[$i]);
				}														
			}
			$AssignedChannels = getMythQAMAssignedChannels($mythADO, $sids[$i]);
			foreach($AssignedChannels['channum'] AS $chanID=>$channum)
			{
				if (isset($_POST['remove_'.$sids[$i].'_'.$channum]))
				{
					$mythADO->Execute('DELETE from channel WHERE channum=\''.$channum.'\'');
				}
				else if (isset($_POST['mythtune_'.$sids[$i].'_'.$channum]))
				{
					tuneChannel($mythADO, $dbADO, $channum,$sids[$i]);
				}				
			}
			if (isset($_POST['publish_'.$sids[$i]]))
			{
				publishLineup($plutomythADO, $mythADO, $_POST['Provider_'.$sids[$i]], $AssignedChannels, $zipCode);
			}
			// end process update
		}
		
		Header('Location: index.php?section=mythSettings&msg='.$TEXT_MYTH_SETTINGS_UPDATED_CONST.' '.@$command);
		exit();
	}
	
	
	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_MYTH_SETTINGS_CONST);
	
	$output->setNavigationMenu(array($TEXT_MY_DEVICES_CONST=>'index.php?section=myDevices',$TEXT_MYTH_SETTINGS_CONST=>"index.php?section=mythSettings"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MYTH_SETTINGS_CONST);			
	$output->output();  		
}

function importLineup($plutomythADO, $mythADO, $PK_Provider, $zipCode, $removeunknown, $sid)
{
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mythSettings.lang.php');

	$firstchan = 1;
	$results = "";

	$rs=$plutomythADO->Execute('SELECT Callsign, Frequency, TPID FROM DTV_Channel WHERE FK_Provider = '.$PK_Provider);
	while($row=$rs->FetchRow())
	{
		$rs2=$mythADO->Execute('SELECT chanid,channum FROM channel WHERE callsign=\''.$row['Callsign'].'\'');
		if ($epgrow=$rs2->FetchRow())
		{
			$rs3=$mythADO->Execute('SELECT channum,chanid FROM channel LEFT JOIN dtv_multiplex ON (channel.mplexid=dtv_multiplex.mplexid)  WHERE frequency=\''.$row['Frequency'].'\' AND serviceid='.$row['TPID']);
			if ($uarow=$rs3->FetchRow())
			{
				if($uarow['chanid']!=$epgrow['chanid'])
				{
					assignChannel($mythADO, $uarow['channum'], $epgrow['chanid']);
					if ($firstchan > 0)
					{
						$mythADO->Execute('UPDATE cardinput SET startchan=\''.$epgrow['channum'].'\' WHERE sourceid='.$sid);
						$firstchan = 0;
					}
					$results.=$TEXT_MYTH_IMPSET_ASSIGNED_CONST.' '.$uarow['channum'].': '.$row['Callsign'].' ('.$epgrow['channum'].')<br>';
				}
				else
				{
					if ($firstchan > 0)
					{
						$mythADO->Execute('UPDATE cardinput SET startchan=\''.$epgrow['channum'].'\' WHERE sourceid='.$sid);
						$firstchan = 0;
					}
					$results.=$TEXT_MYTH_IMPSET_SKIPPED_CONST.' '.$uarow['channum'].' ('.$row['Callsign'].')<br>';
				}
			}
			else
			{
				$results.=$TEXT_MYTH_IMPERR_ATTEMPTINGADD_CONST.' '.$row['Callsign'].'. '.$TEXT_MYTH_IMPERR_NOCHANNEL_CONST.'<br>';
			}							
		}
		else
		{
			$results.=$TEXT_MYTH_IMPERR_ATTEMPTINGADD_CONST.' '.$row['Callsign'].'. '.$TEXT_MYTH_IMPERR_NOEPG_CONST.'<br>';
		}			
	}
	if ($firstchan==1)
	{
		$results.=$TEXT_MYTH_IMPSET_NO_SUCCESS.'<br>';
	} 
	else if($removeunknown)
	{
		$mythADO->Execute('DELETE FROM channel WHERE sourceid='.$sid.' AND xmltvid = \'\' AND mplexid <> 0 AND mplexid <> 32767');
	}
	return $results;
}

function publishLineup($plutomythADO, $mythADO, $providerName, $AssignedChannels, $zipCode)
{
	if ($providerName == '')
		return;
	$rs=$plutomythADO->Execute('SELECT PK_Provider FROM PostalCode_Provider LEFT JOIN Provider ON (FK_Provider = PK_Provider) WHERE EK_PostalCode = '.$zipCode.' AND Description=\''.$providerName.'\';');
	$PK_Provider = 0;
	if ($providerrow=$rs->FetchRow())
	{
		// Found an existing entry.  Delete existing DTV entries for it.
		$PK_Provider = $providerrow['PK_Provider'];
		$plutomythADO->Execute('DELETE FROM DTV_Channel WHERE FK_Provider = '.$PK_Provider);
	}
	else
	{
		$plutomythADO->Execute('INSERT INTO Provider (Description, FK_ProviderType) VALUES (\''.$providerName.'\', 2)');
		$PK_Provider = $plutomythADO->Insert_ID();
		$plutomythADO->Execute('INSERT INTO PostalCode_Provider (EK_PostalCode, FK_Provider) VALUES ('.$zipCode.', '.$PK_Provider.')');
	}
	foreach($AssignedChannels['channum'] AS $chanID=>$channum)
	{
		$rs=$mythADO->Execute('SELECT callsign,frequency,modulation,serviceid FROM channel LEFT JOIN dtv_multiplex ON (channel.mplexid=dtv_multiplex.mplexid) WHERE chanid='.$chanID);
		if ($chanrow=$rs->FetchRow())
		{
			$plutomythADO->Execute('INSERT INTO DTV_Channel (FK_Provider, Callsign, Frequency, Modulation, APID, VPID, TPID) VALUES ('.$PK_Provider.',\''.$chanrow['callsign'].'\',\''.$chanrow['frequency'].'\',\''.$chanrow['modulation'].'\',0,0,'.$chanrow['serviceid'].')');
		}
	}
}

function tuneChannel($mythADO, $dbADO, $chan, $sid)
{
	$MythPlayer = getMythPlayerForMD($dbADO, $_POST['OutputMD']);
	if ($MythPlayer > 0)
	{
		$mythADO->Execute('UPDATE cardinput SET startchan=\''.$chan.'\' WHERE sourceid='.$sid);
		$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 2 1 43 2 '.$MythPlayer.' 29 1';
		exec($command);
	}
	else
	{
		Header('Location: index.php?section=mythSettings&msg='.$TEXT_MYTH_NOMYTHPLAYER_CONST.' (MD='.$_POST['OutputMD'].')');
		exit();
	}	
}

function assignChannel($mythADO, $uachannum, $stationchanid)
{
	$err = '';
	
	// First grab the mplexID, serviceid, and atscrcid from the unassigned channel
	
	$rs=$mythADO->Execute('SELECT mplexid, serviceid, atscsrcid FROM channel WHERE channum=\''.$uachannum.'\'');
	if ($uarow=$rs->FetchRow())
	{
		$mythADO->Execute('UPDATE channel SET mplexid='.$uarow['mplexid'].',serviceid='.$uarow['serviceid'].',atscsrcid='.$uarow['atscsrcid'].' WHERE chanid=\''.$stationchanid.'\'');
		$mythADO->Execute('DELETE from channel WHERE channum=\''.$uachannum.'\'');
	}
	else
	{
		$err = 'Could not find channum '.$uachannum;		
	}
	return $err;
}

function getTemplates($plutomythADO, $zipCode)
{
	$templates=array();
	$rs=$plutomythADO->Execute('SELECT PK_Provider, Description from PostalCode_Provider LEFT JOIN Provider ON (FK_Provider = PK_Provider) WHERE EK_PostalCode = '.$zipCode.' AND FK_ProviderType=2');
	while($row=$rs->FetchRow())
	{
		$templates[$row['PK_Provider']]=$row['Description'];
	}
	return $templates;
}

function getMythQAM($mythADO)
{
	$sids=array();
	$rs=$mythADO->Execute('SELECT DISTINCTROW cardinput.sourceid FROM capturecard JOIN cardinput ON (capturecard.cardid = cardinput.cardid) JOIN dtv_multiplex ON (cardinput.sourceid=dtv_multiplex.sourceid) WHERE LEFT(dtv_multiplex.modulation,3) = \'qam\';');
	while($row=$rs->FetchRow())
	{
		$sids[]=$row['sourceid'];
	}
	return $sids;
}

function getMythQAMUnassignedChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT channum FROM channel WHERE sourceid='.$sid.' AND xmltvid = \'\' AND mplexid <> 0 AND mplexid <> 32767');
	while($row=$rs->FetchRow()){
		$channels[]=$row['channum'];
	}
	return $channels;
}

function getMythQAMUnmappedEPGChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT chanid, channum, name FROM channel WHERE sourceid='.$sid.' AND xmltvid <> \'\' AND mplexid=32767');
	while($row=$rs->FetchRow()){
		$channels['chanid'][$row['chanid']]=$row['chanid'];
		$channels['channum'][$row['chanid']]=$row['channum'];
		$channels['name'][$row['chanid']]=$row['name'];
	}
	return $channels;
}

function getMythPlayerForMD($dbADO, $MD)
{
	$rs=$dbADO->Execute('SELECT Device.PK_Device FROM Device LEFT JOIN Device as Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device LEFT JOIN Device as Parent2 ON Parent.FK_Device_ControlledVia=Parent2.PK_Device LEFT JOIN Device as Parent3 ON Parent2.FK_Device_ControlledVia=Parent3.PK_Device WHERE Device.FK_DeviceTemplate=35 AND (Device.FK_Device_ControlledVia='.$MD.' OR Parent.FK_Device_ControlledVia='.$MD.' OR Parent2.FK_Device_ControlledVia='.$MD.' OR Parent3.FK_Device_ControlledVia='.$MD.')');
	if($row=$rs->FetchRow())
		return $row['PK_Device'];
	
	return 0;
}

function getMythQAMAssignedChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT chanid, channum, name FROM channel WHERE sourceid='.$sid.' AND xmltvid <> \'\' AND mplexid<32767');
	while($row=$rs->FetchRow()){
		$channels['channum'][$row['chanid']]=$row['channum'];
		$channels['name'][$row['chanid']]=$row['name'];
	}
	return $channels;
}

function getMythInputs($mythADO)
{
	global $externalCommand;
	
	$inputs=array();
	$rs=$mythADO->Execute('SELECT cardinputid,inputname,externalcommand FROM cardinput ORDER BY inputname ASC');
	while($row=$rs->FetchRow()){
		$inputs['cardinputid'][]=$row['cardinputid'];
		$inputs['name'][]=$row['inputname'];
		$inputs['device'][]=str_replace($externalCommand,'',$row['externalcommand']);
	}
	return $inputs;
}

function getDevices($categories,$dbADO)
{
	$rs=$dbADO->Execute('
		SELECT PK_Device, Device.Description 
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory IN ('.join(',',$categories).')
		ORDER BY Description ASC');
	$devices=array();
	while($row=$rs->FetchRow())
	{
		$devices[$row['PK_Device']]=$row['Description'];
	}
	
	return $devices;
}
?>
