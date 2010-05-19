#!/usr/bin/perl -w

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
use vars '$DCERouter';
use vars '$PK_Device';
use vars '$MySqlHost';
use vars '$MySqlUser';
use vars '$MySqlPassword';
use vars '$MySqlDBName';

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

for(my $i=0;defined($data[$i]);$i+=2)
{
	my $j=int(($i+2)/2);
	print STDERR "[".$data[$i]."]=".$data[$i+1]."\n";
	`flite -t "To call the $data[$i] press $j" -o /tmp/pluto-security-sos-option$j.wav`;
	#`/usr/bin/sox /tmp/pluto-security-sos-option$j.wav -t raw -r 8000 -s -w -c 1 /tmp/pluto-security-sos-option$j.gsm`;
	`/usr/bin/sox /tmp/pluto-security-sos-option$j.wav -r 8000 -c 1 /tmp/pluto-security-sos-option$j.gsm resample -ql`;
	`rm /tmp/pluto-security-sos-option$j.wav`;
}

$statement->finish();
$db_handle->disconnect();
