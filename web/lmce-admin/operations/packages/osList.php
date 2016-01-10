<?php
function osList($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/osList.lang.php');

	//$dbADO->debug=true;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
	';
	$out.='&nbsp; <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addOS&from=osList\',\'status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1\');">'.$TEXT_CREATE_NEW_OS_CONST.'</a><br><br>';

	$querySelectOSes = "SELECT * FROM OperatingSystem ORDER BY Description ASC";
	$rs = $dbADO->Execute($querySelectOSes);

	if($rs->RecordCount()==0)
		$out.='No records.<br>';

	$out.='<table border="1">';
	while ($row = $rs->FetchRow()) {
		$out.='
		<tr>
			<td>'.$row['PK_OperatingSystem'].'</td>
			<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addOS&from=osList&PK_OperatingSystem='.$row['PK_OperatingSystem'].'\',\'status=0,resizable=1,width=700,height=700,toolbar=1,scrollbars=1\');">'.$row['Description'].'</a>';
		$out.='
		</tr>';
	}
	$rs->Close();
	$out.='</table>';
	$out.="<br><br>&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addOS&from=osList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_OS_CONST</a><br><br>";

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_OPERATING_SYSTEM_CONST);
	$output->setNavigationMenu(array($TEXT_OPERATING_SYSTEM_CONST=>'index.php?section=osList'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_OPERATING_SYSTEM_CONST);
	$output->output();
}
?>
