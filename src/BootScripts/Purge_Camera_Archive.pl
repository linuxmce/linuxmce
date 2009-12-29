#!/usr/bin/perl

use DBI;
require "/usr/pluto/bin/config_ops.pl";

$db = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";

$devices = "";
$datas = "";

$sql = "SELECT PK_DeviceCategory FROM DeviceCategory WHERE Description='Surveillance Cameras'";
$st = $db->prepare($sql) or die "Error in code";
$st->execute() or die "Error on execute";
while($local_row = $st->fetchrow_hashref()) {
	$devicecat = $local_row->{'PK_DeviceCategory'};
	$sql = "SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory='$devicecat'";
	$st2 = $db->prepare($sql) or die "Error in code";
	$st2->execute() or die "Error on execute";
	while($local = $st2->fetchrow_hashref()) {
		$devicetemplate = $local->{'PK_DeviceTemplate'};
		$sql = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$devicetemplate'";
		$st3 = $db->prepare($sql) or die "Error in code";
		$st3->execute() or die "Error on execute";
		while($row = $st3->fetchrow_hashref()) {
			$deviceid = $row->{'PK_Device'};
			$sql = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$deviceid' AND FK_DeviceData='54'";
			$st4 = $db->prepare($sql) or die "Error in code";
			$st4->execute() or die "Error on execute";
			if($rowl = $st4->fetchrow_hashref()) {
				$data = $rowl->{'IK_DeviceData'};
				
				if($devices eq "") {
					$devices = $deviceid.",";
				} else {
					$devices = $devices.$deviceid.",";
				}
				if($datas eq "") {
					$datas = $data.",";
				} else {
					$datas = $datas.$data.",";
				}
				print "Camera $deviceid has a purge period of $data days.\n";
			}
			$st4->finish();
		}
		$st3->finish();
	}
	$st2->finish();
}
$st->finish();

$db->disconnect();

if($devices eq "") {
	print "We have found no camera devices so we will not purge. Exiting...\n";
	exit(-1);
}

if($datas eq "") {
	print "We have found no data concernig purge option. Exiting...\n";
	exit(-1);
}

@da = split(/\,/,$datas);
@de = split(/\,/,$devices);

$count = 0;
foreach $line (@de) {
	if($da[$count] eq "") {
		print "We have a device without data parameter\n";
		exit(-1);
	}
	$count = $count + 1;
}
print "Purging $count camera devices....\n";

($lsec,$lmin,$lhour,$lmday,$lmon,$lyear,$lwday,$lyday,$lisdst) = localtime(time);

#the month is 0-11, lets make it 1-12 to match the directory structure
$lmon = $lmon + 1;
print "DEBUG: lyear = $lyear\n";
$lyear = $lyear - 100;
print "DEBUG: after -100, lyear = $lyear\n";

$lyear = 2000 + $lyear;


$count = 0;
$path = "/home/cameras/";

foreach $dev (@de) {
	# convert purge period days to number of seconds (60*60*24=86400 seconds per day)
	$time = time - $da[$count]*86400;
	# make expiry time varibles from time differential
	($osec,$omin,$ohour,$omday,$omon,$oyear,$owday,$oyday,$oisdst) = localtime($time);
	$oyear = $oyear - 100;
       	$oyear = 2000 + $oyear;
	# convert 0-11 months to 1-12
	$omon = $omon + 1;

	$path1 = "/home/cameras/".$dev;
	opendir(ROOTDIR, $path1);
	@years = grep {!/^\./} readdir(ROOTDIR);

	foreach $year (@years) {
		if($year >= $lyear && $year <= $oyear) {
			$path2 = $path1."/".$year;
			opendir(YEARDIR, $path2);
			@months = grep {!/^\./} readdir(YEARDIR);
			foreach $month (@months) {
				$path3 = $path2."/".$month;
				if($month >= $lmon && $month <= $omon) {
					opendir(MONTHDIR, $path3);
					@days = grep {!/^\./} readdir(MONTHDIR);
					foreach $day (@days) {
						$path4 = $path3."/".$day;
						
						if($day <= $omday || $day > $lday) {

						
							print "Found expired day! Removing $path4\n";
							system("rm -rf $path4");
						}
					}
					closedir(MONTHDIR);
				} else {
					print "Found Expired month! Removing $path3\n";
					system("rm -rf $path3");
				}
			}
			closedir(YEARDIR);
		} else {
			$path2 = $path1."/".$year;
			print "Found Expired year! Removing $path2\n";
			system("rm -rf $path2");
		}
	}
	closedir(ROOTDIR);
	$count = $count + 1;
}
print "[DONE]\n";
