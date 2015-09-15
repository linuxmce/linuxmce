#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
require "/usr/pluto/bin/lmce.pl";

#declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;
my $Device_EXT;
my $Device_SECRET;
my $IntIP;

#check params
if($#ARGV < 5 || $ARGV[0] ne "-d" || $ARGV[2] ne "-i" || $ARGV[4] ne "-m")
{
    print "<USAGE-1>\n$0 -d <Device ID> -i <IP> -m <mac address>\n";
    exit(-1);
}
else
{
    $Device_ID = $ARGV[1];
    $Device_IP = $ARGV[3];
    $Device_MAC = $ARGV[5];
}

#sync with Asterisk DB (practically do nothing but create a new extension number)
`/usr/pluto/bin/db_phone_config.sh`;

# Let's see what the database thinks about the extension of this phone
$Device_EXT = get_device_devicedata($Device_ID,31);
$Device_SECRET = get_device_devicedata($Device_ID,128);

chomp($Device_EXT);
$IntIP = getCoreIP();
if ($IntIP eq "") {
	exit(-1);
}
sleep(10);
system("rm -f /tmp/cookie");
my $ret_status;
$ret_status=system("curl \"http://$Device_IP/cgi-bin/webctrl.cgi\" -d \"action=login_authentication&redirect_action=login_page&login_username=admin&login_password=admin\" -c \"/tmp/cookies.txt\" > /dev/null");
$ret_status=$ret_status/256;
if ($ret_status != 0) {
	for (0..4){
		$ret_status=system("curl \"http://$Device_IP/cgi-bin/webctrl.cgi\" -d \"action=login_authentication&redirect_action=login_page&login_username=admin&login_password=admin\" -c \"/tmp/cookies.txt\" > /dev/null");
		$ret_status=$ret_status/256;
		last if ($ret_status == 0);
		sleep(5);
	}
}
exit(-1) if ($ret_status != 0);


system("curl \"http://$Device_IP/cgi-bin/webctrl.cgi\" -d \"action=sipasetting_update&sip_port0_username=$Device_EXT&sip_port0_displayname=$Device_EXT&sip_port0_auth_username=$Device_EXT&sip_port0_auth_password=$Device_SECRET&sip_port0_confirm_password=$Device_EXT&sip_port0_mwi_username=&sip_port0_mwi_auth_username=&sip_port0_mwi_auth_password=&sip_port0_mwi_confirm_password=&sip_port0_mwi_refresh_timeout=3600&sip_port0_voicemsg_netam_id=%23%2310\" -b \"/tmp/cookies.txt\" > /dev/null");

sleep(5);

system("curl \"http://$Device_IP/cgi-bin/webctrl.cgi\" -d \"action=sipssetting_update&sip_auth_timeout=120&sip_port0_domain=$IntIP&sip_port0_registar_server_addr=$IntIP&sip_port0_registar_server_port=5060&sip_port0_proxy_server_addr=$IntIP&sip_port0_proxy_server_port=5060&sip_port0_outboundproxy_enable=1&sip_port0_outboundproxy_server_addr=$IntIP&sip_port0_outboundproxy_server_port=5060&sip_port0_dnssrv_enable=1&call_waiting_enable=1\" -b \"/tmp/cookies.txt\" > /dev/null");

system("rm -f /tmp/cookies.txt");

