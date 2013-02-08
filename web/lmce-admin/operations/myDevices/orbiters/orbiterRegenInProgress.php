<?
function orbiterRegenInProgress($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/orbiterRegenInProgress.lang.php');
	
	$orbiterID=(int)$_REQUEST['orbiterID'];
	
	if($orbiterID==0){
		return '';
	}
	$orbiterData=$dbADO->Execute('SELECT PK_Orbiter,RegenInProgress,RegenStatus,RegenPercent from Orbiter WHERE PK_Orbiter=?',$orbiterID);
	if($orbiterData->RecordCount()==0){
		return '';
	}
	$row=$orbiterData->FetchRow();
	if($row['RegenInProgress']==1){
		$out='
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">'.percentBox($row['RegenPercent']).'</td>
			</tr>		
			<tr>
				<td align="center" colspan="2">'.$TEXT_REGEN_STATUS_CONST.': '.$row['RegenStatus'].'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><B><span class="confirm">'.$TEXT_ORBITER_GENERATION_IN_PROCESS_CONST.'</span></B></td>
			</tr>	
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=orbiterRegenInProgress&orbiterID='.$orbiterID.'\';
		}
			setTimeout("doReload()",5000);
		</script>
		';
	}else{
		$out='
		<table width="100%" border="0">
			<tr>
				<td colspan="2" align="center"><B>'.$TEXT_ORBITER_GENERATION_FINISHED_CONST.'</B></td>
			</tr>	
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=orbiterRegenInProgress&orbiterID='.$orbiterID.'\';
		}
			setTimeout("doReload()",5000);
		</script>		
		';
	}
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function percentBox($percent){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/orbiterRegenInProgress.lang.php');
	$percent=($percent>100)?100:$percent;
	
	$out='
	<table width="280" cellpadding="0" cellspacing="0" border="0">
		<tr>
			<td width="80">'.$TEXT_PERCENT_CONST.': '.$percent.'%</td>
			<td bgcolor="green" width="'.(2*$percent).'">&nbsp;</td>';
	$remaining=200-2*$percent;
	if($remaining>0){
		$out.='
			<td bgcolor="lightblue" width="'.$remaining.'">&nbsp;</td>';
	}
	$out.='
		</tr>
	</table>';
	
	return $out;
}

?>
