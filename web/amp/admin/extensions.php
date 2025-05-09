<?php /* $Id: extensions.php,v 1.39 2005/08/30 23:19:19 rcourtna Exp $ */
//Copyright (C) 2004 Coalescent Systems Inc. (info@coalescentsystems.ca)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

include 'common/php-asmanager.php';



//make sure we can connect to Asterisk Manager
checkAstMan();

/* User specific stuff */

//script to write extensions_additional.conf file from mysql
$wScript1 = rtrim($_SERVER['SCRIPT_FILENAME'],$currentFile).'retrieve_extensions_from_mysql.pl';

//script to write meetme_additional.conf file from mysql 
$wMeetScript = rtrim($_SERVER['SCRIPT_FILENAME'],$currentFile).'retrieve_meetme_conf_from_mysql.pl';

$dispnum = 'extensions'; //used for switch in config.php

//create vars from the request
extract($_REQUEST);

//make sure we can connect to Asterisk Manager
checkAstMan();

//read in the voicemail.conf and set appropriate variables for display
$uservm = getVoicemail();
$vmcontexts = array_keys($uservm);
$vm=false;
foreach ($vmcontexts as $vmcontext) {
	if(isset($uservm[$vmcontext][$extdisplay])){
		//echo $extdisplay.' found in context '.$vmcontext.'<hr>';
		$incontext = $vmcontext;  //the context for the current extension
		$vmpwd = $uservm[$vmcontext][$extdisplay]['pwd'];
		$name = $uservm[$vmcontext][$extdisplay]['name'];
		$email = $uservm[$vmcontext][$extdisplay]['email'];
		$pager = $uservm[$vmcontext][$extdisplay]['pager'];
		//loop through all options
		$options="";
		if (is_array($uservm[$vmcontext][$extdisplay]['options'])) {
			$alloptions = array_keys($uservm[$vmcontext][$extdisplay]['options']);
			if (isset($alloptions)) {
				foreach ($alloptions as $option) {
					if ( ($option!="attach") && ($option!="envelope") && ($option!="saycid") && ($option!="delete") && ($option!="nextaftercmd") && ($option!='') )
						$options .= $option.'='.$uservm[$vmcontext][$extdisplay]['options'][$option].'|';
				}
				$options = rtrim($options,'|');
				// remove the = sign if there are no options set
				$options = rtrim($options,'=');
				
			}
			extract($uservm[$vmcontext][$extdisplay]['options'], EXTR_PREFIX_ALL, "vmops");
		}
		$vm=true;
	}
}

$vmcontext = $_SESSION["AMP_user"]->_deptname; //AMP Users can only add to their department's context
if (empty($vmcontext)) 
	$vmcontext = ($_REQUEST['vmcontext'] ? $_REQUEST['vmcontext'] : $incontext);
if (empty($vmcontext))
	$vmcontext = 'default';

//check if the extension is within range for this user
if (isset($extension) && !checkRange($extension)){
	echo "<script>javascript:alert('". _("Warning! Extension")." ".$extension." "._("is not allowed for your account").".');</script>";
} else {
	//deviceid and extension are the same and fixed
	$deviceid = $extension;
	$deviceuser = $extension;
	$description = $_REQUEST['name'];

	//if submitting form, update database
	switch ($action) {
		case "add":
			adddevice($deviceid,$tech,$dial,$devicetype,$deviceuser,$description);
			adduser($_REQUEST,$vmcontext);
	
			//write out extensions_additional.conf
			exec($wScript1);			
			//write out meetme_additional.conf
			exec($wMeetScript);
			needreload();
		break;
		case "del":
			deldevice($extdisplay);
			deluser($extdisplay,$incontext,$uservm);
			//write out extensions_additional.conf
			exec($wScript1);			
			//write out meetme_additional.conf
			exec($wMeetScript);
			needreload();
		break;
		case "edit":  //just delete and re-add
			deldevice($extdisplay);
			adddevice($deviceid,$tech,$dial,$devicetype,$deviceuser,$description);
			deluser($extdisplay,$incontext,$uservm);
			adduser($_REQUEST,$vmcontext);
			//write out extensions_additional.conf
			exec($wScript1);			
			//write out meetme_additional.conf
			exec($wMeetScript);
			needreload();
		break;
		case "resetall":  //form a url with this option to nuke the AMPUSER & DEVICE trees and start over.
			users2astdb();
			devices2astdb();
		break;
	}
}
?>
</div>

<div class="rnav">
<?php 
$devices = getdevices();
drawListMenu($devices, $_REQUEST['skip'], $dispnum, $extdisplay, _("Extension"));
?>
</div>


