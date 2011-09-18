<?php /* $Id: page.parking.php 2243 2006-08-12 17:13:17Z p_lindheimer $ */
//Copyright (C) 2006 Astrogen LLC 
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

$dispnum = 'parking'; //used for switch on config.php
$tabindex = 0;

$action = isset($_POST['action'])?$_POST['action']:'';

$parkingenabled = isset($_POST['parkingenabled']) ? $_POST['parkingenabled'] : '';
$parkext = isset($_POST['parkext']) ?  trim($_POST['parkext']) : '70';
$numslots = isset($_POST['numslots']) ? trim($_POST['numslots']) : '8';
$parkingtime = isset($_POST['parkingtime']) ? trim($_POST['parkingtime']) : '45';
$parkingcontext = isset($_POST['parkingcontext']) ? trim($_POST['parkingcontext']) : 'parkedcalls';
$parkalertinfo = isset($_POST['parkalertinfo']) ? trim($_POST['parkalertinfo']) : '';
$parkcid = isset($_POST['parkcid']) ? trim($_POST['parkcid']) : '';
$parkingannmsg_id = isset($_POST['parkingannmsg_id']) ? trim($_POST['parkingannmsg_id']) : '';
$parkinghints = isset($_POST['parkinghints'])?$_POST['parkinghints']:'no';

$ast_ge_18 = version_compare($amp_conf['ASTVERSION'],'1.8','ge');

if ($ast_ge_18) {
  $parking_dest = isset($_POST['parking_dest'])?$_POST['parking_dest']:'device';
  $parkedplay = isset($_POST['parkedplay'])?$_POST['parkedplay']:'both';
  $parkedcalltransfers = isset($_POST['parkedcalltransfers'])?$_POST['parkedcalltransfers']:'caller';
  $parkedcallreparking = isset($_POST['parkedcallreparking'])?$_POST['parkedcallreparking']:'caller';
  $parkedcallhangup = isset($_POST['parkedcallhangup'])?$_POST['parkedcallhangup']:'no';
  $parkedcallrecording = isset($_POST['parkedcallrecording'])?$_POST['parkedcallrecording']:'no';
  $parkedmusicclass = isset($_POST['parkedmusicclass'])?$_POST['parkedmusicclass']:'default';
  $findslot = isset($_POST['findslot'])?$_POST['findslot']:'first';
  $adsipark = isset($_POST['adsipark'])?$_POST['adsipark']:'no';
}

if (isset($_POST['goto0']) && isset($_POST[$_POST['goto0']."0"])) {
        $goto = $_POST[$_POST['goto0']."0"];
} else {
        $goto = '';
}

// do if we are submitting a form
if(isset($_POST['action'])){ 

	if ($action == 'edtPARKING') {
    $pargs['parkingenabled'] = $parkingenabled;
    $pargs['parkext'] = $parkext;
    $pargs['numslots'] = $numslots;
    $pargs['parkingtime'] = $parkingtime;
    $pargs['parkingcontext'] = $parkingcontext;
    $pargs['parkalertinfo'] = $parkalertinfo;
    $pargs['parkcid'] = $parkcid;
    $pargs['parkingannmsg_id'] = $parkingannmsg_id;
    $pargs['goto'] = $goto;
    $pargs['parkinghints'] = $parkinghints;

    if ($ast_ge_18) {
      $pargs['parking_dest'] = $parking_dest;
      $pargs['parkedplay'] = $parkedplay;
      $pargs['parkedcalltransfers'] = $parkedcalltransfers;
      $pargs['parkedcallreparking'] = $parkedcallreparking;
      $pargs['parkedcallhangup'] = $parkedcallhangup;
      $pargs['parkedcallrecording'] = $parkedcallrecording;
      $pargs['parkedmusicclass'] = $parkedmusicclass;
      $pargs['findslot'] = $findslot;
      $pargs['adsipark'] = $adsipark;
    }
		parking_update($pargs);
		needreload();
		redirect_standard();
	}
}
?>
</div>
<div class="content">
<?php

// get the parkinglot settings if not a submit
//
if (!$action) {
	$parkinglot_id = 1; // only 1 lot now but prepare for future
	$parkingInfo = parking_getconfig($parkinglot_id);
	if (is_array($parkingInfo)) extract($parkingInfo);
}

