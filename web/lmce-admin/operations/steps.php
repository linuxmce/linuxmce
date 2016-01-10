<?php
function steps($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('steps.lang.php');
	
	/* @var $dbADO ADOConnection */


	$currentSection = @cleanString($_REQUEST['rightSection']);
	$installationID = (int)@$_SESSION['installationID'];

	if(isset($_REQUEST['senderID']) && (int)$_REQUEST['senderID']>0){
		$senderStep=(int)$_REQUEST['senderID'];

		if($installationID!=0){
			$querySS='SELECT * FROM SetupStep WHERE FK_Installation=? AND FK_PageSetup=?';
			$resSS=$dbADO->Execute($querySS,array($installationID,$senderStep));
			if($resSS->RecordCount()==0){
				$insertSS='INSERT INTO SetupStep (FK_Installation, FK_PageSetup) VALUES (?,?)';
				$dbADO->Execute($insertSS,array($installationID,$senderStep));
			}
		}
	}

$start_time=getmicrotime();		
	$scriptInHead ='
		<script type="text/javascript">
		function setMenuItem(mainUrl,selfPage,sender,anchor)
		{
			if(mainUrl!=\'\'){
				if(top.basefrm.document.forms[0] && top.basefrm.document.forms[0].action){
					top.basefrm.document.forms[0].action.value=\'externalSubmit\';
					top.basefrm.document.forms[0].submit();
				}
				setTimeout("top.basefrm.location=\'"+mainUrl+"\';", 50);
				
			}
			else{
				top.basefrm.location=\'index.php?section=installationSettings\';
			}
			self.location=\'index.php?section=wizard&pageSetup=\'+selfPage+\'&senderID=\'+sender+\'#\'+anchor;				
		}
	
		function highlightMenuItem(){
			try{
				document.getElementById('.(int)@$_REQUEST['pageSetup'].').style.background="#ccc";
				document.getElementById("next_'.(int)@$_REQUEST['pageSetup'].'").style.display="";
			}catch(e){}
		}
		</script>
	';
	

	
	$out='<table style="border: 0; width: 100%" cellpading="2" cellspacing="0">
			<tr>
				<td colspan="3" align="center" height="133" class="left_frame_logo">
					<a href="index.php?section=wizard"><img src="include/images/spacer.gif" border="0" width="220" height="90"></a>
				</td>
			</tr>
			<tr>
				<td colspan="3" align="center" height="9" bgcolor="black"><img src="include/images/spacer.gif" border="0"></td>
			</tr>
	';
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
		$out.='
				<tr>
					<td valign="center" colspan="3" class="left_menu" height="30">'.((@$_SESSION['userLoggedIn']==true)?'<a href="index.php?section=login&action=logout" target="basefrm" >'.translate('TEXT_LOGOUT_CONST').', '.$_SESSION['username'].'</a>':'').'</td>					
				</tr>
		';
	}
		
	if(isset($_SESSION['installationID'])){
		
		$out.='
			<tr>
				<td colspan="3" align="left" class="left_menu">
					<span class="menu_title">'.translate('TEXT_WIZARD_CONST').'</span>
				</td>
			</tr>
			<tr>
				<td colspan="3" align="left" class="left_menu">'.getLeftWizardMenu($dbADO).'</td>
			</tr>';
		$out.='
			<tr>
				<td colspan="3" align="center" class="left_menu">&nbsp;</td>
			</tr>
			<tr>
				<td colspan="3" align="center" class="left_menu"><a href="index.php?section=leftMenu"><B>'.translate('TEXT_SHOW_DEVICES_TREE_CONST').'</B></a></td>
			</tr>';
	}
		$out.='
	</table>';
	$end_time=getmicrotime();			
	//$out.='<br><p class="normaltext">Page generated in '.round(($end_time-$start_time),3).' s.';
	
	$output->setScriptInHead($scriptInHead);
	$output->setScriptInBody(' onload="highlightMenuItem();"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getChildPages($parentNode,$level,$dbADO)
{
	$selectNodes = '
		SELECT DISTINCT PageSetup.*, SetupStep.FK_Installation
		FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (SetupStep.FK_Installation IS NULL OR SetupStep.FK_Installation=?)
		WHERE FK_PageSetup_Parent = ? AND showInTopMenu = 1 AND Website=1 AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND Device.FK_Installation=?))		
		ORDER BY OrderNum ASC';
	$resNodes= $dbADO->Execute($selectNodes,array($_SESSION['installationID'],$parentNode,$_SESSION['installationID']));
	while($row=$resNodes->FetchRow()){
		$GLOBALS['pagesArray'][]=$row['PK_PageSetup'];
		$GLOBALS['linksArray'][]=$row['pageURL'];
		$GLOBALS['descriptionArray'][]=$row['Description'];
		$GLOBALS['levelArray'][]=$level;
		$GLOBALS['isSync'][]=(($row['FK_Installation']!='')?1:0);
		getChildPages($row['PK_PageSetup'],$level+1,$dbADO);
	}
}

