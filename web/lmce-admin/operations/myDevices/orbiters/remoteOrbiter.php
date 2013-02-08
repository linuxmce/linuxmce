<?
function remoteOrbiter($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/remoteOrbiter.lang.php');
	
	$orbiterID=(int)@$_REQUEST['orbiterID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	if(isset($_POST['go'])){
		$address=$_POST['host'];
		$port=$_POST['port'];
		$refresh=$_POST['refresh'];
		$ciscoOrbiter=(int)$_POST['orbiterID'];
		
		// TODO: replace image path
		$iframeSRC='index.php?section=proxySocket&address='.$address.'&port='.$port.'&refresh='.$refresh.'&deviceID='.$ciscoOrbiter;
		$orbiterScreen=
			'<img id="screen" src="include/image.php?imagepath='.getcwd().'/security_images/'.$ciscoOrbiter.'_cisco.png&randno='.rand(10000,99999).'" onClick="sendTouch();">
				<iframe id="imageLoader" src="'.$iframeSRC.'&command=IMAGE"></iframe>';
		$jsFunction='
		<script src="scripts/connectionWizard/connectionWizard.js" type="text/javascript" language="JavaScript"></script>
		<script>
		function findPosX(obj)
		{
			var curleft = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curleft += obj.offsetLeft
					obj = obj.offsetParent;
				}
			}
			else if (obj.x)
				curleft += obj.x;
			return curleft;
		}
		
		function findPosY(obj)
		{
			var curtop = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curtop += obj.offsetTop
					obj = obj.offsetParent;
				}
			}
			else if (obj.y)
				curtop += obj.y;
			return curtop;
		}
				
		function sendTouch()
		{
			xRelative=xMousePos-findPosX(document.getElementById("screen"));
			yRelative=yMousePos-findPosY(document.getElementById("screen"));

			document.getElementById("imageLoader").src="'.$iframeSRC.'&command=TOUCH "+xRelative+"x"+yRelative;
		}
	
		</script>
		';	
		if((int)@$_REQUEST['refresh']>0){
			$jsFunction.='
			<script>
			function do_reload(){
				document.getElementById(\'imageLoader\').contentDocument.location=\''.$iframeSRC.'&command=IMAGE\';
			}
				refresh_interval=setInterval("do_reload()",'.((int)@$_REQUEST['refresh']*1000).');
			</script>';
		}

	}
	
	if($action=='form'){
		$serverIP=(isset($_REQUEST['host']))?$_REQUEST['host']:getCoreIP($dbADO);
		$port=(isset($_POST['port']))?$_POST['port']:'3451';
		$orbiters=getAssocArray('Device','PK_Device','Description',$dbADO,'WHERE FK_Installation='.$_SESSION['installationID'].' AND FK_DeviceTemplate='.$GLOBALS['Cisco7970Orbiter']);
		$out.='
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm"><B>'.@stripslashes($_GET['msg']).'</B></div>
		<form action="index.php" method="POST" name="remoteOrbiter">
			<input type="hidden" name="section" value="remoteOrbiter">
			<input type="hidden" name="action" value="form">
		
		<table cellpadding="3" cellspacing="0">
			<tr>
				<td>'.$TEXT_CHOOSE_ORBITER_CONST.': </td>
				<td>'.pulldownFromArray($orbiters,'orbiterID',$orbiterID).'</td>
			</tr>
			<tr>
				<td>'.$TEXT_IP_ADDRESS_CONST.': </td>
				<td><input type="text" name="host" value="'.$serverIP.'"></td>
			</tr>
			<tr>
				<td>'.$TEXT_PORT_CONST.': </td>
				<td><input type="text" name="port" value="'.$port.'"></td>
			</tr>
			<tr>
				<td>'.$TEXT_REFRESH_S_CONST.': </td>
				<td><input type="text" name="refresh" value="5"></td>
			</tr>
			<tr>
				<td colspan="2">
					<input type="submit" class="button" name="go" value="Submit"> 
					<input type="button" class="button" name="go" value="'.$TEXT_HOME_CONST.'" onClick="document.getElementById(\'imageLoader\').contentDocument.location=\'index.php?section=proxySocket&address='.$serverIP.'&port='.$port.'&command=XML&key=10\';"> 
					<input type="button" class="button" name="go" value="'.$TEXT_BACK_CONST.'" onClick="document.getElementById(\'imageLoader\').contentDocument.location=\'index.php?section=proxySocket&address='.$serverIP.'&port='.$port.'&command=XML&key=11\';">
				</td>
			</tr>
		</table>
		</form>
		'.@$orbiterScreen.'		
		'.@$jsFunction;
	}else{
		
	}	
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}


?>