?>
<h2><?php echo _("Parking Lot Configuration")?></h2>
<form name="parking" action="config.php" method="post" onsubmit="return parking_onsubmit();">
<input type="hidden" name="display" value="parking"/>
<input type="hidden" name="action" value="edtPARKING"/>
<table>
<tr><td colspan="2"><h5><?php echo _("Parking Lot Options")?><hr></h5></td></tr>
	<tr>
	<td><a href=# class="info"><?php echo _("Enable Parking Lot Feature")?><span><?php echo _("Check this box to enable the parking feature")?></span></a></td>
	<td align=right><input type="checkbox" value="s" name="parkingenabled" <?php  echo ($parkingenabled ? 'CHECKED' : '')?> tabindex="<?php echo ++$tabindex;?>"></td>
	</tr>
	<tr>
	<td><a href=# class="info"><?php echo _("Parking Lot Extension:")?><span>
<?php echo _("This is the extension where you will transfer a call to park it.")?><br>
	</span></a></td>
	<td align=right><input type="text" size="3" name="parkext" value="<?php  echo htmlspecialchars($parkext)?>" tabindex="<?php echo ++$tabindex;?>"/></td>
	</tr>
	<tr>
		<td><a href="#" class="info"><?php echo _("Number of Slots:")?><span><?php echo _("The total number of parking lot spaces to configure. Example, if 70 is the extension and 8 slots are configured, the parking slots will be 71-78")?></span></a></td>
		<td align=right>
			<select name="numslots" tabindex="<?php echo ++$tabindex;?>">
			<?php
				$default = (isset($numslots) ? $numslots : 8);
				for ($i=1; $i <= 30; $i++) {
					echo '<option value="'.$i.'" '.($i == $default ? 'SELECTED' : '').'>'.$i.'</option>';
				}
			?>
			</select>
		</td>
	</tr>
	<tr>
	<td><a href="#" class="info"><?php echo _("Parking Timeout:")?><span><?php echo _("The timeout period that a parked call will attempt to ring back the original parker if not answered")?></span></a></td>
		<td align=right>
			<select name="parkingtime" tabindex="<?php echo ++$tabindex;?>">
			<?php
				$default = (isset($parkingtime) ? $parkingtime : 45);
				for ($i=15; $i <= 600; $i+=15) {
					echo '<option value="'.$i.'"'.($i == $parkingtime ? ' SELECTED' : '').'>'.parking_timeString($i,true).'</option>';
				}
			?>
			</select>
		</td>
	</tr>
<?php
  if ($ast_ge_18 && function_exists('music_list')) {
?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Parked Music Class:")?><span><?php echo _("This is the music class that will be played to a parked call while in the parking lot UNLESS the call flow prior to parking the call explicitly set a different music class, such as if the call came in through a queue or ring group.")?></span></a></td>
		<td align="right">
			<select name="parkedmusicclass" tabindex="<?php echo ++$tabindex;?>">
<?php
    $tresults = music_list();
    $none = array_search('none',$tresults);
    if ($none !== false) {
      unset($tresults[$none]);
    }
    if (isset($tresults)) {
      foreach ($tresults as $tresult) {
        $searchvalue="$tresult";
        $ttext = $tresult;
        if($tresult == 'default') $ttext = _("default");						
        echo '<option value="'.$tresult.'" '.($searchvalue == $parkedmusicclass ? 'SELECTED' : '').'>'.$ttext;
      }
    }
?>		
			</select>		
		</td>
	</tr>
<?php
  }
?>
	<tr>
	<td><a href=# class="info"><?php echo _("Parking Lot Context:")?><span>
<?php echo _("This is the parking lot context. You should not change it from the default unless you know what you are doing.")?><br>
	</span></a></td>
	<td align=right><input type="text" size="19" name="parkingcontext" value="<?php  echo htmlspecialchars($parkingcontext)?>" tabindex="<?php echo ++$tabindex;?>"/></td>
	</tr>

	<tr>
	<td><a href=# class="info"><?php echo _("Enable BLF Capabilities:")?><span><?php echo _("Check this box to have Asterisk 'hints' generated to use with BLF buttons.")?></span></a></td>
	<td align=right><input type="checkbox" value="yes" name="parkinghints" <?php  echo ($parkinghints == 'yes' ? 'CHECKED' : '')?> tabindex="<?php echo ++$tabindex;?>"></td>
	</tr>
