<?
function error_message($output) {
	$out='<div class="err" align="center"><B>'.$_SESSION['error_message'].'</B></div><br>
	<div align="center"><a href="'.$_SESSION['retry_url'].'">Retry</a></div>';

	$output->setPageTitle('ERROR!');
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>