function indent($level)
{
	$out='';
	for ($i=0;$i<$level;$i++){
		$out.='&nbsp;&nbsp;&nbsp;&nbsp;';
	}
	return $out;
}

function getLastTimestamp($table,$dbADO){
	$res=$dbADO->Execute('SELECT psc_mod FROM '.$table.' ORDER BY psc_mod desc LIMIT 0,1');
	if(!$res){
		return 0;
	}
	$row=$res->FetchRow();
	return $row['psc_mod'];
}

function geSection($pageSetupID,$dbADO){
	if($pageSetupID==0){
		$pageSetupID=$GLOBALS['InstallationPage'];
	}
	$res=$dbADO->Execute('SELECT pageURL FROM PageSetup WHERE PK_PageSetup=?',array($pageSetupID));
	if(!$res){
		return '';
	}
	$row=$res->FetchRow();
	return $row['pageURL'];
}

function getLeftWizardMenu($dbADO){
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('steps.lang.php');
	
	$GLOBALS['pagesArray']=array();
	$GLOBALS['descriptionArray']=array();
	$GLOBALS['linksArray']=array();
	$GLOBALS['levelArray']=array();
	$GLOBALS['isSync']=array();
		
	$installationID = (int)@$_SESSION['installationID'];
	$currentItem=(isset($_REQUEST['pageSetup']) && (int)$_REQUEST['pageSetup']>0)?(int)$_REQUEST['pageSetup']:$GLOBALS['InstallationPage'];
		
	$cachedLeftWizardMenu=(getcwd()).'/cached/leftWizardMenu';
	$cachedIDs=(getcwd()).'/cached/topMenuIDs';
	$SetupStepTimestampFile=(getcwd()).'/cached/SetupStepTimestamp';

	// get existing devices to compare them with old ones
	$devices=join(',',array_keys(getAssocArray('Device','PK_Device','PK_Device',$dbADO,'WHERE FK_Installation='.$_SESSION['installationID'],'ORDER BY PK_Device ASC')));

	// if cached menu exists but something changed in SetupStep table or device table, rebuild menu
	
	$lastTimestamp=getLastTimestamp('SetupStep',$dbADO);
	if(file_exists($cachedLeftWizardMenu)){
		$oldIDs=@file($cachedIDs);
		$oldTimestamp=@join('',@file($SetupStepTimestampFile));
		if($devices==trim(@$oldIDs[1]) && $lastTimestamp==$oldTimestamp){
			return join('',file($cachedLeftWizardMenu));
		}
	}

	
	$selectWizard = "
			SELECT DISTINCT PageSetup.*, SetupStep.FK_Installation
			FROM PageSetup 
			LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
			LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (SetupStep.FK_Installation IS NULL OR SetupStep.FK_Installation=?)
			WHERE FK_PageSetup_Parent = 1 AND showInTopMenu = 1 AND Website=1 AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND Device.FK_Installation=?))
			ORDER BY OrderNum ASC
			";
	$resWizard = $dbADO->Execute($selectWizard,array($installationID,$installationID));
	while ($rowNode = $resWizard->FetchRow()) {
		$GLOBALS['pagesArray'][]=$rowNode['PK_PageSetup'];
		$GLOBALS['linksArray'][]=$rowNode['pageURL'];
		$GLOBALS['descriptionArray'][]=$rowNode['Description'];
		$GLOBALS['levelArray'][]=0;
		$GLOBALS['isSync'][]=(($rowNode['FK_Installation']!='')?1:0);
		getChildPages($rowNode['PK_PageSetup'],1,$dbADO);
	}

	$leftWizardMenu='<table cellpadding="0" cellspacing="0" width="100%" border="0">';
	foreach ($GLOBALS['descriptionArray'] AS $pos=>$description){
		$fromPage=$GLOBALS['pagesArray'][$pos];
		$toPos=(isset($GLOBALS['pagesArray'][$pos+1]) && $GLOBALS['linksArray'][$pos+1]!='')?$pos+1:$pos+2;

		$toPage=isset($GLOBALS['pagesArray'][$toPos])?$GLOBALS['pagesArray'][$toPos]:$GLOBALS['pagesArray'][$pos];

		$wizardLink=($GLOBALS['linksArray'][$pos]!='')?'<a <a href="#" onClick="setMenuItem(\''.$GLOBALS['linksArray'][$pos].'\',\''.$fromPage.'\',\''.$fromPage.'\',\''.$GLOBALS['pagesArray'][$pos].'\')">'.$description.'</a>':'<b>'.$description.'</b>';
		$pic=($GLOBALS['isSync'][$pos]==1)?'<img src="include/images/pluto_dot.gif" style="vertical-align: middle;">':'<img src="include/images/spacer.gif" style="vertical-align: middle;" width="5">';
		$leftWizardMenu.='
			<tr id="'.@$GLOBALS['pagesArray'][$pos].'">
				<td>'.indent($GLOBALS['levelArray'][$pos]).' '.$pic.'&nbsp;&nbsp;'.$wizardLink.'</td>
				<td align="right"><span id="next_'.@$GLOBALS['pagesArray'][$pos].'" style="display:none;"><a href="#" onClick="setMenuItem(\''.@$GLOBALS['linksArray'][$toPos].'\',\''.$toPage.'\',\''.$fromPage.'\',\''.$GLOBALS['pagesArray'][$pos].'\')">'.translate('TEXT_NEXT_CONST').'</a></span></td>
			</tr>
			';	

		/*	
		if($GLOBALS['levelArray'][$pos]==0){
		$leftWizardMenu.='
			<tr id="'.@$GLOBALS['pagesArray'][$pos].'">
				<td>'.indent($GLOBALS['levelArray'][$pos]).'</td>
				<td colspan="2">'.$wizardLink.'</td>
				<td align="right"><span id="next_'.@$GLOBALS['pagesArray'][$pos].'" style="display:none;"><a href="#" onClick="setMenuItem(\''.@$GLOBALS['linksArray'][$toPos].'\',\''.$toPage.'\',\''.$fromPage.'\',\''.$GLOBALS['pagesArray'][$pos].'\')">'.translate('TEXT_NEXT_CONST').'</a></span></td>
			</tr>
			';	
		}else{
			$leftWizardMenu.='
			<tr id="'.@$GLOBALS['pagesArray'][$pos].'">
				<td>'.indent($GLOBALS['levelArray'][$pos]).'</td>
				<td>'.$pic.'</td>
				<td>'.$wizardLink.'</td>
				<td align="right"><span id="next_'.@$GLOBALS['pagesArray'][$pos].'" style="display:none;"><a href="#" onClick="setMenuItem(\''.@$GLOBALS['linksArray'][$toPos].'\',\''.$toPage.'\',\''.$fromPage.'\',\''.$GLOBALS['pagesArray'][$pos].'\')">'.translate('TEXT_NEXT_CONST').'</a></span></td>
			</tr>
			';	
		}
		*/
	}
	$leftWizardMenu.='</table>';

	// write top menu file and devices/page setup IDs
	writeFile($cachedLeftWizardMenu,$leftWizardMenu,'w');
	writeFile($SetupStepTimestampFile,$lastTimestamp,'w');
	
	return $leftWizardMenu;
}
?>