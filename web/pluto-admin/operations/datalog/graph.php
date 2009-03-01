<?php 
require_once 'Image/Graph.php'; 

$mysqlhost="localhost"; // MySQL-Host
$mysqluser="root"; // MySQL-User
$mysqlpwd=""; // Password
$mysqldb="lmce_datalog";

$unit = mysql_real_escape_string($_GET['unit']);
$device = mysql_real_escape_string($_GET['device']);
$days = mysql_real_escape_string($_GET['days']);

$connection=mysql_connect($mysqlhost, $mysqluser, $mysqlpwd) or die ("ERROR: could not connect to the database!");
mysql_select_db($mysqldb, $connection) or die("ERROR: could not select database!");

$query = mysql_query("select Description,timestamp,Datapoint,Unit from Datapoints,Unit where FK_Unit=PK_Unit and PK_Unit = ".$unit." and EK_Device= ".$device." and timestamp > DATE_SUB(NOW(), INTERVAL ".$days." DAY) order by timestamp");


// create the graph
$Graph =& Image_Graph::factory('graph', array(800, 200));  

// add a TrueType font
$Font =& $Graph->addNew('font', '/usr/share/fonts/truetype/msttcorefonts/verdana.ttf'); 

// set the font size to 11 pixels
$Font->setSize(8); 

$Graph->setFont($Font); 

$Graph->add(
 //   Image_Graph::vertical(
   //     Image_Graph::factory('title', array('Temperature Graph for Device #121', 12)),        
     //   Image_Graph::vertical(
            $Plotarea = Image_Graph::factory('plotarea')
            //$Plotarea = Image_Graph::factory('plotarea', array('axis', 'axis'))
//            $Legend = Image_Graph::factory('legend'),
       //     90
       // ),
      //  5
  //  )
);   
// $Legend->setPlotarea($Plotarea);         


// create the dataset
// $Dataset =& Image_Graph::factory('random', array(10, 2, 15, true)); 
$Dataset =& Image_Graph::factory('dataset');

while ($datapoint = mysql_fetch_array($query)){


$Dataset->addPoint($datapoint[1], $datapoint[2]);


}
// create the 1st plot as smoothed area chart using the 1st dataset
    
$Plot =& $Plotarea->addNew('area', array(&$Dataset)); 
// $Plot =& $Plotarea->addNew('bar', array(&$Dataset)); 

// set a line color
$Plot->setLineColor('gray'); 

// set a standard fill style
$Plot->setFillColor('red@0.4'); 

$AxisA =& $Plotarea->getAxis(IMAGE_GRAPH_AXIS_X);
$AxisA->setFontSize(4); 
$AxisA->setLabelInterval(5);
$AxisA->setTickOptions(-5, 5, 1);
// $AxisA->setLabelInterval('auto', 1);
$AxisA->setLabelOption('offset', 5);
// $AxisA->setTickOptions(-1, 1, 2);
// $AxisA->setLabelOption('showtext', false, 2); 


// output the Graph
$Graph->done(); 


?>

