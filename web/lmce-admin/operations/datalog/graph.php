<?php 
require_once 'Image/Graph.php'; 

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
$keepValue = array(5,9,10); // Units that have the same value until new value i reported
$startTime = mysql_real_escape_string($_GET['startTime']);
$endTime = mysql_real_escape_string($_GET['endTime']);

if (empty($days)) {
     $days=1;
}

if (empty($color)) {
     $color='red@0.4';
}

if (empty($unit)) {
     $unit='\'%\'';
}
if (empty($startTime)) {
    // No start time set, use days to give us the last days of data
    $startTime = date('Y-m-d H:i:s', time() - $days*24*60*60);
}
if (empty($endTime)) {
    if (empty($days)) {
        $endTime = date('Y-m-d H:i:s', time());
    } else {
       $endTime = date('Y-m-d H:i:s',strtotime($startTime) + $days*24*60*60);
    }
}



$sql = "select Description,timestamp,Datapoint,Unit from Datapoints,Unit
     WHERE FK_Unit=PK_unit AND PK_unit LIKE ".$unit." and EK_Device= ".$device." ";
$sql.="AND timestamp > '".$startTime."' ";

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
$Graph =& Image_Graph::factory('graph', array(800, 200)); 

// add a TrueType font
$Font =& $Graph->addNew('font', '/usr/share/fonts/truetype/msttcorefonts/verdana.ttf'); 

// set the font size 
$Font->setSize(7); 
$Graph->setFont($Font); 

// Create a dataset for the graph
$Dataset =& Image_Graph::factory('dataset'); 

// Create the datapoints for the Dataset and the lables ofr x-axis
$t=0; //Counter
$skipFirst = ($unit == 10); // Some data require two value to start of (cumulated energy, for instance, where we display the diff between two values)
while ($datapoint = mysql_fetch_array($query)){
     if ($AxisYName == "") {
          if ( $unit == 10 ) {
	       $AxisYName = "kW";
	  } else {
               $AxisYName = $datapoint[3];
          }
     }
     // Find previous data point (before the current one)
     if ($t==0){
          $firstDate=date ("Y-m-d H:i:s", strtotime('-'.$days.'days'));
          $queryLast = mysql_query('SELECT PK_Datapoints, Datapoint, timestamp FROM Datapoints 
	       WHERE EK_Device = '.$device.' AND timestamp < "'.$startTime.'" AND `FK_Unit` = '.$unit.' ORDER BY timestamp DESC LIMIT 1');
	  if (mysql_numrows($queryLast)==0) {
	       $prevDatapoint = 0;
	       $prevDataTime = $startTime;
	  } else {     
	       $datapointLast = mysql_fetch_row($queryLast);
	       $prevDatapoint = $datapointLast[1];

	       $prevDataTime = $datapointLast[2];
	  }
	  if (!$skipFirst) {
              $Dataset->addPoint(strtotime($startTime)/*strtotime('-'.$days.'days')*/, $datapointLast[1]);
	  }
     }
     $dataTime = $datapoint[1];
     $dataValue = $datapoint[2];
     
     // If this is unit = 10, accumulated energy, it makes more sense to display the diff between the two data points
     if ($unit == 10) {
     	  $timeSpan = (strtotime($dataTime)-strtotime($prevDataTime))/3600;
          $dataValue = ($dataValue - $prevDatapoint) / $timeSpan;
     }

     ++$t;
     // If the device is reporting "state change" asume the device will have the same state until new state is reported
     if (in_array($unit, $keepValue)){
         $Dataset->addPoint(strtotime($prevDataTime), $dataValue);
     }

     $Dataset->addPoint(strtotime($dataTime), $dataValue);
     if ($t==$num){
         $last=strtotime($datapoint[1]);
	 $Dataset->addPoint(strtotime($endTime), $dataValue);
     }

     $prevDataTime = $datapoint[1];
     $prevDatapoint = $datapoint[2];
     $prevDataValue = $dataValue;
}


// Set info in the title
$graphName .= $device.' - '.$name .' '.$startTime.' - '.$endTime;

// Create arrays for x-axis lables
$firstFixed=substr_replace($startTime, '00:00', -5);
$step = (strtotime($endTime)-strtotime($startTime))/12;
for ($fullHour= strtotime($firstFixed)+$step, $i=0 ; $fullHour<$last ; $fullHour=$fullHour+$step, ++$i){
     $array1[$i]=$fullHour;
     $array2[0][$fullHour]=date('jS-H:i', $fullHour);
}

// setup the plotarea
$Graph->add(
   Image_Graph::vertical(
      Image_Graph::factory('title', array($graphName, 10)),        
      $Plotarea = Image_Graph::factory('plotarea', array('axis', 'axis')),
      5
   )
);   

// y-axis Name
$AxisY =& $Plotarea->getAxis(IMAGE_GRAPH_AXIS_Y);
$AxisY->setTitle($AxisYName, "vertical");

// Set minimum value on Y-axis
$paddingY = ($Dataset->maximumY()-$Dataset->minimumY())/10;
$AxisY->forceMinimum($Dataset->minimumY()-$paddingY);

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
//$Plot =& $Plotarea->addNew('Image_Graph_Plot_Smoothed_Area', &$Dataset); 
$Plot =& $Plotarea->addNew('area', &$Dataset); 
$Plot->setFillColor($color); 

// Add grid
$Grid =& Image_Graph::factory('line_grid');
$Plotarea->add($Grid);

// Add marker
/*$Marker =& Image_Graph::factory('Image_Graph_Marker_Box');
$Marker->setFillColor('blue@0.4');
$Marker->setLineColor('black');
$Marker->setSize(3);
$Plot->setMarker($Marker);
*/
$Graph->done(); 
?>

