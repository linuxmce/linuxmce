<?
function viewDatalog($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$deviceID=(int)$_REQUEST['deviceID'];
	$installationID = (int)@$_SESSION['installationID'];


		$out='
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">'.$display.'</td>
			</tr>		
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=viewDatalog&deviceID='.$deviceID.'\';
		}
			setTimeout("doReload()",60000);
		</script>';

	$PK_Datalogger = getDeviceFromDT($installationID,1949 /* DeviceTemplate_DataLogger_Plugin */,$dbADO);
	$deviceString = getDeviceData($PK_Datalogger,59 /* Configuration */,$dbADO);
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if ($action == 'addDevice') {
	   $addDevice = $_REQUEST['addDevice'];
	   if (strpos($deviceString, $addDevice) !== false) {
	       $out.='<p>Device already listed</p>';
	   } else {
	       if (strlen($deviceString) > 0) {
	           $deviceString.=',';
	       }
	       $deviceString.=$addDevice;
	       set_device_data($PK_Datalogger,59,$deviceString,$dbADO);
	       $out.='<p>Graph added for device '.$addDevice.'</p>';
	   }
	} else if ($action == 'removeDevice') {
	   $removeDevice = $_REQUEST['removeDevice'];
	   if (strpos($deviceString, $removeDevice) === false) {
	       $out.='<p>Device not listed</p>';
	   } else {
	       $tempDevices = '';
	       if (strlen($deviceString) > 0) {
		   $devices = explode(',', $deviceString);
		   // iterate all current devices, and add all but the one we want to remove
		   for ($i = 0; $i < sizeof($devices); $i++) {
		       if ($devices[$i] != $removeDevice) {
		           $tempDevices.=$devices[$i].',';
		       }
		   }
		   $deviceString = substr($tempDevices, 0, strlen($tempDevices)-1);
		}		   
//	     	$out.='<p>'.$removeDevice.' - '.$deviceString.'</p>';

	        set_device_data($PK_Datalogger,59,$deviceString,$dbADO);
	   }
	}

	// show add list
	$query = "select * from Device";
	$resGP = $dbADO->Execute($query);
	$out.='<form action="index.php" method="POST" name="datalog">
	<input type="hidden" name="section" value="viewDatalog">
	<input type="hidden" name="action" value="addDevice">';

	$out.='<p>Add graph for device: </p><select name="addDevice" id="addDevice">';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
			$out.= '<option value="'.$row['PK_Device'].'">'.$row['Description'].' ('.$row['PK_Device'].')</option>';
		}
	}
	$out.='</select><input type="submit" class="button" name="add" value="Add" />';
	$out.='</form>';

	// show graphs
	$out.='<form action="index.php" method="POST" name="datalogGraph">
	<input type="hidden" name="section" value="viewDatalog">
	<input type="hidden" name="action" value="removeDevice">
	<input type="hidden" name="removeDevice" value="">';

	if (strlen($deviceString) > 0) {
		$devices = explode(',', $deviceString);
		for ($i = 0; $i < sizeof($devices); $i++) {
		    $out = $out.'<IMG SRC="operations/datalog/graph.php?device='.$devices[$i].'&days=1">';
		    $out = $out.'<input type="submit" class="button" name="remove" onclick="document.datalogGraph.removeDevice.value='.$devices[$i].'" value="Remove"/><br>';
	    	}
	} else {
	  $out = $out.'<p>No devices selected for graph display</p>';
	}
	$out.='</form>';

	$output->setBody($out);
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle("Data Logger");
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

?>
