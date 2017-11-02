<?php
require_once 'Image/Graph.php';

$mysqlhost="localhost"; // MySQL-Host
$mysqluser="root"; // MySQL-User
$mysqlpwd=""; // Password
$mysqldb="lmce_datalog";

$connection=mysqli_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
mysqli_select_db($connection, $mysqldb) or die("ERROR: could not select database!");     


$days = $_REQUEST['days'];
if (empty($days)) {
	$days=7;
}
$startTime=strtotime('-'.$days.' day');
$endTime=strtotime('now');
$unit=5;
$cumulatedEnergyUnit = 10;
if ($days==1){
	$interval=3600;
}
else {
	$interval=86400;
}



// create the graph
$Graph =& Image_Graph::factory('graph', array(1000, 400)); 
// add a TrueType font
$Font =& $Graph->addNew('font', '/usr/share/fonts/truetype/msttcorefonts/verdana.ttf'); 
// set the font size to 11 pixels
$Font->setSize(8);

$Graph->setFont($Font);



$Graph->add(
	Image_Graph::vertical(
		Image_Graph::factory('title', array('Energy graph - '.$days.' days', 12)),        
					  Image_Graph::vertical(
						  $Plotarea = Image_Graph::factory('plotarea'),
									    $Legend = Image_Graph::factory('legend'),
									    80
					  ),
					  5
	)
);
$Legend->setPlotarea($Plotarea);        
$Plotarea->setBackgroundColor('white');