<?php
  if ($ast_ge_18) {
?>
	<tr>
	<td><a href=# class="info"><?php echo _("Use Next Slot:")?><span><?php echo _("Asterisk: findslot. Check this box if you want the parking lot to seek the next sequential parking slot relative to the the last parked call instead of seeking the first available slot. Normally this box is left un-checked.")?></span></a></td>
	<td align=right><input type="checkbox" value="yes" name="findslot" <?php  echo ($findslot == 'yes' ? 'CHECKED' : '')?> tabindex="<?php echo ++$tabindex;?>"></td>
	</tr>
	<tr>
	<td><a href=# class="info"><?php echo _("Enable ADSI Announcements:")?><span><?php echo _("Check this box if you are using ADSI enabled analog phones and want to have ADSI parking announcements sent to the phones. Other adsi.conf configurations may be necessary for this to work.")?></span></a></td>
	<td align=right><input type="checkbox" value="yes" name="adsipark" <?php  echo ($adsipark == 'yes' ? 'CHECKED' : '')?> tabindex="<?php echo ++$tabindex;?>"></td>
	</tr>

<tr><td colspan="2"><br><h5><?php echo _("Returned Call Behavior:")?><hr></h5></td></tr>

  <tr>
  <td><a href="#" class="info"><?php echo _("Pickup Courtesy Tone:")?><span><?php echo _("Whom to play the courtesy tone to when a parked call is retrieved.")?></span></a></td>
  <td>
    <input type="radio" name="parkedplay" id="parkedplay-caller" value="caller" <?php echo $parkedplay == 'caller' ? "checked=\"yes\"" : ""?>/><label for="parkedplay-caller"><?php echo _("Caller") ?></label>
    <input type="radio" name="parkedplay" id="parkedplay-callee" value="callee" <?php echo $parkedplay == 'callee' ? "checked=\"yes\"" : ""?>/><label for="parkedplay-callee"><?php echo _("Parked") ?></label>
    <input type="radio" name="parkedplay" id="parkedplay-both" value="both" <?php echo $parkedplay == 'both' ? "checked=\"yes\"" : ""?>/><label for="parkedplay-both"><?php echo _("Both") ?></label>
  </td>
	</tr>				

  <tr>
  <td><a href="#" class="info"><?php echo _("Transfer Capability:")?><span><?php echo _("Asterisk: parkedcalltransfers. Enables or disables DTMF based transfers when picking up a parked call.")?></span></a></td>
  <td>
    <input type="radio" name="parkedcalltransfers" id="parkedcalltransfers-caller" value="caller" <?php echo $parkedcalltransfers == 'caller' ? "checked=\"yes\"" : ""?>/><label for="parkedcalltransfers-caller"><?php echo _("Caller") ?></label>
    <input type="radio" name="parkedcalltransfers" id="parkedcalltransfers-callee" value="callee" <?php echo $parkedcalltransfers == 'callee' ? "checked=\"yes\"" : ""?>/><label for="parkedcalltransfers-callee"><?php echo _("Parked") ?></label>
    <input type="radio" name="parkedcalltransfers" id="parkedcalltransfers-both" value="both" <?php echo $parkedcalltransfers == 'both' ? "checked=\"yes\"" : ""?>/><label for="parkedcalltransfers-both"><?php echo _("Both") ?></label>
    <input type="radio" name="parkedcalltransfers" id="parkedcalltransfers-no" value="no" <?php echo $parkedcalltransfers == 'no' ? "checked=\"yes\"" : ""?>/><label for="parkedcalltransfers-no"><?php echo _("Neither") ?></label>
  </td>
	</tr>				

  <tr>
  <td><a href="#" class="info"><?php echo _("Re-Parking Capability:")?><span><?php echo _("Asterisk: parkedcallreparking. Enables or disables DTMF based parking when picking up a parked call.")?></span></a></td>
  <td>
    <input type="radio" name="parkedcallreparking" id="parkedcallreparking-caller" value="caller" <?php echo $parkedcallreparking == 'caller' ? "checked=\"yes\"" : ""?>/><label for="parkedcallreparking-caller"><?php echo _("Caller") ?></label>
    <input type="radio" name="parkedcallreparking" id="parkedcallreparking-callee" value="callee" <?php echo $parkedcallreparking == 'callee' ? "checked=\"yes\"" : ""?>/><label for="parkedcallreparking-callee"><?php echo _("Parked") ?></label>
    <input type="radio" name="parkedcallreparking" id="parkedcallreparking-both" value="both" <?php echo $parkedcallreparking == 'both' ? "checked=\"yes\"" : ""?>/><label for="parkedcallreparking-both"><?php echo _("Both") ?></label>
    <input type="radio" name="parkedcallreparking" id="parkedcallreparking-no" value="no" <?php echo $parkedcallreparking == 'no' ? "checked=\"yes\"" : ""?>/><label for="parkedcallreparking-no"><?php echo _("Neither") ?></label>
  </td>
	</tr>				

  <tr>
  <td><a href="#" class="info"><?php echo _("One Touch Recording Capability:")?><span><?php echo _("Asterisk: parkedcallrecording. Enables or disables DTMF based one-touch recording when picking up a parked call.")?></span></a></td>
  <td>
    <input type="radio" name="parkedcallrecording" id="parkedcallrecording-caller" value="caller" <?php echo $parkedcallrecording == 'caller' ? "checked=\"yes\"" : ""?>/><label for="parkedcallrecording-caller"><?php echo _("Caller") ?></label>
    <input type="radio" name="parkedcallrecording" id="parkedcallrecording-callee" value="callee" <?php echo $parkedcallrecording == 'callee' ? "checked=\"yes\"" : ""?>/><label for="parkedcallrecording-callee"><?php echo _("Parked") ?></label>
    <input type="radio" name="parkedcallrecording" id="parkedcallrecording-both" value="both" <?php echo $parkedcallrecording == 'both' ? "checked=\"yes\"" : ""?>/><label for="parkedcallrecording-both"><?php echo _("Both") ?></label>
    <input type="radio" name="parkedcallrecording" id="parkedcallrecording-no" value="no" <?php echo $parkedcallrecording == 'no' ? "checked=\"yes\"" : ""?>/><label for="parkedcallrecording-no"><?php echo _("Neither") ?></label>
  </td>
	</tr>				

  <tr>
  <td><a href="#" class="info"><?php echo _("DTMF Based Hangup Capability:")?><span><?php echo _("Asterisk: parkedcallhangup. Enables or disables DTMF based hangups when picking up a parked call.")?></span></a></td>
  <td>
    <input type="radio" name="parkedcallhangup" id="parkedcallhangup-caller" value="caller" <?php echo $parkedcallhangup == 'caller' ? "checked=\"yes\"" : ""?>/><label for="parkedcallhangup-caller"><?php echo _("Caller") ?></label>
    <input type="radio" name="parkedcallhangup" id="parkedcallhangup-callee" value="callee" <?php echo $parkedcallhangup == 'callee' ? "checked=\"yes\"" : ""?>/><label for="parkedcallhangup-callee"><?php echo _("Parked") ?></label>
    <input type="radio" name="parkedcallhangup" id="parkedcallhangup-both" value="both" <?php echo $parkedcallhangup == 'both' ? "checked=\"yes\"" : ""?>/><label for="parkedcallhangup-both"><?php echo _("Both") ?></label>
    <input type="radio" name="parkedcallhangup" id="parkedcallhangup-no" value="no" <?php echo $parkedcallhangup == 'no' ? "checked=\"yes\"" : ""?>/><label for="parkedcallhangup-no"><?php echo _("Neither") ?></label>
  </td>
	</tr>				
<?php
    $alert_info_tt = _("Alert-Info to add to the call prior to sending back to the Originator or to the Alternate Destination.");
    $caller_id_tt = _("String to prepend to the current Caller ID associated with the parked call prior to sending back to the Originator or the Alternate Destination.");
    $announcement_tt = _("Optional message to be played to the call prior to sending back to the Originator or the Alternate Destination.");
	  $dest_label = _("Alternate Destination");
  } else {
    $alert_info_tt = _("Alert-Info to put in channel before going to defined destination below. This can create distinct rings on some SIP phones and can serve to alert the recipients that the call is from an Orphaned parked call");
    $caller_id_tt = _("String to prepend to the current CallerID associated with this call (if any), before going to defined destination below. This can serve to alert the recipients that the call is from an Orphaned parked call");
    $announcement_tt = _("Announcement:")?><span><?php echo _("Optional message to be played to the orphaned caller prior to going on the to supplied destination below.<br><br>To add additional recordings please use the \"System Recordings\" MENU to the left");
	  $dest_label = _("Destination for Orphaned Parked Calls");
?>
<tr><td colspan="2"><br><h5><?php echo _("Actions for Timed-Out Orphans")?><hr></h5></td></tr>
<?php
  }
