<?php
//session_start();

if ($_REQUEST['log']) {
	$logname=$_REQUEST['log'];
	$_SESSION['log'] = $logname;
} else {
	$logname = $_SESSION['log'];
}

if(!ereg('/var/log/pluto/',$logname) || !is_readable($logname)){
        die('<pre style="background:black;color:white;">Access denied to '.$logname.'</pre>');
}

/**
 * Require the library
 */
require '../../include/PHPTail.php';
/**
 * Initilize a new instance of PHPTail
 * @var PHPTail
 */
$tail = new PHPTail($logname);

/**
 * We're getting an AJAX call
 */
if(isset($_GET['ajax']))  {
	echo $tail->getNewLines($_GET['lastsize'], $_GET['grep'], $_GET['invert']);
	die();
}
/**
 * Regular GET/POST call, print out the GUI
 */
$tail->generateGUI();
?>
