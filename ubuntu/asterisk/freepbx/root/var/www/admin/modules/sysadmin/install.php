<?php

require_once(dirname(__FILE__) . '/functions.inc.php');
global $db, $amp_conf;

$new = $db->query('SELECT * FROM sysadmin_options');
if ($db->IsError($new)) {
	$new = true;
} else {
	$new = false;
}

$sql[] = "CREATE TABLE IF NOT EXISTS `sysadmin_options` (
		`key` VARCHAR(25),
		`value` TEXT,
		UNIQUE KEY `key` (`key`)
		)";

$sql[] = 'CREATE TABLE IF NOT EXISTS `sysadmin_update_log` (
	  `id` int(11) NOT NULL auto_increment,
	  `time` bigint(20) default NULL,
	  `version` varchar(25) default NULL,
	  `status_id` smallint(6) default NULL,
	  `status` varchar(255) default NULL,
	  PRIMARY KEY  (`id`)
	)';
	
if ($new) {
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('fail2ban_whitelist', '127.0.0.1')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('fail2ban_ban_time', '1800')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('fail2ban_max_retry', '8')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('fail2ban_find_time', '600')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('fail2ban_email', ' ')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('ftp_server', '0')";	
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('ups_enabled', '0')";
	$sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('interval', '15')";
        $sql[] = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('staticip', ' ')";
}

foreach ($sql as $q) {
	$result = $db->query($q);
	if($db->IsError($result)){
		die_freepbx($result->getDebugInfo());
	}
}

//add a default cron for system updates if were on pbxact/pbxtebded
$lic = sysadmin_get_license();

//check to see if we need to migrate from the old ddns table
$ip = $db->getOne('SELECT ddns_static_ip FROM ddns_client_config DECS LIMIT 1');
if(!$db->IsError($ip)) {
	if ($ip) {
		sysadmin_set_ddns('59', $ip);
		$db->query('DROP TABLE ddns_client_config');
	}
}

if ($lic['deploy_type'] == 'PBXact' ||  $lic['deploy_type'] == 'PBXtended') {
	//make sure the ddns cron is set
	$ip = sysadmin_get_ddns_install();

	if (isset($ip['interval'], $ip['staticip'])) {
		sysadmin_ddns_set_cron($ip['interval'], $ip['staticip']);
	} else {
		sysadmin_set_ddns_cron(15);
	}
	
	//set a defualt update cron
	sysadmin_service_set_cron('monthly');
	
	//set a defualt license check cron
	sysadmin_set_license_cron();
	
	//set stroage cron
	edit_crontab('storage.php', array('hour' => '*/6', 'command' => '/var/lib/asterisk/bin/storage.php'));
	
	//migrate the update cron pbxact_service table if its exists
	$update = $db->getAll('SELECT * FROM pbxact_service', DB_FETCHMODE_ASSOC);
	if($db->IsError($update)) {
		//echo 'pbxact_service table doesnt exists, probably migrated already';
  	} else {
		foreach ($update as $u) {
			$sql = 'REPLACE INTO sysadmin_options (`key`, value) VALUES (?, ?)';
			$db->query($sql, array('update_' . $u['key'], $u['value']));
			$db->query('DROP TABLE pbxact_service');
		}
		
	}

}

//check to see if we need to migrate from postfix file
$email_setup = $db->getOne('SELECT * FROM sysadmin_options WHERE `key` IN("email_setup_server")');
	if ($email_setup == null) {
		sysadmin_setup_email();
	}

//make sure we dont have a 'broken" license file in place
if (file_exists($amp_conf['ASTAGIDIR'] . '/license-.zl')) {
	unlink($amp_conf['ASTAGIDIR'] . '/license-.zl');
}


//create incron-monitered files
@mkdir('/var/spool/asterisk/sysadmin/');
touch('/var/spool/asterisk/sysadmin/network');
touch('/var/spool/asterisk/sysadmin/dns');
touch('/var/spool/asterisk/sysadmin/reboot');
touch('/var/spool/asterisk/sysadmin/poweroff');
touch('/var/spool/asterisk/sysadmin/tz');
touch('/var/spool/asterisk/sysadmin/vpnstop');
touch('/var/spool/asterisk/sysadmin/vpnstart');
touch('/var/spool/asterisk/sysadmin/vpnget');
touch('/var/spool/asterisk/sysadmin/phone_configs');
touch('/var/spool/asterisk/sysadmin/mdadm');
touch('/var/spool/asterisk/sysadmin/intrusion_detection_stop'); 
touch('/var/spool/asterisk/sysadmin/intrusion_detection_start');
touch('/var/spool/asterisk/sysadmin/intrusion_detection_banned');
touch('/var/spool/asterisk/sysadmin/intrusion_detection');
touch('/var/spool/asterisk/sysadmin/email_setup');
touch('/var/spool/asterisk/sysadmin/ftp_setup');
touch('/var/spool/asterisk/sysadmin/ups_setup');
touch('/var/spool/asterisk/sysadmin/update_system');
touch('/var/spool/asterisk/sysadmin/update_system_cron');
touch('/var/spool/asterisk/sysadmin/restart_httpd');

function sysadmin_get_ddns_install() {
    global $db;
	$sql = 'SELECT * FROM sysadmin_options WHERE `key` IN("ip_update_interval", "staticip")';
	$res = $db->getAll($sql, DB_FETCHMODE_ASSOC);
	$ret = array();
	if($db->IsError($res)){
		die_freepbx($res->getDebugInfo());
	} 
	foreach($res as $r) {
		$ret[$r['key']] = $r['value'];
	}
	
	return $ret;
}

if (! function_exists("out")) {
        function out($text) {
                echo $text."<br />";
        }
}

if (! function_exists("outn")) {
        function outn($text) {
                echo $text;
        }
}

// sysadmin migration
outn(_("checking for ftp_server field.."));
$sql = "SELECT `key` FROM `sysadmin_options` where `key` = 'ftp_server'";
$check = $db->getRow($sql, DB_FETCHMODE_ASSOC);
if($db->IsError($check) || empty($check)) {
        // add new field
        $sql = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('ftp_server', '0');";
        $result = $db->query($sql);
        if($db->IsError($result)) {
                die_freepbx($result->getDebugInfo());
        }
        out(_("OK"));
} else {
        out(_("already exists"));
}

outn(_("checking for ups_enabled field.."));
$sql = "SELECT `key` FROM `sysadmin_options` where `key` = 'ups_enabled'";
$check = $db->getRow($sql, DB_FETCHMODE_ASSOC);
if($db->IsError($check) || empty($check)) {
        // add new field
        $sql = "INSERT INTO `sysadmin_options` (`key`, value) VALUES ('ups_enabled', '0');";
        $result = $db->query($sql);
        if($db->IsError($result)) {
                die_freepbx($result->getDebugInfo());
        }
        out(_("OK"));
} else {
        out(_("already exists"));
}
?>
