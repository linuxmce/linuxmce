<?php
function leftMediaBrowser($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/leftMediaBrowser.lang.php');
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';	
	
	if($action=='form'){
		$out='
		<form action="index.php" method="POST" name="leftMediaBrowser">
		<input type="hidden" name="section" value="leftMediaBrowser">
		<input type="hidden" name="action" value="search">
		
	<table border="0" cellpading="2" cellspacing="0" width="100%">
			<tr>
				<td colspan="3" align="center" height="133" class="left_frame_logo">
					<a href="index.php" target="_top"><img src="include/images/spacer.gif" border="0" width="220" height="90"></a>
				</td>
			</tr>
			<tr>
				<td colspan="3" align="center" height="9" bgcolor="black"><img src="include/images/spacer.gif" border="0"></td>
			</tr>
	';
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
		$out.='
				<tr>
					<td valign="center" colspan="3" class="left_menu" height="30">'.((@$_SESSION['userLoggedIn']==true)?'<a href="index.php?section=login&action=logout" target="basefrm" >'.$TEXT_LOGOUT_CONST.'</a>':'').'</td>					
				</tr>
		';
	}
		$out.='
		<tr>
			<td colspan="2" class="left_menu"><B>'.$TEXT_SEARCH_CONST.'</B></td>
		</tr>
		<tr class="left_menu">
			<td><input type="text" name="searchString" value="'.@$_SESSION['mediaSearchString'].'"></td>
			<td><input type="submit" class="button" name="go" value="'.$TEXT_GO_CONST.'"></td>
		</tr>
		<tr class="left_menu">
			<td colspan="2"><a href="index.php?section=editMediaFile" target="basefrm">Add new file</a></td>
		</tr>
		<tr class="left_menu">
			<td colspan="2"><hr><B>'.$TEXT_JUMP_CONST.'</B></td>
		</tr>
		<tr class="left_menu">
			<td colspan="2" align="center"><a href="#" onClick="javascript:setSearch(\'none\');">#1-9</a></td>
		</tr>
		';
		for($i=65;$i<91;$i++){
			$out.='
			<tr class="left_menu">
				<td colspan="2" align="center"><a href="#" onClick="javascript:setSearch(\''.chr($i).'\');">'.chr($i).'</a></td>
			</tr>
			';	
		}
		$out.='</table>
		</form>';
	}else{
		if(isset($_POST['go'])){
			$_SESSION['mediaSearchString']=$_POST['searchString'];
			$out.='
			<script>
				setSearch(\'\');
			</script>';
		}
		$out.='
			<script>
				self.location=\'index.php?section=leftMediaBrowser\';
			</script>';
	}	

	$scriptInHead=
		'<script>
		function setSearch(value)
		{
			if(!top.basefrm.document.mainMediaBrowser)
				eval(\'top.basefrm.location="index.php?section=mainMediaBrowser&searchType="+value\');
			else{
				top.basefrm.document.mainMediaBrowser.searchType.value=value;
				top.basefrm.document.mainMediaBrowser.submit();
			}
		}
		</script>';
	
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


?>
