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

open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);


my $iaxyconf = "dhcp\n";
$iaxyconf .= "codec: ulaw\n";
$iaxyconf .= "server: $IntIP\n";
$iaxyconf .= "user: $Device_EXT\n";
$iaxyconf .= "pass: $Device_EXT\n";
$iaxyconf .= "register\n";

open(FILE,"> /tmp/iaxy.conf");
print FILE $iaxyconf;
close(FILE);

system("/usr/sbin/iaxyprov $Device_IP /tmp/iaxy.conf");

unlink("/tmp/iaxy.conf");
