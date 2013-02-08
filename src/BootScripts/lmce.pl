#!/usr/bin/perl
# lmce.pl
# 
# Contains helper function to access LinuxMCE infrastructure
# information from Perl
#

use strict;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
my $DB_PL_HANDLE;

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Could not connect to MySQL";

sub getCoreIP {
  my $dbh = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
  my $sth = $dbh->prepare("SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7");
  $sth->execute || die "Sql Error retrieving IP address for devicetemplate 7";
  my $row = $sth->fetchrow_hashref;
  my $IP = $row->{IPaddress};
  return $IP;
}

sub get_device_devicedata() 
{
  # This function should be called with the DeviceID and DeviceData ID
  my $DEVICE_ID;
  my $DEVICE_DATA;
  my $IK_DeviceData;
  $DEVICE_ID = $_[0];
  $DEVICE_DATA = $_[1];
  # Return specific device data for a specific device
  $IK_DeviceData = "";
  my $sth = $DB_PL_HANDLE->prepare("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $DEVICE_ID AND FK_DeviceData = $DEVICE_DATA;");
  $sth->execute || die "Sql Error receiving Device Data $DEVICE_DATA for Device $DEVICE_ID";
  if (my $row = $sth->fetchrow_hashref())
  { 
    $IK_DeviceData = $row->{IK_DeviceData};
  }
  return $IK_DeviceData;
}
