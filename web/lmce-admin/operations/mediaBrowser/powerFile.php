<?
function powerFile($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');

	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$powerFileID=(int)@$_REQUEST['powerFile'];
	$powerFiles=getAssocArray('Device','PK_Device','Description',$dbADO,'WHERE FK_DeviceTemplate='.$GLOBALS['PowerfileC200'].' AND FK_Installation='.$_SESSION['installationID']);	
	if($powerFileID==0){
		$powerFileIDs=array_keys($powerFiles);
		$powerFileID=@$powerFileIDs[0];
	}	
	$slotStatus=getSlotStatus($powerFileID,$dbADO,(int)@$_REQUEST['forced']);
	$rip_format_array=get_rip_format($_SESSION['installationID'],$dbADO);
	//$mediaPluginID=$rip_format_array['mediaPluginID'];

	if($action=='form'){
		// update rip format if it was changed
		$_SESSION['rip_format']=(!isset($_SESSION['rip_format']))?'flac':$_SESSION['rip_format'];
		
		if(isset($_POST['rip_format'])){
			$_SESSION['rip_format']=$_POST['rip_format'];
		}
		
				
	$scriptInHead='
		<script>
		var slots_checked=0;
		function selAllCheckboxes()
		{
		   val=(document.powerFile.sel_all.checked)?true:false;
		   for (i = 0; i < powerFile.elements.length; i++)
		   {
		     if (powerFile.elements[i].type == "checkbox" && powerFile.elements[i].disabled!=true)
		     {
		         powerFile.elements[i].checked = val;
				 if(val==true){
					slots_checked++;
				 }else{
				 	slots_checked--;
				 }
		     }
		   }
		}
	
		function validateForm(){
			if(slots_checked==0){
				alert("Please select at least a slot.");
				return false;
			}
	
			return true;
		}
	
		function update_count(val){
			eval(\'selected_ckb=document.getElementById("slot_\'+val+\'")\');
			if(selected_ckb.disabled==false){
				slots_checked=(selected_ckb.checked)?slots_checked+1:slots_checked-1
			}
		}
		</script>';		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<h3 align="center">Jukebox</h3>
		<form action="index.php" method="POST" name="powerFile" onSubmit="return validateForm();">
			<input type="hidden" name="section" value="powerFile">
			<input type="hidden" name="action" value="rip">
		
			<table align="center" width="600">
				<tr>
					<td width="150" align="right">'.$TEXT_CHOOSE_JUKEBOX_CONST.':</td>
					<td>'.pulldownFromArray($powerFiles,'powerFile',$powerFileID,'onChange="document.powerFile.action.value=\'form\';document.powerFile.submit();"','key','').'</td>
					<td width="100" align="right">'.$TEXT_CHOOSE_FORMAT_CONST.':</td>
					<td align="left">'.pulldownFromArray($rip_format_array['formats'],'rip_format',@$_SESSION['rip_format'],'onChange="document.powerFile.action.value=\'form\';document.powerFile.submit();"','key','').'</td>
				</tr>
				<tr>
					<td colspan="4">'.$TEXT_RIP_DISCS_INFO_CONST.'</td>
				</tr>		
			</table><br>
			
		
			'.showPowerFileSlots($slotStatus,$powerFileID,$mediadbADO).'
		</form>';
	}elseif($action=='rip'){
		$rip_all=(int)@$_REQUEST['rip_all'];
		if($rip_all==1){
			$out=rip_all_traks($powerFileID,$mediadbADO);
		}elseif(!isset($_REQUEST['rip_selected_tracks'])){
			$out=select_tracks_to_rip($powerFileID,$mediadbADO);
		}else{
			$out=rip_selected_traks($powerFileID);
		}
		
	}elseif($action=='monitor'){
		$out='
		<B>Monitor status</B><br>
		<iframe src="index.php?section=ripStatus&pf='.$powerFileID.'" style="width:800px;height:600px;border:0;scrollbars=1;"></iframe>
		';
		
	}else{
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=powerFile'.$suffix);
		exit();
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_POWERFILE_CONST);

	$output->setScriptInHead(@$scriptInHead);	
	$output->setNavigationMenu(array($TEXT_POWERFILE_CONST=>'index.php?section=powerFile'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_POWERFILE_CONST);
	$output->output();
}

function getSlotStatus($powerFileID,$dbADO,$forced=0){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
		
	$forcedParm=($forced==0)?'':' 21 1';
	
	// get powerfile status
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 703'.$forcedParm;
	exec($cmd,$retArray);

	foreach ($retArray AS $line){
		if(strpos($line,'153:')!==false){
			$status=str_replace('153:','',$line);
		}
	}

// todo: remove hardcoded test
// $status='D0=Full-2,D1=Empty,S1=Full,S2=Empty,S3=Full,S4=Full,S5=Full,S6=Full,S7=Full,S8=Full,S9=Full,S10=Full,S11=Full,S12=Full,S13=Full,S14=Full,S15=Full,S16=Full,S17=Full,S18=Full,S19=Full,S20=Full,S21=Full,S22=Full,S23=Full,S24=Full,S25=Full,S26=Full,S27=Full,S28=Full,S29=Full,S30=Full,S31=Full,S32=Full,S33=Full,S34=Full,S35=Full,S36=Full,S37=Full,S38=Full,S39=Full,S40=Full,S41=Full,S42=Full,S43=Full,S44=Full,S45=Full,S46=Full,S47=Full,S48=Full,S49=Full,S50=Full,S51=Full,S52=Full,S53=Full,S54=Full,S55=Full,S56=Full,S57=Full,S58=Full,S59=Full,S60=Full,S61=Full,S62=Full,S63=Full,S64=Full,S65=Full,S66=Full,S67=Full,S68=Full,S69=Full,S70=Full,S71=Full,S72=Full,S73=Full,S74=Full,S75=Full,S76=Full,S77=Full,S78=Full,S79=Full,S80=Full,S81=Full,S82=Full,S83=Full,S84=Full,S85=Full,S86=Full,S87=Full,S88=Full,S89=Full,S90=Full,S91=Full,S92=Full,S93=Full,S94=Full,S95=Full,S96=Full,S97=Full,S98=Full,S99=Full,S100=Full,S101=Full,S102=Full,S103=Full,S104=Full,S105=Full,S106=Full,S107=Full,S108=Full,S109=Full,S110=Full,S111=Full,S112=Full,S113=Full,S114=Full,S115=Full,S116=Full,S117=Full,S118=Full,S119=Full,S120=Full,S121=Full,S122=Full,S123=Full,S124=Full,S125=Full,S126=Full,S127=Full,S128=Full,S129=Full,S130=Full,S131=Full,S132=Full,S133=Full,S134=Full,S135=Full,S136=Full,S137=Full,S138=Full,S139=Full,S140=Full,S141=Full,S142=Full,S143=Full,S144=Full,S145=Full,S146=Full,S147=Full,S148=Full,S149=Full,S150=Full,S151=Full,S152=Full,S153=Empty,S154=Empty,S155=Empty,S156=Empty,S157=Empty,S158=Empty,S159=Empty,S160=Empty,S161=Empty,S162=Empty,S163=Empty,S164=Empty,S165=Empty,S166=Empty,S167=Empty,S168=Empty,S169=Empty,S170=Empty,S171=Empty,S172=Empty,S173=Empty,S174=Empty,S175=Empty,S176=Empty,S177=Empty,S178=Empty,S179=Empty,S180=Empty,S181=Empty,S182=Empty,S183=Empty,S184=Empty,S185=Empty,S186=Empty,S187=Empty,S188=Empty,S189=Empty,S190=Empty,S191=Empty,S192=Empty,S193=Empty,S194=Empty,S195=Empty,S196=Empty,S197=Empty,S198=Empty,S199=Empty,S200=Empty';
	if(!isset($status)){
		error_redirect($TEXT_ERROR_CANNOT_RETRIEVE_STATUS_CONST,'index.php?section=powerFile');
	}

	$arr=explode(',',$status);
	$statArray=array();
	foreach ($arr AS $item){
		if(strpos($item,'D0')!==false || strpos($item,'D1')!==false || strpos($item,'D2')!==false){
			array_shift($arr);
		}else{
			$parts=explode('=',$item);
			$statArray[$parts[0]]=$parts[1];
		}
	}

	return $statArray;
}

function showPowerFileSlots($slotStatus,$powerFileID,$mediadbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
	
	// check if ripping is in progress and return HTML block to display it
	if(ripInProgress($powerFileID)>0){
		return ripInProgressHTML($powerFileID);
	}
	
	$slotInfo=get_slots_info($powerFileID,$mediadbADO);
	$alreadyRipped=get_already_ripped($powerFileID,$mediadbADO);

	$table1='<table cellpadding="0" cellspacing="0">';
	$table2='<table cellpadding="0" cellspacing="0">';
	$table3='<table cellpadding="0" cellspacing="0">';
	$table4='<table cellpadding="0" cellspacing="0">';	
	for($i=1;$i<=200;$i++){
		$checkbox_stat='';
		$checkbox_value=1;
		if($slotStatus['S'.$i]=='Full'){
			if(isset($slotInfo[$i]['EK_MediaType'])){
				// there is a record in Disc, check attributes to display label
				
				// EK_MediaType==2 (CD)
				if($slotInfo[$i]['EK_MediaType']==2){
					if(isset($slotInfo[$i]['Performer']) && isset($slotInfo[$i]['Album'])){
						$slotName='CD: '.$slotInfo[$i]['Performer'].' - '.$slotInfo[$i]['Album'];
						$color="green";
						if(isset($alreadyRipped[$i]) && $alreadyRipped[$i]!=''){
							$color="purple";
							$slotName.=' ('.$TEXT_ALREADY_RIPPED_CONST.')';
						}						
					}else{
						$slotName='CD: '.$TEXT_INCOMPLETE_ID_CONST;
						$checkbox_value='2';
						$color="red";
					}
				}elseif($slotInfo[$i]['EK_MediaType']==3){
					// EK_MediaType==2 (DVD)
					if(isset($slotInfo[$i]['Title'])){
						$slotName='DVD: '.$slotInfo[$i]['Title'];
						$color="green";
						if(isset($alreadyRipped[$i]) && $alreadyRipped[$i]!=''){
							$color="purple";
							$slotName.=$TEXT_ALREADY_RIPPED_CONST;
						}
						
					}else{
						$slotName='DVD: '.$TEXT_INCOMPLETE_ID_CONST;
						$checkbox_value='2';
						$color="red";
					}
				}else{
					$slotName=$TEXT_UNKNOWN_MEDIA_TYPE_CONST;
					$checkbox_stat='disabled';
					$color="red";
				}
				
				
			}else{
				// no record in Disc, disc is not identified
				$slotName=$TEXT_NOT_IDENTIFIED_CONST;
				$color="blue";
				$checkbox_value='2';
			}
			$coverArt=(@$slotInfo[$i]['Picture']!='' && file_exists($GLOBALS['mediaPicsPath'].@$slotInfo[$i]['Picture'].'_tn.jpg'))?'<img src="mediapics/'.@$slotInfo[$i]['Picture'].'_tn.jpg" width="30" height="30" align="middle">':'';			
			$status='<a href="index.php?section=discAttributes&slot='.$i.'&pf='.$powerFileID.'"><span style="color:'.$color.';">Slot '.$i.': '.$slotName.'</span></a>';
		}else{
			$coverArt='';
			$color="#9F9F9F";
			$status='<span style="color:'.$color.'">Slot '.$i.': Empty</span>';
		}
		

		
		$color=($i%2==1)?'#F0F3F8':'#FFFFFF';
		$row='
		<tr bgcolor="'.$color.'">
			<td><input type="checkbox" name="slot_'.$i.'" id="slot_'.$i.'" value="'.$checkbox_value.'" '.(($slotStatus['S'.$i]=='Empty')?'disabled':$checkbox_stat).' onClick="update_count('.$i.');"></td>
			<td height="30"><table width="100%" cellpadding="0" cellspacing="0">
				<tr>
					<td>'.$status.'</td>
					<td width="35" align="right">'.$coverArt.'</td>
				</tr>
			</table></td>
		</tr>';
		if($i<=50){
			$table1.=$row;
		}elseif($i<=100){
			$table2.=$row;
		}elseif($i<=150){
			$table3.=$row;
		}else{
			$table4.=$row;
		}
	}
	$table1.='</table>';
	$table2.='</table>';
	$table3.='</table>';
	$table4.='</table>';

	$out='
	<div align="center"><input type="checkbox" name="rip_all" value="1" checked>Rip all traks</div>
	<table align="center">
		<tr>
			<td valign="top">'.$table1.'</td>
			<td valign="top">'.$table2.'</td>
			<td valign="top">'.$table3.'</td>
			<td valign="top">'.$table4.'</td>
		</tr>
		<tr>
			<td colspan="4"><input type="checkbox" name="sel_all" value="1" onClick="selAllCheckboxes()"> Check/uncheck all </td>
		</tr>
		<tr>
			<td colspan="4" align="center">	
				<input type="submit" class="button" name="rip_public" value="'.$TEXT_RIP_SELECTED_AS_PUBLIC.'"> 
				<input type="submit" class="button" name="rip_private" value="'.$TEXT_RIP_SELECTED_AS_PRIVATE_CONST.'"> 
				<input type="button" class="button" name="force_refresh" value="Refresh status" onClick="self.location=\'index.php?section=powerFile&forced=1\'">
				<input type="submit" class="button" name="mass_identify" value="Identify selected">
				<input type="button" class="button" name="monitor" value="Monitor progress" onClick="self.location=\'index.php?section=powerFile&action=monitor\'">
			</td>
		</tr>	
	</table>';
	
	return $out;
}

// value=1 mean identified slots, 2 they are not identified
function get_checked_slots($value=0){
	$checkedSlots=array();
	for($i=1;$i<201;$i++){
		if(isset($_REQUEST['slot_'.$i]) && ($_REQUEST['slot_'.$i]==$value || $value==0)){
			$checkedSlots[]=$i;
		}
	}
	
	return $checkedSlots;
}



function ripInProgressHTML($powerFileID){
	$out='<br>
	<table align="center">
		<tr>
			<td><span class="err"><B>You cannot start a new job right now</B></span> because a job is still active, click <a href="index.php?section=powerFile&pf='.$powerFileID.'&action=monitor">Monitor Progress</a> to view it or cancel it</td>
		</tr>
	</table>';
	
	return $out;
}

function ripInProgress($powerFileID){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
	
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 739';
	exec($cmd,$retArray);

	// todo: remove hardcoded
	 // $retArray=array(0=>'OK:',1=>'158:S1-S~S2-F de aia~S5-R bubu~S10-I cucu~S100-N');
	
	foreach ($retArray AS $line){
		if(strpos($line,'158:')!==false){
			$status=str_replace('158:','',$line);
		}
	}

	if(!isset($status)){
		error_redirect($TEXT_ERROR_CANNOT_RETRIEVE_RIP_STATUS_CONST.join('<br>',$retArray),'index.php?section=powerFile');
	}	
	$active=0;
	if($status==''){
		// Status: idle.
		return 0;
	}	
	$slotStatusArray=explode('~',$status);

	foreach ($slotStatusArray AS $slot){
		$parts=explode('-',$slot);
		switch (substr($parts[1],0,1)){
			case 'S';
				// completed
			break;
			case 'F';
				// failed - 
			break;
			case 'R';
				// ripping
				$active++;
			break;
			case 'N';
				// pending
				$active++;
			break;
			case 'I';
				// identifying
				$active++;
			break;
		}
	}
	return $active;
}

function get_already_ripped($powerFileID,$mediadbADO){
	$res=$mediadbADO->Execute('
		SELECT Disc.*,File_Attribute.FK_File,Attribute.FK_AttributeType,Attribute.Name
		FROM Disc
		LEFT JOIN Disc_Attribute ON FK_Disc=PK_Disc AND Disc_Attribute.Section=0 AND Disc_Attribute.Track=0
		LEFT JOIN Attribute ON Disc_Attribute.FK_Attribute=PK_Attribute
		LEFT JOIN File_Attribute ON File_Attribute.FK_Attribute=PK_Attribute AND File_Attribute.Track=0 AND File_Attribute.Section=0
		WHERE (FK_AttributeType IN (2,3,4,13,14) OR FK_AttributeType IS NULL) AND EK_Device=?
		ORDER BY Slot',$powerFileID);
	$ripped=array();
	while($row=$res->FetchRow()){
		if((!is_null($row['FK_File']) && $row['FK_AttributeType']==3) || (!is_null($row['FK_File']) && $row['FK_AttributeType']==4)){
			$ripped[$row['Slot']]=$row['FK_File'];
		}
	}

	return $ripped;
}

// ripp all traks for selected checkboxes
function rip_all_traks($powerFileID){
	$checkedSlots=get_checked_slots();
	$unindentifiedSlots=get_checked_slots(2);
	
	$rip_format=$_POST['rip_format'];
	
	if(isset($_POST['mass_identify'])){
		// mass identity command
		$ripParms=(isset($_REQUEST['sel_all']) && $_REQUEST['sel_all']==1)?'*':join(',',$checkedSlots);		
		$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 740 157 "'.$ripParms.'"';
	}else{
		// mass ripping command
		$ripParms=(isset($_REQUEST['sel_all']) && $_REQUEST['sel_all']==1)?'*':join(',',array_diff($checkedSlots,$unindentifiedSlots));

		if(isset($_POST['rip_private'])){
			$privateParm=' 17 '.$_SESSION['userID'];
		}
		$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 720 20 "'.$rip_format.'" 157 '.$ripParms.@$privateParm;
	}
	exec($cmd);
	$out='
		<B>Riping status</B><br>
		<iframe src="index.php?section=ripStatus&pf='.$powerFileID.'" style="width:800px;height:600px;border:0;scrollbars=1;"></iframe>
		';
	
	return $out;
}

function select_tracks_to_rip($powerFileID,$mediadbADO){
	$checkedSlots=get_checked_slots(1);
	$slotsInfo=get_slots_info($powerFileID,$mediadbADO);
	$tracksInfo=get_tracks($powerFileID,$mediadbADO);

	
	$out='
		<script>

		function selAllCheckboxes(slot)
		{
		   eval("val=(document.powerFile.select_all_"+slot+".checked)?true:false");

		   for (i = 0; i < powerFile.elements.length; i++)
		   {
		     if (powerFile.elements[i].type == "checkbox" && powerFile.elements[i].disabled!=true && powerFile.elements[i].value==slot)
		     {
		         powerFile.elements[i].checked = val;
		     }
		   }
		}
	
		</script>
	
		<form action="index.php" method="POST" name="powerFile">
			<input type="hidden" name="section" value="powerFile">
			<input type="hidden" name="action" value="rip">
			<input type="hidden" name="rip_format" value="'.$_SESSION['rip_format'].'">
	<table>';
	foreach ($checkedSlots AS $slot){
		$media=($slotsInfo[$slot]['EK_MediaType']==2)?'CD':'DVD';
		$disc=(isset($slotsInfo[$slot]['PK_Disc']))?$slotsInfo[$slot]['Performer'].' - '.$slotsInfo[$slot]['Album']:$slotsInfo[$slot]['Title'];			
		$out.='
		<tr>
			<td><B>Slot '.$slot.' - '.$media.': '.$disc.'</B></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="select_all_'.$slot.'" value="1" checked onclick="selAllCheckboxes('.$slot.')"> Select/Unselect all tracks</td>
		</tr>
		<tr>
			<td>'.format_tracks($slot,@$tracksInfo[$slot]).'</td>
		</tr>	
		<input type="hidden" name="tracks_'.$slot.'" value="'.@join(',',@array_keys($tracksInfo[$slot])).'">	
		';
	}
	$out.='
		<tr>
			<td colspan="2" align="center">
				<input type="submit" class="button" name="rip_public" value="'.$TEXT_RIP_SELECTED_AS_PUBLIC_CONST.'"> 
				<input type="submit" class="button" name="rip_private" value="'.$TEXT_RIP_SELECTED_AS_PRIVATE_CONST.'"> 
			</td>
		<tr>	
	</table>
	<input type="hidden" name="checkedSlots" value="'.join(',',$checkedSlots).'">
	<input type="hidden" name="rip_selected_tracks" value="1">
	</form>';
	
	return $out;
}

// retrive all tracks info for selected 
function get_tracks($powerFileID,$mediadbADO){
	$res=$mediadbADO->Execute('
		SELECT Attribute.*,AttributeType.Description AS atype, Disc.Slot,Track 
		FROM Disc_Attribute
		INNER JOIN Disc ON FK_Disc=PK_Disc
		INNER JOIN Attribute ON FK_Attribute=PK_Attribute
		INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
		WHERE Track!=0 AND EK_Device=?',$powerFileID);
	$tracksInfo=array();
	while($row=$res->FetchRow()){
		$tracksInfo[$row['Slot']][$row['Track']][$row['atype']]=$row['Name'];
	}
		
	return $tracksInfo;
}

function format_tracks($slot,$tracksInfo){
	if(count($tracksInfo)==0){
		return '&nbsp;';
	}
	$out='<table>';
	foreach ($tracksInfo AS $track=>$info){
		$out.='
		<tr>
			<td><input type="checkbox" name="track_'.$slot.'_'.$track.'" value="'.$slot.'" checked> Track '.$track.': '.$info['Song'].' - '.$info['Performer'].'</td>
		</tr>';
	}
	$out.='
	</table>
	';
	
	return $out;
}

// ripp selected traks for selected checkboxes
function rip_selected_traks($powerFileID){
	$rip_format=$_POST['rip_format'];
	
	$checkedSlots=explode(',',$_REQUEST['checkedSlots']);
	$ripParms='';
	foreach ($checkedSlots AS $slot){
		$ripParms.=','.$slot;
		$tracks=explode(',',$_REQUEST['tracks_'.$slot]);
		
		$selectedTracks=array();
		foreach ($tracks AS $track){
			if(isset($_REQUEST['track_'.$slot.'_'.$track])){
				$selectedTracks[]=$track;
			}
		}
		if(count($selectedTracks)>0){
			$ripParms.='|'.join('|',$selectedTracks);
		}
	}
	$ripParms=substr($ripParms,1);
	
	if(isset($_POST['rip_private'])){
		$privateParm=' 17 '.$_SESSION['userID'];
	}
	
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 720 20 '.$rip_format.' 157 "'.$ripParms.'"'.@$privateParm;

	exec($cmd);
	$out='
		<B>'.$TEXT_RIPING_STATUS_CONST.'</B><br>
		<iframe src="index.php?section=ripStatus&pf='.$powerFileID.'" style="width:800px;height:600px;border:0;scrollbars=1;"></iframe>
		';
	
	return $out;
}


?>