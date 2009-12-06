#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

my $DECLARED_USERNAME;
my $DECLARED_USERPASSWD;
my $DECLARED_NUMBER;
my $DECLARED_HOST;
my $DECLARED_PREFIX = "";
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

#check params
unless (defined($ARGV[0]) && defined($ARGV[1]) && defined($ARGV[2]))
{
    print "USAGE :$0 <username> <password> <phone_number> [<register_host>] [<prefix_to_use_the_line>]\n";
    exit(-1);
}

#fix permissions on each run
`chmod g+w /etc/asterisk/*`;
#add local prefixes
&get_local_prefixes();

$DECLARED_USERNAME=$ARGV[0];
$DECLARED_USERPASSWD=$ARGV[1];
$DECLARED_NUMBER=$ARGV[2];
$DECLARED_HOST=$ARGV[3] if(defined($ARGV[3]));
$DECLARED_PREFIX=$ARGV[4] if(defined($ARGV[4]));

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
$TRUNK_VARS{'channelid'}="messagenet";
#$TRUNK_VARS{'peerdetails'} ="authname=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'} ="auth=md5\n";
#$TRUNK_VARS{'peerdetails'}.="canreinvite=no\n";
$TRUNK_VARS{'peerdetails'}.="context=from-trunk\n";
#$TRUNK_VARS{'peerdetails'}.="dtmf=inband\n";
#$TRUNK_VARS{'peerdetails'}.="dtmfmode=inband\n";
#$TRUNK_VARS{'peerdetails'}.="fromdomain=$DECLARED_HOST\n";
$TRUNK_VARS{'peerdetails'}.="fromuser=$DECLARED_USERNAME\n";
$TRUNK_VARS{'peerdetails'}.="host=$DECLARED_HOST\n";
$TRUNK_VARS{'peerdetails'}.="insecure=invite,port\n";
$TRUNK_VARS{'peerdetails'}.="port=5061\n";
#$TRUNK_VARS{'peerdetails'}.="nat=yes\n";
$TRUNK_VARS{'peerdetails'}.="qualify=yes\n";
$TRUNK_VARS{'peerdetails'}.="secret=$DECLARED_USERPASSWD\n";
$TRUNK_VARS{'peerdetails'}.="type=peer\n";
#$TRUNK_VARS{'peerdetails'}.="user=phone\n";
$TRUNK_VARS{'peerdetails'}.="username=$DECLARED_USERNAME\n";

#$TRUNK_VARS{'usercontext'}="$DECLARED_HOST";
#$TRUNK_VARS{'userconfig'} ="context=from-pstn\n";
#$TRUNK_VARS{'userconfig'} .="dtmf=inband\n";
#$TRUNK_VARS{'userconfig'} .="dtmfmode=inband\n";
#$TRUNK_VARS{'userconfig'} .="fromdomain=$DECLARED_HOST\n";
#$TRUNK_VARS{'userconfig'} .="host=$DECLARED_HOST\n";
#$TRUNK_VARS{'userconfig'} .="insecure=very\n";
#$TRUNK_VARS{'userconfig'} .="nat=yes\n";
#$TRUNK_VARS{'userconfig'} .="secret=$DECLARED_USERPASSWD\n";
#$TRUNK_VARS{'userconfig'} .="type=user\n";
#$TRUNK_VARS{'userconfig'} .="user=$DECLARED_USERNAME\n";
#$TRUNK_VARS{'userconfig'} .="username=$DECLARED_USERNAME\n";

$TRUNK_VARS{'register'}="$DECLARED_USERNAME:$DECLARED_USERPASSWD\@$DECLARED_HOST\:5061\/$DECLARED_USERNAME";
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
    if($_ =~ /[<]option value[=]\"([^\"]+)\"[>]SIP\/messagenet[<]\/option[>]/)
    {
        $OUT_ROUTE=$1;
    }
}
close(PAGE);
$OUT_VARS{'display'}="routing";
$OUT_VARS{'extdisplay'}="";
$OUT_VARS{'action'}="addroute";
$OUT_VARS{'routename'}="messagenet";
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

#run AMP's scripts to generate asterisk's config
`curl 'http://localhost/admin/config.php?handler=reload' > /dev/null`;
#create telecom defaults
`/usr/pluto/bin/create_telecom_defaults.pl`;
#reload asterisk
`asterisk -r -x reload`;

sub get_local_prefixes()
{
    my $DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
    my $DB_STATEMENT;
    my $DB_SQL;
    my $DB_ROW;

    #$LOCAL_PREFIX1 = "112\n411\n911\n9|.\n";
    $LOCAL_PREFIX1 = "53.\n";
    $LOCAL_PREFIX2 = $LOCAL_PREFIX1;
    $DB_SQL = "SELECT IK_DeviceData,FK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceTemplate=34 AND (FK_DeviceData=141 OR FK_DeviceData=142 OR FK_DeviceData=143) ORDER BY FK_DeviceData;";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

    return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
    return unless($DB_ROW->{'FK_DeviceData'} == 141);
    if($DB_ROW->{'IK_DeviceData'} > 0)
    {
        my $prefix = $DB_ROW->{'IK_DeviceData'};
        return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
        return unless($DB_ROW->{'FK_DeviceData'} == 142);
        my $digit = $DB_ROW->{'IK_DeviceData'};
        return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
        return unless($DB_ROW->{'FK_DeviceData'} == 143);
        my $length = $DB_ROW->{'IK_DeviceData'};
        my $short = "";
        my $long = "";
        for( my $i=0;$i<$length;$i++)
        {
            $short .= "X";
            $long .= "X";
        }
        for( my $i=0;$i<length($prefix);$i++)
        {
            $long .= "X";
        }
        $LOCAL_PREFIX1 =~ s/\n[^\n]+\n$/\n/s;
        $LOCAL_PREFIX2 =~ s/\n[^\n]+\n$/\n/s;
        $LOCAL_PREFIX1 .= ($digit<0?"":$digit.$prefix."+").$short."\n";
        $LOCAL_PREFIX1 .= ($digit<0?"":$digit."+").$long."\n";
        $LOCAL_PREFIX1 .= $DECLARED_PREFIX."|.\n";
        $LOCAL_PREFIX2 .= $short."\n";
        $LOCAL_PREFIX2 .= $long."\n";
        $LOCAL_PREFIX2 .= "9|112\n9|411\n9|911\n";
        $LOCAL_PREFIX2 .= "9|".($digit<0?"":$digit).$long."\n";
		$LOCAL_PREFIX2 .= "9|0.\n9|*.\n";		
    }
}
