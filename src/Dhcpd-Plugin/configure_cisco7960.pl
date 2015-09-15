#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
require "/usr/pluto/bin/lmce.pl";

# Declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;
my $Device_EXT;
my $Device_SECRET;
my $IntIP;

# Check params
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

# Sync with Asterisk DB (practically do nothing but create a new extension number)
#`/usr/pluto/bin/db_phone_config.sh`;

# Let's see what the database thinks about the extension of this phone
$Device_EXT = get_device_devicedata($Device_ID,31);
$Device_SECRET = get_device_devicedata($Device_ID,128);

chomp($Device_EXT);
$IntIP = getCoreIP();
if ($IntIP eq "") {
        exit(-1);
}

$Device_MAC =~ s/[^0-9A-Fa-f]//g;

$Device_MAC = uc($Device_MAC);

my $str_SIPDefault = "";
my $str_SIPMAC = "";

$str_SIPDefault .= "proxy1_address: \"$IntIP\"\n";
$str_SIPDefault .= "proxy1_port: 5060\n";
$str_SIPDefault .= "line1_name: \"\"\n";
$str_SIPDefault .= "line1_authname: \"\"\n";
$str_SIPDefault .= "line1_password: \"\"\n";
$str_SIPDefault .= "line1_shortname: \"\"\n";
$str_SIPDefault .= "proxy2_address: \"\"\n";
$str_SIPDefault .= "proxy2_port: 5060\n";
$str_SIPDefault .= "line2_name: \"\"\n";
$str_SIPDefault .= "line2_authname: \"\"\n";
$str_SIPDefault .= "line2_password: \"\"\n";
$str_SIPDefault .= "line2_shortname: \"\"\n";
$str_SIPDefault .= "proxy3_address: \"\"\n";
$str_SIPDefault .= "proxy3_port: 5060\n";
$str_SIPDefault .= "line3_name: \"\"\n";
$str_SIPDefault .= "line3_authname: \"\"\n";
$str_SIPDefault .= "line3_password: \"\"\n";
$str_SIPDefault .= "line3_shortname: \"\"\n";
$str_SIPDefault .= "proxy4_address: \"\"\n";
$str_SIPDefault .= "proxy4_port: 5060\n";
$str_SIPDefault .= "line4_name: \"\"\n";
$str_SIPDefault .= "line4_authname: \"\"\n";
$str_SIPDefault .= "line4_password: \"\"\n";
$str_SIPDefault .= "line4_shortname: \"\"\n";
$str_SIPDefault .= "proxy5_address: \"\"\n";
$str_SIPDefault .= "proxy5_port: 5060\n";
$str_SIPDefault .= "line5_name: \"\"\n";
$str_SIPDefault .= "line5_authname: \"\"\n";
$str_SIPDefault .= "line5_password: \"\"\n";
$str_SIPDefault .= "line5_shortname: \"\"\n";
$str_SIPDefault .= "proxy6_address: \"\"\n";
$str_SIPDefault .= "proxy6_port: 5060\n";
$str_SIPDefault .= "line6_name: \"\"\n";
$str_SIPDefault .= "line6_authname: \"\"\n";
$str_SIPDefault .= "line6_password: \"\"\n";
$str_SIPDefault .= "line6_shortname: \"\"\n";
$str_SIPDefault .= "proxy_register: 1\n";
$str_SIPDefault .= "timer_register_expires: 180\n";
$str_SIPDefault .= "preferred_codec: g711ulaw\n";
$str_SIPDefault .= "tos_media: 5\n";
$str_SIPDefault .= "dtmf_inband: 1\n";
$str_SIPDefault .= "dtmf_outofband: avt\n";
$str_SIPDefault .= "dtmf_db_level: 3\n";
$str_SIPDefault .= "timer_t1: 500\n";
$str_SIPDefault .= "timer_t2: 4000\n";
$str_SIPDefault .= "sip_retx: 10\n";
$str_SIPDefault .= "sip_invite_retx: 6\n";
$str_SIPDefault .= "timer_invite_expires: 180\n";
$str_SIPDefault .= "tftp_cfg_dir: \"./\"\n";
$str_SIPDefault .= "dnd_control: 0\n";
$str_SIPDefault .= "callerid_blocking: 0\n";
$str_SIPDefault .= "anonymous_call_block: 0\n";
$str_SIPDefault .= "dtmf_avt_payload: 101\n";
$str_SIPDefault .= "sync: 1\n";
$str_SIPDefault .= "phone_label: \"Pluto\"\n";

$str_SIPMAC .= "line1_name: \"$Device_EXT\"\n";
$str_SIPMAC .= "line1_authname: \"$Device_EXT\"\n";
$str_SIPMAC .= "line1_password: \"$Device_SECRET\"\n";
$str_SIPMAC .= "line1_shortname: \"$Device_EXT\"\n";

open(FILE,"> /tftpboot/SIPDefault.cnf");
print FILE $str_SIPDefault;
close(FILE);

open(FILE,"> /tftpboot/SIP$Device_MAC.cnf");
print FILE $str_SIPMAC;
close(FILE);
