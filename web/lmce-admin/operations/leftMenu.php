<?php
function leftMenu($output,$dbADO) {
	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('leftMenu.lang.php');

	$currentSection = @cleanString($_REQUEST['rightSection']);
	$installationID = (int)@$_SESSION['installationID'];
	global $devicesArray;
	
	$devicesArray=array();
	
	$treeType = isset($_REQUEST['treeType'])?cleanString(strip_tags($_REQUEST['treeType'])):'devices';
	$out= '<table border="0" cellpading="2" cellspacing="0" width="100%">
			<tr>
				<td class="left_frame_logo" height="133">
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
					<td valign="top">'.($currentSection=='login'?'&raquo;':'').'<a href="index.php?section=login&action=logout" target="basefrm" >'.translate('TEXT_LOGOUT_CONST').', '.$_SESSION['username'].'</a></td>					
				</tr>
		';
	} 	
	
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {

		$selectedInstallation = isset($_REQUEST['installationNo'])?(int)$_REQUEST['installationNo']:(int)$_SESSION['installationID'];

		//load the selected installation into SESSION (and check if is a valid installation for this user)
		if (in_array($selectedInstallation,$_SESSION['installationIDs'])) {
			$_SESSION['installationID'] = $selectedInstallation;
			$installationID=$_SESSION['installationID'];	
		} else {
			$selectedInstallation = $_SESSION['installationID'];
		}
		
		if ($treeType=='scenarios') {
			$jsTree='';
				$query = "SELECT * FROM Array Where CommandGroup = 1 Order BY Description ASC";	
				$res1 = $dbADO->_Execute($query);
					if ($res1) {
						while ($row1 = $res1->FetchRow()) {
							$jsTree.='				    
								auxS'.(100000+$row1['PK_Array']).' = insFld(foldersTree, gFld("'.removeCR($row1['Description']).'", "index.php?section=myScenarios&action=showArrayCommands&array='.$row1['PK_Array'].'"));
								auxS'.(100000+$row1['PK_Array']).'.xID = '.(100000+$row1['PK_Array']).';
							';
							
							$queryGetCommands = 'SELECT * FROM CommandGroup WHERE FK_Installation = ? AND FK_Array = ?';
							$resGetCommands = $dbADO->Execute($queryGetCommands,array($installationID,$row1['PK_Array']));
							
							while ($rowGetCommands = $resGetCommands->FetchRow()) {
								$jsTree.='				    
									auxSa'.$rowGetCommands['PK_CommandGroup'].' = insFld(auxS'.(100000+$row1['PK_Array']).', gFld("'.removeCR($rowGetCommands['Description']).(($rowGetCommands['Hint']!='')?': '.$rowGetCommands['Hint']:'').'", "index.php?section=editCommandGroup&cgID='.$rowGetCommands['PK_CommandGroup'].'"));
									auxSa'.$rowGetCommands['PK_CommandGroup'].'.xID = '.$rowGetCommands['PK_CommandGroup'].';
								';
							}
							
						}
					}			
		}	else {
				$jsTree='';

				$cachedLeftMenu=(getcwd()).'/cached/LeftMenu';
				$cachedDevices=(getcwd()).'/cached/CachedDevices';
				$cachedQuickJump=(getcwd()).'/cached/QuickJump';

				// get existing devices to compare them with old ones
				$devices=getAssocArray('Device','PK_Device','PK_Device',$dbADO,'WHERE FK_Installation='.$installationID,'ORDER BY PK_Device ASC');
				
				// if something changed in Device table, or the cached menu does not exist, rebuild menu
				if(file_exists($cachedLeftMenu)){
					$oldDevices=file($cachedDevices);
					$last_psc_mod=array_values(getAssocArray('Device','PK_Device','psc_mod',$dbADO,'','ORDER BY psc_mod DESC LIMIT 0,1'));
					if(serialize($devices)==trim($oldDevices[1]) && $last_psc_mod[0]==trim($oldDevices[0])){
						// get menu JS from cache
						$jsTree=join('',file($cachedLeftMenu));
						$quickJumpPulldown=join('',file($cachedQuickJump));
					}else{
						$rebuildMenu=1;
					}
				}else{
					$rebuildMenu=1;
				}
				
				if(@$rebuildMenu==1){
					// rebuild menu JS for devices

					$query1 = "select * from Device where FK_Device_ControlledVia IS NULL and FK_Installation = $installationID ORDER BY PK_Device ASC";
					$res1 = $dbADO->_Execute($query1);
					if ($res1) {
						while ($row1 = $res1->FetchRow()) {
							$jsTree.='
							auxS'.$row1['PK_Device'].' = insFld(foldersTree, gFld("'.str_replace('"','\"',removeCR($row1['Description'])).'", "index.php?section=editDeviceParams&deviceID='.$row1['PK_Device'].'"));
							auxS'.$row1['PK_Device'].'.xID = -'.$row1['PK_Device'].';
						';
							$jsTree.=getDeviceChilds($row1['PK_Device'],$dbADO);
							$devicesArray[$row1['PK_Device']]=$row1['PK_Device'].' '.$row1['Description'];
						}
					}
					
					// write menu and devices to catched files
					writeFile($cachedLeftMenu,$jsTree,'w');
					$last_psc_mod=array_values(getAssocArray('Device','PK_Device','psc_mod',$dbADO,'','ORDER BY psc_mod DESC LIMIT 0,1'));
					$IDs=$last_psc_mod[0]."\n";
					$IDs.=serialize($devices);
					writeFile($cachedDevices,$IDs,'w');
					// reorder devices based on PK_Device
					asort($devicesArray,SORT_NUMERIC);
					$quickJumpPulldown=pulldownFromArray($devicesArray,'quickJump',0,'');
					writeFile($cachedQuickJump,$quickJumpPulldown,'w');
				}
		}

			
			$installationTxt = '';
			
				$installationTxt.='
					
					'.translate('TEXT_CHANGE_INSTALLATION_CONST').':
					<input type="hidden" name="section" value="leftMenu">
					<input type="hidden" name="rightSection" value="'.$currentSection.'">
					<select name="installationNo" onChange="parent.frames[1].location.href=\'index.php?section=userHome&reloadTree=false\';this.form.submit();">';
			
					foreach ($_SESSION['installationIDs'] as $elem) {
						$installationTxt.="<option ".($selectedInstallation==$elem?" selected ":'')." value='".$elem."'>$elem</option>";
					}

					$installationTxt.='</select> <input type="submit" class="button" name="submitX" value="'.translate('TEXT_GO_CONST').'">
				
				';
			ksort($devicesArray,SORT_NUMERIC);
			$out.='
			<form action="index.php" method="post" name="leftMenu">
			<tr>
				<td><hr /></td>
			</tr>
			<tr>
				<td>
					'.$installationTxt.'
				</td>
			</tr>';
			if ($treeType!='scenarios'){
				$out.='
			<tr>
				<td align="right">&nbsp;<fieldset><legend>'.translate('TEXT_GO_TO_DEVICE_CONST').'</legend>
					<table>
						<tr>
							<td>'.$quickJumpPulldown.'</td>
							<td><input type="button" class="button" value="'.translate('TEXT_QUICK_JUMP_CONST').'" onClick="showDevice();"></td>
						</tr>
					</table>
				</fieldset>
				</td>
			</tr>';
			}
			$out.='
			</form>			
			';		
			
			$scriptInHead = "
	
			<!-- As in a client-side built tree, all the tree infrastructure is put in place
			     within the HEAD block, but the actual tree rendering is trigered within the
			     BODY -->
			
			<!-- Code for browser detection -->
			<script src=\"scripts/treeview/ua.js\"></script>
			
			<!-- Infrastructure code for the tree -->
			<script src=\"scripts/treeview/ftiens4.js\"></script>
			
			<!-- Execution of the code that actually builds the specific tree.
			     The variable foldersTree creates its structure with calls to
				 gFld, insFld, and insDoc -->
			<script>
			USETEXTLINKS = 1
			STARTALLOPEN = 0
			USEFRAMES = 1
			USEICONS = 1
			WRAPTEXT = 1
			PRESERVESTATE = 1
			ICONPATH = 'scripts/treeview/' 
			HIGHLIGHT = 1
			GLOBALTARGET = 'R'
			
			foldersTree = gFld('<b>".($treeType=='scenarios'?'My Scenarios':'My Devices')."</b>', \"index.php?section=".($treeType=='scenarios'?'myScenarios':'myDevices')."&action=showBasicInfo\");
			foldersTree.xID = 1001635872
			$jsTree
			
			".($treeType=='devices'?"
								auxS = insFld(foldersTree, gFld(\"".translate('TEXT_ADD_TOP_LEVEL_DEVICE_CONST')."\", \"index.php?section=addMyDevice&parentID=0\"));
								auxS.xID = 0;
								auxS.iconSrc = ICONPATH + \"diffFolder.gif\"
			  					auxS.iconSrcClosed = ICONPATH + \"diffFolder.gif\" 
			":'')."
			
			foldersTree.treeID = 't2';
			";
			// set highlighted device
			if((int)@$_REQUEST['deviceID']!=0)
				$scriptInHead.="highlightObjLink(auxS".((int)@$_REQUEST['deviceID']).");";

			$scriptInHead.="
			function getObj(obj) {
					if (document.layers) {
						if (typeof obj == 'string') {
							return document.layers[obj];
						} else 	{
							return obj;
						}
					}
					if (document.all) {
						if (typeof obj == 'string') {
							return document.all(obj);
						} else {
							return obj;
						}
					}
					if (getObj) {
						if (typeof obj == 'string') {
							return getObj(obj);
						} else {
							return obj;
						}
					}
					return null;	
				} 
			
				function showHideObject(obj) {
					obj = getObj(obj);					
					if(document.all || getObj) {  			
							if (obj.style.visibility == \"visible\") {
								obj.style.visibility = \"hidden\";
								obj.style.display = \"none\";
							} else {
							    obj.style.visibility = \"visible\";
								obj.style.display = \"block\";
							}
					} else if (document.layers) {
							if (obj.style.visibility == \"visible\") {
								obj.visibility = \"hide\";	
							} else {
							obj.visibility = \"show\";	
							}
					}
				} 
			
			function showDevice()
			{
				if(document.leftMenu.quickJump.value!=0)
					top.basefrm.location='index.php?section=editDeviceParams&deviceID='+document.leftMenu.quickJump.value;
			}				
			
			
			</script>
			";
			
			$out.= '
				<tr valign="top">
					<td valign="top" nowrap>
						<span style="font-color:#FFFFFF">
								<table border=0 valign="top">
									<tr style="display:none">
										<td>
											<font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font>
										</td>
									</tr>
								</table>
						</span>
								<span>
								<script>
									initializeDocument();						
								</script>
								<noscript>
								A tree for site navigation will open here if you enable JavaScript in your browser.
								</noscript>
					</td>
				</tr>
			<tr>
				<td>
					<br /><a href="index.php?section=leftMenu&treeType='.($treeType=='scenarios'?'devices':'scenarios').'">'.($treeType=='scenarios'?translate('TEXT_MY_DEVICES_CONST'):translate('TEXT_MY_SCENARIOS_CONST')).'</a>
				</td>
			</tr>
			';


			$output->setScriptInHead($scriptInHead);

	}



	$out.='</table>';

	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


function getDeviceChilds($parentID,$dbADO) {
	global $devicesArray;
	$queryGP = "select * from Device where FK_Device_ControlledVia = $parentID ORDER BY PK_Device ASC";
	$resGP = $dbADO->Execute($queryGP);
	$jsTree='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$jsTree.= '
					auxS'.$row['PK_Device'].' = insFld(auxS'.$parentID.', gFld("'.str_replace('"','\"',removeCR($row['Description'])).'", "index.php?section=editDeviceParams&deviceID='.$row['PK_Device'].'"))
					auxS'.$row['PK_Device'].'.xID = '.$row['PK_Device'].';
				';
				$jsTree.=getDeviceChilds($row['PK_Device'],$dbADO);
				$devicesArray[$row['PK_Device']]=$row['PK_Device'].' '.$row['Description'];
		}
	}
	return $jsTree;
}

function removeCR($str){
	$str=str_replace(array("\r\n","\n\r","\r","\n")," ",$str);
	
	return $str;
}
?>
