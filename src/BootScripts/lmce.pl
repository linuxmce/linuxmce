# lmce.pl
# 
# Contains helper function to access LinuxMCE infrastructure
# information from Perl
#

use strict;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Could not connect to MySQL";

function get_device_devicedata(( {
  # This function should be called with the DeviceID and DeviceData ID
  my ($DEVICE_ID, $DEVICE_DATA) = @_;
  # Return specific device data for a specific device
  my $IK_DeviceData;
  my $sth = $DB_PL_HANDLE->prepare("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $DEVICE_ID AND FK_DeviceData = $DEVICE_DATA;");
  $sth->execute || die "Sql Error receiving Device Data $DEVICE_DATA for Device $DEVICE_ID";
  if (my $row = $sth->fetchrow_hashref())
  { 
    $IK_DeviceData = $row->{IK_DeviceData};
  }
  return $IK_DeviceData;
}