<div class="content">
<?php 
	if ($action == 'del') {
		echo '<br><h3>'.$extdisplay.' deleted!</h3><br><br><br><br><br><br><br><br>';
	} else if(empty($tech) && empty($extdisplay)) {
?>
		<h2><?php echo _("Add an Extension")?></h2>
		<h5><?php echo _("Select device technology:")?></h5>
		<li><a href="<?php echo $_REQUEST['PHP_SELF'].'?display='.$display; ?>&tech=sip"><?php echo _("SIP")?></a><br><br>
		<li><a href="<?php echo $_REQUEST['PHP_SELF'].'?display='.$display; ?>&tech=iax2"><?php echo _("IAX2")?></a><br><br>
		<li><a href="<?php echo $_REQUEST['PHP_SELF'].'?display='.$display; ?>&tech=zap"><?php echo _("ZAP")?></a><br><br>
		<li><a href="<?php echo $_REQUEST['PHP_SELF'].'?display='.$display; ?>&tech=custom"><?php echo _("Custom")?></a><br><br>
<?php
	} else {
		$delURL = $_REQUEST['PHP_SELF'].'?'.$_SERVER['QUERY_STRING'].'&action=del';
?>
<?php if ($extdisplay) {	
	$deviceInfo=getdeviceInfo($extdisplay);
	extract($deviceInfo,EXTR_PREFIX_ALL,'devinfo');
	$extenInfo=getExtenInfo($extdisplay);
	extract($extenInfo);
	$tech = $devinfo_tech;
	if (is_array($deviceInfo)) extract($deviceInfo);
?>
		<h2><?php echo strtoupper($tech)." "._("Extension")?>: <?php echo extdisplay; ?></h2>
		<p><a href="<?php echo $delURL ?>"><?php echo _("Delete Extension")?> <?php echo $extdisplay ?></a></p>
<?php } else { ?>
		<h2><?php echo _("Add")." ".strtoupper($tech)." "._("Extension")?></h2>
<?php } ?>
		<form name="addNew" action="<?php $_REQUEST['PHP_SELF'] ?>" method="post">
		<input type="hidden" name="display" value="<?php echo $dispnum?>">
		<input type="hidden" name="action" value="<?php echo ($extdisplay ? 'edit' : 'add') ?>">
		<input type="hidden" name="extdisplay" value="<?php echo $extdisplay ?>">
		<input type="hidden" name="tech" value="<?php echo $tech ?>">
		<table>
		
		<tr><td colspan="2"><h5><?php echo ($extdisplay ? _('Edit Extension') : _('Add Extension')) ?><hr></h5></td></tr>

		<tr <?php echo ($extdisplay ? 'style="display:none"':'') ?>>
			<td>
				<a href="#" class="info"><?php echo _("Extension Number")?><span><?php echo _('Use a unique number.  The device will use this number to authenicate to the system, and users will dial it to ring the device.')?></span></a>:
			</td>
			<td>
				<input type="text" name="extension" value="<?php echo $extdisplay ?>">
			</td>
		</tr>

		<tr>
			<td>
				<a href="#" class="info"><?php echo _("Display Name")?><span><?php echo _("The caller id name for this device will be set to this.")?><br></span></a>:
			</td><td>
				<input type="text" name="name" value="<?php echo $devinfo_description ?>"/>
			</td>
		</tr>
		<input type="hidden" name="devicetype" value="fixed"/>
		<input type="hidden" name="deviceuser" value="same"/>
		<input type="hidden" name="password" value=""/>
		
		<tr>
			<td colspan="2">
				<h5><br>Extension Options<hr></h5>
			</td>
		</tr>
		
		<tr>
			<td>
				<a href="#" class="info"><?php echo _("Outbound CID")?><span><?php echo _("Overrides the caller id when dialing out a trunk. Any setting here will override the common outbound caller id set in the Trunks admin.<br><br>Format: <b>\"caller name\" &lt;#######&gt;</b><br><br>Leave this field blank to disable the outbound callerid feature for this extension.")?><br></span></a>:
			</td><td>
				<input type="text" name="outboundcid" value="<?php echo $outboundcid ?>"/>
			</td>
		</tr>
		
		<tr>
			<td>
				<a href="#" class="info"><?php echo _("Record Incoming")?><span><?php echo _("Record all inbound calls received at this extension.")?><br></span></a>:
			</td><td>
				<select name="record_in"/>
					<option value="Adhoc" <?php  echo ($record_in == "On-Demand") ? 'selected' : '' ?>><?php echo _("On Demand")?>
					<option value="Always" <?php  echo ($record_in == "Always") ? 'selected' : '' ?>><?php echo _("Always")?>
					<option value="Never" <?php  echo ($record_in == "Never") ? 'selected' : '' ?>><?php echo _("Never")?>
				</select>
			</td>
		</tr>
		
		<tr>
			<td>
				<a href="#" class="info"><?php echo _("Record Outgoing")?><span><?php echo _("Record all outbound calls received at this extension.")?><br></span></a>:
			</td><td>
				<select name="record_out"/>
					<option value="Adhoc" <?php  echo ($record_out == "On-Demand") ? 'selected' : '' ?>><?php echo _("On Demand")?>
					<option value="Always" <?php  echo ($record_out == "Always") ? 'selected' : '' ?>><?php echo _("Always")?>
					<option value="Never" <?php  echo ($record_out == "Never") ? 'selected' : '' ?>><?php echo _("Never")?>
				</select>
			</td>
		</tr>
		
		<tr>
			<td colspan="2">
				<h5><br>Device Options<hr></h5>
			</td>
		</tr>
		
<?php
switch(strtolower($tech)) {
	case "zap":
		$basic = array(
			'channel' => '',
		);
		$advanced = array(
			'context' => 'from-internal',
			'signalling' => 'fxo_ks',
			'echocancel' => 'yes',
			'echocancelwhenbridged' => 'no',
			'echotraining' => '800',
			'busydetect' => 'no',
			'busycount' => '7',
			'callprogress' => 'no',
			'dial' => '',
			'accountcode' => ''
		);
	break;
	case "iax2":
		$basic = array(
			'secret' => '',
		);
		$advanced = array(
			'notransfer' => 'yes',
			'context' => 'from-internal',
			'host' => 'dynamic',
			'type' => 'friend',
			'port' => '4569',
			'qualify' => 'no',
			'disallow' => '',
			'allow' => '',
			'dial' => '',
			'accountcode' => ''
		);		
	break;
	case "sip":
		$basic = array(
			'secret' => '',
			'dtmfmode' => 'rfc2833'
		);
		$advanced = array(
			'canreinvite' => 'no',
			'context' => 'from-internal',
			'host' => 'dynamic',
			'type' => 'friend',
			'nat' => 'never',
			'port' => '5060',
			'qualify' => 'no',
			'callgroup' => '',
			'pickupgroup' => '',
			'disallow' => '',
			'allow' => '',
			'dial' => '',
			'accountcode' => ''
		);
	break;
	case "custom":
		$basic = array(
			'dial' => '',
		);
		$advanced = array();
	break;
}

if($extdisplay) {
	foreach($basic as $key => $value) {
		echo "<tr><td>{$key}</td><td><input type=\"text\" name=\"{$key}\" value=\"{$$key}\"/></td></tr>";
	}
	foreach($advanced as $key => $value) {
		echo "<tr><td>{$key}</td><td><input type=\"text\" name=\"{$key}\" value=\"{$$key}\"/></td></tr>";
	}
} else {
	foreach($basic as $key => $value) {
		echo "<tr><td>{$key}</td><td><input type=\"text\" name=\"{$key}\" value=\"{$value}\"/></td></tr>";
	}
	foreach($advanced as $key => $value) {
		echo "<input type=\"hidden\" name=\"{$key}\" value=\"{$value}\"/>";
	}
}
?>

			<tr><td colspan=2>
				<h5><br><br><?php echo _("Voicemail & Directory:")?>&nbsp;&nbsp;&nbsp;&nbsp;
					<select name="vm" onchange="checkVoicemail(addNew);">
						<option value="enabled" <?php  echo ($vm) ? 'selected' : '' ?>><?php echo _("Enabled");?></option> 
						<option value="disabled" <?php  echo (!$vm) ? 'selected' : '' ?>><?php echo _("Disabled");?></option> 
					</select>
				<hr></h5>
			</td></tr>
			<tr><td colspan=2>
				<table id="voicemail" <?php  echo ($vm) ? '' : 'style="display:none;"' ?>>
				<tr>
					<td>
						<a href="#" class="info"><?php echo _("voicemail password")?><span><?php echo _("This is the password used to access the voicemail system.<br><br>This password can only contain numbers.<br><br>A user can change the password you enter here after logging into the voicemail system (*98) with a phone.")?><br><br></span></a>: 
					</td><td>
						<input size="10" type="text" name="vmpwd" value="<?php echo $vmpwd ?>"/>
					</td>
				</tr>
				<tr>
					<td><a href="#" class="info"><?php echo _("email address")?><span><?php echo _("The email address that voicemails are sent to.")?></span></a>: </td>
					<td><input type="text" name="email" value="<?php  echo $email; ?>"/></td>
				</tr>
				<tr>
					<td><a href="#" class="info"><?php echo _("pager email address")?><span><?echo _("Pager/mobile email address that short voicemail notifcations are sent to.")?></span></a>: </td>
					<td><input type="text" name="pager" value="<?php  echo $pager; ?>"/></td>
				</tr>
				<tr>
 					<td><a href="#" class="info"><?php echo _("email attachment")?><span><?php echo _("Option to attach voicemails to email.")?></span></a>: </td>
 					<?php if ($vmops_attach == "yes"){?>
 					<td><input  type="radio" name="attach" value="attach=yes" checked=checked/> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="attach" value="attach=no"/> <?php echo _("no");?></td>
 					<?php } else{ ?>
 					<td><input  type="radio" name="attach" value="attach=yes" /> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="attach" value="attach=no" checked=checked /> <?php echo _("no");?></td> <?php }?>
 				</tr>
 
				<tr>
 					<td><a href="#" class="info"><?php echo _("Play CID")?><span><?php echo _("Read back caller's telephone number prior to playing the incoming message, and just after announcing the date and time the message was left.")?></span></a>: </td>
 					<?php if ($vmops_saycid == "yes"){?>
 					<td><input  type="radio" name="saycid" value="saycid=yes" checked=checked/> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="saycid" value="saycid=no"/> <?php echo _("no");?></td>
 					<?php } else{ ?>
 					<td><input  type="radio" name="saycid" value="saycid=yes" /> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="saycid" value="saycid=no" checked=checked /> <?php echo _("no");?></td> <?php }?>
 				</tr>

				<tr>
 					<td><a href="#" class="info"><?php echo _("Play Envelope")?><span><?php echo _("Envelope controls whether or not the voicemail system will play the message envelope (date/time) before playing the voicemail message. This settng does not affect the operation of the envelope option in the advanced voicemail menu.")?></span></a>: </td>
 					<?php if ($vmops_envelope == "yes"){?>
 					<td><input  type="radio" name="envelope" value="envelope=yes" checked=checked/> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="envelope" value="envelope=no"/> <?php echo _("no");?></td>
 					<?php } else{ ?>
 					<td><input  type="radio" name="envelope" value="envelope=yes" /> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="envelope" value="envelope=no" checked=checked /> <?php echo _("no");?></td> <?php }?>
 				</tr>

				<tr>
 					<td><a href="#" class="info"><?php echo _("Play Next")?><span><?php echo _("If set to \"yes,\" after deleting or saving a voicemail message, the system will automatically play the next message, if no the user will have to press \"6\" to go to the next message")?></span></a>: </td>
 					<?php if ($vmops_nextaftercmd == "yes"){?>
 					<td><input  type="radio" name="nextaftercmd" value="nextaftercmd=yes" checked=checked/> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="nextaftercmd" value="nextaftercmd=no"/> <?php echo _("no");?></td>
 					<?php } else{ ?>
 					<td><input  type="radio" name="nextaftercmd" value="nextaftercmd=yes" /> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="nextaftercmd" value="nextaftercmd=no" checked=checked /> <?php echo _("no");?></td> <?php }?>
 				</tr>

				<tr>
 					<td><a href="#" class="info"><?php echo _("Delete Vmail")?><span><?php echo _("If set to \"yes\" the message will be deleted from the voicemailbox (after having been emailed). Provides functionality that allows a user to receive their voicemail via email alone, rather than having the voicemail able to be retrieved from the Webinterface or the Extension handset.  CAUTION: MUST HAVE attach voicemail to email SET TO YES OTHERWISE YOUR MESSAGES WILL BE LOST FOREVER.")?>
</span></a>: </td>
 					<?php if ($vmops_delete == "yes"){?>
 					<td><input  type="radio" name="delete" value="delete=yes" checked=checked/> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input  type="radio" name="delete" value="delete=no"/> <?php echo _("no");?></td>
 					<?php } else{ ?>
 					<td><input  type="radio" name="delete" value="delete=yes" /> <?php echo _("yes");?> &nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="delete" value="delete=no" checked=checked /> <?php echo _("no");?></td> <?php }?>
 				</tr>
 				
 				<tr>
					<td><a href="#" class="info">vm options<span><?php echo _("Separate options with pipe ( | )")?><br><br>ie: review=yes|maxmessage=60</span></a>: </td>
					<td><input size="20" type="text" name="options" value="<?php  echo $options; ?>" /></td>
				</tr>
				<tr>
					<td><?php echo _("vm context:")?> </td>
					<td><input size="20" type="text" name="vmcontext" value="<?php  echo $vmcontext; ?>" /></td>
				</tr>
			</table>
		</td></tr>
		
		
		<tr>
			<td colspan=2>
				<br><br><h6><input name="Submit" type="button" value="<?php echo _("Submit")?>" onclick="javascript:if(addNew.extension.value=='' || parseInt(addNew.extension.value)!=addNew.extension.value) {alert('<?php echo _("Please enter an extension id")?>')} else {addNew.submit();}"></h6>
			</td>
		</tr>
		</table>
		
		</form>
<?php 		
	} //end if action == delGRP
	

?>


