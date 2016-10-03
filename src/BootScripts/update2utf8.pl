#!/usr/bin/perl

use DBI;
require "/usr/pluto/bin/config_ops.pl";

# We try the regular LinuxMCE/Pluto credentials. If they fail, we try a local connection
$db = DBI->connect(&read_pluto_cred()) or $db = DBI->connect("DBI:mysql:") or die "Couldn't connect to database: $DBI::errstr\n";

@databases = ("pluto_main","pluto_media","pluto_telecom","asterisk","asteriskcdrdb","mythconverg","pluto_mediatomb","pluto_myth","pluto_security","pluto_telecom","pluto_vdr");

# fix calldate in asterisk.cdr, sqlCVS takes care of lmce databases
$sql = "UPDATE asterisk.cdr set calldate='1970-01-01 00:00:00' WHERE calldate=0;";
$st = $db->prepare($sql) or die "Error in prepare $sql\n";
$st->execute() or die "Error on execute $sql\n";
$sql = "ALTER TABLE asterisk.cdr MODIFY calldate datetime NULL DEFAULT NULL;";
$st = $db->prepare($sql) or die "Error in prepare $sql\n";
$st->execute() or die "Error on execute $sql\n";
$sql = "UPDATE asterisk.cdr set calldate=NULL WHERE calldate=0 OR calldate='1970-01-01 00:00:00';";
$st = $db->prepare($sql) or die "Error in prepare $sql\n";
$st->execute() or die "Error on execute $sql\n";
 
foreach $dbname (@databases) {
        # Select all tbles that don't have utf8_general_ci as there collation
	$sql = "SELECT TABLE_NAME FROM information_schema.TABLES Where TABLE_SCHEMA = '$dbname' AND TABLE_NAME != 'Schema' AND TABLE_COLLATION != 'utf8_general_ci';";
#	print "$sql\n";
	$st = $db->prepare($sql) or die "Error in prepare $sql\n";
	$st->execute() or die "Error on execute $sql\n";
	$first = 1;
	while($local_row = $st->fetchrow_hashref()) {
                if ($first == 1) {
                        $first = 0;
                        print "\nWorking on non-UTF-8 tables in database $dbname\n"
                }
		$table_name = $local_row->{'TABLE_NAME'};
		$sql = "ALTER TABLE $dbname.$table_name CHARACTER SET utf8;";
#		print "$sql\n";
		$st2 = $db->prepare($sql) or die "Error in prepare $sql";
		$st2->execute() or die "Error on execute $sql";
		print ".";
		$st2->finish();
	}
	$st->finish();

        # Select all columns that have a specific character set, and reset them to default
	$sql_column = "SELECT TABLE_NAME,COLUMN_NAME, COLUMN_TYPE, IS_NULLABLE, COLUMN_DEFAULT, COLUMN_KEY FROM information_schema.COLUMNS where TABLE_SCHEMA = '$dbname' ";
	$sql_column .= " AND (DATA_TYPE = 'varchar' OR DATA_TYPE = 'text' OR DATA_TYPE = 'char' OR DATA_TYPE = 'longtext') ";
	$sql_column .= " AND CHARACTER_SET_NAME IS NOT NULL";
	$sql_column .= " AND CHARACTER_SET_NAME != 'utf8';";
	$st_column = $db->prepare($sql_column);
	$st_column->execute() or die "Error on execute $sql_column";
	$first = 1;
	while ($local_column = $st_column->fetchrow_hashref()) {
                if ($first == 1) {
                        $first = 0;
                        print "\nWorking on non-UTF-8 columns in database $dbname\n"
                }
	        $table_name = $local_column->{'TABLE_NAME'};
	        $column_name = $local_column->{'COLUMN_NAME'};
		$column_type = $local_column->{'COLUMN_TYPE'};
		$sql = "ALTER TABLE $dbname.`$table_name` MODIFY COLUMN `$column_name` $column_type";
		print $sql . "\n";
		$st2 = $db->prepare($sql) or die "Error in prepare $sql";
		$st2->execute() or print "\nNot updating column $column_name in $dbname.$table_name on execute $sql\n";
		$st2->finish();
		print ".";
        }

}

# Add new field to CommandGroup
# Check to see if table contains the column alreads
$sql = "SELECT COUNT(*) AS NUMBER FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = 'pluto_main' and TABLE_NAME = 'CommandGroup' AND COLUMN_NAME = ' FK_Text';";
$st2 = $db->prepare($sql) or die "Error on prepare $sql\n";
$st2->execute() or die "Error on execute $sql\n";
$row = $st2->fetchrow_hashref();
$st2->finish;
if ($row{'NUMBER'} == 1) {
  print "\nAltering table pluto_main.CommandGroup to add additional field FK_Text\n";
  $sql = "ALTER TABLE pluto_main.CommandGroup ADD FK_Text INT(11) DEFAULT NULL AFTER TemplateParm2";
  $st2 = $db->prepare($sql);
  # or die "Error in prepare $sql\n";
  $st2->execute();
  # or die "Error on execute $sql\n";
  $st2->finish();
}                
                 
$db->disconnect();

print "[DONE]\n";
