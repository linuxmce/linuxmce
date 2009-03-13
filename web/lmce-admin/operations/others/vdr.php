<?
function vdr($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/vdr.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	$vdr_plugin=getDeviceFromDT($installationID,$GLOBALS['VDR_Plugin'],$dbADO);	
	$selectedProvider=getDeviceData($vdr_plugin,$GLOBALS['DD_provider'],$dbADO);

	// check if database pluto_vdr exist
	$databases=$dbADO->MetaDatabases();
	if(!in_array('pluto_vdr',$databases)){
		error_redirect($TEXT_PACKAGE_NOT_INSTALLED_CONST,'index.php?section=vdr');
	}
	
	
	if($action=='form'){
		
		$service=0;
		if($selectedProvider!=0 && !is_null($selectedProvider)){
			$providerData=getFields('pluto_vdr.Provider','PK_Provider,Description,filename,FK_ProviderType,FTA,Radio,CA',$dbADO,'WHERE PK_Provider='.$selectedProvider);
			$service=$providerData[0]['FK_ProviderType'];
		}	
		
		$service=(isset($_REQUEST['service']))?(int)@$_REQUEST['service']:$service;
		$area=(isset($_REQUEST['area']))?cleanString($_REQUEST['area']):'--all--';
					
		$services=getAssocArray('pluto_vdr.ProviderType','PK_ProviderType','Description',$dbADO);
				
		if(isset($_REQUEST['service'])){
			$fta_flag=(int)@$_REQUEST['fta_flag'];
			$radio_flag=(int)@$_REQUEST['radio_flag'];
			$ca_flag=(int)@$_REQUEST['ca_flag'];
		}elseif($selectedProvider!=0){
			$fta_flag=$providerData[0]['FTA'];
			$radio_flag=$providerData[0]['Radio'];
			$ca_flag=$providerData[0]['CA'];
		}else{
			$fta_flag=1;
			$radio_flag=1;
			$ca_flag=1;
		}
				
		$out.='
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="vdr" enctype="multipart/form-data">
			<input type="hidden" name="section" value="vdr">
			<input type="hidden" name="action" value="add">
		
			<table>
				<tr>
					<td><b>'.$TEXT_CHOOSE_TV_SERVICE_CONST.'</b></td>
					<td>'.pulldownFromArray($services,'service',$service,'onChange="document.vdr.action.value=\'form\';document.vdr.area.value=\'--all--\';document.vdr.submit();"').'
						<input type="checkbox" name="fta_flag" value="1" '.(($fta_flag==1)?'checked':'').' title="'.htmlentities($TEXT_FTA_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> FTA  
						<input type="checkbox" name="radio_flag" value="1" '.(($radio_flag==1)?'checked':'').' title="'.htmlentities($TEXT_RADIO_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> Radio 
						<input type="checkbox" name="ca_flag" value="1" '.(($ca_flag==1)?'checked':'').' title="'.htmlentities($TEXT_CA_CONST).'" onClick="document.vdr.action.value=\'form\';document.vdr.submit();"> CA
					</td>
				</tr>
				';
			if($service!=0){
				$where="WHERE FK_ProviderType=$service AND FTA=$fta_flag AND Radio=$radio_flag AND CA=$ca_flag";
				$areas=array('--all--'=>'All')+getAssocArray('pluto_vdr.Provider','DISTINCT Area AS Area','Area',$dbADO,$where,'ORDER BY Area ASC');
				
				$where.=($area!='--all--')?' AND area=\''.$area.'\'':'';
				$providers=getFields('pluto_vdr.Provider','PK_Provider,Description,filename,Area',$dbADO,$where);
				
				if(count($providers)>0){
					$out.='
					<tr>
						<td><b>'.$TEXT_AREA_CONST.'</b></td>
						<td>'.pulldownFromArray($areas,'area',$area,'onChange="document.vdr.action.value=\'form\';document.vdr.submit();"','key','').'</td>
					</tr>
					<tr>
						<td colspan="2">
						<table width="100%" cellpadding="1" cellspacing="1">
							<tr>
								<td colspan="4"  style="background:#FFFFFF;"><b>'.$TEXT_PROVIDERS_CONST.'</b></td>
							</tr>';
							for($i=0;$i<count($providers);$i++){
								$color=($providers[$i]['PK_Provider']==$selectedProvider)?'#DEDEDE':'#FFFFFF';
								$out.='
								<tr>
									<td  style="background:'.$color.';">'.$providers[$i]['Description'].'</td>
									<td  style="background:'.$color.';">'.$providers[$i]['Area'].'</td>
									<td  style="background:'.$color.';">'.$providers[$i]['filename'].'</td>
									<td  style="background:'.$color.';" align="right"><a href="index.php?section=vdr&action=pick&providerID='.$providers[$i]['PK_Provider'].'">Pick</a></td>
								</tr>						
								';
							}
						$out.='</table></td>
							</tr>';	
				}else{
					$out.='
					<tr>
						<td>'.$TEXT_NO_RECORDS_CONST.'</td>
						<td></td>
					</tr>';
				}				
			}elseif($selectedProvider==0){
					$out.='
					<tr>
						<td>'.$TEXT_CUSTOM_PROVIDER_SELECTED_CONST.'</td>
						<td></td>
					</tr>';
			}
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td></td>
				</tr>			
				<tr>
					<td colspan="2">'.$TEXT_UPLOAD_FILE_INFO_CONST.'</td>
				</tr>			
				<tr>
					<td><b>'.$TEXT_UPLOAD_CUSTOM_FILE_CONST.'</b></td>
					<td><input type="file" name="cfg_file" value=""> <input type="submit" class="button" name="upload" value="'.$TEXT_UPLOAD_CONST.'"></td>
				</tr>			
				
			</table>
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=vdr&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	
		if(isset($_POST['upload'])){
			setVDR($_FILES['cfg_file']['tmp_name']);
			set_device_data($vdr_plugin,$GLOBALS['DD_provider'],0,$dbADO);
			
			header("Location: index.php?section=vdr&msg=".urlencode('Provider updated.'));
			exit();
		}

		
		$providerID=(int)@$_REQUEST['providerID'];
		if($providerID!=0){
			$providerData=getFields('pluto_vdr.Provider','PK_Provider,Description,filename',$dbADO,'WHERE PK_Provider='.$providerID);
			if(count($providerData)==0){
				header("Location: index.php?section=vdr&error=".urlencode('Provider not found'));
				exit();
			}else{
				
				setVDR($GLOBALS['vdr_from_path'].$providerData[0]['filename']);
				set_device_data($vdr_plugin,$GLOBALS['DD_provider'],$providerID,$dbADO);
				
				header("Location: index.php?section=vdr&msg=".urlencode('Provider updated.'));
				exit();
			}
		}
		
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_VDR_SETUP_CONST);
	
	$output->setNavigationMenu(array($TEXT_VDR_SETUP_CONST=>'index.php?section=vdr'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_VDR_SETUP_CONST);
	$output->output();
}

function setVDR($filename){

	$cmd='sudo -u root /usr/pluto/bin/setVDR.sh "'.$filename.'"';
	exec_batch_command($cmd);
}
?>