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

$IntIP = getCoreIP();
if ($IntIP eq "") {
        exit(-1);
}

#sync with Asterisk DB (practically do nothing but create a new extension number)
`/usr/pluto/bin/db_phone_config.sh`;

# Let's see what the database thinks about the extension of this phone
$Device_EXT = get_device_devicedata($Device_ID,31);
$Device_SECRET = get_device_devicedata($Device_ID,128);

#open(FILE,"/tmp/phone${Device_ID}extension");
#$Device_EXT=<FILE>;
#close(FILE);

chomp($Device_EXT);

sleep(10);
system("curl -d \"user_active1=on&user_realname1=$Device_EXT&user_name1=$Device_EXT&user_pass1=$Device_SECRET&user_host1=$IntIP&user_pname1=$Device_EXT&Settings=Save\" \"http://$Device_IP/line_login.htm?l=1\" > /dev/null");
sleep(3);
system("curl -d \"user_expiry1=60&Settings=Save\" \"http://$Device_IP/line_sip.htm?l=1\" > /dev/null");
sleep(3);
system("curl -d \"REBOOT=Yes\" http://$Device_IP/confirm.htm > /dev/null");
