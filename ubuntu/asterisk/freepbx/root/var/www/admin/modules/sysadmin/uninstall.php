<?php

$cron_out = array();

require_once(dirname(__FILE__) . '/functions.inc.php');
//remove old crons
sysadmin_set_ddns_cron(0);
sysadmin_set_license_cron('delete');

echo "dropping service tables...\n";
sql("DROP TABLE IF EXISTS `ddns_client_config`");
sql("DROP TABLE IF EXISTS `sysadmin_options`");
echo "done<br>\n";


//remove incron-monitered files
unlink('/var/spool/asterisk/sysadmin/network');
unlink('/var/spool/asterisk/sysadmin/dns');
unlink('/var/spool/asterisk/sysadmin/reboot');
unlink('/var/spool/asterisk/sysadmin/poweroff');
unlink('/var/spool/asterisk/sysadmin/tz');
unlink('/var/spool/asterisk/sysadmin/vpnstop');
unlink('/var/spool/asterisk/sysadmin/vpnstart');
unlink('/var/spool/asterisk/sysadmin/vpnget');
unlink('/var/spool/asterisk/sysadmin/phone_configs');
unlink('/var/spool/asterisk/sysadmin/mdadm');
unlink('/var/spool/asterisk/sysadmin/intrusion_detection_stop'); 
unlink('/var/spool/asterisk/sysadmin/intrusion_detection_start');
unlink('/var/spool/asterisk/sysadmin/update_system');
unlink('/var/spool/asterisk/sysadmin/update_system_cron');
unlink('/var/spool/asterisk/sysadmin/restart_httpd');

//rm dir if its empty
if (count(scandir('/var/spool/asterisk/sysadmin/')) <= 2)  {  
      rmdir('/var/spool/asterisk/sysadmin/');
}