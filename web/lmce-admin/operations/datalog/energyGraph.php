<?php
require_once 'Image/Graph.php';

$mysqlhost="localhost"; // MySQL-Host
$mysqluser="root"; // MySQL-User
$mysqlpwd=""; // Password
$mysqldb="lmce_datalog";

$connection=mysqli_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
mysqli_select_db($connection, $mysqldb) or die("ERROR: could not select database!");     


$days = mysqli_real_escape_string($_GET['days']);
if (empty($days)) {
	$days=7;
}
$startTime=strtotime('-'.$days.' day');
$endTime=strtotime('now');
$unit=5;
if ($days==1){
	$interval=3600;
}
else {
	$interval=86400;
}



// create the graph
$Graph =& Image_Graph::factory('graph', array(1000, 300)); 
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

$deviceQuery = mysqli_query($connection, 'SELECT EK_Device, Description, IK_DeviceData FROM Datapoints, pluto_main.Device, pluto_main.Device_DeviceData 
WHERE FK_Unit='.$unit.' AND EK_Device=PK_Device AND EK_Device=FK_Device AND FK_DeviceData=289 
GROUP BY EK_Device ORDER BY FK_Room');

$i=0;
while ($device=mysqli_fetch_array($deviceQuery)){
	
	$query = mysqli_query($connection, 'SELECT Datapoint, timestamp FROM Datapoints 
	WHERE EK_Device = '.$device['EK_Device'].' AND timestamp > DATE_SUB(NOW(), INTERVAL '.$days.' DAY) ORDER BY timestamp' );
	$num=mysqli_num_rows($query);
	// create the dataset
	$Dataset[$i]=& Image_Graph::factory('dataset'); 
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
	}
	$Dataset[$i]->setName($device['Description']); 
	++$i;
}
$Plot =& $Plotarea->addNew('bar', array($Dataset,'stacked' ));

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