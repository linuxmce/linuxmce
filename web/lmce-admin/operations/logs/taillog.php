<?
$logname=$_REQUEST['log'];
if(!ereg('/var/log/pluto/',$logname)){
	die('<pre style="background:black;color:white;">Access denied to '.$logname.'</pre>');
}
?>

<pre style="background:black;color:white;">
<?
print $logname.'<br><br>';
if($logname!=''){
	$cmd='tail -f '.$logname.' | /usr/pluto/bin/ansi2html';
	system($cmd);
}
?>
</pre>