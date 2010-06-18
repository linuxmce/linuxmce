<?

     $mysqlhost="localhost"; // MySQL-Host
     $mysqluser="root"; // MySQL-User
     $mysqlpwd=""; // Password
     $mysqldb="lmce_datalog";
     
     $connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
     mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");     


$days = mysql_real_escape_string($_GET['days']);
if (empty($days)) {
     $days=7;
}
$startTime=strtotime('-'.$days.' day');
$endTime=strtotime('now');
$unit=5;
$interval=$days*3600;

require_once 'Image/Graph.php';

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
            90
        ),
        5
    )
);
$Legend->setPlotarea($Plotarea);        

$deviceQuery = mysql_query('SELECT EK_Device, Description, IK_DeviceData FROM Datapoints, pluto_main.Device, pluto_main.Device_DeviceData 
     WHERE FK_Unit='.$unit.' AND EK_Device=PK_Device AND EK_Device=FK_Device AND FK_DeviceData=289 
     GROUP BY EK_Device ORDER BY FK_Room');

$i=0;
while ($device=mysql_fetch_array($deviceQuery)){

     $query = mysql_query('SELECT Datapoint, timestamp FROM Datapoints 
     WHERE EK_Device = '.$device['EK_Device'].' AND timestamp > DATE_SUB(NOW(), INTERVAL '.$days.' DAY)');
     $num=mysql_numrows($query);
     // create the dataset
     $Dataset[$i]=& Image_Graph::factory('dataset'); 
     // set names for datasets (for legend)
     
     for ($plotTime = ($startTime+$interval), $prevTimestampMod=$startTime, $t=0; $plotTime <= $endTime ; $plotTime=($plotTime+$interval), ++$t){
	  $energy=0;
	  $ONtime=0;
	  // find the state of the device at the start of the timeperiode
	  if ($t==0){ 
	       $queryPreTimeframe = mysql_query('SELECT Datapoint, timestamp FROM Datapoints 
		    WHERE EK_Device = '.$device['EK_Device'].' AND timestamp < DATE_SUB(NOW(), INTERVAL '.$days.' DAY) ORDER BY timestamp DESC LIMIT 1');
	       if (mysql_numrows($queryPreTimeframe)==0){
			 $state=0;
		    }
	       else {     
		    $datapointPreTimeframe = mysql_fetch_row($queryPreTimeframe);
		    $state=$datapointPreTimeframe[0];
	       }
	       $datapoint=array('Datapoint'=>$state,'timestamp'=>date ("Y-m-d H:i:s", $startTime));
	       $nextDatapoint=mysql_fetch_array($query);
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
		    $nextDatapoint=mysql_fetch_array($query) or 
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
$Fill->addColor('blue@0.2');
$Fill->addColor('orange@0.2');
$Fill->addColor('pink@0.2');
$Fill->addColor('green@0.2');
$Fill->addColor('red@0.2');
$Fill->addColor('yellow@0.2');
$Fill->addColor('purple@0.2');
$Plot->setFillStyle($Fill);

// output the Graph
$Graph->done();


?>
