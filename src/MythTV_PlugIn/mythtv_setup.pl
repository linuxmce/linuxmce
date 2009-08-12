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

print "$MythTV_Installed\n";

sub ClearMythTVStorageGroups {
  #Exit if mythtv is not installed
  return unless $MythTV_Installed;
  
	print "Cleaning out MythTV Storage Groups...\n";
	#Allow users to have custom storage groups. Lets store them now so we can restore them after clearing the storagegroups table
	UseDB("mythconverg");
  
	$sql="SELECT groupname,hostname,dirname FROM storagegroup WHERE groupname LIKE 'custom:%'";
	my @results = RunSQL($sql);

	#Clear the table.. This also resets th auto-increment number
	$sql="TRUNCATE TABLE storagegroup";
	$dbh->do($sql);

	#restore custom storage groups.
	foreach my $row (@results){
    $groupName=$row->{"groupname"};
		$hostName=$row->{"hostname"};
		$dirName=$row->{"dirname"};

		print "Retaining custom storagegroup:     '$groupName'\n";
		$sql="INSERT INTO storagegroup (groupname,hostname,dirname) VALUES ('$groupName','$hostName','$dirName')";
		$dbh->do($sql);
	}
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
  UseDB("pluto_main");
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
	UseDB("mythconverg");
	$dbh->do($sql);
}
#CleanMythTVSettings();

@TPL_STORAGE_DEVICES=(1790, 1794, 1768, 1769, 1854, 1851, 1849);
$DD_USERS=3;

getopt('d');
$CommaSeparatedDeviceList = $opt_d;

## A list containing the pluto users that need to use those directories
$sql="SELECT PK_Users, UserName FROM Users";
#UseDB("pluto_main");
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
	#UseDB("pluto_main");
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
	#UseDB("pluto_main");
	@results=RunSQL($sql);
	foreach $row (@results) {
    push(@Devices,$row);
  }
}

## Make a list of hostnames
$sql="SELECT p.PK_Device FROM pluto_main.Device p 
LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1";
#UseDB("pluto_main");
@results=RunSQL($sql);
@hostNames=('localhost','dcerouter');
foreach $row (@results) {
  push(@hostNames,"moon" . $row->{"PK_Device"});
}

# Exit gracefully if no new MD's or storage devices were added.
# This makes sure that we don't have to reset the backend more than we should have to...
foreach $thisHost (@hostNames) {
	$sql="SELECT count(hostname) as count FROM storagegroup WHERE hostname='$thisHost' AND dirname NOT LIKE '\%/home/\%/data/pvr\%'";
  UseDB("mythconverg");
	@results=RunSQL($sql);
	if ($results[0] && $results[0]->{"count"}) {
		$UpdateStorageGroups=1;
	}
}

foreach $thisDevice (@Devices) {
	$Device_Description = addslashes($thisDevice->{"Description"});
	$sql="SELECT count(id) as count from storagegroup WHERE dirname LIKE '\%$Device_Description\%' AND dirname NOT LIKE '\%/home/\%/data/pvr/$Device_Description\%'";
	@results=RunSQL($sql);
	if ($results[0] && $results[0]->{"count"}) {
		$UpdateStorageGroups=1;
	}
}

unless ($UpdateStorageGroups) {
	print "No new media directors or storage devices found, skipping storagegroup update and backend reload...\n";
	exit 0;
}

## Something has changed, so we must update the storagegroups and reset the backends!!
## Before we start, lets clean out mythconverg.storagegroups
ClearMythTVStorageGroups();

##For every host...
foreach my $hostName (@hostNames) {

	##lets handle the /home/public paths....
	AddMythTVStorageGroup("/home/public/data/pvr","Default","$hostName");     #Put the special "Default" storage group in. 
	AddMythTVStorageGroup("/home/public/data/pvr","LiveTV","$hostName");    #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
	AddMythTVStorageGroup("/home/public/data/pvr","public","$hostName");	


	## For every user
	foreach my $User (@Users) {
		my $User_ID=$User->{"PK_Users"};
		my $User_Uname=$User->{"UserName"};
		my $User_UnixUname=lc($User_Uname);
		$User_UnixUsername =~ s/[^a-z0-9-]//;
		$User_UnixUname="pluto_$User_UnixUname";
		AddMythTVStorageGroup("/home/user_$User_ID/data/pvr","$User_Uname","$hostName");
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
		AddMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","Default","$hostName");      #Put the special "Default" storage group in. 
		AddMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","LiveTV","$hostName");       #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
		AddMythTVStorageGroup("/home/public/data/pvr/$Device_Description [$Device_ID]","public: $Device_Description [$Device_ID]","$hostName");


		## For every user
		foreach my $User (@Users) {
		  my $User_ID=$User->{"PK_Users"};
      my $User_Uname=$User->{"UserName"};
		  my $User_UnixUname=lc($User_Uname);
		  $User_UnixUsername =~ s/[^a-z0-9-]//;
		  $User_UnixUname="pluto_$User_UnixUname";
		
			AddMythTVStorageGroup("/home/user_$User_ID/data/pvr/$Device_Description [$Device_ID]","$User_Uname: $Device_Description [$Device_ID]","$hostName");

		}
	}
}

#Reset the MythTV Backend so that the new storage groups are available
if ($MythTV_Installed) {
       `/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh`;
}
