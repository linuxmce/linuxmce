<?php



function energyMonitor($output,$dbADO){

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
/*	
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
			self.location=\'index.php?section=energyMonitor&deviceID='.$deviceID.'\';
		}
			setTimeout("doReload()",60000);
		</script>';
*/
     $mysqlhost="localhost"; // MySQL-Host
     $mysqluser="root"; // MySQL-User
     $mysqlpwd=""; // Password
     $mysqldb="lmce_datalog";

//     $device=103;
     $unit=5;
     $effect=60; // VA
     $days=5;


     $connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
     mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");

//	$PK_Datalogger = getDeviceFromDT($installationID,1949 /* DeviceTemplate_DataLogger_Plugin */,$dbADO);
//	$deviceString = getDeviceData($PK_Datalogger,59 /* Configuration */,$dbADO); 

     foreach ($_REQUEST as $k => $v) {
//          $out.= '$_REQUEST['.$k.'] => '.$v.'<br>';
          if (substr($k,0,7)=='device_'){
//               $out.='hehe - '.substr($k,7).'<br>';
               $PK_Device=substr($k,7);
               set_device_data($PK_Device,289,$v,$dbADO);
          }
     }

     $query = mysql_query('SELECT EK_Device, Description, IK_DeviceData FROM Datapoints, pluto_main.Device, pluto_main.Device_DeviceData 
          WHERE FK_Unit='.$unit.' AND EK_Device=PK_Device AND EK_Device=FK_Device AND FK_DeviceData=289 
          GROUP BY EK_Device ORDER BY FK_Room');


     
     $out.='<form action="index.php" method="POST" name="energyMonitor">
	<input type="hidden" name="section" value="energyMonitor">
	<input type="hidden" name="action" value="uppdateEC">'; // EC = Energy Consumption
     
     $out.='<table cellpadding="4" cellspacing="1" border="0" align="center">
			<tr class="tablehead">
				<td align="center"><B>'.$TEXT_DEVICE_CONST.'</B></td>
				<td align="center"><B>Wats</B></td>
				<td align="center"><B>Last 24h</B></td>
				<td align="center"><B>Last 7 days</B></td>
				<td align="center"><B>Last 31 days</B></td>
				<td align="center"><B>Last year</B></td>
				
			</tr>';

	$pos =0;		
     while ($device=mysql_fetch_array($query)){
     ++$pos;
     $out.='<tr class="'.(($pos%2==0)?'alternate_back':'').'">
               <td align="left"><B>'.$device['Description'].' - '.$device['EK_Device'].'</B></td>
               <td align="center"><B><input type="text" name="device_'.$device['EK_Device'].'" value="'.$device['IK_DeviceData'].'" size="3" /></B></td>
               <td align="center"><B>'.round(((getEnergyConsumption($device['EK_Device'],1)/3600000)*$device['IK_DeviceData']),4).'</B></td>
               <td align="center"><B>'.round(((getEnergyConsumption($device['EK_Device'],7)/3600000)*$device['IK_DeviceData']),4).'</B></td>
               <td align="center"><B>'.round(((getEnergyConsumption($device['EK_Device'],31)/3600000)*$device['IK_DeviceData']),4).'</B></td>
               <td align="center"><B>'.round(((getEnergyConsumption($device['EK_Device'],365)/3600000)*$device['IK_DeviceData']),4).'</B></td>
               
          </tr>';
          
     }
     $out.='<tr><td></td><td align="center"><input type="submit" class="button" name="update" value="Update" /></td></tr></table>';
     $out.='<table cellpadding="4" cellspacing="1" border="0" align="center"><tr><td><IMG SRC="operations/datalog/energyGraph.php?days='.$_REQUEST['plot_days'].'"/><br>';
     $out.='Days to plot graph: </select><select id="plot_days" name="plot_days"><option value="'.$_REQUEST['plot_days'].'">-Days-</option>
	  <option value="1">1-day</option><option value="7">7-days</option>
	  <option value="31">31-days</option></select><input type="submit" class="button" name="update" value="Update" />';
     $out.='</tr></td></table></form>';
     //debug
     //$out.=print_array($_REQUEST);
     


	$output->setBody($out);
	$output->setMenuTitle($TEXT_AUTOMATION_CONST.' |');
	$output->setPageTitle("Energy Monitor");
	$output->setNavigationMenu(array('Energy Monitor'=>'index.php?section=energyMonitor'));	
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function getEnergyConsumption($device,$days){

     $query = mysql_query('SELECT PK_Datapoints, Datapoint, timestamp FROM Datapoints 
          WHERE EK_Device = '.$device.' AND timestamp > DATE_SUB(NOW(), INTERVAL '.$days.' DAY) ORDER BY timestamp');
     $prevON = strtotime('-'.$days.' day'); //Find time x days ago
     $ONtime = 0; // Initaiate Variable
     $t = 0;
     $num = mysql_numrows($query); //find number off datapoints

     while ($datapoint = mysql_fetch_array($query)){
          //$out.= '<br>$datapoint[1] strtotime($datapoint[2]) - '.$datapoint[1].'  '.strtotime($datapoint[2]).'<br>';
          // find the state of the device at the start of the timeperiode
          if ($t==0){ 
               $queryLast = mysql_query('SELECT PK_Datapoints, Datapoint, timestamp FROM Datapoints 
                    WHERE EK_Device = '.$device.' AND timestamp < DATE_SUB(NOW(), INTERVAL '.$days.' DAY) ORDER BY timestamp DESC LIMIT 1');
               if (mysql_numrows($queryLast)==0){
                   $lastState=0;
               }
               else {     
                    $datapointLast = mysql_fetch_row($queryLast);
                    $lastState=$datapointLast[1];
                    //$out.='$datapointLast = '.$datapointLast[0].' - '.$lastState.'<br>';
               }
          }
          if ($lastState > 0){
               $ONtime = $ONtime + (strtotime($datapoint[2]) - $prevON);
          } 
          if ($datapoint[1] > 0) {
               $prevON = strtotime($datapoint[2]);
          }
          $lastState = $datapoint[1];
          ++$t;
          //$out.='$ONtime - '.$ONtime.'<br>';
          // extend the last state to "now"
          if ($t == $num && $lastState > 0){
          $ONtime = $ONtime + (strtotime('now') - $prevON);
          }
     //$out.='<br>'.$ONtime;
     }
     return ($ONtime);
}

function energyGraph(){
     // blabla
}

?>
