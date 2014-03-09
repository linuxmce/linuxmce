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
		$res=$dbADO->Execute('SELECT IRData, FK_Command, Command.Description from InfraredGroup_Command, Command where PK_InfraredGroup_Command=? and PK_Command=FK_Command',$irgcID);
		if($res->RecordCount()!=0){
			$row=$res->FetchRow();
			$oldData=$row['IRData'];
		}else{
			$_GET['error']='IR/GSD code doesn\'t exists.';
		}
		
		$out.='
		
	<!-- CodePress -->	
	<style>
	body {margin:15px;}
	fieldset {background-color: #FFF;}
	#codepress {width:100%;height:100%;border:1px solid gray;frameborder:0;}
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
	function AddPageEvent(o,e,f){
		if (o.addEventListener){ o.addEventListener(e,f,true); return true; }
		else if (o.attachEvent){ return o.attachEvent("on"+e,f); }
		else { return false; }
	}

	function setCode(){
		document.editCode.irdata.value=getAsPlainText();
		document.editCode.submit();
	}
	function resizeEditor(event) {
		document.getElementById("codepress").style.height = (window.innerHeight - 120) + "px";
	}
	AddPageEvent(window,"load",resizeEditor);
	AddPageEvent(window,"resize",resizeEditor);
	</script>

	<form action="index.php" method="post" name="editCode">
	<input type="hidden" name="section" value="editCode">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="irgcID" value="'.$irgcID.'">		
	<input type="hidden" name="from" value="'.$from.'">		
	<input type="hidden" name="irdata" value="">		

	<fieldset>
	<legend>Edit code #'.$irgcID.' - #'.$row['FK_Command'].' '.$row['Description'] .'</legend>
	<div class="err">'.stripslashes(@$_GET['error']).'</div>
	<div class="confirm" align="center"><B>'.stripslashes(@$_GET['msg']).'</B></div>
	<iframe id="codepress" src="codeLoader.php?irgcID='.$irgcID.'"></iframe><br /><br/>
	<div align="center"><input type="button" class="button" name="save" value="'.$TEXT_UPDATE_CONST.'" onClick="setCode();"> <input type="button" class="button" name="close" value="'.$TEXT_CLOSE_CONST.'" onclick="self.close();"></div>
	</fieldset>
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
