#!/usr/bin/perl

# by foxi352 - september 2011:
#
# THIS HAS BEEN REPLACED BY A NEW SHELL SCRIPT.
# THIS FILE STILL EXISTS WAITING FOR EVERY CALL TO IT TO BE REPLACED
#
# TODO: REPLACE EVERY CALL TO THIS FILE BY db_phone_config.sh AND DELETE THIS PERL SCRIPT
if (-e '/usr/pluto/bin/db_phone_config.sh') {
	`/usr/pluto/bin/db_phone_config.sh`;
	exit 0;
} 

use strict;
#use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
# Some helper functions
require "/usr/pluto/bin/lmce.pl";

#declare vars (it's safer this way)
my $DEVICE_ID=0;
my $DEVICE_EXT = 0;
my $DEVICE_SECRET = "";
my $DEVICE_TYPE = "";
my $DEVICE_PORT = 0;

my $DB_PL_HANDLE;
my $DB_AS_HANDLE;

my $STATUS = 0;

# Set the secret to be used for connecting
$DEVICE_SECRET=$DEVICE_EXT;

#fix permissions on each run
`chmod g+w /etc/asterisk/*`;

#connect to databases
$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Could not connect to MySQL";
$DB_AS_HANDLE = DBI->connect(&read_pluto_cred('asterisk')) or die "Could not connect to MySQL";

#replace with the ip address of the asterisk server
my $serv_IP=getIP();
open(DAT, '/etc/asterisk/sccp.conf');
my @data=<DAT>;
open(OUT, ">/etc/asterisk/sccp.conf.$$");

for my $line (@data) {
        if($line=~/bindaddr/) {
                $line =~ s/bindaddr = .*(; .+)$/bindaddr = $serv_IP $1/g;
                print OUT $line;
        } else {
                print OUT $line;
        }
}
`mv /etc/asterisk/sccp.conf.$$ /etc/asterisk/sccp.conf`;

my %DEVICES = ();
my %EXTENSIONS = ();
&add_embed_phones();

while(my $I = shift @ARGV)
{
    $DEVICES{$I}=1;
}

foreach my $I (sort keys %DEVICES)
{
    $DEVICE_ID = $I;
    $DEVICE_EXT = 0;
    $DEVICE_SECRET="";
    &read_pluto_device_data();
    
    if($STATUS == -1)
    {
        &writelog("Will delete device #$DEVICE_ID from FreePBX\n");
        &remove_from_asterisk_db();
    }
    else
    {
        # If we do not have an extension, we find the next unused extension
        # add a record to the devicedata table, and generate a secret for it
        if($DEVICE_EXT == 0)
        {
            &find_next_extension();
            &update_device_data();
        }
        unless(defined $EXTENSIONS{$DEVICE_EXT})
        {
            $EXTENSIONS{$DEVICE_EXT}=$DEVICE_ID;
        }
        else
        {
            &writelog("Conflict detected for extension $DEVICE_EXT with devices #".$EXTENSIONS{$DEVICE_EXT}." and #$DEVICE_ID\n");
            die "Conflict detected";
        }
        &writelog("Processing #$DEVICE_ID ($DEVICE_EXT)\n");
        $DEVICE_PORT = 4569 if($DEVICE_TYPE =~ /^iax/i);
        $DEVICE_PORT = 5060 if($DEVICE_TYPE =~ /^sip$/i);
	$DEVICE_PORT = 5061 if($DEVICE_TYPE =~ /^sip5061$/i);
        &update_asterisk_db();
        #export the extension for other scripts
        `echo $DEVICE_EXT > /tmp/phone${DEVICE_ID}extension`;
    }
}

$DB_PL_HANDLE->disconnect();
$DB_AS_HANDLE->disconnect();

#run AMP's scripts to generate asterisk's config
`/var/lib/asterisk/bin/retrieve_conf`;

# Change dtmf mode (thx freepbx for being so buggy)
`sed -i 's/^dtmfmode=.*\$/dtmfmode=auto/g' /etc/asterisk/sip_additional.conf`;

#reload asterisk
`/usr/sbin/asterisk -r -x reload`;

#helpers
sub writelog()
{
    my $str = shift;
    print $str;
    `echo -n -e "$str" >> /tmp/sync.log`;
}

sub getIP {
        my $sth = $DB_PL_HANDLE->prepare("SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7");
        $sth->execute || die "Sql Error";
        my $row = $sth->fetchrow_hashref;
        my $IP = $row->{IPaddress};
        return $IP;
}

sub read_pluto_device_data()
{
    my $DB_SQL;
    my $DB_STATEMENT;

    $DB_SQL = "select FK_Device, IK_DeviceData, FK_DeviceData from Device_DeviceData where FK_Device=$DEVICE_ID and (FK_DeviceData in (31,29,128))";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $STATUS = -1;
    while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        if($DB_ROW->{'FK_DeviceData'} == 31)
        {
            $DEVICE_EXT = $DB_ROW->{'IK_DeviceData'};
        }
        elsif($DB_ROW->{'FK_DeviceData'} == 29)
        {
            $DEVICE_TYPE = $DB_ROW->{'IK_DeviceData'};
        }
        elsif($DB_ROW->{'FK_DeviceData'} == 128) 
        {
            $DEVICE_SECRET = $DB_ROW->{'IK_DeviceData'};
        }
        $STATUS=0;
    }
    $DB_STATEMENT->finish();
}

