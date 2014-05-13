<?php
function connectionWizard($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/connectionWizard.lang.php');

	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'avEquipment';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootAVEquipment'];
	$output->setHelpSrc('/wiki/index.php/AV_Devices');
	
	$_SESSION['AVentertainArea']=(isset($_REQUEST['entertainArea']) && (int)$_REQUEST['entertainArea']!=0)?(int)$_REQUEST['entertainArea']:@$_SESSION['AVentertainArea'];
	$entertainArea=@$_SESSION['AVentertainArea'];

	//$connectorsArray=array(0=>'other.gif',1=>'composite.gif',2=>'svideo.gif',3=>'component.gif',4=>'dvi.gif',5=>'vga.gif',6=>'scart.gif',8=>'hdmi.gif');
	$connectorsArray=getConnectorsArray($dbADO);
	$pipeTypesArray=array(1=>'audio',2=>'video');
	
if ($action == 'form') {
	$output->setScriptAnotherJS('scripts/connectionWizard/connectionWizard.js');
	$out.='
	<script type="text/javascript" src="scripts/connectionWizard/wz_jsgraphics.js"></script>
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="connectionWizard">
	<input type="hidden" name="section" value="connectionWizard">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="devicesCoords" value="">
	<input type="hidden" name="oldEntertainArea" value="'.$entertainArea.'">
		
	<h3>'.$TEXT_AV_CONNECTION_WIZARD_CONST.'</h3>
	<table width="100%">
		<tr>
			<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
		</tr>
	</table>
			
	<table>
		<tr>
			<td valign="top">Edit devices in:'.generatePullDown('entertainArea','EntertainArea','PK_EntertainArea','Description',$_SESSION['AVentertainArea'],$dbADO,' INNER JOIN Room ON FK_Room=PK_Room WHERE FK_Installation='.(int)$_SESSION['installationID'],'onChange="savePositions();"').' '.(($entertainArea!=0)?'<input type="button" class="button" name="button" value="'.$TEXT_ADD_DEVICE_CONST.'" onClick="document.connectionWizard.action.value=\'externalSubmit\';document.connectionWizard.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=connectionWizard&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"> <input type="button" class="button" value="'.$TEXT_UPDATE_CONST.'" onClick="savePositions();">':'').'<br>
			<div align="center"><input type="button" class="button" name="close" onClick="self.close();" value="'.$TEXT_CLOSE_CONST.'"></div></td>
			<td>';
		if($entertainArea!=0){		
		$out.='
			<div style="position:relative; top:0px; background:#DDDDDD;float:right;Z-INDEX:1;"> 
			<table align="left" border="0" width="300">
				<tr>
					<td width="50"><B>'.$TEXT_FROM_CONST.'</B>: </td>
					<td bgcolor="#EEEEEE"> <span id="fromMessage"></span></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_TO_CONST.'</B>: </td>
					<td bgcolor="#EEEEEE"> <span id="toMessage"></span></td>

				</tr>
				<tr>
					<td></td>
					<td></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_MESSAGES_CONST.'</B>:</td>
					<td bgcolor="#EEEEEE"> <span id="generalMessage"></span></td>
				</tr>
				<tr>
					<td><B>'.$TEXT_PIPE_TYPE_CONST.'</B>:</td>
					<td bgcolor="#EEEEEE"><input type="radio" name="pipeType" value="audio" checked onclick="setPipeType();"> <font color="red">'.$TEXT_AUDIO_CONST.'</font> <input type="radio" name="pipeType" value="video" onclick="setPipeType();"> <font color="blue">'.$TEXT_VIDEO_CONST.'</font> </td>
				</tr>
			</table>
			</div>
		';
		}
			$out.='&nbsp;
			</td>
		</tr>
	</table>';
	
	
	$devicesList=array();
	if($entertainArea!=0){		

		$avDTsArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO);
		$GLOBALS['childsDeviceCategoryArray']=array();
		$mdArray=getDeviceTemplatesFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
		$avDTsArray=array_merge($avDTsArray,$mdArray);
		
		if(count($avDTsArray)==0)
			$avDTsArray[]=0;
		$queryDevice='
			SELECT Device.*
			FROM Device 
			INNER JOIN Device_EntertainArea ON FK_Device=PK_Device
			WHERE FK_DeviceTemplate IN ('.join(',',$avDTsArray).') AND FK_Installation=? AND FK_EntertainArea=?';	
		$resDevice=$dbADO->Execute($queryDevice,array((int)$_SESSION['installationID'],$entertainArea));
		$topPosC1=230;
		$topPosC2=230;

		
		// get existing devices in EA to compare with those from cookie
		$devicesInEA=array();
		$devicesInEA_Names=array();
		while($rowD=$resDevice->FetchRow()){
			$devicesInEA[]=$rowD['PK_Device'];
			$devicesInEA_Names[]=$rowD['Description'];
		}
		$existingPipes=array();
		if(count($devicesInEA)==0)	// fix for no devices in EA
			$devicesInEA[]=0;

		$resPipes=$dbADO->Execute('
			SELECT 
				Device_Device_Pipe.*, 
				C1.Description AS nameInput, 
				C2.Description AS nameOutput,
				DeviceTemplate_Input.FK_ConnectorType AS connectorIn,
				DeviceTemplate_Output.FK_ConnectorType AS connectorOut
			FROM Device_Device_Pipe 
			INNER JOIN Device Dfrom ON FK_Device_From=Dfrom.PK_Device
			INNER JOIN Device Dto ON FK_Device_To=Dto.PK_Device
			LEFT JOIN Command C1 ON FK_Command_Input=C1.PK_Command 
			LEFT JOIN Command C2 ON FK_Command_Output=C2.PK_Command 
			LEFT JOIN DeviceTemplate_Input ON DeviceTemplate_Input.FK_Command=FK_Command_Input AND DeviceTemplate_Input.FK_DeviceTemplate=Dto.FK_DeviceTemplate
			LEFT JOIN DeviceTemplate_Output ON DeviceTemplate_Output.FK_Command=FK_Command_Output AND DeviceTemplate_Output.FK_DeviceTemplate=Dfrom.FK_DeviceTemplate
			WHERE FK_Device_From IN ('.join(',',$devicesInEA).')');
		while($row=$resPipes->FetchRow()){
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['from']=$row['FK_Device_From'];
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['to']=$row['FK_Device_To'];
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['input']=$row['FK_Command_Input'];
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['output']=$row['FK_Command_Output'];
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['isDisplayed']=0;

			$fromDeviceName=$devicesInEA_Names[array_search($row['FK_Device_From'],$devicesInEA)];
			$toDeviceName=$devicesInEA_Names[array_search($row['FK_Device_To'],$devicesInEA)];
			$outName=($row['nameOutput']=='')?'Output':$row['nameOutput'];
			$outName.=($row['connectorOut']!='')?' ('.$connectorsArray[$row['connectorOut']].')':'';
			$inName=($row['nameInput']=='')?'Input':$row['nameInput'];
			$inName.=($row['connectorIn']!='')?' ('.$connectorsArray[$row['connectorIn']].')':'';
			$existingPipes[$row['FK_Device_From']][$row['FK_Pipe']]['description']='From '.$outName.' on '.$fromDeviceName.' to '.$inName.' on '.$toDeviceName;

		}

		// read existing coordinates from cookie 
		if(isset($_COOKIE['PlutoAdminConnectionWizard_'.$_SESSION['installationID']])){
			$cookieArray=unserialize(stripslashes($_COOKIE['PlutoAdminConnectionWizard_'.$_SESSION['installationID']]));
			
			// coords for current entertain area
			if(isset($cookieArray[$entertainArea])){
				// grab coordinates for devices and pipes
				$positionsArray=explode(';',substr($cookieArray[$entertainArea],1));
				$oldDevice=array();
				$jsDrawPipes='';

				if($positionsArray[0]!=''){
					for($i=0;$i<count($positionsArray);$i=$i+5){
						if(in_array($positionsArray[$i],$devicesInEA)){
							$oldDevice[$positionsArray[$i]]['deviceX']=$positionsArray[$i+1];
							$oldDevice[$positionsArray[$i]]['deviceY']=$positionsArray[$i+2];
							if($positionsArray[$i+3]!='none'){
								$parts=explode(':',$positionsArray[$i+3]);
								if(in_array($parts[0],$devicesInEA)){
									// check if cookie pipe match with database
									if(isset($existingPipes[$positionsArray[$i]]) && $existingPipes[$positionsArray[$i]][1]['to']==$parts[0]){
										$jsDrawPipes.='
											audioPipe['.$positionsArray[$i].']=new Array();
											audioPipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
											audioPipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
											audioPipe['.$positionsArray[$i].'][\'description\']=\''.$parts[4].'\';
											drawPipe(\''.$positionsArray[$i].'\',\'audio\','.$parts[1].');
										';
										$existingPipes[$positionsArray[$i]]['isDisplayed']=1;
									}
								}
							}
							if($positionsArray[$i+4]!='none'){
								$parts=explode(':',$positionsArray[$i+4]);
								if(in_array($parts[0],$devicesInEA)){
									if(isset($existingPipes[$positionsArray[$i]]) && $existingPipes[$positionsArray[$i]][2]['to']==$parts[0]){
										$jsDrawPipes.='
											videoPipe['.$positionsArray[$i].']=new Array();
											videoPipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
											videoPipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
											videoPipe['.$positionsArray[$i].'][\'description\']=\''.$parts[4].'\';
											drawPipe(\''.$positionsArray[$i].'\',\'video\','.$parts[1].');
										';
									}
								}
							}
							
						}						
					
					}
				}
			}
		}
		
		$resDevice->MoveFirst();
		while($rowD=$resDevice->FetchRow()){
			$devicesList[$rowD['PK_Device']]=$rowD['Description'];
			$inputsForDevice=array();
			$queryInput='
				SELECT Command.Description,PK_Command,FK_ConnectorType
				FROM DeviceTemplate_Input
				INNER JOIN Command ON FK_Command=PK_Command
				WHERE FK_DeviceTemplate=?';
			$resInputs=$dbADO->Execute($queryInput,$rowD['FK_DeviceTemplate']);
			$inputHeight=20;
			while($rowInputs=$resInputs->FetchRow()){
				$img=(@$rowInputs['FK_ConnectorType']=='')?$connectorsArray[0]:@$connectorsArray[@$rowInputs['FK_ConnectorType']];
				$img=(file_exists('include/images/connectors/'.$img) && $img!='')?$img:$connectorsArray[0];
				$inputsForDevice[]='<div align="left" id="in_'.$rowD['PK_Device'].'_cmd_'.$rowInputs['PK_Command'].'" style="position:absolute;Z-INDEX: 1000;width:110px;height:20px;left:5px;top:'.$inputHeight.'px;border: 1px solid black;align:left;valign:middle;background-color:white;" title="'.$rowInputs['Description'].'" onClick="setPipe(\''.$rowD['PK_Device'].'\',\''.$rowInputs['PK_Command'].'\',\'in_\',\''.htmlspecialchars($rowInputs['Description']).' ('.str_replace('.png','',$img).') on '.htmlspecialchars($rowD['Description']).'\');"><img src="include/images/connectors/'.$img.'" align="top"> '.$rowInputs['Description'].'</div>';
				$inputHeight+=25;
			}
			
			$outputsForDevice=array();
			$queryOutput='
				SELECT Command.Description,PK_Command,FK_ConnectorType
				FROM DeviceTemplate_Output
				INNER JOIN Command ON FK_Command=PK_Command
				WHERE FK_DeviceTemplate=?';
			$resOutput=$dbADO->Execute($queryOutput,$rowD['FK_DeviceTemplate']);
			$outHeight=20;
			$outputsForDevice[]='<div align="right" id="out_'.$rowD['PK_Device'].'_cmd_0" style="position:absolute;Z-INDEX: 1000;width:110px;height:20px;left:235px;top:'.$outHeight.'px;border: 1px solid black;background-color:white;" title="Output" onClick="setPipe(\''.$rowD['PK_Device'].'\',\'0\',\'out_\',\'Output on '.htmlspecialchars($rowD['Description']).'\');">Output <img src="include/images/connectors/none.png" align="top"></div>';
			$outHeight+=25;
			while($rowOutputs=$resOutput->FetchRow()){
				$img=($rowOutputs['FK_ConnectorType']=='')?$connectorsArray[0]:$connectorsArray[$rowOutputs['FK_ConnectorType']];
				$outputsForDevice[]='<div align="right" id="out_'.$rowD['PK_Device'].'_cmd_'.$rowOutputs['PK_Command'].'" style="position:absolute;Z-INDEX: 1000;width:110px;height:20px;left:235px;top:'.$outHeight.'px;border: 1px solid black;background-color:white;" title="'.$rowOutputs['Description'].'" onClick="setPipe(\''.$rowD['PK_Device'].'\',\''.$rowOutputs['PK_Command'].'\',\'out_\',\''.htmlspecialchars($rowOutputs['Description']).' ('.str_replace('.png','',$img).') on '.htmlspecialchars($rowD['Description']).'\');">'.$rowOutputs['Description'].' <img src="include/images/connectors/'.$img.'" align="middle"></div>';
				$outHeight+=25;
			}
			$height=($inputHeight>$outHeight)?$inputHeight:$outHeight;
			$height=(($height>150)?$height:150);
			
			$deviceInfo=getDeviceInformation($rowD['PK_Device'],$dbADO);
			$deviceInfoText='<br>DT: <B>'.$deviceInfo['DeviceTemplate'].'</B><br>Category: <B>'.$deviceInfo['Category'].'</B><br>Manufacturer: <B>'.$deviceInfo['Manufacturer'].'</B>';
			
			// set device box coordinates
			if(isset($oldDevice[$rowD['PK_Device']])){
				$left=$oldDevice[$rowD['PK_Device']]['deviceX'];
				$top=$oldDevice[$rowD['PK_Device']]['deviceY'];
			}else{
				if(!isset($existingPipes[$rowD['PK_Device']])){
					$left='550px';
					$top=$topPosC2;
					$topPosC2+=(15+$height);
				}else{
					$left='100px';
					$top=$topPosC1;
					$topPosC1+=(15+$height);
				}
			}
			
			// fix left/top
			$left = ensurePx($left);
			$top = ensurePx($top);
						
			$out.='
<div id="device_'.$rowD['PK_Device'].'_pipe_1" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_2" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_3" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_4" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>	
<script>
	var audio_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_1");
	var video_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_2");			
</script>
<DIV id="device_'.$rowD['PK_Device'].'" style="BORDER-RIGHT: 2px outset; BORDER-TOP: 2px outset; DISPLAY: ; Z-INDEX: 1; BORDER-LEFT: 2px outset; WIDTH: 350px; BORDER-BOTTOM: 2px outset; POSITION: absolute; LEFT: '.$left.'; TOP: '.$top.'; HEIGHT: '.$height.'px" onclick="bring_to_front(\'device_'.$rowD['PK_Device'].'\')" onmousedown="bring_to_front(\'device_'.$rowD['PK_Device'].'\')">
  <TABLE height="100%" cellSpacing=0 cellPadding=0 width="100%" bgColor=#EEEEEE border=0 onContextMenu="showInfoToolbar('.$rowD['PK_Device'].');return false;">
      <TBODY>
        <TR onmouseup="end_drag(\'device_'.$rowD['PK_Device'].'\')" onmousedown="start_drag(\'device_'.$rowD['PK_Device'].'\')" id="head_'.$rowD['PK_Device'].'"> 
			<TD align="center" bgColor="#DFFFFE" height="15" width=120 valign="middle"><B>Inputs</B></TD>
          	<TD align="center" class="tablehead" height="15" valign="middle"><B>'.$rowD['Description'].'</B></TD>
			<TD align="center" bgColor="#DFFFFE" height="15" width="110" valign="middle"><B>Outputs</B></TD>
        </TR>
        <TR> 
          <TD align=middle colspan="3"><table width="100%" border="0">
			<tr>
				<td align="center" width="40"><div position:relative;>'.join('',$inputsForDevice).'</div></td>
				<td align="center" width="100%"><div align="center" style="width:100px;">Device # <B>'.$rowD['PK_Device'].'</B>'.$deviceInfoText.'</div></td>
				<td align="center" width="40"><div position:relative;>'.join('',$outputsForDevice).'</div></td>
			</tr>	
		</table></TD>
        </TR>
  </TABLE>
</DIV>

<div id="deleteToolbar" style="display:none; position:absolute; top:0px;left:0px;BORDER: 1px outset;background-color:#FFFFFF;z-index:1000;" onMouseOut="if (mouseLeaves(this, event))document.getElementById(\'deleteToolbar\').style.display=\'none\';">
<input type="checkbox" id="del_audio" value="1" onClick="removePipe(this);"> Audio: <span style="color:red" id="del_audio_text">not set</span> <br>
<input type="checkbox" id="del_video" value="1" onClick="removePipe(this);">  Video: <span style="color:blue" id="del_video_text">not set</span> <br>
</div>
';
		}

		// if they are pipes that are not displayed, call for javascript function who will serch for coordinates and display them
		$jsMissingPipes='';
		if(count($existingPipes)!=0){
			foreach ($existingPipes as $devicePipe){
				foreach($pipeTypesArray as $pipeType=>$pipeTypeDescription){
					if(isset($devicePipe[$pipeType]) && $devicePipe[$pipeType]['isDisplayed']==0){
						$jsMissingPipes.='generatePipe('.$devicePipe[$pipeType]['from'].','.(int)$devicePipe[$pipeType]['output'].','.$devicePipe[$pipeType]['to'].','.(int)$devicePipe[$pipeType]['input'].',\''.$pipeTypeDescription.'\',\''.htmlspecialchars($devicePipe[$pipeType]['description']).'\');';	
					}
				}
			}
		}
		
	}

		
	$out.='	
	</form>';
	if(count(@$devicesList)>1){
		$out.='<script>var layersArray = new Array('.join(',',array_keys($devicesList)).');</script>';
	}else{
		$out.='<script>
			var layersArray = new Array();';
		if(count(@$devicesList)!=0){
			$out.='
			layersArray[0]='.join('',array_keys(@$devicesList)).';';
		}
		$out.='	
		</script>';
	}
	
	// ToDO: add rebuild later
	$out.='<script>
	'.@$jsMissingPipes.'
/*	'.@$jsDrawPipes.' */
	</script>';
	

	$output->SetScriptInBody('onmousemove="drag_layer();"');
} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=connectionWizard&error=You are not authorised to change the installation.");
			exit();
		}
		
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				//$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID,$ret);	
				$insertID=createDevice($deviceTemplate,$installationID,0,NULL,$dbADO);	
							
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);
				
				if(isInfrared($deviceTemplate,$dbADO)){
					$dbADO->Execute('INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData) VALUES (?,?)',array($insertID,$GLOBALS['InfraredPort']));
				}
			}
			header("Location: index.php?section=connectionWizard&msg=$TEXT_DEVICE_ADDED_CONST.");
			exit();
		}
		
		$entertainArea=@(int)$_REQUEST['entertainArea'];
		$oldEntertainArea=(int)$_REQUEST['oldEntertainArea'];

		if($entertainArea!=0 && $_REQUEST['devicesCoords']!=''){
			$cookieArray[$oldEntertainArea]=$_REQUEST['devicesCoords'];
			// update database
			foreach ($cookieArray AS $entArea=>$params){
				if($entArea!=0){
					$positionsArray=explode(';',substr($cookieArray[$entArea],1));
					for($i=0;$i<count($positionsArray);$i=$i+5){
						addDeletePipe($positionsArray[$i+3],$positionsArray[$i],1,$dbADO);
						addDeletePipe($positionsArray[$i+4],$positionsArray[$i],2,$dbADO);
					}
				}
			}
			
			$cookieData=serialize($cookieArray);
			setcookie("PlutoAdminConnectionWizard_".$_SESSION['installationID'],$cookieData,(time()+31536000),'/',false);
		}
				
		header("Location: index.php?section=connectionWizard&msg=$TEXT_DEVICES_UPDATED_CONST&entertainArea=$entertainArea");		
	}

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_AV_CONNECTION_WIZARD_CONST);
	$output->output();
}

