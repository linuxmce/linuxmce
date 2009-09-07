#!/usr/bin/perl
	
use DBI;
#DBI->trace(1);
use Getopt::Std;
require "/usr/pluto/bin/config_ops.pl";

$dbh = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";

sub addslashes {
    my $text = shift;
    $text =~ s/\\/\\\\/g;
    $text =~ s/'/\\'/g;
    $text =~ s/"/\\"/g;
    $text =~ s/\(/\\\(/g;
    $text =~ s/\)/\\\)/g;
    $text =~ s/\[/\\\[/g;
    $text =~ s/\]/\\\]/g;
    $text =~ s/\\0/\\\\0/g;
    return $text;
}

sub RunSQL {
  my $sql = shift;
  my $sth = $dbh->prepare($sql) or die $dbh->err;
	$sth->execute() or die $sth->err;
	my @results;
	while (my $row = $sth->fetchrow_hashref()) {
    push(@results,$row);
  }
  $sth->finish();
	return(@results);
}

sub UseDB {
  my $db = shift;
  my $sql = "use $db";
  $dbh->do($sql);
}

#Create a way to determine if MythTV is installed...
$sql="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36";
@results = RunSQL($sql);
if($results[0]) {
	$MythTV_Installed = $results[0]->{'PK_Device'};
}

sub CleanMythTVSettings
{
  # The mythconverg.settings table may contain entries for hosts that no longer exist... This can happen if a user deletes a MD device.
  # This function will remove entries from the settings table where the host no longer exists...

  #Exit if mythtv is not installed
  return unless $MythTV_Installed;

  print "Performing cleanup of mythtv.settings table...\n";

  $sql="SELECT p.PK_Device FROM pluto_main.Device p 
  LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
  WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1";
  my @results=RunSQL($sql);
  @hostNames=('localhost','dcerouter');
  foreach $row (@results) {
    push(@hostNames,"moon" . $row->{"PK_Device"});
	}

	$sql="DELETE FROM settings WHERE hostname NOT IN('" . join("','",@hostNames) . "')";
	UseDB("mythconverg");
	$dbh->do($sql);
}

sub AddMythTVStorageGroup
{
	## Parameters 
	## 1 - path
	## 2 - groupname
	my $path=shift;
	my $name=shift;
	my $host=shift;

	#Exit if mythtv is not installed
	return unless $MythTV_Installed;
	
	print "Adding MythTV storage group $name for host $host\n";
	$sql="INSERT INTO mythconverg.storagegroup (groupname,hostname,dirname) VALUES ('$name','$host','$path')";
	$dbh->do($sql);
}

sub CheckMythTVStorageGroup
{
	## Parameters 
	## 1 - path
	## 2 - groupname
	my $path=shift;
	my $name=shift;
	my $host=shift;

	#Exit if mythtv is not installed
	return unless $MythTV_Installed;
	
	print "Checking MythTV storage group $name for host $host...";
	$sql="SELECT count(*) AS count FROM storagegroup  WHERE groupname='$name' AND hostname='$host' and dirname='$path'";
	my @results = RunSQL($sql);
	if (!$results[0]->{count}) {
    print "missing!\n";
    AddMythTVStorageGroup($path,$name,$host);
    return;
  }
  print "Ok\n";
}

@TPL_STORAGE_DEVICES=(1790, 1794, 1768, 1769, 1854, 1851, 1849);
$DD_USERS=3;

getopt('d');
$CommaSeparatedDeviceList = $opt_d;

## A list containing the pluto users that need to use those directories
$sql="SELECT PK_Users, UserName FROM Users";
@results = RunSQL($sql);
foreach $row (@results) {
  push(@Users,$row);
}

## Get a list of storage devices that use the pluto dir structure
if ($CommaSeparatedDeviceList eq '') {
	$sql="
		SELECT 
		Device.PK_Device,
		Device.Description
	FROM 
		Device 
		JOIN Device_DeviceData ON FK_Device = PK_Device
	WHERE 
		FK_DeviceTemplate IN (" . join(",",@TPL_STORAGE_DEVICES) . ")
		AND
		FK_DeviceData = $DD_USERS
		AND
		IK_DeviceData LIKE '%-1%'
		AND
		FK_Device_ControlledVia = '$PK_Device'
	ORDER BY PK_Device
	";
	@results=RunSQL($sql);
	foreach $row (@results) {
    push(@Devices,$row);
  }
} else {
        $sql="
                SELECT
                PK_Device,
		Description
        FROM
                Device
                JOIN Device_DeviceData ON FK_Device = PK_Device
        WHERE
                FK_DeviceTemplate IN (" . join(",",@TPL_STORAGE_DEVICES) . ")
                AND
                FK_DeviceData = $DD_USERS
                AND
                IK_DeviceData LIKE '%-1%'
                AND
		PK_Device IN ($CommaSeparatedDeviceList)
	 ORDER BY PK_Device
        ";
	@results=RunSQL($sql);
	foreach $row (@results) {
    push(@Devices,$row);
  }
}