// Get data for status and static power-based devices
$deviceQuery = mysqli_query($connection, 'SELECT EK_Device, FK_DeviceTemplate, Description, IK_DeviceData, FK_Unit FROM Datapoints, pluto_main.Device, pluto_main.Device_DeviceData 
WHERE FK_Unit='.$unit.' AND FK_DeviceData=289 AND EK_Device=PK_Device AND EK_Device=FK_Device 
GROUP BY EK_Device ORDER BY FK_Room');

$i=0;
while ($device=mysqli_fetch_array($deviceQuery)){

	// create the dataset
	$Dataset[$i]=& Image_Graph::factory('dataset'); 
	$statment = mysqli_prepare($connection, 'SELECT Datapoint, timestamp FROM Datapoints 
	WHERE EK_Device = ? AND timestamp > DATE_SUB(NOW(), INTERVAL ? DAY) ORDER BY timestamp');
	mysqli_stmt_bind_param($statement, "ii", $device['EK_Device'], $days);
	$query = mysqli_stmt_execute($statment); //mysqli_query($connection, ');
	$num=mysqli_num_rows($query);
	// set names for datasets (for legend)

	for ($plotTime = ($startTime+$interval), $prevTimestampMod=$startTime, $t=0; $plotTime <= $endTime ; $plotTime=($plotTime+$interval), ++$t){
		$energy=0;
		$ONtime=0;
		// find the state of the device at the start of the timeperiode
		if ($t==0){ 
			$queryPreTimeframe = mysqli_query($connection, 'SELECT Datapoint, timestamp FROM Datapoints 
			WHERE EK_Device = '.$device['EK_Device'].' AND timestamp < DATE_SUB(NOW(), INTERVAL '.$days.' DAY) ORDER BY timestamp DESC LIMIT 1');
			if (mysqli_num_rows($queryPreTimeframe)==0){
				$state=0;
			}
			else {     
				$datapointPreTimeframe = mysqli_fetch_row($queryPreTimeframe);
				$state=$datapointPreTimeframe[0];
			}
			$datapoint=array('Datapoint'=>$state,'timestamp'=>date ("Y-m-d H:i:s", $startTime));
			if ($num==0){
				$nextDatapoint=$datapointPreTimeframe[0];
			}
			else{
				$nextDatapoint=mysqli_fetch_array($query);
			}    
			$timestampMod=$prevTimestampMod;
		}
	
		while ($timestampMod <= $plotTime){
			if (strtotime($nextDatapoint['timestamp'])<$plotTime){
				// If this is a new loop the $timestampMod need to be updated
				if ($timestampMod == $plotTime-$interval){
					$timestampMod=strtotime($nextDatapoint['timestamp']);
				}
				if ($datapoint['Datapoint'] > 0){
					$ONtime = $ONtime+($timestampMod-$prevTimestampMod);
				}
				$datapoint=$nextDatapoint;
				$nextDatapoint=mysqli_fetch_array($query) or 
				$nextDatapoint=array('timestamp' => date ("Y-m-d H:i:s", $endTime+1),'Datapoint' => $datapoint['Datapoint']);
				$prevTimestampMod=$timestampMod;
				$timestampMod=(strtotime($nextDatapoint['timestamp']))<($plotTime)?strtotime($nextDatapoint['timestamp']):$plotTime;
			}
			elseif ($timestampMod == $plotTime){
				if ($datapoint['Datapoint'] > 0){
					$ONtime = $ONtime+($timestampMod-$prevTimestampMod);
				}
				$prevTimestampMod=$timestampMod;
				break;
			}
			else {
				if ($datapoint['Datapoint'] > 0){
					$ONtime = $ONtime+($timestampMod-$prevTimestampMod);
				}
				$prevTimestampMod=$timestampMod;
				$timestampMod=$plotTime;
			}
			
		}
		$energy = $energy+($ONtime*$device['IK_DeviceData']/3600000);
		$Dataset[$i]->addPoint(date('d-H',$plotTime),$energy);
		if (!$usedPerInterval[$plotTime])
		   $usedPerInterval[$plotTime] = $energy;
		else
		   $usedPerInterval[$plotTime] += $energy;
	}
	$Dataset[$i]->setName($device['Description']); 
	++$i;
}

// Get any total energy values from possible "Standard energy meter" devices
$deviceQuery = mysqli_query($connection, 'SELECT EK_Device, FK_DeviceTemplate, Description, IK_DeviceData, FK_Unit FROM Datapoints, pluto_main.Device, pluto_main.Device_DeviceData 
WHERE FK_Unit='.$cumulatedEnergyUnit.' AND FK_DeviceTemplate = 2268 
AND EK_Device=PK_Device AND EK_Device=FK_Device 
GROUP BY EK_Device ORDER BY FK_Room');

while ($device=mysqli_fetch_array($deviceQuery)){
    $Dataset[$i]=& Image_Graph::factory('dataset'); 
    for ($plotTime = ($startTime+$interval), $prevTimestampMod=$startTime, $t=0; $plotTime <= $endTime ; $plotTime=($plotTime+$interval), ++$t){
	for ($plotTime = ($startTime+$interval); $plotTime <= $endTime; $plotTime = ($plotTime+$interval)){
            // Get first and last datapoint in the time period and substract them to get the difference
	    $intervalStop = date("Y-m-d H:i:s", $plotTime);
	    $intervalStart = date("Y-m-d H:i:s", ($plotTime - $interval));
	    $sql = 'SELECT Datapoint, timestamp FROM Datapoints 
WHERE EK_Device = '.$device['EK_Device'].' AND timestamp <= "'.$intervalStop.'" AND timestamp >= "'.$intervalStart.'" 
ORDER BY timestamp LIMIT 1';
      	    $query = mysqli_query($connection, $sql);
      	    $firstValue = mysqli_fetch_array($query);
	    $query = mysqli_query($connection, 'SELECT Datapoint, timestamp FROM Datapoints 
WHERE EK_Device = '.$device['EK_Device'].' AND timestamp <= "'.$intervalStop.'" AND timestamp >= "'.$intervalStart.'" 
ORDER BY timestamp DESC LIMIT 1' );
      	    $lastValue = mysqli_fetch_array($query);
	    $energy = $lastValue['Datapoint'] - $firstValue['Datapoint'];
	    error_log("Value: ".$firstValue['Datapoint'].":".$lastValue['timestamp']." - ".$lastValue['Datapoint'].":".$firstValue['timestamp']);
	    $totalEnergy = $energy - $usedPerInterval[$plotTime];
	    $Dataset[$i]->addPoint(date('d-H', $plotTime), $totalEnergy);
	}
    }
    $Dataset[$i]->setName("Total usage"); 
    $i++;
}

$Plot =& $Plotarea->addNew('bar', array($Dataset,'stacked' ));

// Vertical axis name/unit
$AxisY =& $Plotarea->getAxis(IMAGE_GRAPH_AXIS_Y);
$AxisY->setTitle("kWh", "vertical");

// Add grid
$Grid =& Image_Graph::factory('line_grid');
$Plotarea->add($Grid);

// set a line color
$Plot->setLineColor('gray');
		
$Fill =& Image_Graph::factory('Image_Graph_Fill_Array');
$Fill->addColor('blue@0.3');
$Fill->addColor('orange@0.3');
$Fill->addColor('pink@0.3');
$Fill->addColor('green@0.3');
$Fill->addColor('red@0.3');
$Fill->addColor('yellow@0.3');
$Fill->addColor('darkmagenta@0.3');
$Fill->addColor('blue@0.7');
$Fill->addColor('orange@0.7');
$Fill->addColor('pink@0.7');
$Fill->addColor('green@0.7');
$Fill->addColor('red@0.7');
$Fill->addColor('yellow@0.7');
$Fill->addColor('brown@0.7');
$Plot->setFillStyle($Fill);

// output the Graph
$Graph->done();


?>