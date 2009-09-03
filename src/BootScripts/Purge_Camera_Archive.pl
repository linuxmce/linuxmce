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
print "Found $count camera devices. Purgeing ...";

($lsec,$lmin,$lhour,$lmday,$lmon,$lyear,$lwday,$lyday,$lisdst) = localtime(time);
$lmon = $lmon + 1;

$lyear = $lyear - 100;
if($lyear < 10) {
	$lyear = 2000 + $lyear;
} else {
	$lyear = 2000 + $lyear;
}

$count = 0;
$path = "/home/cameras/";
foreach $dev (@de) {
	$path1 = "/home/cameras/".$dev;
	$time = time - $da[$count]*86400;
	($osec,$omin,$ohour,$omday,$omon,$oyear,$owday,$oyday,$oisdst) = localtime($time);
	$oyear = $oyear - 100;
	if($oyear < 10) {
        	$oyear = 2000 + $oyear;
        } else {
                $oyear = 2000 + $oyear;
        }
	$omon = $omon + 1;
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
						if($day >= $lmday && $day <= $omday) {
							system("rm -rf $path4");
						}
					}
					closedir(MONTHDIR);
				} else {
					system("rm -rf $path3");
				}
			}
			closedir(YEARDIR);
		} else {
			$path2 = $path1."/".$year;
			system("rm -rf $path2");
		}
	}
	closedir(ROOTDIR);
	$count = $count + 1;
}
print "[DONE]\n";
