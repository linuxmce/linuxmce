#!/usr/bin/perl


use DBI;
use diagnostics;
use strict;
require "/usr/pluto/bin/config_ops.pl";

sub showUsage
{
        print "\n$_[0] -d <Device ID>\n";
        exit(-1);
}

showUsage $0 if ( $#ARGV < 1 || $ARGV[0] ne "-d" );


my $deviceID = $ARGV[1];

my $dbConnection;
my $machineID;
my $installationID;


sub findMachineID
{
        map
        {
                if ( /^(\w+)\s+=\s+(.*)$/ )
                {
                        my $name = $1;
                        my $value = $2;

                        $machineID = $value if ( $name eq "PK_Device" );
                }
        } `cat /etc/pluto.conf`;

}

sub findInstallationID
{
	my $statement = $dbConnection->prepare("select FK_Installation from Device where PK_Device = $machineID");
	
	$statement->execute();
	return $statement->fetchrow_hashref()->{'FK_Installation'};
}

sub updateProxyOrbiterPort {
  my $lastPort = 0;
  my $sth = $dbConnection->prepare("SELECT d.PK_Device AS device,dd.IK_DeviceData AS port FROM Device d INNER JOIN Device_DeviceData dd ON dd.FK_Device=d.PK_Device WHERE FK_Installation=? AND FK_DeviceTemplate=1749 AND FK_DeviceData=119 ORDER BY IK_DeviceData ASC");
  my $sth2 = $dbConnection->prepare("UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=119");
  $sth->execute($installationID);
  while (my $row=$sth->fetchrow_hashref()) {
    if (!$lastPort) {
      # first run, set last port to current port
      $lastPort = $row->{"port"};
    } elsif ($row->{"port"} == $lastPort) {
      # two orbiters using the same port, need to renumber one and start over
      $sth2->execute($row->{"port"}+1,$row->{"device"});
      print "01 Correcting duplicate port " . $row->{"port"} . " for device " . $row->{"device"} . "\n";
      return 0;
    } elsif ($row->{"port"} == $lastPort+1) {
      # good to go
      $lastPort++;
    }
  }
  return 1;
}

$dbConnection = DBI->connect(&read_pluto_cred()) or die "Could not connect to MySQL";

findMachineID;
$installationID = findInstallationID;

print "\n\n00 Configuring new proxy orbiter: $deviceID\n";
print "00 Connected to database\n";
print "00 Found local machine ID: $machineID with installation ID: $installationID\n";

my $result = 0;
while (!$result) {
  $result = updateProxyOrbiterPort
}
