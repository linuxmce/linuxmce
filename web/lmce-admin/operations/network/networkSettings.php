<?
function networkSettings($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/networkSettings.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	// # TODO - We should take wlan adapters into account as well, especially for the external network
	$number_of_cards = exec('/sbin/ifconfig -s | awk \'$1 != "Iface" && $1 != "lo" { print $1 }\' | wc -l');
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	$OfflineMode=exec('sudo -u root /usr/pluto/bin/OfflineMode.sh get');

	$queryDevice='
		SELECT Device.*
			FROM Device
		INNER JOIN DeviceTemplate ON 
			FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND Device.FK_Installation=?
		';
	$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['CategoryCore'],$installationID));
	if($resDevice->RecordCount()!=0){
		$rowDevice=$resDevice->FetchRow();
		$coreID=$rowDevice['PK_Device'];
	}
	$domain=getDeviceData($coreID,$GLOBALS['ComputerDomain'],$dbADO);
	$cname=getDeviceData($coreID,$GLOBALS['ComputerName'],$dbADO);
	
	$queryDHCP='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
	$resDHCP=$dbADO->Execute($queryDHCP,array($coreID,$GLOBALS['DHCPDeviceData']));
	$rowDHCP=$resDHCP->FetchRow();
	$enableDHCP=($rowDHCP['IK_DeviceData']!='')?1:0;
	
	$nonPlutoIP=0;
	$oldCoreDHCP = $rowDHCP['IK_DeviceData'];
	if(ereg(',',$oldCoreDHCP)){
		$nonPlutoIP=1;
		$ipArray=explode(',',$oldCoreDHCP);
		$coreDHCPArray=explode('.',str_replace('-','.',$ipArray[0]));
		$nonPlutoIPArray=explode('.',str_replace('-','.',$ipArray[1]));
	}else{
		$coreDHCPArray=explode('.',str_replace('-','.',$oldCoreDHCP));
	}

	$queryNC='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
	$resNC=$dbADO->Execute($queryNC,array($coreID,$GLOBALS['NetworkInterfaces']));
	if($resNC->RecordCount()>0){
		$rowNC=$resNC->FetchRow();
		$interfacesArray=explode('|',$rowNC['IK_DeviceData']);
		$externalInterfaceArray=explode(',',$interfacesArray[0]);
		if($externalInterfaceArray[1]!='dhcp'){
			$coreIPArray=explode('.',$externalInterfaceArray[1]);
			$coreNetMaskArray=explode('.',$externalInterfaceArray[2]);
			$coreGWArray=explode('.',$externalInterfaceArray[3]);
			$coreDNS1Array=explode('.',$externalInterfaceArray[4]);
			$coreDNS2Array=explode('.',@$externalInterfaceArray[5]);
			$ipFromDHCP=0;
		}else{
			$ipFromDHCP=1;
		}
		$internalInterfaceArray=explode(',',$interfacesArray[1]);
		// old internal core ip
		$oldInternalCoreIP=$internalInterfaceArray[1];

		$internalCoreIPArray=explode('.',$internalInterfaceArray[1]);
		$internalCoreNetMaskArray=explode('.',$internalInterfaceArray[2]);
	}else{
		$fatalError='No record in Device_DeviceData for Network Interfaces.';
	}	

	$swaphtml = "";
  if (!empty($externalInterfaceArray[0]) && !empty($internalInterfaceArray[0]))

	{
		$swaphtml = '<br><input type="submit" class="button" name="swap" value="Swap Interfaces"><br>';
	}
	if ($action == 'form') {
		if(!isset($fatalError)){
			$out.='
	<script>
	function ipFromDHCP()
	{
		newVal=(!document.networkSettings.ipForAnonymousDevices.checked)?true:false;
		newColor=(document.networkSettings.ipForAnonymousDevices.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++){
			eval("document.networkSettings.nonPlutoIP_"+i+".disabled="+newVal+";");
		}
		document.getElementById("nonPluto").style.color=newColor;
		
	}

	function validateElement(elementNameArray,errorMsg)
	{
		for(i=0;i<elementNameArray.length;i++){
			tmp=eval(\'document.networkSettings.\'+elementNameArray[i]+\'.value==""\');
			if(tmp){
				eval(\'document.networkSettings.\'+elementNameArray[i]+\'.focus()\');
				alert(errorMsg);
				return false;
			}
		}
		return true;
	}
			
	function validateForm()
	{
		if(document.networkSettings.ipFrom[1].checked ){
			validIP=validateElement(new Array("coreIP_1","coreIP_2","coreIP_3","coreIP_4"),"Please enter Core\'s IP address.");
			if(validIP)
				validNetMask=validateElement(new Array("coreNetMask_1","coreNetMask_2","coreNetMask_3","coreNetMask_4"),"Please enter Subnet Mask address.");
			if(validNetMask)
				validGW=validateElement(new Array("coreGW_1","coreGW_2","coreGW_3","coreGW_4"),"Please enter Gateway address.");
			if(validGW)
				validDNS1=validateElement(new Array("coreDNS1_1","coreDNS1_2","coreDNS1_3","coreDNS1_4"),"Please enter first DNS address.");
			if(validDNS1)
				document.networkSettings.submit();
		}else
			document.networkSettings.submit();
	}
		
	function setIPRange()
	{
		newVal=(!document.networkSettings.enableDHCP.checked)?true:false;
		newColor=(document.networkSettings.enableDHCP.checked)?"#4E6CA6":"#CCCCCC";

		for(i=1;i<9;i++){
			eval("document.networkSettings.coreDHCP_"+i+".disabled="+newVal+";");
		}
		document.networkSettings.ipForAnonymousDevices.disabled=newVal;
		if(!document.networkSettings.enableDHCP.checked)
			document.networkSettings.ipForAnonymousDevices.checked=false;
		document.getElementById("range").style.color=newColor;
		document.getElementById("provide").style.color=newColor;
		ipFromDHCP();
	}
		
	function setStaticIP(newVal)
	{
		for(i=1;i<5;i++){
			eval("document.networkSettings.coreIP_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreNetMask_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreGW_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreDNS1_"+i+".disabled="+newVal+";");
			eval("document.networkSettings.coreDNS2_"+i+".disabled="+newVal+";");
		}
		newColor=(newVal==false)?"#4E6CA6":"#CCCCCC";
		document.getElementById("coreIPtext").style.color=newColor;
		document.getElementById("coreNMtext").style.color=newColor;
		document.getElementById("coreGWtext").style.color=newColor;
		document.getElementById("coreDNS1text").style.color=newColor;
		document.getElementById("coreDNS2text").style.color=newColor;
	}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="networkSettings">
	<input type="hidden" name="section" value="networkSettings">
	<input type="hidden" name="action" value="add">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0">
		<tr>
			<td colspan="4" class="alternate_back">'.get_network_settings().'</td>
		</tr>	
		<tr>
			<td colspan="4"><B>Domain</B> &nbsp; <input type="text" name="domain" value="'.$domain.'"> &nbsp; <B>Computer name</B> &nbsp; <input type="text" name="cname" value="'.$cname.'"></td>
		</tr>
	
		<tr>
			<td colspan="3">DHCP server on Core:</td>
		</tr>
		<tr>
			<td width="20"><input type="checkbox" name="enableDHCP" value="1" '.(($enableDHCP==1)?'checked':'').' onclick="setIPRange();"></td>
			<td align="left" colspan="2">Enable DHCP server</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><span id="range" style="color:'.(($enableDHCP!=1)?'#CCCCCC':'').'">Range of IP addresses for Pluto devices: <input type="text" maxlength="3" name="coreDHCP_1" size="3" value="'.@$coreDHCPArray[0].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_2" size="3" value="'.@$coreDHCPArray[1].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_3" size="3" value="'.@$coreDHCPArray[2].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_4" size="3" value="'.@$coreDHCPArray[3].'" '.(($enableDHCP!=1)?'disabled':'').'> - <input type="text" maxlength="3" name="coreDHCP_5" size="3" value="'.@$coreDHCPArray[4].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_6" size="3" value="'.@$coreDHCPArray[5].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_7" size="3" value="'.@$coreDHCPArray[6].'" '.(($enableDHCP!=1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDHCP_8" size="3" value="'.@$coreDHCPArray[7].'" '.(($enableDHCP!=1)?'disabled':'').'></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2"><input type="checkbox" name="ipForAnonymousDevices" value="1" '.((@$nonPlutoIP==1)?'checked':'').' onClick="ipFromDHCP()" '.(($enableDHCP==1)?'':'disabled').'><span id="provide" style="color:'.(($enableDHCP!=1)?'#CCCCCC':'').'"> Provide IP addresses for anonymous devices not in Pluto\'s database.</span></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="nonPluto" style="color:'.(($enableDHCP!=1)?'#CCCCCC':'').'">Range of IP addresses for non-Pluto devices: </span><input type="text" maxlength="3" name="nonPlutoIP_1" size="3" value="'.@$nonPlutoIPArray[0].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_2" size="3" value="'.@$nonPlutoIPArray[1].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_3" size="3" value="'.@$nonPlutoIPArray[2].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_4" size="3" value="'.@$nonPlutoIPArray[3].'" '.(($enableDHCP==1)?'':'disabled').'> - <input type="text" maxlength="3" name="nonPlutoIP_5" size="3" value="'.@$nonPlutoIPArray[4].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_6" size="3" value="'.@$nonPlutoIPArray[5].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_7" size="3" value="'.@$nonPlutoIPArray[6].'" '.(($enableDHCP==1)?'':'disabled').'>.<input type="text" maxlength="3" name="nonPlutoIP_8" size="3" value="'.@$nonPlutoIPArray[7].'" '.(($enableDHCP==1)?'':'disabled').'></td>
		</tr>
		<tr>
			<td colspan="3"><b>Number of network adapters</b>: '.$number_of_cards.'</td>
		</tr>
		<tr>
			<td colspan="3"><p>Your core has the following network adapters:<br><B>1. External network card '.@$externalInterfaceArray[0].'</B> </td>
		</tr>
		<tr>
			<td colspan="3"><input type="radio" name="ipFrom" value="DHCP" onclick="setStaticIP(true);"'.(($ipFromDHCP==1)?'checked':'').'> Obtain an IP address from DHCP</td>
		</tr>
		<tr>
			<td colspan="3"><input type="radio" name="ipFrom" value="static" onclick="setStaticIP(false);" '.(($ipFromDHCP==0)?'checked':'').'> Use a static IP address</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td width="150"><span id="coreIPtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">Core\'s IP address:</span></td>
			<td><input type="text" maxlength="3" name="coreIP_1" size="3" value="'.@$coreIPArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_2" size="3" value="'.@$coreIPArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_3" size="3" value="'.@$coreIPArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreIP_4" size="3" value="'.@$coreIPArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>						
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreNMtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">Subnet mask:</span></td>
			<td><input type="text" maxlength="3" name="coreNetMask_1" size="3" value="'.@$coreNetMaskArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_2" size="3" value="'.@$coreNetMaskArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_3" size="3" value="'.@$coreNetMaskArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreNetMask_4" size="3" value="'.@$coreNetMaskArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreGWtext" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">Gateway:</span></td>
			<td><input type="text" maxlength="3" name="coreGW_1" size="3" value="'.@$coreGWArray[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_2" size="3" value="'.@$coreGWArray[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_3" size="3" value="'.@$coreGWArray[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreGW_4" size="3" value="'.@$coreGWArray[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreDNS1text" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">Nameserver (DNS) #1:</span></td>
			<td><input type="text" maxlength="3" name="coreDNS1_1" size="3" value="'.@$coreDNS1Array[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_2" size="3" value="'.@$coreDNS1Array[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_3" size="3" value="'.@$coreDNS1Array[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS1_4" size="3" value="'.@$coreDNS1Array[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td><span id="coreDNS2text" style="color:'.(($ipFromDHCP==1)?'#CCCCCC':'').'">Nameserver (DNS) #2:</span></td>
			<td><input type="text" maxlength="3" name="coreDNS2_1" size="3" value="'.@$coreDNS2Array[0].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_2" size="3" value="'.@$coreDNS2Array[1].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_3" size="3" value="'.@$coreDNS2Array[2].'" '.(($ipFromDHCP==1)?'disabled':'').'>.<input type="text" maxlength="3" name="coreDNS2_4" size="3" value="'.@$coreDNS2Array[3].'" '.(($ipFromDHCP==1)?'disabled':'').'></td>
		</tr>		
		<tr>
			<td colspan="3"><B>2. Internal network card '.@$internalInterfaceArray[0].'</B> </td>
		<tr>
		<tr>
			<td>&nbsp;</td>
			<td width="150">IP address:</td>
			<td><input type="text" maxlength="3" name="internalCoreIP_1" size="3" value="'.@$internalCoreIPArray[0].'">.<input type="text" maxlength="3" name="internalCoreIP_2" size="3" value="'.@$internalCoreIPArray[1].'">.<input type="text" maxlength="3" name="internalCoreIP_3" size="3" value="'.@$internalCoreIPArray[2].'">.<input type="text" maxlength="3" name="internalCoreIP_4" size="3" value="'.@$internalCoreIPArray[3].'"></td>
		</tr>						
		<tr>
			<td>&nbsp;</td>
			<td>Subnet mask:</td>
			<td><input type="text" maxlength="3" name="internalCoreNetMask_1" size="3" value="'.@$internalCoreNetMaskArray[0].'">.<input type="text" maxlength="3" name="internalCoreNetMask_2" size="3" value="'.@$internalCoreNetMaskArray[1].'">.<input type="text" maxlength="3" name="internalCoreNetMask_3" size="3" value="'.@$internalCoreNetMaskArray[2].'">.<input type="text" maxlength="3" name="internalCoreNetMask_4" size="3" value="'.@$internalCoreNetMaskArray[3].'"></td>
		</tr>
		<tr>
			<td colspan=3>'.$swaphtml.'</td>
		</tr>
		<tr>
			<td><input type="checkbox" name="OfflineMode" value="1" '.(($OfflineMode=='true')?'checked':'').'></td>
			<td colspan="2"><B>'.$TEXT_OFFLINEMODE_CONST.'</B></td>
		</tr>			
		<tr>
			<td colspan="3" align="center" bgcolor="#EEEEEE"><input type="button" class="button" name="update" value="Update" onClick="validateForm()"> <input type="reset" class="button" name="reset" value="Reset"></td>
		</tr>		
	</table>
	You may need to open up ports in the firewall for some programs that run on your internal computers, like video conferencing, file sharing, etc.  To do this, visit the Advanced, <a href="index.php?section=firewall">Firewall</a> page.		
	</form>
		<script>
		 	var frmvalidator = new formValidator("networkSettings");
 			frmvalidator.addValidation("internalCoreIP_1","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreIP_2","req","Please enter an IP address.");			
			frmvalidator.addValidation("internalCoreIP_3","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreIP_4","req","Please enter an IP address.");
			frmvalidator.addValidation("internalCoreNetMask_1","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_2","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_3","req","Please enter the subnet mask.");
			frmvalidator.addValidation("internalCoreNetMask_4","req","Please enter the subnet mask.");
		</script>
	';
		}else{
			$out.='<h2 style="color:red;">'.$fatalError.'</h2>';
		}
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=networkSettings&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		$newEnableDHCP=(isset($_POST['enableDHCP']))?1:0;
		
		// new ip range
		$coreDHCP=getIpFromParts('coreDHCP_',1).'-'.getIpFromParts('coreDHCP_',5);

		// new internal core ip
		$internalCoreIP=getIpFromParts('internalCoreIP_');


		$isChanged=0;
		$ipForAnonymousDevices=isset($_POST['ipForAnonymousDevices'])?1:0;
		if($ipForAnonymousDevices==1){
			$coreDHCP.=','.getIpFromParts('nonPlutoIP_').'-'.getIpFromParts('nonPlutoIP_',5);
		}

		if($newEnableDHCP==1){
			if($coreDHCP!=$oldCoreDHCP){
				$SQL='REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_Devicedata) VALUES(?,?,?)';
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['DHCPDeviceData'],$coreDHCP));
			}
		}
		else{
			$SQL='REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_Devicedata) VALUES(?,?,?)';
			$dbADO->Execute($SQL,array($coreID,$GLOBALS['DHCPDeviceData'],''));
		}
		$needReboot=0;
		$willReboot=0;

		$externalInterface=$externalInterfaceArray[0];
		$internalInterface=$internalInterfaceArray[0];
		if(isset($_POST['swap'])){
      $externalInterface=$internalInterfaceArray[0];
      $internalInterface=$externalInterfaceArray[0];

			$needReboot=1;
		}
		if($resNC->RecordCount()>0){
			$networkInterfaces=$externalInterface;
			if($_POST['ipFrom']=='DHCP'){
				$networkInterfaces.=',dhcp|';
			}else{
				$dns1 = getIpFromParts('coreDNS1_');
				$dns2 = getIpFromParts('coreDNS2_');
				$dns_string = $dns1 . ($dns2 === "" ? "" : ",$dns2");
				$networkInterfaces.=','.getIpFromParts('coreIP_').','.getIpFromParts('coreNetMask_').','.getIpFromParts('coreGW_').','.$dns_string.'|';
			}
			if ($internalInterface !== ""){
				$networkInterfaces.=$internalInterface.','.$internalCoreIP.','.getIpFromParts('internalCoreNetMask_');
			}
			if($networkInterfaces!=$rowNC['IK_DeviceData']){
				$SQL="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(?,?,?)";
				$dbADO->Execute($SQL,array($coreID,$GLOBALS['NetworkInterfaces'],$networkInterfaces));
			}
		}

		// NOTE: Please don't reboot the computer before these commands have completed, OK?
		// NOTE: Well, unless you like breaking /etc/network/interfaces randomly because the mysql server shutdown was faster than the scripts
		$commands = array('Network_Setup.sh', 'Network_Firewall.sh');
		for ($i = 0; $i < count($commands); $i++)
		{
			$cmd = "sudo -u root /usr/pluto/bin/{$commands[$i]}";
			exec_batch_command($cmd);
		}
		
		$OfflineMode=(int)@$_REQUEST['OfflineMode'];
		$OfflineMode=($OfflineMode==1)?'true':'false';
		exec_batch_command('sudo -u root /usr/pluto/bin/OfflineMode.sh set '.$OfflineMode);
		
		// set domain & computer name
		$domain=cleanString($_POST['domain']);
		if($domain!=''){
			set_device_data($coreID,$GLOBALS['ComputerDomain'],$domain,$dbADO);
		}
		$cname=cleanString($_POST['cname']);
		if($cname!=''){
			set_device_data($coreID,$GLOBALS['ComputerName'],$cname,$dbADO);
		}

		// NOTE: Serial stuff completed. Now it's OK to reboot.
		if($coreDHCP !=$oldCoreDHCP){
			writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tIP range changed from $oldCoreDHCP to $coreDHCP\n",'a+');
		}
		
		if($internalCoreIP!=$oldInternalCoreIP){
			writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tIP changed from $oldInternalCoreIP to $internalCoreIP\n",'a+');
			// v-- THIS THING REBOOTS THE COMPUTER WITHOUT ASKING!!!
			exec_batch_command('sudo -u root /usr/pluto/bin/InternalIPChange.sh');
			// ^-- THIS THING REBOOTS THE COMPUTER WITHOUT ASKING!!!
			$willReboot=1;
		}
		
		// TODO: call a script who will set the domain and computer name
		
		if ($needReboot)
			$willReboot=1;

		if ($willReboot)
			$ipchanged_msg=' The system will reboot in the following moments.';
		$msg=urlencode("Network settings updated.".@$ipchanged_msg);
		
		header("Location: index.php?section=networkSettings&msg=".$msg);
		if ($needReboot)
			exec_batch_command("sudo -u root /sbin/reboot");
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_NETWORK_SETTINGS_CONST);
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_NETWORK_SETTINGS_CONST=>'index.php?section=networkSettings'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Network Settings');
	$output->output();
}

function getIpFromParts($partName,$startIndex=1)
{
	$ipArray=array();
	for($i=$startIndex;$i<($startIndex+4);$i++)
	{
		$part = @$_POST[$partName.$i];
		if ($part === "")
			return "";
		$ipArray[] = $part;
	}
	return join('.',$ipArray);
}

function get_network_settings(){
	$command='sudo -u root /usr/pluto/bin/Network_DisplaySettings.sh --all';
	$ret=exec_batch_command($command,1);
	
	$lines=explode("\n",$ret);

	if(count($lines)==0){
		return '';
	}
	$out='<table>';
	foreach ($lines AS $line){
		$parts=explode('=',$line);
		if(count($parts)==2){
			$out.='
			<tr>
				<td><b>'.trim($parts[0]).'</b></td>
				<td>'.trim($parts[1]).'</td>
			</tr>';
		}
	}
	$out.='</table>';
	
	return $out;
}
?>
