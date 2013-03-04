<?php
function distroList($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/distroList.lang.php');

	//$dbADO->debug=true;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	$PK_Distro = (isset($_REQUEST['PK_Distro']))?cleanInteger($_REQUEST['PK_Distro']):0;
	$FK_OperatingSystem=(isset($_REQUEST['FK_OperatingSystem']))?$_REQUEST['FK_OperatingSystem']:0;

	$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
	';
	$out.="&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addDistro&from=distroList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_DISTRO_CONST</a><br><br>";

	$out.='
 	<form action="index.php" method="post" name="distroList">
	<input type="hidden" name="section" value="distroList">

	'.$TEXT_FILTER_BY_OPERATING_SYSTEM_CONST.'
	<table>
 	<tr>
		<td>
			<select name="FK_OperatingSystem" onChange="">
			<option value="0" >-'.$TEXT_ANY_CONST.'-</option>
	';
			$queryOperatingSystem = "SELECT * FROM OperatingSystem ORDER BY Description ASC";
			$rs = $dbADO->Execute($queryOperatingSystem);
			while ($row = $rs->FetchRow()) {
				$selected=($FK_OperatingSystem==$row['PK_OperatingSystem'])?'selected':'';
			        $out.='
				<option value="'.$row['PK_OperatingSystem'].'" '.$selected.' >'.$row['Description'].'</option>';
			}
			$rs->Close();
	$out.='
			</select>
		</td>
	</tr>
	<tr>
		<td><input type="submit" class="button" name="submitX" value="'.$TEXT_APPLY_CONST.'"></td>
	</tr>
	</table>
	</form>
	';

	$querySelectDistros = "
		SELECT	Distro.*,
				OperatingSystem.Description AS OperatingSystemName
			FROM Distro
				LEFT JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem ";
	if($FK_OperatingSystem!=0)
		$querySelectDistros.=" WHERE FK_OperatingSystem=".$FK_OperatingSystem." ";
	$querySelectDistros.=" ORDER BY OperatingSystem.Description ASC, Distro.Description ASC";
	$rs = $dbADO->Execute($querySelectDistros);

	if($rs->RecordCount()==0)
		$out.=$TEXT_NO_RECORDS_CONST.'<br>';

	$out.='
	<B>'.$TEXT_DISTRIBUTION_CONST.'</B>
	<table border="1">
	';

	$firstOperatingSystem=$TEXT_NO_DISTROS_CONST;
	while ($row = $rs->FetchRow()) {
		if($firstOperatingSystem!=$row['OperatingSystemName']){
			$out.='
			<tr>
				<td colspan="2"><b>OS: '.(($row['OperatingSystemName']!='')?$row['OperatingSystemName']:$TEXT_NO_OPERATINGSYSTEM_CONST).'</b></td>
			</tr>
			';
			$firstOperatingSystem=$row['OperatingSystemName'];
		}
		$out.='
		<tr>
			<td>'.$row['PK_Distro'].'</td>
			<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDistro&subAction=edit&from=distroList&PK_Distro='.$row['PK_Distro'].'\',\'status=0,resizable=1,width=700,height=700,toolbar=1,scrollbars=1\');">'.$row['Description'].'</a>
		</tr>
		';
	}
	$rs->Close();
	$out.='</table>';
	$out.="<br><br>&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addDistro&from=distroList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_DISTRO_CONST</a><br><br>";

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_DISTRO_CONST);
	$output->setNavigationMenu(array($TEXT_DISTRO_CONST=>'index.php?section=distroList'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_DISTRO_CONST);
	$output->output();
}
?>
