#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

# Some helper functions
require "/usr/pluto/bin/create_amp_LocalPrefixHelper.pl";

my $DECLARED_USERNAME;
my $DECLARED_USERPASSWD;
my $DECLARED_NUMBER;
my $DECLARED_HOST;
my $DECLARED_PREFIX = "9";
my $LOCAL_PREFIX1 = "";
my $LOCAL_PREFIX2 = "";

my $TRUNK_URL = 'http://localhost/admin/config.php?display=trunks&tech=SIP';
my %TRUNK_VARS = ();
my $TRUNK_DATA = "";

my $OUT_URL = 'http://localhost/admin/config.php?display=routing';
my %OUT_VARS = ();
my $OUT_DATA = "";

my $IN_URL = 'http://localhost/admin/config.php?display=did';
my %IN_VARS = ();
my $IN_DATA = "";

# Check params
unless (defined($ARGV[0]) && defined($ARGV[1]) && defined($ARGV[2]))
{
    print "USAGE :$0 <username> <password> <phone_number> [<register_host>] [<prefix_to_use_the_line>]\n";
    exit(-1);
}

# Fix permissions on each run
`chmod g+w /etc/asterisk/*`;

# Open Firewall Ports
&open_firewallports();

$DECLARED_USERNAME=$ARGV[0];
$DECLARED_USERPASSWD=$ARGV[1];
$DECLARED_NUMBER=$ARGV[2];
$DECLARED_HOST=$ARGV[3] if(defined($ARGV[3]));

# This is not currently passed into the script
# Can be used in the future for multiple trunk support
# $DECLARED_PREFIX=$ARGV[4] if(defined($ARGV[4]));

($LOCAL_PREFIX1, $LOCAL_PREFIX2) = &get_local_prefixes($DECLARED_PREFIX);
# print "LOCAL_PREFIX1: $LOCAL_PREFIX1 \n";
# print "LOCAL_PREFIX2: $LOCAL_PREFIX2 \n";

### ADD TRUNK
$TRUNK_VARS{'display'}="trunks";
$TRUNK_VARS{'extdisplay'}="";
$TRUNK_VARS{'action'}="addtrunk";
$TRUNK_VARS{'tech'}="sip";
$TRUNK_VARS{'outcid'}="";
$TRUNK_VARS{'maxchans'}="";
$TRUNK_VARS{'dialrules'}=$LOCAL_PREFIX1;
$TRUNK_VARS{'autopop'}="";
$TRUNK_VARS{'dialoutprefix'}="";
$TRUNK_VARS{'channelid'}="MagicJack";

$TRUNK_VARS{'peerdetails'}.="canreinvite=no\n";
$TRUNK_VARS{'peerdetails'}.="context=from-pstn\n";
$TRUNK_VARS{'peerdetails'}.="dtmfmode=inband\n";
$TRUNK_VARS{'peerdetails'}.="fromuser=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'}.="host=127.0.0.1\n";
$TRUNK_VARS{'peerdetails'}.="insecure=very\n";
$TRUNK_VARS{'peerdetails'}.="nat=no\n";
$TRUNK_VARS{'peerdetails'}.="port=5070\n";
$TRUNK_VARS{'peerdetails'}.="qualify=2000\n";
$TRUNK_VARS{'peerdetails'}.="secret=$DECLARED_USERPASSWD\n";
$TRUNK_VARS{'peerdetails'}.="type=friend\n";
$TRUNK_VARS{'peerdetails'}.="useragent=MagicJack/2.0.554f (SJ Labs)\n";
$TRUNK_VARS{'peerdetails'}.="username=$DECLARED_USERNAME\n";

$TRUNK_VARS{'register'}="$DECLARED_USERNAME:$DECLARED_USERPASSWD\@127.0.0.1:5070/$DECLARED_NUMBER";

foreach my $var (keys %TRUNK_VARS)
{
    my $str = $TRUNK_VARS{$var};
    $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
    $TRUNK_DATA    .=$var."=".$str."&";
}

`curl -d '$TRUNK_DATA' '$TRUNK_URL' > /dev/null`;

### ADD OUTGOING ROUTING
`curl -L '$OUT_URL&extdisplay=001-9_outside&action=delroute' > /tmp/curl.log`;
open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
my $OUT_ROUTE = "";

while(<PAGE>)
{
    chomp;
    if($_ =~ /[<]option value[=]\"([^\"]+)\"[>]SIP\/MagicJack[<]\/option[>]/)
    {
        $OUT_ROUTE=$1;
    }
}

close(PAGE);

$OUT_VARS{'display'}="routing";
$OUT_VARS{'extdisplay'}="";
$OUT_VARS{'action'}="addroute";
$OUT_VARS{'routename'}="MagicJack";
$OUT_VARS{'routepass'}="";
$OUT_VARS{'dialpattern'}=$LOCAL_PREFIX2;
$OUT_VARS{'trunkpriority[0]'}=$OUT_ROUTE;

exit unless($OUT_ROUTE ne "");

foreach my $var (keys %OUT_VARS)
{
    my $str = $OUT_VARS{$var};
    $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
    $OUT_DATA .=$var."=".$str."&";
}

`rm -f /tmp/curl.log ; curl -d '$OUT_DATA' '$OUT_URL' > /dev/null`;

### ADD INCOMING ROUTING
$IN_VARS{'display'}="did";
$IN_VARS{'extdisplay'}="";
$IN_VARS{'action'}="addIncoming";
$IN_VARS{'extension'}=$DECLARED_NUMBER;
$IN_VARS{'goto0'}="custom";
$IN_VARS{'custom0'}="custom-linuxmce,10".$1.",1" if($OUT_ROUTE=~/(\d)$/);

foreach my $var (keys %IN_VARS)
{
    my $str = $IN_VARS{$var};
    $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
    $IN_DATA.=$var."=".$str."&";
}

`curl -d '$IN_DATA' '$IN_URL' > /dev/null`;

# Run AMP's scripts to generate asterisk's config
`curl 'http://localhost/admin/config.php?handler=reload' > /dev/null`;

# Create telecom defaults
`/usr/pluto/bin/create_telecom_defaults.pl`;

# Generate the MjProxy startup script
open (MYFILE, '>/etc/init.d/StartMjProxy.sh');
print MYFILE "#!/bin/bash\n/usr/pluto/bin/mjproxy 0.0.0.0 5070 $DECLARED_HOST 5070 $DECLARED_USERPASSWD\n";
close (MYFILE);

# Chmod the script and run it
chmod(0777, "/etc/init.d/StartMjProxy.sh");
`/etc/init.d/StartMjProxy.sh`;

# Update RC to run StartMjProxy.sh after each reboot
`update-rc.d StartMjProxy.sh defaults`;

# Reload asterisk
`asterisk -rx "sip reload"`;

sub open_firewallports()
{
    my $DB_PL_HANDLE;
    my $DB_STATEMENT;
    my $DB_SQL;
    my $DB_ROW;
    my $Port = 0;

    $DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
    $DB_SQL = "select SourcePort from Firewall where SourcePort = '5070' and Protocol = 'tcp'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    { $Port = $DB_ROW->{'SourcePort'}; }

    $DB_STATEMENT->finish();

    if($Port != 5070)
    {
	$DB_SQL = "insert into Firewall (Protocol,SourcePort,RuleType) values ('tcp','5070','core_input');";
        $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->finish();

        $DB_SQL = "insert into Firewall (Protocol,SourcePort,RuleType) values ('udp','5070','core_input');";
        $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
        $DB_STATEMENT->finish();
    }
}
