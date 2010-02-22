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

#sync with AMP (practically do nothing but create a new extension number)
`/usr/pluto/bin/sync_pluto2amp.pl $Device_ID`;

# Let's see what the database thinks about the extension of this phone
$Device_EXTdb = getDevice_DeviceData($Device_ID,31);
open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);
$IntIP = getCoreIP();
if ($IntIP eq "") {
	$IntIP="192.168.80.1";
}
sleep(10);
system("rm -f /tmp/cookie");
my $ret_status;
$ret_status=system("curl -m 10 -c /tmp/cookie -d \"P2=admin&gnkey=0b82\" http://$Device_IP/dologin.htm > /dev/null");
$ret_status=$ret_status/256;
if ($ret_status != 0) {
	for (0..4){
		$ret_status=system("curl -m 10 -c /tmp/cookie -d \"P2=admin&gnkey=0b82\" http://$Device_IP/dologin.htm > /dev/null");
		$ret_status=$ret_status/256;
		last if ($ret_status == 0);
		sleep(5);
	}
}
exit(-1) if ($ret_status != 0);
$Device_SECRET = $Device_EXT;
system("curl -m 10 -b /tmp/cookie -d \"P47=$IntIP&P48=&P32=1&P34=$Device_SECRET&P35=$Device_EXT&P36=$Device_EXT&P271=1&P270=&P3=&P103=0&P63=0&P31=1&P81=0&P40=5060&P52=1&P99=0&P197=&P33=&P73=0&P29=0&P66=&P191=1&P260=180&P261=90&P262=0&P263=0&P264=0&P266=0&P267=1&P265=0&P272=0&P104=0&P65=0&P90=0&P57=0&P58=8&P59=4&P60=18&P61=3&P62=0&P46=0&P98=0&P198=100&update=Update&gnkey=0b82\" http://$Device_IP/update.htm > /dev/null");
sleep(5);

system("curl -m 10 -b /tmp/cookie http://$Device_IP/rs.htm > /dev/null");
system("rm -f /tmp/cookie");