sub find_next_extension()
{
    my $DB_SQL;
    my $DB_STATEMENT;
    my $DB_ROW;

    $DB_SQL = "select IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        my $extention = $DB_ROW->{'IK_DeviceData'};
        if($extention > $DEVICE_EXT)
        {
            $DEVICE_EXT = $extention;
        }
    }
    if($DEVICE_EXT == 0)
    {
        $DEVICE_EXT = 200; #200 is default for AMP
    }
    else
    {
        $DEVICE_EXT = $DEVICE_EXT+1;
    }
    $DB_STATEMENT->finish();    
}

sub update_device_data()
{
    my $DB_SQL;
    my $DB_STATEMENT;
    my $DB_ROW;

    $DB_SQL = "update Device_DeviceData SET IK_DeviceData='$DEVICE_EXT' WHERE FK_Device='$DEVICE_ID' AND FK_DeviceData='31'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->finish();

    # Generate a random 16 character secret and update the device data with it
    $DEVICE_SECRET = generateSecret(16);

    $DB_SQL = "update Device_DeviceData SET IK_DeviceData='$DEVICE_SECRET' WHERE FK_Device='$DEVICE_ID' AND FK_DeviceData='128'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->finish();
}

sub update_asterisk_db()
{
    &remove_from_asterisk_db();
    &add_to_asterisk_db();
}

sub remove_from_asterisk_db()
{
    my $DB_SQL;
    my $DB_STATEMENT;
    my $DB_ROW;
    my $TBL;

    $DB_SQL = "select user from devices where (description like 'pl_".$DEVICE_ID."')";
    $DB_STATEMENT = $DB_AS_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        my $old_ext = $DB_ROW->{'user'};
#        `curl 'http://localhost/admin/config.php?display=extensions&extdisplay=$old_ext&action=del' &> /dev/null`;  LmceCape's fix from the forum.
        `curl 'http://localhost/admin/config.php?type=setup&display=extensions&extdisplay=$old_ext&skip=0&action=del' &> /dev/null`;
    }
    $DB_STATEMENT->finish();    
}

sub add_to_asterisk_db()
{
    my %EXT_VARS = ();
    my $EXT_DATA = "";

    $DEVICE_TYPE = 'iax2' if($DEVICE_TYPE =~ /^iax/i);
    $DEVICE_TYPE = 'sip' if($DEVICE_TYPE =~ /^sip/i);
    $DEVICE_TYPE = 'zap' if($DEVICE_TYPE =~ /^zap/i);
    $DEVICE_TYPE = 'custom' if($DEVICE_TYPE =~ /^sccp/i);

    unless($DEVICE_TYPE =~/^(iax2|sip|zap|custom)$/)
    {
        &writelog("Unsupported device type : \"$DEVICE_TYPE\"\n");
        return;
    }

    $EXT_VARS{'display'}="extensions";
    $EXT_VARS{'extdisplay'}="";
    $EXT_VARS{'action'}="add";
    $EXT_VARS{'tech'}=$DEVICE_TYPE;
    $EXT_VARS{'extension'}=$DEVICE_EXT;
    $EXT_VARS{'name'}="pl_".$DEVICE_ID;
    $EXT_VARS{'devicetype'}="fixed";
    $EXT_VARS{'deviceuser'}="same";
    $EXT_VARS{'password'}="";
    $EXT_VARS{'outboundcid'}="\"pl_".$DEVICE_ID."\" <$DEVICE_EXT>";
    $EXT_VARS{'qualify'}="yes" if ($DEVICE_TYPE eq 'sip');
    $EXT_VARS{'vm'}="disabled";
    $EXT_VARS{'secret'}=$DEVICE_SECRET if ($DEVICE_TYPE ne 'custom');
    $EXT_VARS{'devinfo_dial'}="SCCP/".$DEVICE_EXT if ($DEVICE_TYPE eq 'custom');
    $EXT_VARS{'port'}=$DEVICE_PORT;
    foreach my $var (keys %EXT_VARS)
    {
        my $str = $EXT_VARS{$var};
        $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
        $EXT_DATA .=$var."=".$str."&";
    }
    `curl -d '$EXT_DATA' 'http://localhost/admin/config.php?display=extensions' &> /dev/null`;
}

sub add_embed_phones()
{
    my $DB_SQL;
    my $DB_STATEMENT;

    $DB_SQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=1759";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $DEVICES{$DB_ROW->{'PK_Device'}}=1;
    }
    $DB_STATEMENT->finish();    
}

sub generateSecret {

        my $secret;
        my $_rand;

        my $secret_length = $_[0];
                if (!$secret_length) {
                $secret_length = 10;
        }

        my @chars = split(" ",
                "a b c d e f g h i j k l m n o
                p q r s t u v w x y z - _ % # |
                0 1 2 3 4 5 6 7 8 9");

        srand;

        for (my $i=0; $i <= $secret_length ;$i++) {
                $_rand = int(rand 41);
                $secret .= $chars[$_rand];
        }

        return $secret;
}


# fix some permisions
`chown asterisk.asterisk /usr/share/asterisk/agi-bin/* /etc/asterisk/*`;
