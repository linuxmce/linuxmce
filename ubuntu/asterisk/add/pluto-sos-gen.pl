#!/usr/bin/perl -w

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

#$|=1;

my $db_handle;
my $sql;
my $statement;
my $row;
my @data;

my $mode=0;

#connect to pluto_main database
$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
$sql = "select PK_Device from Device where FK_DeviceTemplate=33;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO SECURITY PLUGIN\n";
    exit(1);
}
$statement->finish();
$sql = "select IK_DeviceData from Device_DeviceData where FK_Device=\'".$row->{PK_Device}."\' and FK_DeviceData='96';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO DEVICEDATA FOR SECURITY PLUGIN\n";
    exit(1);
}
$statement->finish();
@data = split(/[,]/,$row->{IK_DeviceData});

my $list = "";

for(my $i=0;defined($data[$i]);$i+=2)
{
	my $j=int(($i+2)/2);
	&generate_voice("To call the $data[$i] press $j","/tmp/pluto-security-sos-option$j.gsm");
	$list .= "/tmp/pluto-security-sos-option$j.gsm ";
}

print "Generating speech for SOS menu options.\n";
`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/pluto-security-sos-options.gsm`;

$statement->finish();
$db_handle->disconnect();

sub generate_voice()
{
        &generate_voice_festival(@_);
}

sub generate_voice_festival()
{
        my $TEXT = shift;
        my $FILE = shift;
        my $DB_PL_HANDLE2;$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

        my $DB_STATEMENT2;
        my $DB_SQL2;

        #TODO: make proper mysql query to get default voice for voice generation! This is a bit of an ugly hack, but should work!
        $DB_SQL2 = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=283 LIMIT 1";
        $DB_STATEMENT2 = $db_handle->prepare($DB_SQL2) or die "Couldn't prepare query '$DB_SQL2': $DBI::errstr\n";
        $DB_STATEMENT2->execute() or die "Couldn't execute query '$DB_SQL2': $DBI::errstr\n";
        my $DB_ROW2 = $DB_STATEMENT2->fetchrow_hashref();
        my $defaultVoice=$DB_ROW2->{'IK_DeviceData'};
        $DB_STATEMENT2->finish();

        # Create a text file for festival to use
        open TF,">/tmp/festival.txt";
        print TF "$TEXT\n";
        close TF;

        # Create the wave file
        system('text2wave /tmp/festival.txt -o /tmp/festival.wav -eval "(' . $defaultVoice . ')"');

        # Resample and create permanent file
        `/usr/bin/sox /tmp/festival.wav -r 8000 -c 1 $FILE resample -ql`;

        # Clean up the temporary files
        unlink "/tmp/festival.wav";
        unlink "/tmp/festival.txt";
}

