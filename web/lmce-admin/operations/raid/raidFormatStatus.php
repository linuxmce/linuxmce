<?
function raidFormatStatus($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	$raidID=(int)$_REQUEST['raidID'];

	if($raidID==0){
		return '';
	}
	$command='cat /usr/pluto/var/'.$raidID.'_Raid.log';
	$ret=exec_batch_command($command,1);
	

	
	
	if(strpos($ret,'No such file or directory')===false){
		if(strpos($ret,'FORMAT')===false){
			$display=$ret;
		}else{
			$parts=explode(',',$ret);
			$display=formatPercentBox($parts[1]);
		}

		$out='
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">'.$display.'</td>
			</tr>		
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=raidFormatStatus&raidID='.$raidID.'\';
		}
			setTimeout("doReload()",5000);
		</script>
		';
	}else{
		return '';
	}
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function formatPercentBox($percent){
	$out='
	<table width="200" cellpadding="0" cellspacing="0" border="0">
		<tr>
			<td bgcolor="green" width="'.(2*$percent).'">&nbsp;</td>
			<td bgcolor="lightblue" width="'.(200-2*$percent).'">&nbsp;</td>
		</tr>
	</table>';
	
	return $out;
}
?>
