<?
function ripStatus($output,$mediadbADO,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
	
	$powerFileID=(int)@$_REQUEST['pf'];
	
	if(isset($_REQUEST['sa'])){
		$sa=$_REQUEST['sa'];

		$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 743 151 '.$sa;
		exec($cmd);
	}
	
	// send get status command
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 739';
	

	exec($cmd,$retArray);

	// todo: remove hardcoded
	// $retArray=array(0=>'OK:',1=>'158:S1-S~S2-F de aia~S5-R bubu~S10-I cucu~S42-N');
	
	foreach ($retArray AS $line){
		if(strpos($line,'158:')!==false){
			$status=str_replace('158:','',$line);
		}
	}

	if(!isset($status)){
		die('<font color="red">'.$TEXT_ERROR_CANNOT_RETRIEVE_RIP_STATUS_CONST.'</font><br>'.join('<br>',$retArray));
	}
	
	$slotsInfo=get_slots_info($powerFileID,$mediadbADO);
	
	$remaining=0;
	if($status==''){
		$out='Status: idle.';
	}else{
		$slotStatusArray=explode('~',$status);
		$tableHeadRow='
			<tr class="tablehead">
				<td align="center"><B>Slot</B></td>
				<td align="center"><B>Disc</B></td>
				<td align="center"><B>Status</B></td>
			</tr>';
		
		$out='
		<table cellpadding="5" cellspacing="3">
			<tr>
				<td align="center">
				<table>'.$tableHeadRow;
		$pos=0;
		$max=count($slotStatusArray);
		$itemsPerCol=ceil($max/4);
		
		foreach ($slotStatusArray AS $slot){
			$pos++;
			$parts=explode('-',$slot);
			$message=substr($parts[1],1);
			$message=($message!=' ' && $message!='')?'- '.$message:'';
			$index=str_replace('S','',$parts[0]);
			
			switch (substr($parts[1],0,1)){
				case 'S';
					$slotStatus='<span style="color:green;"><B>completed</B></span>';
				break;
				case 'F';
					$slotStatus='<span class="err">failed '.$message.'</span>';
				break;
				case 'R';
					$slotStatus='<span class="confirm">ripping '.$message.'</span>';
					$remaining++;
				break;
				case 'N';
					$slotStatus='<span style="color:blue">pending '.$message.'</span> <a href="index.php?section=ripStatus&pf='.$powerFileID.'&sa='.$index.'">[Abort job]</a>';
					$remaining++;
				break;
				case 'I';
					$slotStatus='<span class="confirm">identifying '.$message.'</span>';
					$remaining++;
				break;				
				case 'C';
					$slotStatus='<span class="err">canceled '.$message.'</span>';
				break;				
			}
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';

			$disc=(isset($slotsInfo[$index]['Performer']))?'<span class="confirm">'.$slotsInfo[$index]['Performer'].' - '.$slotsInfo[$index]['Album'].'</span>':'<span class="confirm">'.@$slotsInfo[$index]['Title'].'</span>';			
			
			$out.='
				<tr bgcolor="'.$color.'">
					<td align="center">'.$parts[0].'</td>
					<td align="center">'.$disc.'</td>
					<td align="center">'.$slotStatus.'</td>
				</tr>';
		}
	}
	$out.='</table>';
	
	if($remaining>0){
		Header("Refresh: 5; url=index.php?section=ripStatus&pf=$powerFileID");	
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>