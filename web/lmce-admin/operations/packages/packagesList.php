<?php
function packagesList($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/packagesList.lang.php');
	
	//$dbADO->debug=true;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$PK_Package = (isset($_REQUEST['PK_Package']))?cleanInteger($_REQUEST['PK_Package']):0;
	$packageOrder=(isset($_REQUEST['packageOrder']))?$_REQUEST['packageOrder']:0;
	$showDependancy=(isset($_REQUEST['showDependancy']))?$_REQUEST['showDependancy']:0;

	$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
	';
	$out.="&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=packagesList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_PACKAGE_CONST</a><br><br>";
	$out.='
	<form action="index.php" method="get" name="packagesList">
	<input type="hidden" name="section" value="packagesList">
	<input type="hidden" name="action" value="form">
	<table>
		<tr>
			<td><input type="radio" name="packageOrder" value="0" onChange="document.packagesList.submit();" '.(($packageOrder==0)?'checked':'').'></td>
			<td><B>'.$TEXT_GROUP_BY_MANUFACTURER_CONST.'</B></td>
			<td><input type="checkbox" name="showDependancy" value="1" onChange="document.packagesList.submit();" '.(($showDependancy==1)?'checked':'').'> <B>'.$TEXT_SHOW_DEPENDENCIES_CONST.'</B></td>
		</tr>
		<tr>
			<td><input type="radio" name="packageOrder" value="1" onChange="document.packagesList.submit();" '.(($packageOrder==1)?'checked':'').'></td>
			<td><B>'.$TEXT_ALPHABETICAL_ORDER_CONST.'</B></td>
			<td>&nbsp;</td>
		</tr>

	</table>
	</form>
	';
	// order by manufacturer
	$firstManufacturer=$TEXT_NO_MANUFACTURER_CONST;
	if($packageOrder==0){
		$querySelectPackages = "
			SELECT 
				Package.*,
				Manufacturer.Description AS ManufacturerName
			FROM Package
				LEFT JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			ORDER BY Manufacturer.Description ASC, Package.Description ASC";
	}else{
		$querySelectPackages = "SELECT * FROM Package ORDER BY Description ASC";
	}
	$rs = $dbADO->Execute($querySelectPackages);
	if($rs->RecordCount()==0)
		$out.='No records.<br>';
	$out.='<table border="1">';
	while ($row = $rs->FetchRow()) {
		if($packageOrder==0){
			if($firstManufacturer!=$row['ManufacturerName']){
				$out.='
					<tr>
						<td colspan="2"><b>'.(($row['ManufacturerName']!='')?$row['ManufacturerName']:$TEXT_NO_MANUFACTURER_CONST).'</b></td>
					</tr>
				';
				$firstManufacturer=$row['ManufacturerName'];
			}
		}
		if($showDependancy==1){
	        $querySelectPackages = "
	        	SELECT * FROM Package_Package 
	        		LEFT JOIN Package ON Package.PK_Package = Package_Package.FK_Package_DependsOn 
	        	WHERE Package_Package.FK_Package='".$row['PK_Package']."' ORDER BY Description ASC";
	        $rsdeps = $dbADO->Execute($querySelectPackages);
	        $deps = "";
	        while ($deprow = $rsdeps->FetchRow()) {
	            $deps .= "
	            	<tr>
	            		<td>".$deprow['FK_Package_DependsOn']."</td>
	            	 ".'<td><a href="index.php?section=addPackageToMasterDevice&from=packagesList&PK_Package='.$deprow['FK_Package_DependsOn']."\" target=\"_blank\">".$deprow['Description']."</a></td>
	            	</tr>\n";
	        }
	        $queryUsedBy = "SELECT * FROM DeviceTemplate WHERE FK_Package='".$row['PK_Package']."' LIMIT 1";
	        $rsusedby = $dbADO->Execute($queryUsedBy);
	        $usedbyrow = $rsusedby->FetchRow();
	        $usedby = $usedbyrow["Description"];
		}
		$out.='
		<tr>
			<td>'.$row['PK_Package'].'</td>
			<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addPackageToMasterDevice&from=packagesList&PK_Package='.$row['PK_Package'].'\',\'status=0,resizable=1,width=700,height=700,toolbar=1,scrollbars=1\');">'.$row['Description'].'</a>';
		if($showDependancy==1){
			$out.='
            <td><table border="1">'.$deps.'</table></td>
            <td>'.$usedby.'</td>';
		}
		$out.='
		</tr>';
	}
	$rs->Close();
	$out.='</table>';
	$out.="<br><br>&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=packagesList','status=0,resizable=1,width=700,height=850,toolbar=1,scrollbars=1');\">$TEXT_CREATE_NEW_PACKAGE_CONST</a>";
	
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_PACKAGES_CONST);
	$output->setNavigationMenu(array($TEXT_PACKAGES_CONST=>'index.php?section=packagesList'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PACKAGES_CONST);			
	$output->output();
}
?>
