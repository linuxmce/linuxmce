<?
function fullLog($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/logs.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$rowsPerPage=5000;
	$from=(isset($_REQUEST['from']))?(int)$_REQUEST['from']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='';
	$deviceID=(int)@$_REQUEST['deviceID'];
	$installationID=(int)@$_SESSION['installationID'];
	if($deviceID==0){
		die('Invalid Device ID specified.');
	}

	if ($action == 'form') {
		$selectDevice='
			SELECT Device.Description,PK_Device,DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory,FK_DeviceTemplate,CommandLine
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			WHERE PK_Device=?';
		$resDevice=$dbADO->Execute($selectDevice,$deviceID);
		$rowDevice=$resDevice->FetchRow();
		$isGSD=($rowDevice['CommandLine']=='Generic_Serial_Device')?1:0;
		
		$logName=getLogName($deviceID,$rowDevice['FK_DeviceTemplate'],$rowDevice['Template'],(int)@$_REQUEST['parentID'],(int)@$_REQUEST['orbiter'],$isGSD,$rowDevice['CommandLine']);
		$pagesArray=array();
		if(file_exists($logName)){
			exec('wc -l '.$logName,$retArray);	// (afisheaza: <nr. linii> <spatziu> <nume fishier>)
			$total=explode(' ',$retArray[0]);
			
			$to=(($from+$rowsPerPage)>=$total[0])?$total[0]:($from+$rowsPerPage);
			$nrPages=ceil($total[0]/$rowsPerPage);
			
			$currentPage=$from/$rowsPerPage;
			for($i=0;$i<$nrPages;$i++){
				$pagesArray[]=($i==$currentPage)?($i+1):'<a href="fullLog.php?deviceID='.$deviceID.'&from='.($i*$rowsPerPage).'">'.($i+1).'</a>';
			}
			
			$aqquireCommand='awk \'NR>='.$from.' && NR<='.($from+$rowsPerPage).'\' '.$logName.' | /usr/pluto/bin/ansi2html';
			exec($aqquireCommand,$linesArray);
			$formatedText = join('<br>',$linesArray);
			$numbersNotice=$total[0].' lines, displaying from '.$from.' to '.$to;
		}else{
			$formatedText =$TEXT_ERROR_LOG_NOT_FOUND_CONST.': '.$logName;
		}
		
		
		$out= '
		<input type="hidden" name="action" value="add">	
		<h3>Complete log</h3>
			<table width="100%">
				<tr>
					<td valign="top" colspan="3">'.$TEXT_DEVICE_CONST.' <B>'.$rowDevice['Description'].'</B>, # <B>'.$rowDevice['PK_Device'].'</B> <td>
				</tr>
				<tr>
					<td valign="top" colspan="3">'.$TEXT_DEVICE_TEMPLATE_CONST.' <B>'.$rowDevice['Template'].'</B>, '.strtolower($TEXT_DEVICE_CATEGORY_CONST).' <B>'.$rowDevice['Category'].'</B>, '.strtolower($TEXT_MANUFACTURER_CONST).' <B>'.$rowDevice['Manufacturer'].'</B>.<td>
				</tr>
				<tr>
					<td valign="top" colspan="3"><hr><td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" align="center">'.@$numbersNotice.'<td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" align="center">'.join(' ',$pagesArray).'<td>
				</tr>	
				<tr>
					<td valign="top" colspan="3" style="background:black;color:white;"><pre>'.$formatedText.'<td>
				</tr>		
			</table>
			</form>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=fullLog&deviceID=$deviceID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		header("Location: index.php?section=fullLog&deviceID=$deviceID");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_FULL_LOG_CONST);
	$output->output();
}
?>