## Make a list of hostnames
$sql="SELECT p.PK_Device FROM pluto_main.Device p 
LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1";
@results=RunSQL($sql);
@hostNames=('localhost','dcerouter');
foreach $row (@results) {
  push(@hostNames,"moon" . $row->{"PK_Device"});
}

# Check for and remove extra or invalid storage group entries
foreach $thisHost (@hostNames) {
	$sql="FROM storagegroup WHERE hostname='$thisHost' AND groupname NOT LIKE 'custom:%' AND dirname NOT LIKE '/home/\%/data/pvr\%'";
  UseDB("mythconverg");
  @results=RunSQL("SELECT count(hostname) as count " . $sql);
	if ($results[0] && $results[0]->{"count"}) {
    $dbh->do("DELETE " . $sql);
    print "Removed $results[0]->{count} invalid Host entries.\n";
	}
}

$sql="FROM storagegroup WHERE hostname not in ('" . join("','",@hostNames) . "')";
@results=RunSQL("SELECT count(hostname) as count " . $sql);
if ($results[0] && $results[0]->{"count"}) {
  $dbh->do("DELETE " . $sql);
  print "Removed $results[0]->{count} extraneous Host entries.\n";
}


foreach $thisDevice (@Devices) {
	$Device_Description = addslashes($thisDevice->{"Description"});
	$sql="FROM storagegroup WHERE dirname LIKE '\%$Device_Description\%' AND groupname NOT LIKE 'custom:%' AND dirname NOT LIKE '/home/\%/data/pvr/$Device_Description \[$thisDevice->{PK_Device}\]'";
	@results=RunSQL("SELECT count(id) as count " . $sql);
	if ($results[0] && $results[0]->{"count"}) {
    $dbh->do("DELETE " . $sql);
    print "Removed $results[0]->{count} invalid Device entries.\n";
	}
}

$sql = "FROM storagegroup s where dirname NOT LIKE '%pvr' AND groupname NOT LIKE 'custom:%'";
foreach $thisDevice (@Devices) {
	$Device_Description = addslashes($thisDevice->{"Description"});
	$sql .= " AND dirname NOT LIKE '%$Device_Description \[$thisDevice->{PK_Device}\]'";
}

@results=RunSQL("SELECT count(id) as count " . $sql);
if ($results[0] && $results[0]->{"count"}) {
  $dbh->do("DELETE " . $sql);
  print "Removed $results[0]->{count} extraneous Device entries.\n";
}

# Now check that every required entry is there, inserting a new one as needed

#For every host...
foreach my $hostName (@hostNames) {

	##lets handle the /home/public paths....
	CheckMythTVStorageGroup("/home/public/data/pvr","Default","$hostName");     #Put the special "Default" storage group in. 
	CheckMythTVStorageGroup("/home/public/data/pvr","LiveTV","$hostName");    #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
	CheckMythTVStorageGroup("/home/public/data/pvr","public","$hostName");	


	## For every user
	foreach my $User (@Users) {
		my $User_ID=$User->{"PK_Users"};
		my $User_Uname=$User->{"UserName"};
		my $User_UnixUname=lc($User_Uname);
		$User_UnixUsername =~ s/[^a-z0-9-]//;
		$User_UnixUname="pluto_$User_UnixUname";
		CheckMythTVStorageGroup("/home/user_$User_ID/data/pvr","$User_Uname","$hostName");
	}



	## And now add the directories to each extra storage device.
	for my $Device (@Devices) {
		my $Device_ID=$Device->{"PK_Device"};
		my $Device_Description=$Device->{"Description"};

		my $Device_MountPoint="/mnt/device/$Device_ID";

		my $Device_IsMounted=`cd /mnt/device/$Device_ID && mount | grep "\/mnt\/device\/$Device_ID "`; 
		unless ($Device_IsMounted) {
		        print "WARNING: Device $Device_ID is not mounted, skiping ..." ;
		        next; 
		} 

		#public storage groups
		CheckMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","Default","$hostName");      #Put the special "Default" storage group in. 
		CheckMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","LiveTV","$hostName");       #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
		CheckMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","public: $Device_Description [$Device_ID]","$hostName");


		## For every user
		foreach my $User (@Users) {
		  my $User_ID=$User->{"PK_Users"};
      my $User_Uname=$User->{"UserName"};
		  my $User_UnixUname=lc($User_Uname);
		  $User_UnixUsername =~ s/[^a-z0-9-]//;
		  $User_UnixUname="pluto_$User_UnixUname";
		
			CheckMythTVStorageGroup("/home/user_$User_ID/data/pvr/$Device_Description [$Device_ID]","$User_Uname: $Device_Description [$Device_ID]","$hostName");

		}
	}
}
# no mythbackend restart is required; it will find changes on the fly
