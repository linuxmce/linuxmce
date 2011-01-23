<?



function datalogDetails($output,$dbADO){
    $out = "<script>
    function updateGraph(graphNo) {
    	var device = document.getElementById('device').value;
        var startTime = document.getElementById('startTime_'+graphNo).value;
        var endTime = document.getElementById('endTime_'+graphNo).value;
	var color = document.getElementById('color_'+graphNo).value;
	var unit = document.getElementById('unit_'+graphNo).value;
	var name = document.getElementById('name').value;
	var graphURL = 'operations/datalog/detailsGraph.php?device='+device+'&startTime='+startTime+'&endTime='+endTime+'&color='+color+'&unit='+unit+'&name='+name;
	document.getElementById('graph_'+graphNo).src = graphURL;
    }

    </script>";

	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$deviceID=(int)$_REQUEST['deviceID'];
	$installationID = (int)@$_SESSION['installationID'];


		$out.='
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">'.$display.'</td>
			</tr>		
		</table>';

     $mysqlhost="localhost"; // MySQL-Host
     $mysqluser="root"; // MySQL-User
     $mysqlpwd=""; // Password
     $mysqldb="lmce_datalog";

     $connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
     mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");

     $device = $_REQUEST['device'];
     $unit = $_REQUEST['unit'];
     $color = $_REQUEST['color'];
     $days = $_REQUEST['days'];

     $action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

     $out.='<a href="index.php?section=viewDatalog">Back</a>';

     // show graphs
     $out.='<form method="POST" name="datalogDetails">
	    <input type="hidden" name="section" value="datalogDetails">';


    if (!empty($device)) {

	$q = "SELECT `Description` FROM `Device` where `PK_Device` = ".$device;
       	$resGP = $dbADO->Execute($q);
    	$row=$resGP->FetchRow();
	$out.='<p>'.$row['Description'].'</p>';
	$graphNo = 1;

        $out.='<input type="hidden" name="device" id="device" value="'.$device.'">';
        $out.='<input type="hidden" name="name" id="name" value="'.$row['Description'].'">';
        $out.='<input type="hidden" id="color_'.$graphNo.'" name="color_'.$graphNo.'" value="'.$color.'">';
        $out.='<input type="hidden" id="unit_'.$graphNo.'" name="unit_'.$graphNo.'" value="'.$unit.'">';

	$out = $out.'<img id="graph_'.$graphNo.'" src="operations/datalog/detailsGraph.php?device='.$device.'&days='.$days.'&unit='.$unit.'&name='.$row['Description'].'">';

	// Date fields
	$endTime = date('Y-m-d H:i', time());
	$startTime = date('Y-m-d H:i', time() - $days*24*60*60);
	$out.='<p><label for="startTime_'.$graphNo.'">Start time</label> <input id="startTime_'.$graphNo.'" name="startTime_'.$graphNo.'" length="16" value="'.$startTime.'"/>';
	$out.='&nbsp;-&nbsp;<label for="endTime_'.$graphNo.'">End time</label> <input id="endTime_'.$graphNo.'" name="endTime_'.$graphNo.'" length="16" value="'.$endTime.'"/>';
	$out = $out.' <input type="button" class="button" name="update" onclick="updateGraph(1);" value="Update"/>';
	$out = $out.' <input type="button" class="button" name="update" onclick="location.reload();" value="Reload"/><br>';

    } else {
	$out = $out.'<p>No devices selected for graph display</p>';
    }

    $out.='</p>';
    $out.='</form>';

    $output->setBody($out);
    $output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
    $output->setPageTitle("Data Logger - Details");
    $output->setNavigationMenu(array('Data Logger'=>'index.php?section=datalogDetails'));
    $output->setTitle(APPLICATION_NAME);
    $output->output();
}

?>