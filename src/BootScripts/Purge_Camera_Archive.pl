#!/usr/bin/perl

use DBI;
require "/usr/pluto/bin/config_ops.pl";
use vars '$DCERouter';
use vars '$PK_Device';
use vars '$MySqlHost';
use vars '$MySqlUser';
use vars '$MySqlPassword';
use vars '$MySqlDBName';
$db = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";

$devices = "";
$datas = "";

$sql = "SELECT PK_DeviceCategory FROM DeviceCategory WHERE Description='Surveillance Cameras' OR Description='IP Cameras'";
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

($curSec,$curMin,$curHour,$curDayOfMonth,$curMonth,$curYear,$curDayOfWeek,$curDayOfYear,$curIsDst) = localtime(time);

#the month is 0-11, lets make it 1-12 to match the directory structure
$curMonth = $curMonth + 1;

$curYear = $curYear - 100;

$curYear = 2000 + $curYear;


$count = 0;
$path = "/home/cameras/";

foreach $dev (@de) {
	# convert purge period days to number of seconds (60*60*24=86400 seconds per day)
	$time = time - $da[$count]*86400;
	# make expiry time varibles from time differential
	($ExpSec,$ExpMin,$ExpHour,$ExpDayOfMonth,$ExpMonth,$ExpYear,$ExpDayOfWeek,$ExpDayOfYear,$ExpIsDst) = localtime($time);
	print $ExpSec." segundos \n";
	print $ExpMin." minutos \n";
	print $ExpHour." hora \n";
	print $ExpDayOfMonth." day \n";
	print $ExpMonth."mes \n";
	print $ExpYear."ano \n";
	print $ExpDayOfWeek." dia da semana\n";
	print $ExpDayOfYear." dia do ano \n";
	print $ExpIsDst."\n";
	#exit;
	$ExpYear = $ExpYear - 100;
       	$ExpYear = 2000 + $ExpYear;
	# convert 0-11 months to 1-12
	$ExpMonth = $ExpMonth + 1;

	$path1 = "/home/cameras/".$dev;
	opendir(ROOTDIR, $path1);
	@years = grep {!/^\./} readdir(ROOTDIR);

	foreach $year (@years) {
		# Skip the special lastsnap.jpg file
		next if($year=="lastsnap.jpg");

		if($year >= $curYear && $year <= $ExpYear) {
			$path2 = $path1."/".$year;
			opendir(YEARDIR, $path2);
			@months = grep {!/^\./} readdir(YEARDIR);
			foreach $month (@months) {
				$path3 = $path2."/".$month;
				
				
				if( $month <= $ExpMonth) {
				        print "-----entroue----".$curMonth."--------\n";
				        
					opendir(MONTHDIR, $path3);
					@days = grep {!/^\./} readdir(MONTHDIR);
					foreach $day (@days) {                   	
						$path4 = $path3."/".$day;
						
						if($day <= $ExpDayOfMonth ) {
							print "Found expired day! Removing $path4\n";
							system("rm -rf $path4");
						}
					}
					@days = grep {!/^\./} readdir(MONTHDIR);
					if (scalar( @days) == 0) {
							print "Found empty Month! Removing month\n";
							system("rm -rf $path3");
					
					} 
					closedir(MONTHDIR);
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
