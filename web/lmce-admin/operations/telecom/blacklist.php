<?php
function blacklist($output,$astADO,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	//includeLangFile('blacklist.lang.php');
	
	$out='';
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	if ($action == 'form') {
	
	$cmd='sudo -u root /usr/sbin/asterisk -rx "database show blacklist"';
		$numbers=exec_batch_command($cmd,1);
		$numbersarr=explode("\n",$numbers);
		$num=count($numbersarr);
		$num=$num-1;
		unset($numbersarr[$num]);
		$num=$num-1;
		unset($numbersarr[$num]);
		$num=$num-1;
		$countnumbers=$numbersarr[$num];
		unset($numbersarr[$num]);
		
	$out.='<script>
	function confirmDeleteRule(Number)
	{
		if(confirm("Are you sure you want to delete "+Number+" from the blacklist?")){
			document.getElementById(\'blacklist\').remove.value=Number;
			document.getElementById(\'blacklist\').action.value="remove";
			document.getElementById(\'blacklist\').submit();
		}
	}</script><p><center>'.$countnumbers.'</center></p><table border="0" align="center">
			<form id="blacklist" action="index.php" method="POST" name="blacklist">
			<input type="hidden" name="section" value="blacklist">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="add" value="add">
			<tr class="tablehead">
				<td>remove</td>
				<td>CallerID</td>
				<td>Note</td>
				<td></td>
			</tr>';
			foreach ($numbersarr as $number) {
				$number=explode(':', $number);
				$number['0']=str_replace("/blacklist/", "", $number['0']);
				$number['0']=trim($number['0'], " \t.");
				$out.='<tr><td><input type="checkbox" name="remove" value="'.$number['0'].'" onclick="if (this.checked) { confirmDeleteRule(\''.$number["0"].'\') }" /> </td><td>'.$number['0'].'</td><td>'.$number['1'].'</td></tr>';
			}
			$out.='<tr><td></td><td><input type="number" name="CallerID" /></td><td><input type="text" name="Note"  /></td><td><input type="submit" class="button" name="save" value="save" /><input type="reset" class="button" name="clear" value="reset" /></td>
			</tr>
			</form>
		   </table>';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=blacklist&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
		
		if(isset($_POST['add'])){
			$callerid=$_POST['CallerID'];		
			$note=$_POST['Note'];
			$cmd='sudo -u root /usr/sbin/asterisk -rx "database put blacklist '.$callerid.' \"'.$note.'\""';
			$response=exec_batch_command($cmd,1);
		}
		
		if($_POST['action'] == 'remove') {
			$callerid=$_POST['remove'];
			$cmd='sudo -u root /usr/sbin/asterisk -rx "database del blacklist '.$callerid.'"';
			$response=exec_batch_command($cmd,1);
		}
		
		header("Location: index.php?section=blacklist&msg=".$response);
	
	}
	$output->setMenuTitle(translate('TEXT_TELECOM_CONST').' |');
	$output->setPageTitle(translate('TEXT_BLACKLIST_CONST_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_BLACKLIST_CONST')=>'index.php?section=blacklist'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_BLACKLIST_CONST'));
	$output->output();
}

?>