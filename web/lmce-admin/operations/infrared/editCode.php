<?php
function editCode($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');

	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$irgcID = (int)@$_REQUEST['irgcID'];
	if($irgcID==0){
		die('Invalid ID.');
	}
	
	if ($action=='form') {		
		$res=$dbADO->Execute('SELECT IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irgcID);
		if($res->RecordCount()!=0){
			$row=$res->FetchRow();
			$oldData=$row['IRData'];
		}else{
			$_GET['error']='IR/GSD code doesn\'t exists.';
		}
		
		$out.='
		
	<!-- CodePress -->	
	<style>
	body {color:#000;background-color:white;font:15px georgia, "Lucida Grande", "Lucida Sans Unicode", Arial, Helvetica, sans-serif; letter-spacing:0.01em;margin:15px;}
	p {margin:0 0 15px 0;}
	a,a:visited {color:#7f0055;}
	select {background:#ffffe1;}
	button {margin-top:5px;}
	#logo {text-align:center;background-color:#d6d6d6;padding:10px;-moz-border-radius:5px;border:1px solid silver;}
	#container {width:700px;margin:20px auto;padding:25px;border:3px solid #d9d9d9;-moz-border-radius:10px;background:#f8f8f8;}
	#languages {margin:5px 0;}
	#codepress {width:100%;height:400px;border:1px solid gray;frameborder:0;}
	#default {font-weight:bold;color:red;}
	</style>
	
	<script>
	function getAsPlainText() {
		var IFrameObj = document.getElementById("codepress").contentWindow;
		var textWithoutHighlighting = IFrameObj.CodePress.plainText()
		
		return textWithoutHighlighting;
	}
	
	last = null;
	</script>	
	
	<!-- end CodePress -->	
	<script>
	function setCode(){
		document.editCode.irdata.value=getAsPlainText();
		document.editCode.submit();
	}
	
	</script>
	
	

	
		<form action="index.php" method="post" name="editCode">
		<input type="hidden" name="section" value="editCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="irgcID" value="'.$irgcID.'">		
		<input type="hidden" name="from" value="'.$from.'">		
		<input type="hidden" name="irdata" value="">		
		
		<h3>Edit code #'.$irgcID.'</h3>
		<table width="100%">
			<tr>
				<td bgcolor="black"><img src="include/images/spacer.gif" border="0" height="1" width="1"></td>
			</tr>
		</table>
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm" align="center"><B>'.stripslashes(@$_GET['msg']).'</B></div>

		
	<div id="container">

	<iframe id="codepress" src="codeLoader.php?irgcID='.$irgcID.'"></iframe><br /><br/>
	
	
	<div align="center"><input type="button" class="button" name="save" value="'.$TEXT_UPDATE_CONST.'" onClick="setCode();"> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onclick="self.close();"></div>
	<br/><br/>
	
	</div><!--/container-->
		
		</form>
		';
		
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=editCode&irgcID=$irgcID&error=You are not authorised to change the installation.");
			exit(0);
		}
		

		$irData=$_POST['irdata'];

		
		$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array(trim($irData),$irgcID));
		$out.="
			<script>
			    opener.document.forms.{$from}.action.value='form';
			    opener.document.forms.{$from}.submit();
				self.location='index.php?section=editCode&irgcID=$irgcID&from=$from&msg=The code was updated.';
			</script>
				";			
		
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit IR/GSD code');			
	$output->output();
}

function unhtmlentities($chaineHtml)
{ 
	$tmp = get_html_translation_table(HTML_ENTITIES);
	$tmp = array_flip ($tmp);
	$chaineTmp = strtr ($chaineHtml, $tmp);
	return $chaineTmp;
}
?>