function addDeletePipe($operation,$deviceFrom,$pipe,$dbADO)
{
	if($operation=='none'){
		$dbADO->Execute('DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Pipe=?',array($deviceFrom,$pipe));
	}else{
		$arr=explode(':',$operation);
		$deviceTo=$arr[0];
		$commandOut=($arr[3]!=0)?$arr[3]:NULL;
		// must delete any existing pipes of this type from
		// this output before adding the new one
		$dbADO->Execute('DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Pipe=?',array($deviceFrom,$pipe));
		$dbADO->Execute('INSERT IGNORE INTO Device_Device_Pipe (FK_Device_From,FK_Device_To,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES (?,?,?,?,?)',array($deviceFrom,$deviceTo,$pipe,$arr[2],$commandOut));
	}
}

function getConnectorsArray($dbADO){
	$ids=getAssocArray('ConnectorType','PK_ConnectorType','PK_ConnectorType',$dbADO);
	$pics=array(0=>'other.png');
	foreach ($ids AS $id){
		$pics[$id]=$id.'_connector.png';
	}
	
	return $pics;
}

function ensurePx($str) {
  if (strlen($str) < 2 || strtolower(substr($str, strlen($str)-2)) != "px") {
    return $str."px";
  } else {
    return $str;
  }
}

?>
