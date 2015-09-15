#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;

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

#sync with Asterisk DB
`/usr/pluto/bin/db_phone_config.sh`;

open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);
$Device_MAC =~ s/[^0-9A-Fa-f]//g;

$Device_MAC = uc($Device_MAC);

my $str_SIPDefault = "";
my $str_SIPMAC = "";

$str_SIPDefault .= "dhcp: 1\n";
$str_SIPDefault .= "sip registration time: 180\n";
$str_SIPDefault .= "sip rtp port: 10000\n";
$str_SIPDefault .= "sip silence suppression: 0\n";
$str_SIPDefault .= "sip proxy ip: \"$IntIP\"\n";
$str_SIPDefault .= "sip proxy port: 5060\n";
$str_SIPDefault .= "sip registrar ip: \"$IntIP\"\n";
$str_SIPDefault .= "sip registrar port: 5060\n";
$str_SIPDefault .= "sip digit timeout: 4\n";
$str_SIPDefault .= "time server disabled: 0\n";
$str_SIPDefault .= "time server1: \"$IntIP\"\n";

$str_SIPMAC .= "sip line1 auth name: \"$Device_EXT\"\n";
$str_SIPMAC .= "sip line1 password: \"$Device_EXT\"\n";
$str_SIPMAC .= "sip line1 user name: \"$Device_EXT\"\n";
$str_SIPMAC .= "sip line1 display name: \"$Device_EXT\"\n";
$str_SIPMAC .= "sip line1 screen name: \"$Device_EXT\"\n";

open(FILE,"> /tftpboot/aastra.cfg");
print FILE $str_SIPDefault;
close(FILE);

open(FILE,"> /tftpboot/$Device_MAC.cfg");
print FILE $str_SIPMAC;
close(FILE);

