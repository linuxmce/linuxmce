#!/usr/bin/perl

use DBI;
require "/usr/pluto/bin/config_ops.pl";

$db = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";

$devices = "";
$datas = "";

@databases = ("pluto_main","pluto_media","pluto_telecom","asterisk","asteriskcdrdb","mythconverg","pluto_mediatomb","pluto_myth","pluto_security","pluto_telecom","pluto_vdr");

foreach $dbname (@databases) {
	$sql = "SELECT TABLE_NAME FROM information_schema.TABLES Where TABLE_SCHEMA = '$dbname' AND TABLE_NAME != 'Schema' AND TABLE_COLLATION != 'utf8_general_ci';";
	print "$sql\n";
	$st = $db->prepare($sql) or die "Error in code";
	$st->execute() or die "Error on execute";
	while($local_row = $st->fetchrow_hashref()) {
		$table_name = $local_row->{'TABLE_NAME'};
		$sql = "ALTER TABLE $dbname.$table_name CHARACTER SET utf8;";
		print "$sql\n";
		$st2 = $db->prepare($sql) or die "Error in code";
		$st2->execute() or die "Error on execute";
		$st2->finish();
	}
	$st->finish();
}
$db->disconnect();

print "[DONE]\n";
