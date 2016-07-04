<?php

// Creates an array from a string created with 'multimplode'
function multiexplode($string){
     $expl1 = explode(',',$string);
     $i=0;
     while ($i < count($expl1)){
          $expl2[$i]=explode(';',$expl1[$i]);
          ++$i;
     }
     return($expl2);
}


//Creates a string from an array level1 separated using(,) and level2 using (;)
function multimplode($array){
     if (!empty($array)){
          reset($array);
          while (list($key, $val) = each($array)) {
               $imp[$key] = implode(';',$val);
          }
          return(implode(',',$imp));
     }
     else {
          return ('');
     }
}

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

     $mysqlhost="localhost"; // MySQL-Host
     $mysqluser="root"; // MySQL-User
     $mysqlpwd=""; // Password
     $mysqldb="lmce_datalog";

     $connection=mysqli_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
     mysqli_select_db($connection, $mysqldb) or die("ERROR: could not select database!");

	$PK_Datalogger = getDeviceFromDT($installationID,1949 /* DeviceTemplate_DataLogger_Plugin */,$dbADO);
	$deviceString = getDeviceData($PK_Datalogger,59 /* Configuration */,$dbADO); 

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

// Creates an array from the Template configuration
     if (!empty($deviceString)){
          $conf=multiexplode($deviceString);
     }

// Add ore remove devices from the page
     if ($action=='addDevice' && $_REQUEST['addDevice'] != 0){
          $addDevice = explode(',',$_REQUEST['addDevice']);
          $conf[]=array($addDevice[0],$_REQUEST['days'],$_REQUEST['color'],$addDevice[1]);
          set_device_data($PK_Datalogger,59,multimplode($conf),$dbADO);
     }
     else if ($action=='removeDevice'){
          unset ($conf[$_REQUEST['removeDevice']]);
          $conf = array_values($conf);
          set_device_data($PK_Datalogger,59,multimplode($conf),$dbADO);
     }

// Form to select layout
     $out.='<form name="datalog" method="POST" action="index.php">
    	<input type="hidden" value="viewDatalog" name="section">
    	<input type="hidden" value="addDevice" name="action">
    	<p>Add graph for device: </p>
    	<select id="addDevice" name="addDevice"><option value="0">-Sensor-</option>';

// Create list of devices that have data in lmce_datalog
     $query = mysqli_query($connection, 'SELECT  DISTINCT EK_Device,FK_Unit FROM Datapoints WHERE timestamp > DATE_SUB(NOW(), INTERVAL 7 DAY) ORDER BY EK_Device');
     while ($devices = mysqli_fetch_array($query)){
          $queryDescription = "SELECT `Description` FROM `Device` where `PK_Device` = ".$devices['EK_Device'];
          $resGP = $dbADO->Execute($queryDescription);
          $row=$resGP->FetchRow();
          $rowUnit = mysqli_fetch_row(mysqli_query($connection, 'SELECT Description FROM Unit WHERE PK_Unit ='.$devices['FK_Unit']));
         	$out.= '<option value="'.$devices['EK_Device'].','.$devices['FK_Unit'].'">'.$devices['EK_Device'].' - '.$row['Description'].' ('.$rowUnit[0].')</option>';
     }

     $out.='</select><select id="days" name="days"><option value="">-Days-</option><option value="1">1-day</option><option value="3">3-days</option>
     <option value="7">7-days</option>
     <option value="14">14-days</option>
     <option value="30">30-days</option>
        </select>
    	<select id="color" name="color"><option value="">-Color-</option><option value="red@0.4">red</option><option value="blue@0.4">blue</option>
    	<option value="yellow@0.4">yellow</option><option value="green@0.4">green</option></select>
    	<input type="submit" value="Add" name="add" class="button"></form>';
	
// show graphs
	$out.='<form action="index.php" method="POST" name="datalogGraph">
	<input type="hidden" name="section" value="viewDatalog">
	<input type="hidden" name="action" value="removeDevice">
	<input type="hidden" name="removeDevice" value="">';

	if (!empty($conf)) {
		for ($i = 0; $i < count($conf); $i++) {
		    $q = "SELECT `Description` FROM `Device` where `PK_Device` = ".$conf[$i][0];
              $resGP = $dbADO->Execute($q);
              $row=$resGP->FetchRow();
                    $deviceURL = 'device='.$conf[$i][0].'&days='.$conf[$i][1].'&color='.$conf[$i][2].'&unit='.$conf[$i][3].'&name='.$row['Description'];
		    $out = $out.'<a href="index.php?section=datalogDetails&'.$deviceURL.'"><img style="border-style: none;" src="operations/datalog/graph.php?'.$deviceURL.'" /></a>';
		    $out = $out.'<input type="submit" class="button" name="remove" onclick="document.datalogGraph.removeDevice.value='.$i.'" value="Remove"/><br>';
	    	}
	} 
	else {
	  $out = $out.'<p>No devices selected for graph display</p>';
	}

     $out.='</form>';

	$output->setBody($out);
	$output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
	$output->setPageTitle("Data Logger");
	$output->setNavigationMenu(array('Data Logger'=>'index.php?section=viewDatalog'));	
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

?>
