<?php
global $db;
global $amp_conf;
//for translation only
if (false) {
_("Pickup ParkedCall Any");
}

$fcc = new featurecode('parking', 'parkedcall');
$fcc->setDescription('Pickup ParkedCall Any');
$fcc->setDefault('*85');
$fcc->setProvideDest();
$fcc->update();
unset($fcc);

$sql = "
CREATE TABLE IF NOT EXISTS `parkinglot` 
(
	`id` VARCHAR( 20 ) NOT NULL default '1',
	`keyword` varchar(40) NOT NULL default '',
	`data` varchar(150) NOT NULL default '',
	PRIMARY KEY  (`id`,`keyword`)
)
";
$check = $db->query($sql);
if(DB::IsError($check)) {
	die_freepbx("Can not create parkinglot table");
}

$results = array();
$sql = "SELECT id, keyword, data FROM parkinglot WHERE keyword = 'goto'";
$results = $db->getAll($sql, DB_FETCHMODE_ASSOC);
if (!DB::IsError($results)) { // error - table must not be there
	foreach ($results as $result) {
		$old_dest  = $result['data'];
		$id        = $result['id'];
		$keyword   = $result['keyword'];

		$new_dest = merge_ext_followme(trim($old_dest));
		if ($new_dest != $old_dest) {
			$sql = "UPDATE parkinglot SET data = '$new_dest' WHERE id = '$id'  AND keyword = '$keyword' AND data = '$old_dest'";
			$results = $db->query($sql);
			if(DB::IsError($results)) {
				die_freepbx($results->getMessage());
			}
		}
	}
}

// Version 2.5 migrate to recording ids
//
outn(_("Migrating recordings if needed.."));

$sql = "SELECT `data` FROM `parkinglot` WHERE  `id` = '1' AND `keyword` = 'parkingannmsg'";
$results = $db->getAll($sql, DB_FETCHMODE_ASSOC);
if(DB::IsError($results)) {
	die_freepbx($results->getMessage());
}
if (isset($results[0])) {
	if (trim($results[0]['data']) != '') {
		$rec_id = recordings_get_or_create_id($results[0]['data'], 'parking');
	} else {
		$rec_id = '';
	}
	// Delete just in case
	$sql="DELETE FROM `parkinglot` WHERE `keyword` = 'parkingannmsg_id'";
	$results = $db->query($sql);
	if(DB::IsError($results)) {
		out(_("fatal error"));
		die_freepbx($results->getMessage());
	}
	$sql="INSERT INTO `parkinglot` (`id`, `keyword`, `data`) VALUES ('1', 'parkingannmsg_id', '$rec_id')";
	$results = $db->query($sql);
	if(DB::IsError($results)) {
		out(_("fatal error"));
		die_freepbx($results->getMessage());
	}
	// Either way, delete it if it were there
	$sql="DELETE FROM `parkinglot` WHERE `keyword` = 'parkingannmsg'";
	$results = $db->query($sql);
	if(DB::IsError($results)) {
		out(_("fatal error"));
		die_freepbx($results->getMessage());
	}
	out(_("ok"));
} else {
	out(_("not needed"));
}

// Initialize parkinghints to yes since that was the defacto standard before. If already there
// then this will fail but we don't care since we don't want to replace what is there
//
$sql="INSERT INTO `parkinglot` (`id`, `keyword`, `data`) VALUES ('1', 'parkinghints', 'yes')";
$results = $db->query($sql);

$freepbx_conf =& freepbx_conf::create();

// PARKINGPATCH
//
$set['value'] = false;
$set['defaultval'] =& $set['value'];
$set['readonly'] = 0;
$set['hidden'] = 0;
$set['level'] = 3;
$set['module'] = 'parking';
$set['category'] = 'System Setup';
$set['emptyok'] = 0;
$set['name'] = 'Use Old Parking Patch';
$set['description'] = 'Set to generate some additional dialplan if using a particular patch that was available in Asterisk 1.2 and 1.4 to add behavior to parking like adding Alert Info and CID prepends to timed out parked calls. This patch is mostly obsoleted and the setting will probably go away at some point when Asterisk 1.6+ parking enhacements are added to the module.';
$set['type'] = CONF_TYPE_BOOL;
$freepbx_conf->define_conf_setting('PARKINGPATCH',$set,true);

