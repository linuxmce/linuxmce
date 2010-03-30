<?php 
require_once 'Image/Graph.php'; 

$mysqlhost="localhost"; // MySQL-Host
$mysqluser="root"; // MySQL-User
$mysqlpwd=""; // Password
$mysqldb="lmce_datalog";

$unit = mysql_real_escape_string($_GET['unit']);
$device = mysql_real_escape_string($_GET['device']);
$days = mysql_real_escape_string($_GET['days']);
$color = mysql_real_escape_string($_GET['color']);

if (empty($days)) {
     $days=1;
}

if (empty($color)) {
     $color='red@0.4';
}

$connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");

$query = mysql_query("select Description,timestamp,Datapoint,Unit from Datapoints,Unit where FK_Unit=PK_Unit and EK_Device= ".$device." and timestamp > DATE_SUB(NOW(), INTERVAL ".$days." DAY) order by timestamp");

// Create the graph and set the size
$Graph =& Image_Graph::factory('graph', array(800, 200)); 

// add a TrueType font
$Font =& $Graph->addNew('font', '/usr/share/fonts/truetype/msttcorefonts/verdana.ttf'); 

// set the font size 
$Font->setSize(7); 
$Graph->setFont($Font); 

// Create a dataset for the graph
$Dataset =& Image_Graph::factory('dataset'); 

$t=0; //Counter
$num = mysql_numrows($query); //find number off datapoints for the graph

// Create the datapoints for the Dataset and the lables for x-axis
while ($datapoint = mysql_fetch_array($query)){
     if ($AxisYName == "") {
          $AxisYName = $datapoint[3];
     }
     if ($graphName == "") {
          $graphName = "Device ".$device.' - '.$datapoint[0];
     }
     $Dataset->addPoint(strtotime($datapoint[1]), $datapoint[2]);
     ++$t;
     if ($t==1){
          $first=strtotime($datapoint[1]);
          $firstDate=$datapoint[1];
     }
     if ($t==$num){
          $last=strtotime($datapoint[1]);
     }
}

// Create arrays for x-axis lables
$firstFixed=substr_replace($firstDate, '00:00', -5);

for ($fullHour= strtotime($firstFixed)+3600, $i=0 ; $fullHour<$last ; $fullHour=$fullHour+(7200*$days), ++$i){
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
$Plot =& $Plotarea->addNew('area', &$Dataset); 
$Plot->setFillColor($color); 

// Add grid
$Grid =& Image_Graph::factory('line_grid');
$Plotarea->add($Grid);

$Graph->done(); 
?>