?>
	<tr>
	<td><a href=# class="info"><?php echo _("Parking Alert-Info:")?><span><?php echo $alert_info_tt ?><br></span></a></td>
	<td align=right><input type="text" size="40" name="parkalertinfo" value="<?php  echo htmlspecialchars($parkalertinfo)?>" tabindex="<?php echo ++$tabindex;?>"/></td>
	</tr>
	<tr>
	<td><a href=# class="info"><?php echo _("CallerID Prepend:")?><span>
<?php echo $caller_id_tt ?><br>
	</span></a></td>
	<td align=right><input type="text" size="20" name="parkcid" value="<?php  echo htmlspecialchars($parkcid)?>" tabindex="<?php echo ++$tabindex;?>"/></td>
	</tr>
<?php if(function_exists('recordings_list')) { //only include if recordings is enabled?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Announcement:")?><span><?php echo $announcement_tt  ?></span></a></td>
		<td align=right>
			<select name="parkingannmsg_id" tabindex="<?php echo ++$tabindex;?>">
			<?php
				$tresults = recordings_list();
				$default = (isset($parkingannmsg_id) ? $parkingannmsg_id : '');
				echo '<option value="">'._("None")."</option>";
				if (isset($tresults[0])) {
					foreach ($tresults as $tresult) {
						echo '<option value="'.$tresult['id'].'"'.($tresult['id'] == $default ? ' SELECTED' : '').'>'.$tresult['displayname']."</option>\n";
					}
				}
			?>
			</select>
		</td>
	</tr>
<?php } else { ?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Announcement:")?><span><?php echo $announcement_tt . '<br /><br />' .  _("You must install and enable the \"Systems Recordings\" Module to edit this option")?></span></a></td>
		<td align=right>
			<?php
				$default = (isset($parkingannmsg_id) ? $parkingannmsg_id : '');
			?>
			<input type="hidden" name="parkingannmsg_id" value="<?php echo $default; ?>"><?php echo ($default != '' ? $default : 'None'); ?>
		</td>
	</tr>
