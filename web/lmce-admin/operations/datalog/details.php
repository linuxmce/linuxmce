<?php



function datalogDetails($output,$dbADO){
    $out = "<script>
    var graphs = 1;
    function updateGraph(graphNo) {
    	var device = document.getElementById('device').value;
	var startTime = new Array();
        startTime[0] = document.getElementById('startTime_0').value;
        var endTime = document.getElementById('endTime_0').value;
	var color = document.getElementById('color_0').value;
	var unit = document.getElementById('unit_0').value;
	var name = document.getElementById('name').value;

	var graphURL = 'operations/datalog/detailsGraph.php?device='+device+'&startTime='+startTime+'&endTime='+endTime+'&color='+color+'&unit='+unit+'&name='+name;

	var i = 1;
	while (document.getElementById('startTime_'+i) != null) {
	    startTime[i] = document.getElementById('startTime_'+i).value;
	    graphURL += '&startTime'+i+'='+startTime[i];
	    i++;
        }

	document.getElementById('graph').src = graphURL;
    }

    function addGraph() {

        var div = document.getElementById('graphDiv');
        var newDiv = document.createElement('div');
	newDiv.id = 'graphDiv_'+graphs;
        div.appendChild(newDiv);
        var text = document.createElement('span');
	text.appendChild(document.createTextNode('Graph '+(graphs+1)+' : '));
	text.id = 'graph_id_'+graphs;
	newDiv.appendChild(text);
	var label = document.createElement('label');
	
	newDiv.appendChild(label);

	var startTime = document.getElementById('startTime_0').value;
	var field = document.createElement('input');
	field.id = 'startTime_'+graphs;
	field.length = 12;
	field.value = startTime;
	newDiv.appendChild(field);
	newDiv.appendChild(document.createTextNode(' '));

	var button = document.createElement('input');
	button.type='button';
	button.className='button';
        button.onclick=new Function('removeGraph('+graphs+');');
        button.value='Remove';
	newDiv.appendChild(button);
	graphs++;
    }

    function removeGraph(graphNo) {
        var i = graphNo;
        while (document.getElementById('graphDiv_'+(i+1)) != null) {
	    var startTimeEl = document.getElementById('startTime_'+i);
	    startTimeEl.value = document.getElementById('startTime_'+(i+1)).value;
	    i++;
	}
        var graphDiv = document.getElementById('graphDiv_'+i);
	var parent = graphDiv.parentNode;
	parent.removeChild(graphDiv);
	graphs--;
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

     $connection=mysqli_connect($mysqlhost, $mysqluser, $mysqlpwd, $mysqldb) or die ("ERROR: could not connect to the database!");

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
	$graphNo = 0;

        $out.='<input type="hidden" name="device" id="device" value="'.$device.'">';
        $out.='<input type="hidden" name="name" id="name" value="'.$row['Description'].'">';
        $out.='<input type="hidden" id="color_'.$graphNo.'" name="color_'.$graphNo.'" value="'.$color.'">';
        $out.='<input type="hidden" id="unit_'.$graphNo.'" name="unit_'.$graphNo.'" value="'.$unit.'">';

	$out = $out.'<img id="graph" src="operations/datalog/detailsGraph.php?device='.$device.'&days='.$days.'&unit='.$unit.'&name='.$row['Description'].'">';

	$out .= '<p>';
	$out = $out.' <input type="button" class="button" name="update" onclick="updateGraph(0);" value="Update"/>';
	$out = $out.' <input type="button" class="button" name="update" onclick="location.reload();" value="Reload"/><br>';
	$out .= '</p>';

	// Date fields
	$endTime = date('Y-m-d H:i', time());
	$startTime = date('Y-m-d H:i', time() - $days*24*60*60);
	$out.='<p><label for="startTime_'.$graphNo.'">Graph 1: Start time</label> <input id="startTime_'.$graphNo.'" name="startTime_'.$graphNo.'" length="12" value="'.$startTime.'"/>';
	$out.='&nbsp;-&nbsp;<label for="endTime_'.$graphNo.'">End time</label> <input id="endTime_'.$graphNo.'" name="endTime_'.$graphNo.'" length="12" value="'.$endTime.'"/>';
	
	$out .= '<div id="graphDiv">';
	$out .= '</div>';

	$out .= '<p>Add new time period to graph : ';
	$out .= '<input type="button" class="button" name="update" onclick="addGraph();" value="Add"/>';

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