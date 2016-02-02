<?php 
require_once 'Image/Graph.php'; 

function createDataSet($device, $startTime, $endTime, $points, $unit, $keepValue) {

    $sql = "select Description,timestamp,Datapoint,Unit from Datapoints,Unit
         WHERE FK_Unit=PK_unit AND PK_unit LIKE ".$unit." and EK_Device= ".$device." ";
    $sql.="AND timestamp > '".$startTime."' ";

    if (isset($endTime)) {
        $sql.="AND timestamp < '".$endTime."' ";
    }
    $sql.=" order by timestamp";
    $query=mysql_query($sql);

    $num = mysql_numrows($query); //find number off datapoints for the graph
    if ($num == 0) {
        return null;
    }

    $adjustFactor = $points / $num;
    // Create a dataset for the graph
    $Dataset =& Image_Graph::factory('dataset'); 

    // Create the datapoints for the Dataset and the lables ofr x-axis
    $t=0; //Counter
    $avgValue=0;
    $lastTime = $startTime;
    // TODO: cannot use counter on X axis, because devices may report status with different intervals, thus we
    // can skew one graph in relation to another
    while ($datapoint = mysql_fetch_array($query)){
        if ($t==0){
	    $queryLast = mysql_query('SELECT PK_Datapoints, Datapoint, timestamp FROM Datapoints 
	           WHERE EK_Device = '.$device.' AND timestamp < "'.$startTime.'" AND `FK_Unit` = '.$unit.' ORDER BY timestamp DESC LIMIT 1');
	    if (mysql_numrows($queryLast)==0){
	        $prevDatapoint=0;
            } else {     
	        $datapointLast = mysql_fetch_row($queryLast);
	        $prevDatapoint=$datapointLast[1];
            }
	    $Dataset->addPoint(floor($t*$adjustFactor)/*strtotime($startTime)*//*strtotime('-'.$days.'days')*/, $datapointLast[1]);
         }
         // If the device is reporting "state change" asume the device will have the same state until new state is reported
         if (in_array($unit, $keepValue)){
             $Dataset->addPoint(floor($t*$adjustFactor)/*(strtotime($datapoint[1])-1)*/, $prevDatapoint);
             $prevDatapoint=$datapoint['Datapoint'];
         }
         $Dataset->addPoint(floor($t*$adjustFactor)/*strtotime($datapoint[1])*/, $datapoint[2]);
         $avgValue = $avgValue + (strtotime($datapoint[1])-strtotime($lastTime))*$datapoint[2];
         $lastTime = $datapoint[1];
         ++$t;
         if ($t==$num){
             $last=strtotime($datapoint[1]);
	     $Dataset->addPoint(floor($t*$adjustFactor)/*strtotime($endTime)*/, $datapoint[2]);
	     $UnitName=$datapoint[3];
        }
    }
    $avgValue = round($avgValue / (strtotime($endTime)-strtotime($startTime)));
    $Dataset->setName($startTime.' - '.$endTime.', Max: '.$Dataset->maximumY().' '.$UnitName.', Min: '.$Dataset->minimumY().' '.$UnitName.', Avg: '.$avgValue.' '.$UnitName);
    
    return $Dataset;
}

$mysqlhost="localhost"; // MySQL-Host
$mysqluser="root"; // MySQL-User
$mysqlpwd=""; // Password
$mysqldb="lmce_datalog";
$connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");

$device = mysql_real_escape_string($_GET['device']);
$days = mysql_real_escape_string($_GET['days']);
$color = mysql_real_escape_string($_GET['color']);
$name = mysql_real_escape_string($_GET['name']);
$unit = mysql_real_escape_string($_GET['unit']); 
$keepValue = array(5,9); // Units that have the same value until new value is reported
$startTime[0] = mysql_real_escape_string($_GET['startTime']);
$endTime = mysql_real_escape_string($_GET['endTime']);

// Get all graphs start time
$i = 1;
while (!empty($_REQUEST['startTime'.$i])) {
    $startTime[$i] = mysql_real_escape_string($_REQUEST['startTime'.$i]);
    $i++;
}
$numGraphs = $i;

if (empty($days)) {
     $days=1;
}

if (empty($color)) {
     $color='red@0.4';
}

if (empty($unit)) {
     $unit='\'%\'';
}
if (empty($startTime[0])) {
    // No start time set, use days to give us the last days of data
    $startTime[0] = date('Y-m-d H:i:s', time() - $days*24*60*60);
}
if (empty($endTime)) {
    if (empty($days)) {
        $endTime = date('Y-m-d H:i:s', time());
    } else {
       $endTime = date('Y-m-d H:i:s',strtotime($startTime[0]) + $days*24*60*60);
    }
}

$sql = "select Description,timestamp,Datapoint,Unit from Datapoints,Unit
     WHERE FK_Unit=PK_unit AND PK_unit LIKE ".$unit." and EK_Device= ".$device." ";
$sql.="AND timestamp > '".$startTime[0]."' ";

if (isset($endTime)) {
     $sql.="AND timestamp < '".$endTime."' ";
}
$sql.=" order by timestamp";
$query=mysql_query($sql);

$num = mysql_numrows($query); //find number off datapoints for the graph


// If there is not enough data in the db to create a graph 
if ($num<=1) {
     $my_img = imagecreate( 800, 25 );
     $background = imagecolorallocate( $my_img, 255, 255, 255 );
     $text_colour = imagecolorallocate( $my_img, 0, 0, 0 );
     imagestring( $my_img, 4, 5, 5, "No data to display for device ".$device.' '.$name,$text_colour );
     header( "Content-type: image/png" );
     imagepng( $my_img );
     imagecolordeallocate( $text_color );
     imagecolordeallocate( $background );
     imagedestroy( $my_img );
     exit;
}
// Create the graph and set the size
$Graph =& Image_Graph::factory('graph', array(875, 250+12*$numGraphs)); 

// add a TrueType font
$Font =& $Graph->addNew('font', '/usr/share/fonts/truetype/msttcorefonts/verdana.ttf'); 

// set the font size 
$Font->setSize(7); 
$Graph->setFont($Font); 

// Create a dataset for the graph
$Dataset =& Image_Graph::factory('dataset'); 

// Create the datapoints for the Dataset and the lables ofr x-axis
$t=0; //Counter
$avgValue=0;
$lastTime = $startTime[0];
while ($datapoint = mysql_fetch_array($query)){
     if ($AxisYName == "") {
          $AxisYName = $datapoint[3];
     }
     if ($t==0){
          $firstDate=date ("Y-m-d H:i:s", strtotime('-'.$days.'days'));
          $queryLast = mysql_query('SELECT PK_Datapoints, Datapoint, timestamp FROM Datapoints 
	       WHERE EK_Device = '.$device.' AND timestamp < DATE_SUB(NOW(), INTERVAL '.$days.' DAY) AND `FK_Unit` = '.$unit.' ORDER BY timestamp DESC LIMIT 1');
	  if (mysql_numrows($queryLast)==0){
	       $prevDatapoint=0;
	  }
	  else {     
	       $datapointLast = mysql_fetch_row($queryLast);
	       $prevDatapoint=$datapointLast[1];
	  }
	  $Dataset->addPoint($t/*strtotime($startTime[0])*//*strtotime('-'.$days.'days')*/, $datapointLast[1]);
     }
     // If the device is reporting "state change" asume the device will have the same state until new state is reported
     if (in_array($unit, $keepValue)){
         $Dataset->addPoint($t-1, $prevDatapoint);
         $prevDatapoint=$datapoint['Datapoint'];
     }
     $Dataset->addPoint($t, $datapoint[2]);
     $avgValue = $avgValue + (strtotime($datapoint[1])-strtotime($lastTime))*$datapoint[2];
     $lastTime = $datapoint[1];
     ++$t;
     if ($t==$num){
          $last=strtotime($datapoint[1]);
	  $Dataset->addPoint($t/*strtotime($endTime)*/, $datapoint[2]);
     }
}
$avgValue = round($avgValue / (strtotime($endTime)-strtotime($startTime[0])));
$Dataset->setName($startTime[0].' - '.$endTime.', Max: '.$Dataset->maximumY().' '.$AxisYName.', Min: '.$Dataset->minimumY().' '.$AxisYName.', Avg: '.$avgValue.' '.$AxisYName);
// Set info in the title
$graphName .= $device.' - '.$name;

// Create arrays for x-axis lables
$stepTime = (strtotime($endTime)-strtotime($startTime[0]))/24;
$firstFixed=$startTime[0];//substr_replace($startTime[0], '00', -2);
$step = $num / 24;
//TODO: does not work properly for datapoints with different intervals
// possible fix: convert the timestamp of the datapoint to a number which is proportional to the time
// use this number when adding points to the dataset
// Note: the problem here, is how to add timestamps to the graph when the different graph datasets have completely different time/days
for ($fullHour= strtotime($firstFixed), $i=0, $item ; $item < $num ; $fullHour=$fullHour+$stepTime, ++$i, $item = $item + $step){
    $array1[$i]=$item;
    $array2[0][$item]=date('H:i', $fullHour);
}

// setup the plotarea
$Graph->add(
   Image_Graph::vertical(
   Image_Graph::vertical(
      Image_Graph::factory('title', array($graphName, 10)),        
      $Plotarea = Image_Graph::factory('plotarea', array('axis', 'axis')),
      5
   ),
      $Legend = Image_Graph::factory('legend', array()),
(90-3*$numGraphs))
);   

$Legend->setPlotArea($Plotarea);

$Plotarea->setBackgroundColor('white');

// y-axis Name
$AxisY =& $Plotarea->getAxis(IMAGE_GRAPH_AXIS_Y);
$AxisY->setTitle($AxisYName, "vertical");

// Set minimum value on Y-axis
//$AxisY->forceMinimum(10); 

// x-axis Name
$AxisX =& $Plotarea->getAxis(IMAGE_GRAPH_AXIS_X);
$AxisX->setTitle('Time', "horizontal");       

// Lable interval on the x-axis
$AxisX->setLabelInterval($array1); 

// create an data label array for formatting label data based on an array
$ArrayData =& Image_Graph::factory('Image_Graph_DataPreprocessor_Array',$array2);

// use the data label array on the X axis
$AxisX->setDataPreprocessor($ArrayData);     

// Create the Plot
$i = 0;
$Plot[$i] =& $Plotarea->addNew('line', $Dataset);
$Plot[$i]->setLineColor('red');

// Add grid
$Grid =& Image_Graph::factory('line_grid');
$Plotarea->add($Grid);

// Add additional graphs with different start times, but use the same period as the first graph

$colorArray = array('blue','green','yellow','purple','limegreen','cyan','orange','black');
$points = $num;
$period = strtotime($endTime)-strtotime($startTime[0]);
for ($i =  1; $i < count($startTime); $i++) {
    if (!empty($startTime[$i])) {
        $endTime = date('Y-m-d H:i:s',strtotime($startTime[$i]) + $period);
        $Dataset2 =& createDataSet($device, $startTime[$i], $endTime, $points, $unit, $keepValue);
        if ($Dataset2 != null) {
            $Plot[$i] =& $Plotarea->addNew('line', $Dataset2); 
            $Plot[$i]->setLineColor($colorArray[$i-1]);
        }
    }
}

$Graph->done(); 
?>

