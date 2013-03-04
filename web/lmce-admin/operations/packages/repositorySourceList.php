<?php
function repositorySourceList($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/repositorySourceList.lang.php');

	//$dbADO->debug=true;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	$FK_Distro = (isset($_REQUEST['Distro']))?cleanInteger($_REQUEST['Distro']):0;
	$FK_OperatingSystem=(isset($_REQUEST['OperatingSystem']))?$_REQUEST['OperatingSystem']:0;

	$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
	';
	$out.="&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editRepositorySource&from=repositorySourceList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_REPOSITORY_SOURCE_CONST</a><br><br>";


	$out.='
 	<form action="index.php" method="post" name="repositorySourceList">
	<input type="hidden" name="section" value="repositorySourceList">

	'.$TEXT_FILTER_BY_CONST.'
	<table>
 	<tr>
		<td>
			<select name="OperatingSystem" onChange="">
			<option value="0" >-'.$TEXT_ANY_CONST.'-</option>
	';
			$queryOperatingSystem = "SELECT * FROM OperatingSystem ORDER BY Description ASC";
			$rs = $dbADO->Execute($queryOperatingSystem);
			while ($row = $rs->FetchRow()) {
				$selected=($FK_OperatingSystem==$row['PK_OperatingSystem'])?'selected':'';
			        $out.='
				<option value="'.$row['PK_OperatingSystem'].'" '.$selected.' >'.$row['Description'].'</option>
				';
			}
			$rs->Close();
 	$out.='
			</select>
		</td>
		<td>
			<select name="Distro" onChange="">
			<option value="0" >-'.$TEXT_ANY_CONST.'-</option>
	';
			if ($FK_OperatingSystem!=0) {
				$queryDistro.=" SELECT * FROM Distro WHERE FK_OperatingSystem=? ORDER BY Description ASC";
				$rs = $dbADO->Execute($queryDistro,array($FK_OperatingSystem));
			} else {
				$queryDistro = "SELECT * FROM Distro ORDER BY Description ASC";
				$rs = $dbADO->Execute($queryDistro);
			}
			while ($row = $rs->FetchRow()) {
				$selected=($FK_Distro==$row['PK_Distro'])?'selected':'';
			        $out.='
				<option value="'.$row['PK_Distro'].'" '.$selected.' >'.$row['Description'].'</option>
				';
			}
			$rs->Close();
	$out.='
			</select>
		</td>
		<td></td>
	</tr>
	<tr>
		<td><input type="submit" class="button" name="submitX" value="'.$TEXT_APPLY_CONST.'"></td>
	</tr>
	</table>
	</form>
	';

	$querySelectRepositorySources = " SELECT RepositorySource.*,
				OperatingSystem.Description AS OperatingSystemName,
				Distro.Description AS DistroName
			FROM RepositorySource
				LEFT JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem 
				LEFT JOIN Distro ON FK_Distro=PK_Distro ";
	if($FK_Distro!=0) {
		$querySelectRepositorySources.=" WHERE PK_Distro=".$FK_Distro;
	} elseif($FK_OperatingSystem!=0) {
		$querySelectRepositorySources.=" ".($FK_Distro?"AND":"WHERE")." PK_OperatingSystem=".$FK_OperatingSystem;
	}
	$querySelectRepositorySources.=" ORDER BY OperatingSystem.Description ASC, Distro.Description ASC";
	$rs = $dbADO->Execute($querySelectRepositorySources);

	if($rs->RecordCount()==0)
		$out.=$TEXT_NO_RECORDS_CONST.'<br>';

	$out.='
	<h3>'.$TEXT_REPOSITORY_SOURCES_CONST.'</h3>
	';


	$firstOperatingSystem=$TEXT_UNDEFINED_OPERATING_SYSTEM_CONST;
	$firstDistro=$TEXT_UNDEFINED_DISTRO_CONST;
	while ($row = $rs->FetchRow()) {
		if($firstOperatingSystem!=$row['OperatingSystemName']) {
			if ($firstOperatingSystem!=$TEXT_UNDEFINED_OPERATING_SYSTEM_CONST) {
				$out.='
				</table>
				</fieldset>
				';
			}
			$out.='
			<fieldset>
			<legend>OS: '.(($row['OperatingSystemName']!='')?$row['OperatingSystemName']:$TEXT_UNDEFINED_OPERATING_SYSTEM_CONST).'</legend>
			<table border="0">
			';
		}

		if($firstDistro!=$row['DistroName'] || $firstOperatingSystem!=$row['OperatingSystemName']) {
			if ($firstDistro!=$TEXT_UNDEFINED_DISTRO_CONST && $firstOperatingSystem==$row['OperatingSystemName']) {
				$out.='
				<tr><td colspan="2">&nbsp;</td></tr>
				';
			}
			$out.='
				<tr>
				<td colspan="2">'.$TEXT_DISTRIBUTION_CONST.': '.(($row['DistroName']!='')?$row['DistroName']:$TEXT_UNDEFINED_DISTRO_CONST).'</td>
				</tr>
			';
			$firstDistro=$row['DistroName'];
		}
		$firstOperatingSystem=$row['OperatingSystemName'];

		$out.='
			<tr>
				<td>'.$row['PK_RepositorySource'].'</td>
				<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editRepositorySource&from=repositorySourceList&RepositorySource='.$row['PK_RepositorySource'].'\',\'status=0,resizable=1,width=700,height=700,toolbar=1,scrollbars=1\');">'.$row['Description'].'</a>
			</tr>
		';
	}
	$rs->Close();

	$out.='
		</table>
		</fieldset>
	';

	$out.="<br><br>&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editRepositorySource&from=repositorySourceList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_REPOSITORY_SOURCE_CONST</a><br><br>";

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_REPOSITORY_SOURCES_CONST);
	$output->setNavigationMenu(array($TEXT_REPOSITORY_SOURCES_CONST=>'index.php?section=repositorySourceList'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_REPOSITORY_SOURCES_CONST);
	$output->output();
}
?>
