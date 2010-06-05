#!/usr/bin/perl -w

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

use vars ('$DCERouter','$PK_Device','$MySqlHost','$MySqlPort','$MySqlUser','$MySqlPassword','$MySqlDBName');

#$|=1;

my $db_handle;
my $sql;
my $statement;
my $row;
my @data;
my $secpluginid;
my $telpluginid;

# Connect to pluto_main database
$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

# Lookup the security plugin device id
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

# Lookup the telecom plugin device id
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

# Lookup devices that can speeak in the house
$sql = "select FK_Device, IK_DeviceData from Device_DeviceData where FK_DeviceData='124';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
my $selected_devices="";
while($row = $statement->fetchrow_hashref())
{
	if($row->{IK_DeviceData} eq '1')
	{
		$selected_devices .= "," if(length($selected_devices)>0);
		$selected_devices .= $row->{FK_Device};
	}
}
$statement->finish();

unless(length($selected_devices)>0)
{
    print STDERR "NO DEVICES TO SPEAK IN THE HOUSE\n";
    exit(1);
}

$sql = "select FK_Device, IK_DeviceData from Device_DeviceData where (FK_Device in ($selected_devices)) and (FK_DeviceData='31');";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
@data=();
while($row = $statement->fetchrow_hashref())
{
	push(@data,$row->{FK_Device});
}

for(my $i=0;defined($data[$i]);$i++)
{
	my $phone=$data[$i];
	print STDERR "Will call device ".$phone."\n";
	# 921 - Make Call   184 - FK_Device_From   83 - PhoneExtension
	`/usr/pluto/bin/MessageSend $DCERouter -targetType device $phone $telpluginid 1 921 184 $phone 83 997`;
}

$statement->finish();
$db_handle->disconnect();
