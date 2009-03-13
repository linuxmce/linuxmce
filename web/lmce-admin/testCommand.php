<?
session_name('Web-admin');
session_start();


require('include/config/config.inc.php');
require('include/utils.inc.php');

$no=(int)@$_REQUEST['no'];

if($no!=0){
	for($i=1;$i<=$no;$i++){
		$cmd=cleanString(@$_REQUEST['cmd'.$i]);
		exec_batch_command($cmd);
	}
	
	$script='
	<script>
		alert("The command was sent.");
		self.location="testCommand.php";
	</script>';
	
	die($script);
}else{
	// do nothing
}
?>