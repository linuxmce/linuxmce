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
	if ($path =~ /livetv$/) {
		`mkdir -p "$path"`;
	        `chown root:public "$path"`;
	        `chmod 2775 "$path"`;
		`touch "$path.folderlock"`;
	}
}

sub CheckMythTVStorageGroup
{
	## Parameters 
	## 1 - path
	## 2 - groupname
	## 3 - hostname

	my $path=shift;
	my $name=shift;
	my $host=shift;

	#Exit if mythtv is not installed
	return unless $MythTV_Installed;
	
	#truncate the group name to 32 chars so it fits the field size in the database
	$name = substr($name,0,32) if (length($name) > 32);

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

# getHostFromMD
# This function takes a primary key as input, and maps that primary key
# to a hostname. PK=1 is always the core, and most MD's have a "moon"xxx host name.
# The oddball here is the hybrid MD - it needs to map back to dcerouter as well. This function takes care of that.
sub getHostFromMD {
	my $thisPK = shift;

	#find out if this is a Hybrid MD. If it is, return the dcerouter host
	$sql="SELECT Count(*) FROM Device WHERE FK_Device_ControlledVia=1 AND PK_Device=$thisPK";
	UseDB("pluto_main");
	@results=RunSQL($sql);
	if ($results[0]->{"Count(*)"} eq "1" || $thisPK eq 1) {
		return "dcerouter"
	}

	# Otherwise, return "moon"+primary key
	return "moon".$thisPK;
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

#Clean up all entries except custom ones, they will get recreated below
UseDB('mythconverg');
$sql = "FROM storagegroup where groupname NOT LIKE 'custom:%'";

@results=RunSQL("SELECT count(id) as count " . $sql);
if ($results[0] && $results[0]->{"count"}) {
  $dbh->do("DELETE " . $sql);
  print "Cleaned $results[0]->{count} Device entries.\n";
}


# Now check that every required entry is there, inserting a new one as needed

#For every host...
foreach my $hostName (@hostNames) {

	##lets handle the /home/public paths....
	CheckMythTVStorageGroup("/home/public/data/pvr","Default","$hostName");     #Put the special "Default" storage group in. 
	CheckMythTVStorageGroup("/home/public/data/pvr/livetv","LiveTV","$hostName");    #Put the special "LiveTV" storage group into the moons root pvr* directory	
	CheckMythTVStorageGroup("/home/public/data/pvr","Default: [core]","$hostName");	


	## For every user
	foreach my $User (@Users) {
		my $User_ID=$User->{"PK_Users"};
		my $User_Uname=$User->{"UserName"};
		my $User_UnixUname=lc($User_Uname);
		$User_UnixUsername =~ s/[^a-z0-9-]//;
		$User_UnixUname="pluto_$User_UnixUname";
		CheckMythTVStorageGroup("/home/user_$User_ID/data/pvr","$User_Uname","$hostName");
		CheckMythTVStorageGroup("/home/user_$User_ID/data/pvr","$User_Uname: [core]","$hostName");
	}



	## And now add the directories to each extra storage device.
	for my $Device (@Devices) {
		my $Device_ID=$Device->{"PK_Device"};
		my $Device_Description=$Device->{"Description"};
		## only configure if "use automatically" flag is set
		UseDB("pluto_main");
        	$sql="SELECT
                	PK_Device,
                	Description
        	FROM
                	Device
                	JOIN Device_DeviceData ON FK_Device = PK_Device
        	WHERE
                	PK_Device = $Device_ID
                	AND
                	FK_DeviceData = 134
                	AND
                	IK_DeviceData = '1'
        	";
        	@results=RunSQL($sql);
		UseDB('mythconverg');
        	foreach $row (@results) {

			# MythTV will ignore any shares that are offline temporarily, so we should always add them even if they are offline right now.

			my $Device_MountPoint="/mnt/device/$Device_ID";

			my $Device_IsMounted=`cd /mnt/device/$Device_ID && mount | grep "\/mnt\/device\/$Device_ID "`; 
			unless ($Device_IsMounted) {
		        	print "WARNING: Device $Device_Description [$Device_ID] is not mountable. If this device no longer exists, it should be removed from the device tree." ;
			} 

			#all devices should be in the "Default" and "LiveTV" special storage groups
			CheckMythTVStorageGroup("/mnt/device/$Device_ID/public/data/pvr","Default","$hostName"); 
			CheckMythTVStorageGroup("/mnt/device/$Device_ID/public/data/pvr/livetv","LiveTV","$hostName");

			#public storage groups
			CheckMythTVStorageGroup("/mnt/device/$Device_ID/public/data/pvr","Default: $Device_Description [$Device_ID]","$hostName");      #Put the special "Default" storage group in. 


			## For every user
			foreach my $User (@Users) {
		  		my $User_ID=$User->{"PK_Users"};
		  		my $User_Uname=$User->{"UserName"};
		  		my $User_UnixUname=lc($User_Uname);
		  		$User_UnixUsername =~ s/[^a-z0-9-]//;
		  		$User_UnixUname="pluto_$User_UnixUname";
		        	CheckMythTVStorageGroup("/mnt/device/$Device_ID/user_$User_ID/data/pvr","$User_Uname","$hostName");
				CheckMythTVStorageGroup("/mnt/device/$Device_ID/user_$User_ID/data/pvr","$User_Uname: $Device_Description [$Device_ID]","$hostName");

			}
		}
	}
}

##############################
# Synchronize AC3/DTS Settings
##############################
print "Synchronizing MythTV audio settings...\n";
# Lets build a list of all of the primary keys to the MD's (including the Hybrid if it exists) (note that these settings don't apply to the core device, but its hybrid MD)
$sql="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28";
UseDB("pluto_main");
@results=RunSQL($sql);
foreach $row (@results) {
	push(@md_devices,$row->{"PK_Device"});
}

# Loop through all of the MD's, and update various settings
foreach my $thisMD (@md_devices) {
	# Synchronize AC3/DTS passthru settings
	$sql="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$thisMD' AND FK_DeviceData=88";
	UseDB("pluto_main");
	@results=RunSQL($sql);
	$LMCE_Audio_Setting=$results[0]->{"IK_DeviceData"};	

	$host=getHostFromMD($thisMD);

	if ($LMCE_Audio_Setting && $LMCE_Audio_Setting =~ /3/) {
		$sql="UPDATE settings SET data=1 WHERE (value='ACPassThru' OR value='DTSPassThru') AND hostname='$host'";
		print "Updating DTS/AC3 MythTV settings for $host -- setting them to true to match MD settings.\n"
	} else {
		$sql="UPDATE settings SET data=0 WHERE (value='ACPassThru' OR value='DTSPassThru') AND hostname='$host'";
		print "Updating DTS/AC3 MythTV settings for $host -- setting them to false to match MD settings.\n"
	}
	UseDB("mythconverg");
	$dbh->do($sql); #RunSQL doesn't seem to support results not being returned, so do it this way

	print "Disabling realtime priority threads\n";
	# Disable realtime priority threads due to problems with VDPAU
	$sql="UPDATE settings SET data=0 WHERE value='RealtimePriority' AND hostname='$host'";
	$dbh->do($sql);
}
UseDB("mythconverg");
# Set the disk schedular to use free space as the only weighting
print "Setting the disk scheduler to use free space weighting\n";
$sql="UPDATE settings SET data='BalancedFreeSpace' WHERE value='StorageScheduler'";
$dbh->do($sql);

# no mythbackend restart is required; it will find changes on the fly
