<?php
function floorplanWizard($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/floorplanWizard.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$installationID = (int)@$_SESSION['installationID'];
	$page=(isset($_REQUEST['page']))?$_REQUEST['page']:1;
	$type=(isset($_REQUEST['type']))?$_REQUEST['type']:1;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$path=$_SERVER['DOCUMENT_ROOT'].'/lmce-admin/floorplans/inst'.$installationID;
	$floorPlanImage='include/images/image_not_found.gif';
	$maxWidth=400;
	$maxHeight=400;

	if($installationID==0){
		header('Location: index.php?section=login');
		exit();
	}

	$colors = array('pixel_lightblue.gif','pixel_green.gif','pixel_blue.gif','pixel_pink.gif','pixel_red.gif','pixel_orange.gif');

	$queryFloorplans='SELECT Page,Description FROM Floorplan WHERE FK_Installation=? ORDER BY Page ASC';
	$resFloorplans=$dbADO->Execute($queryFloorplans,$installationID);

	if($action=='form'){
		$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'ORDER BY Description ASC');

		$out=setLeftMenu($dbADO).'
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div class="confirm" align="center"><B>'.@stripslashes($_GET['msg']).'</B></div>
		<form name="form1" action="index.php" onsubmit="return submitForm();" method="POST">
			<input type="hidden" name="section" value="floorplanWizard">
			<input type="hidden" name="action" value="update">
		
		<h2 align="center" title="">'.$TEXT_FLOOR_PLAN_CONST.'</h2>
		<p align="center">'.$TEXT_FLOORPLAN_NOTICE_CONST.'</p>
		<table bgcolor="#EEEEEE" cellpadding="0" cellspacing="0" border="0" align="center">
			<tr>
				<td><select name="page" onchange="document.form1.page_orig.value=\''.$page.'\';submitForm();">';
		while($rowFloorplans=$resFloorplans->FetchRow()){
			$out.='<option value="'.$rowFloorplans['Page'].'" '.(($page==$rowFloorplans['Page'])?'selected':'').'>'.$rowFloorplans['Description'].'</option>';
		}
		$out.='
					</select>
				<select name="type" onchange="submitForm();">';
		$queryFloorplanTypes='SELECT PK_FloorplanType,Description FROM FloorplanType WHERE PK_FloorplanType NOT IN (4,6) ORDER BY Description ASC';
		$resFloorplanTypes=$dbADO->Execute($queryFloorplanTypes);
		while($rowFloorplanTypes=$resFloorplanTypes->FetchRow()){
			$out.='<option value="'.$rowFloorplanTypes['PK_FloorplanType'].'" '.(($type==$rowFloorplanTypes['PK_FloorplanType'])?'selected':'').'>'.$rowFloorplanTypes['Description'].'</option>';
		}
		$out.='
				</select>
				</td>
				<td valign="middle">
					<a href="index.php?section=uploadFloorplan" class="normalLink">'.$TEXT_ADD_ANOTHER_FLOORPLAN_CONST.'</a><br />
					<a href="index.php?section=uploadFloorplan&fpID='.$page.'&type='.$type.'" class="normalLink">'.$TEXT_EDIT_UPLOAD_CONST.'</a><br>
					<a href="javascript:if(confirm(\''.$TEXT_DELETE_CONFIRMATION_CONST.'\')){document.form1.action.value=\'remove\';document.form1.submit();}" class="normalLink">'.$TEXT_DELETE_FLOORPLAN_CONST.'</a><br />
				</td>
			</tr>
			<tr>
				<td colspan="2">
					<table bgcolor="#FEFEFE" cellpadding="3" cellspacing="0" width="90%" border="0" align="center"><tr>';
		$queryFloorplanObjectType='SELECT * FROM FloorplanObjectType WHERE FK_FloorplanType=? ORDER BY Description ASC';
		$objectsPerPage=4;
		$resFloorplanObjectType=$dbADO->Execute($queryFloorplanObjectType,$type);
		$itemsCount=0;
		while($rowFloorplanObjectType=$resFloorplanObjectType->FetchRow()){
			$objColor[$rowFloorplanObjectType['PK_FloorplanObjectType']]=$colors[$itemsCount%count($colors)];
			$out.=($itemsCount%$objectsPerPage==0)?'</tr><tr>':'';
			$out.='<td><img src="scripts/floorplan/images/'.$objColor[$rowFloorplanObjectType['PK_FloorplanObjectType']].'" width="20" height="20" /> '.$rowFloorplanObjectType['Description'].'</td>';
			$itemsCount++;
		}
		$out.='
						</tr>
					</table>
				</td>
			</tr>
			<tr>
				<td colspan="2">&nbsp;<img src="inc/images/pixel_blank.gif" width="652" height="1" border="0"></td>
			</tr>				
			<tr>
				<td align="center" colspan="2"><input type="button" class="button" value="'.$TEXT_CANCEL_CONST.'" onclick="cancel();">&nbsp; &nbsp; &nbsp;	<input type="button" class="button" value="'.$TEXT_SAVE_CONST.'" onclick="submitForm();"><br /></td>
			</tr>
			<tr>
				<td valign="top" colspan="2">&nbsp;</td>
			</tr>						
			<tr>
				<td valign="top" colspan="2">Show devices in room: '.pulldownFromArray($roomsArray,'sel_room',@$_SESSION['sel_room'],'onChange="submitForm();"','key','All rooms').'</td>
			</tr>					
			<tr>
				<td valign="top" colspan="2"><script language="javascript" type="text/javascript">objSensorPositioner.writeHTML();</script></td>
			</tr>
		</table>
		<input type="hidden" name="hidFloorplanData" value="" />
		<input type="hidden" name="page_orig" value="0" />
		<input type="hidden" name="type_orig" value="0" />
		';
		if(file_exists($path.'/'.$page.'.png')){
			$randNumber=rand(0,99999);
			$floorPlanImage='include/image.php?imagepath='.$GLOBALS['floorplansPath'].'/inst'.$installationID.'/'.$page.'.png&rand='.$randNumber;
			//$floorPlanImage='floorplans/inst'.$installationID.'/'.$page.'.png';
			$imgArray=getimagesize($path.'/'.$page.'.png');
			$origWidth=$imgArray[0];
			$origHeight=$imgArray[1];

			if($origWidth/$origHeight>1){
				$scaleFactor=$maxWidth/$origWidth;
				$origWidth=$maxWidth;
				$origHeight=floor($origHeight*$scaleFactor);
			}
			else{
				$scaleFactor=$maxHeight/$origHeight;
				$origHeight=$maxHeight;
				$origWidth=floor($origWidth*$scaleFactor);
			}

			$out.='<input type="hidden" name="scaleFactor" value="'.$scaleFactor.'">';
		}else{
			$origWidth=384;
			$origHeight=359;
			$scaleFactor=1;
		}
		$out.='</form>';

		$roomFilter=((int)@$_SESSION['sel_room']!=0)?' AND FK_Room='.(int)$_SESSION['sel_room']:'';
		switch ($type){
			case $GLOBALS['EntertainmentZone']:
			$queryCoords = "
			SELECT 
				EntertainArea.PK_EntertainArea as id, 
				EntertainArea.Description as description, 
				EntertainArea.FK_FloorplanObjectType as itemtype,	
				EntertainArea.FloorplanInfo as coordinates,
				PK_FloorplanObjectType,
				Room.Description AS Room
			FROM EntertainArea
				INNER JOIN Room ON FK_Room=PK_Room
				INNER JOIN FloorplanObjectType ON EntertainArea.FK_FloorplanObjectType=PK_FloorplanObjectType
			WHERE 
				FK_Installation=?
				AND FK_FloorplanType=?
				$roomFilter
			ORDER BY itemtype
			";
			break;
			case $GLOBALS['RoomsZone']:
			$queryCoords = "
			SELECT 
				Room.PK_Room as id, 
				Room.Description as description, 
				Room.FK_FloorplanObjectType as itemtype,	
				Room.FloorplanInfo as coordinates,
				PK_FloorplanObjectType,
				Room.Description AS Room
			FROM Room
				INNER JOIN FloorplanObjectType ON Room.FK_FloorplanObjectType=PK_FloorplanObjectType
			WHERE 
				FK_Installation=?
				AND FK_FloorplanType=?
			ORDER BY itemtype
			";

			break;
			default:
			$queryCoords = "
			SELECT 
				d.PK_Device as id, 
				d.Description as description, 
				itemtype.IK_DeviceData as itemtype,	
				coord.IK_DeviceData as coordinates,
				PK_FloorplanObjectType,
				Room.Description AS Room
			FROM Device d
				LEFT JOIN Room ON FK_Room=PK_Room
				INNER JOIN DeviceTemplate mdl ON d.FK_DeviceTemplate=mdl.PK_DeviceTemplate
				INNER JOIN DeviceCategory dc ON mdl.FK_DeviceCategory=dc.PK_DeviceCategory
				INNER JOIN Device_DeviceData itemtype 
					ON itemtype.FK_Device=d.PK_Device 
					AND itemtype.FK_DeviceData=".$GLOBALS['FloorplanObjectType']."
				INNER JOIN FloorplanObjectType fpot ON itemtype.IK_DeviceData=fpot.PK_FloorplanObjectType
				LEFT JOIN Device_DeviceData coord 
					ON coord.FK_Device=d.PK_Device 
					AND coord.FK_DeviceData=".$GLOBALS['FloorplanInfo']."
			WHERE 
				d.FK_Installation=?
				AND fpot.FK_FloorplanType=?
				$roomFilter
			ORDER BY itemtype
			";

			break;
		}

		$resCoords=$dbADO->Execute($queryCoords,array($installationID,$type));

		$scriptInHead='
			<script language="Javascript" src="scripts/floorplan/javascript/dynapi/src/dynapi.js" type="text/javascript"></script>
			<script language="Javascript" src="scripts/floorplan/SensorPositioner.js" type="text/javascript"></script>
			<script language="JavaScript" type="text/javascript">
			<!--
			DynAPI.setLibraryPath("scripts/floorplan/javascript/dynapi/src/lib/");
			DynAPI.include("dynapi.api.browser.js");
			DynAPI.include("dynapi.api.dynlayer.js");
			DynAPI.include("dynapi.api.dyndocument.js");
			DynAPI.include("dynapi.event.*");
	
			objSensorPositioner.imageURL = "'.$floorPlanImage.'";
			objSensorPositioner.imageWidth = '.$origWidth.';
			objSensorPositioner.imageHeight ='.$origHeight.';

		';
		$countCoords=0;
		while($rowCoord=$resCoords->FetchRow()){
			$item=array();
			$item['hascoordinatesthispage'] = false;
			$item['x'] = -1;
			$item['y'] = -1;
			$item['pages'] = array();
			$item['hascoordinates'] = ($rowCoord['coordinates'] != '');
			if ($item['hascoordinates'])
			{
				//go thru and look for the coords for the current page
				$intCursor = 0;
				$arValues = explode(',', $rowCoord['coordinates']);
				while(isset($arValues[$intCursor]))
				{
					//the following is for making updates below
					$item['pages'][$arValues[$intCursor]] = array( 'x'=>$arValues[$intCursor+1] , 'y'=>$arValues[$intCursor+2] );
					//the following is for positioning on the current page only

					if ($arValues[$intCursor] == $page && $arValues[$intCursor+1]!=-1 && $arValues[$intCursor+2] !=-1)
					{
						$item['hascoordinatesthispage'] = true;
						$item['x'] = floor(($arValues[$intCursor+1] * $scaleFactor)-10);
						$item['y'] = floor(($arValues[$intCursor+2] * $scaleFactor)-10);
						if($item['x']>$origWidth || $item['y']>$origHeight)
						$item['hascoordinatesthispage']=false;
					}
					$intCursor += 3;
				}
			}
			$scriptInHead.='
			objSensor = new SensorInstance();
			objSensor.SensorURL = "scripts/floorplan/images/'.$objColor[$rowCoord['PK_FloorplanObjectType']].'";
			objSensor.SensorWidth = 20;
			objSensor.SensorHeight = 20;
			objSensor.Description = "'.$rowCoord['description'].' - '.trim($rowCoord['Room']).' #'.$rowCoord['id'].'";
			//objSensor.Description = "'.$item['x'].' '.$item['y'].'";
			objSensor.ID = '.$rowCoord['id'].';
			objSensor.Type = '.$rowCoord['itemtype'].';
			objSensor.hasCoordinates = '.(($item['hascoordinatesthispage']) ? 'true' : 'false').';
			objSensor.SensorX = '.$item['x'].';
			objSensor.SensorY = '.$item['y'].';
			objSensorPositioner.SensorInstances[objSensorPositioner.SensorInstances.length] = objSensor;
		';
			$countCoords++;
		}
		$scriptInHead.='
	
			function submitForm()
			{
				document.form1.hidFloorplanData.value = objSensorPositioner.getSerial();
				document.form1.submit();
			}
			function cancel()
			{
				document.location = "index.php?section=floorplanWizard&page='.$page.'&type='.$type.'";
			}

			//-->
			</script>
	';

	}else{
		// process
		if(isset($_REQUEST['sel_room'])){
			$_SESSION['sel_room']=$_REQUEST['sel_room'];
		}


		if($action=='remove'){
			$dbADO->Execute('DELETE FROM Floorplan WHERE FK_Installation=? AND Page=?',array($installationID,$page));
			@unlink($path.'/'.$page.'.png');
			header('Location: index.php?section=floorplanWizard&msg=The floorplan was deleted.');
			exit();
		}

		$pageToRedirect=$page;
		if((int)$_POST['page_orig']!=0){
			$page=(int)$_POST['page_orig'];
		}

		$hidFloorplanData=$_POST['hidFloorplanData'];
		$intCursor = 1; //the first one should be empty
		$arIncomingCoords = explode(',',$hidFloorplanData);
		$scaleFactor=isset($_POST['scaleFactor'])?$_POST['scaleFactor']:1;

		switch ($type){
			case $GLOBALS['EntertainmentZone']:
			while (isset($arIncomingCoords[$intCursor])){
				$entAreaID=$arIncomingCoords[$intCursor];
				$queryExistingCoord='SELECT * FROM EntertainArea WHERE PK_EntertainArea=?';
				$res=$dbADO->Execute($queryExistingCoord,array($entAreaID));
				$resFloorplans->MoveFirst();
				$entAreaCoords=array();
				if($res->RecordCount()==0){
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$entAreaCoords[]=$rowFloorplans['Page'];
						$entAreaCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+1]/$scaleFactor)):-1;
						$entAreaCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+2]/$scaleFactor)):-1;
					}

					$updateCoords='UPDATE EntertainArea SET FloorplanInfo=? WHERE PK_EntertainArea=?';
					$dbADO->Execute($updateCoords,array(join(',',$entAreaCoords),$entAreaID));
				}else{
					$oldCoordsRow=$res->FetchRow();
					$oldCoordsArray=explode(',',$oldCoordsRow['FloorplanInfo']);
					$key=0;
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$entAreaCoords[$key]=$rowFloorplans['Page'];
						$xcoord=($arIncomingCoords[$intCursor+1]!=-1)?ceil(($arIncomingCoords[$intCursor+1]+10)/$scaleFactor):-1;
						$ycoord=($arIncomingCoords[$intCursor+2]!=-1)?ceil(($arIncomingCoords[$intCursor+2]+10)/$scaleFactor):-1;
						$entAreaCoords[$key+1]=($rowFloorplans['Page']==$page)?$xcoord:(isset($oldCoordsArray[$key+1])?$oldCoordsArray[$key+1]:-1);
						$entAreaCoords[$key+2]=($rowFloorplans['Page']==$page)?$ycoord:(isset($oldCoordsArray[$key+2])?$oldCoordsArray[$key+2]:-1);
						$key+=3;
					}
					$updateCoords='UPDATE EntertainArea SET FloorplanInfo=? WHERE PK_EntertainArea=?';
					$dbADO->Execute($updateCoords,array(join(',',$entAreaCoords),$entAreaID));
				}
				unset($entAreaCoords);
				$intCursor+=3;
			}
			break;
			case $GLOBALS['RoomsZone']:
			while (isset($arIncomingCoords[$intCursor])){
				$roomID=$arIncomingCoords[$intCursor];
				$queryExistingCoord='SELECT * FROM Room WHERE PK_Room=?';
				$res=$dbADO->Execute($queryExistingCoord,array($roomID));
				$resFloorplans->MoveFirst();
				$roomCoords=array();
				if($res->RecordCount()==0){
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$roomCoords[]=$rowFloorplans['Page'];
						$roomCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+1]/$scaleFactor)):-1;
						$roomCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+2]/$scaleFactor)):-1;
					}

					$updateCoords='UPDATE Room SET FloorplanInfo=? WHERE PK_Room=?';
					$dbADO->Execute($updateCoords,array(join(',',$roomCoords),$roomID));
				}else{
					$oldCoordsRow=$res->FetchRow();
					$oldCoordsArray=explode(',',$oldCoordsRow['FloorplanInfo']);
					$key=0;
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$roomCoords[$key]=$rowFloorplans['Page'];
						$xcoord=($arIncomingCoords[$intCursor+1]!=-1)?ceil(($arIncomingCoords[$intCursor+1]+10)/$scaleFactor):-1;
						$ycoord=($arIncomingCoords[$intCursor+2]!=-1)?ceil(($arIncomingCoords[$intCursor+2]+10)/$scaleFactor):-1;
						$roomCoords[$key+1]=($rowFloorplans['Page']==$page)?$xcoord:(isset($oldCoordsArray[$key+1])?$oldCoordsArray[$key+1]:-1);
						$roomCoords[$key+2]=($rowFloorplans['Page']==$page)?$ycoord:(isset($oldCoordsArray[$key+2])?$oldCoordsArray[$key+2]:-1);
						$key+=3;
					}
					$updateCoords='UPDATE Room SET FloorplanInfo=? WHERE PK_Room=?';
					$dbADO->Execute($updateCoords,array(join(',',$roomCoords),$roomID));
				}
				unset($roomCoords);
				$intCursor+=3;
			}

			break;
			default:
			while (isset($arIncomingCoords[$intCursor])){
				$deviceID=$arIncomingCoords[$intCursor];
				$queryExistingCoord='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$res=$dbADO->Execute($queryExistingCoord,array($deviceID,$GLOBALS['FloorplanInfo']));
				$resFloorplans->MoveFirst();
				$deviceCoords=array();
				if($res->RecordCount()==0){
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$deviceCoords[]=$rowFloorplans['Page'];
						$deviceCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+1])/$scaleFactor):-1;
						$deviceCoords[]=($rowFloorplans['Page']==$page)?floor(($arIncomingCoords[$intCursor+2])/$scaleFactor):-1;
					}

					$insertDD='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,?)';
					$dbADO->Execute($insertDD,array($deviceID,$GLOBALS['FloorplanInfo'],join(',',$deviceCoords)));
				}else{
					$oldCoordsRow=$res->FetchRow();
					$oldCoordsArray=explode(',',$oldCoordsRow['IK_DeviceData']);
					$key=0;
					while($rowFloorplans=$resFloorplans->FetchRow()){
						$deviceCoords[$key]=$rowFloorplans['Page'];
						$xcoord=($arIncomingCoords[$intCursor+1]!=-1)?ceil(($arIncomingCoords[$intCursor+1]+10)/$scaleFactor):-1;
						$ycoord=($arIncomingCoords[$intCursor+2]!=-1)?ceil(($arIncomingCoords[$intCursor+2]+10)/$scaleFactor):-1;
						$deviceCoords[$key+1]=($rowFloorplans['Page']==$page)?$xcoord:(isset($oldCoordsArray[$key+1])?$oldCoordsArray[$key+1]:-1);
						$deviceCoords[$key+2]=($rowFloorplans['Page']==$page)?$ycoord:(isset($oldCoordsArray[$key+2])?$oldCoordsArray[$key+2]:-1);
						$key+=3;
					}
					if(join(',',$deviceCoords)!=join(',',$oldCoordsArray)){
						//$dbADO->debug=true;
						$updateDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
						$dbADO->Execute($updateDD,array(join(',',$deviceCoords),$deviceID,$GLOBALS['FloorplanInfo']));
						$dbADO->debug=false;
					}
				}
				unset($deviceCoords);
				$intCursor+=3;
			}
			break;
		}

		setOrbitersNeedConfigure($installationID,$dbADO);
		header("Location: index.php?section=floorplanWizard&page=".$pageToRedirect."&type=".$type);
	}

	$output->setNavigationMenu(array($TEXT_FLOORPLAN_WIZARD_CONST=>'index.php?section=floorplanWizard'));
	$output->setScriptInHead($scriptInHead);
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_FLOORPLAN_WIZARD_CONST);
	$output->output();
}

?>
