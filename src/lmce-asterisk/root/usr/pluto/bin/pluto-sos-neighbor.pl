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
my $secpluginid;
my $telpluginid;
my $asteriskid;

my $mode=0;

# Connect to pluto_main database
$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
$sql = "select PK_Device from Device where FK_DeviceTemplate=33;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO SECURITY PLUGIN\n";
    exit(1);
}

$secpluginid=$row->{PK_Device};
$statement->finish();

$sql = "select PK_Device from Device where FK_DeviceTemplate=34;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO TELECOM PLUGIN\n";
    exit(1);
}

$telpluginid=$row->{PK_Device};
$statement->finish();

$sql = "select PK_Device from Device where FK_DeviceTemplate=45;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO ASTERISK\n";
    exit(1);
}

$asteriskid=$row->{PK_Device};
$statement->finish();

$sql = "select IK_DeviceData from Device_DeviceData where FK_Device=\'".$secpluginid."\' and FK_DeviceData='36';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO DEVICEDATA FOR SECURITY PLUGIN\n";
    exit(1);
}

unless($row->{IK_DeviceData} ne "") {
	print STDERR "NO EXTENSTIONS FOR SECURITY PLUGIN\n";
	exit(1);
}

print "neighbors: ";
print $row->{IK_DeviceData};
print "\n\n";

@data = split(/[,]/,$row->{IK_DeviceData}); 

for(my $i=0;defined($data[$i]);$i+=2)
{
	print 'calling neighbor: ';
	print $data[$i];
	print ": \n";
	`/usr/pluto/bin/MessageSend $DCERouter -targetType device $secpluginid $asteriskid 1 233 75 9$data[$i + 1]\@trusted/n 83 997 84 LinuxMCE 82 Local`;
#	print `/usr/pluto/bin/MessageSend $DCERouter -targetType device $secpluginid $asteriskid 1 233 75 9$data[$i + 1]\@trusted/n 83 997 84 LinuxMCE 82 Local`;
}

$statement->finish();
$db_handle->disconnect();

