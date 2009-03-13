<?
function viewDatalog($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$deviceID=(int)$_REQUEST['deviceID'];

		$out='
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">'.$display.'</td>
			</tr>		
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=viewDatalog&deviceID='.$deviceID.'\';
		}
			setTimeout("doReload()",60000);
		</script>


<IMG SRC="operations/datalog/graph.php?unit=4&device=75&days=1"><br>
<IMG SRC="operations/datalog/graph.php?unit=3&device=75&days=1"><br>
<IMG SRC="operations/datalog/graph.php?unit=2&device=75&days=1"><br>
<IMG SRC="operations/datalog/graph.php?unit=1&device=101&days=1"><br>


		';
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

?>
