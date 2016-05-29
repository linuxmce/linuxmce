<?php
function sqlcvs_update($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/sqlcvs.lang.php');
	
	global $dbPlutoMainDatabase;
	global 	$dbMythType, $dbMythUser, $dbMythPass, $dbMythServer;
	$dbPlutoMythDatabase='pluto_myth';
	$databasesArray=array($dbPlutoMainDatabase=>'pluto_main',$dbPlutoMythDatabase=>'pluto_myth');
	
	$database=(isset($_REQUEST['database']))?$_REQUEST['database']:$dbPlutoMainDatabase;
	if($database==$dbPlutoMainDatabase){
		$sqlcvsADO=$dbADO;
	}else{
	  	$sqlcvsADO= &ADONewConnection('mysqli');
	  	$sqlcvsADO->NConnect($dbMythServer,urlencode($dbMythUser),urlencode($dbMythPass),urlencode($dbPlutoMythDatabase));
	}

	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$tablesArray=$sqlcvsADO->MetaTables('TABLES');	
		
	if ($action == 'form') {
		$out.='
		<script>
		function selAllCheckboxes(group,val)
		{
		   eval("sqlcvs_update.table_"+group+".checked="+val);
		   for (i = 0; i < sqlcvs_update.elements.length; i++)
		   {
			tmpName=sqlcvs_update.elements[i].name;
		     if (sqlcvs_update.elements[i].type == "checkbox" && tmpName.indexOf(group+"_")!=-1)
		     {
		         sqlcvs_update.elements[i].checked = val;
		     }
		   }
		}
		
		function groupCheck(group,tablename){
			eval("val=(document.sqlcvs_update.table_"+group+".checked)?true:false");
			eval("tval=(document.sqlcvs_update."+group+"_"+tablename+".checked)?true:false");
			if(tval==true && val==false){
				eval("sqlcvs_update.table_"+group+".checked=true");
			}
		}
		</script>
				
		<div class="err" align="center">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="sqlcvs_update">
		<input type="hidden" name="section" value="sqlcvs_update">
		<input type="hidden" name="action" value="add">	
		
	<table width="400" cellpadding="3" cellspacing="0">
		<tr>
			<td colspan="3"><B>'.$TEXT_SQLCVS_HOST_CONST.':</B></td>
			<td><input type="text" name="host" value="'.((isset($_REQUEST['host']))?$_REQUEST['host']:'schema.linuxmce.org').'"></td>
		</tr>
		<tr>
			<td colspan="3"><B>'.$TEXT_PORT_CONST.':</B></td>
			<td><input type="text" name="port" value="'.(($database==$dbPlutoMainDatabase)?'3999':'4000').'"></td>
		</tr>		
		<tr>
			<td colspan="3"><B>'.$TEXT_USERNAME_CONST.':</B></td>
			<td><input type="text" name="username" value="'.((isset($_REQUEST['username']))?@$_REQUEST['username']:'anonymous').'"></td>
		</tr>
		<tr>
			<td colspan="3"><B>'.$TEXT_PASSWORD_CONST.':</B></td>
			<td><input type="text" name="password" value="'.((isset($_REQUEST['password']))?@$_REQUEST['password']:'nopass').'"></td>
		</tr>
		<tr>
			<td colspan="3"><B>'.$TEXT_DATABASE_CONST.':</B></td>
			<td>'.pulldownFromArray($databasesArray,'database',$database,'onchange="document.sqlcvs_update.action.value=\'form\';document.sqlcvs_update.submit();"','key','').'</td>
		</tr>
		<tr>
			<td colspan="4">&nbsp;</td>
		</tr>
		<tr>
			<td colspan="4">'.$TEXT_SQLCVS_UPDATE_HINT.'</td>
		</tr>
		<tr>
			<td colspan="4">&nbsp;</td>
		</tr>
		<tr>
			<td><input type="submit" class="button" name="update" value="'.$TEXT_NEXT_CONST.'"></td>
			<td colspan="3">&nbsp;</td>
		</tr>		
		';
	

	for($i=0;$i<count($tablesArray);$i++){
		$table=$tablesArray[$i];
		if(preg_match('/psc_(.*)_tables/',$table)){
			$cleanTable=str_replace(array('psc_','_tables'),array('',''),$table);
			if($cleanTable!='local'){
				$out.='
				<tr bgcolor="#F0F3F8">
					<td width="50">&nbsp;</td>
					<td width="20">
						<input type="checkbox" name="table_'.$cleanTable.'" value="1">			
					</td>
					<td colspan="2"><B>'.$cleanTable.'</B></td>
				</tr>
				<tr bgcolor="#F0F3F8">
					<td colspan="2">&nbsp;</td>
					<td colspan="2"><a href="javascript:selAllCheckboxes(\''.$cleanTable.'\',true);">[ Check all ]</a> <a href="javascript:selAllCheckboxes(\''.$cleanTable.'\',false);">[ Uncheck all ]</a></td>
				</tr>';
				$fieldsArray=getAssocArray($table,'PK_'.$table,'Tablename',$sqlcvsADO,'','ORDER BY Tablename ASC');
				foreach ($fieldsArray AS $key=>$value){
					$out.='
				<tr>
					<td width="50">&nbsp;</td>
					<td width="20">&nbsp;</td>
					<td width="20"><input type="checkbox" name="'.$cleanTable.'_'.$value.'" value="1" onClick="groupCheck(\''.$cleanTable.'\',\''.$value.'\')"></td>
					<td>'.$value.'</td>
				</tr>';
				}
			}
		}
	}	
	$out.='	
		<tr>
			<td><input type="submit" class="button" name="update" value="'.$TEXT_NEXT_CONST.'"></td>
			<td colspan="3">&nbsp;</td>
		</tr>		
	</table>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sqlcvs_update&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$host=stripslashes($_POST['host']);
		$port=(int)$_POST['port'];
		$username=stripslashes($_POST['username']);
		$password=stripslashes($_POST['password']);
		$rParmArray=array();
		$tParmArray=array();
		$tParmByRepArray=array();
		
		if($host=='' || $port==''){
			header("Location: index.php?section=sqlcvs_update&error=$TEXT_ERROR_HOST_OR_PORT_NOT_SPECIFIED_CONST");
			exit();			
		}
		
		for($i=0;$i<count($tablesArray);$i++){
			$table=$tablesArray[$i];
			if(preg_match('/psc_(.*)_tables/',$table)){
				$cleanTable=str_replace(array('psc_','_tables'),array('',''),$table);
				if(isset($_POST['table_'.$cleanTable])){
					$rParmArray[]=$cleanTable;	
				}
				$fieldsArray=getAssocArray($table,'PK_'.$table,'Tablename',$sqlcvsADO,'','ORDER BY Tablename ASC');
				foreach ($fieldsArray AS $key=>$value){
					if(isset($_POST[$cleanTable.'_'.$value])){
						if(!in_array($cleanTable,$rParmArray)){
							$rParmArray[]=$cleanTable;	
						}
						$tParmArray[]=$value;
						$tParmByRepArray[$cleanTable][]=$value;
					}
				}			
			}
		}
		
		$parmList='-r '.join(',',$rParmArray).' -t ';
		foreach ($rParmArray AS $rep){
			if(!isset($tParmByRepArray[$rep])){
				header("Location: index.php?section=sqlcvs_update&error=$TEXT_ERROR_NO_TABLE_SELECTED_CONST");
				exit();
			}
		}
		$parmList.=join(',',$tParmArray);

		$cmd='/usr/pluto/bin/sqlCVS -R '. $port.' -H '.$host.' -h localhost -a -n '.$parmList.' -d "'.$username.'" -U "'.$username.'~'.$password.'" -D '.$database.' -e update';

		$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
			windowOpen(\'operations/logs/executeLog.php?script=2&command='.urlencode($cmd).'\',\'width=1024,height=768,scrollbars=1,resizable=1\')
			self.location="index.php?section=sqlcvs_update";		
		</script>';
		
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_SQLCVS_UPDATE_CONST);
	$output->setNavigationMenu(array($TEXT_SQLCVS_UPDATE_CONST=>'index.php?section=sqlcvs_update'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SQLCVS_UPDATE_CONST);
	$output->output();
}
?>