<?php }
?>
	<tr><td colspan="2"><br><h5><?php echo $dest_label?>:<hr></h5></td></tr>
<?php
      if ($ast_ge_18) {
?>
  <tr>
  <td><a href="#" class="info"><?php echo _("Return Destination Behavior:")?><span><?php echo _("Where to send a parked call that has timed out. The call can be sent back to the original device that parked the call, or it can be sent back to the alternate destination. In both cases, any configured Alert-Info, Caller ID prepend, or announcement will be applied to the call. If configured to send back to the Originator and they are not available (phone is offline), the alternate destination will be used.")?></span></a></td>
  <td>
    <input type="radio" name="parking_dest" id="parking_dest-device" value="device" <?php echo $parking_dest == 'device' ? "checked=\"yes\"" : ""?>/><label for="parking_dest-device"><?php echo _("Originator") ?></label>
    <input type="radio" name="parking_dest" id="parking_dest-dest" value="dest" <?php echo $parking_dest == 'dest' ? "checked=\"yes\"" : ""?>/><label for="parking_dest-dest"><?php echo _("Alternate Destination") ?></label>
  </td>
	</tr>
  <tr><td><br /></td></tr>
<?php }
//draw goto selects
echo drawselects($goto,0);
?>
	<tr>
		<td colspan="2"><br><h6><input name="Submit" type="submit" value="<?php echo _("Submit Changes")?>" tabindex="<?php echo ++$tabindex;?>"></h6></td>
	</tr>
	</table>

<script language="javascript">
<!--

var theForm = document.parking;

function parking_onsubmit() {
	var msgInvalidExtension = "<?php echo _('Please enter a valid numeric parking lot extension'); ?>";

	defaultEmptyOK = false;
	if (!isInteger(theForm.parkext.value))
		return warnInvalid(theForm.parkext, msgInvalidExtension);
	return true;
}

//-->
</script>
</form>

