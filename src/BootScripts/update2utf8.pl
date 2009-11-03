#!/usr/bin/perl

use DBI;
require "/usr/pluto/bin/config_ops.pl";

$db = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";

@databases = ("pluto_main","pluto_media","pluto_telecom","asterisk","asteriskcdrdb","mythconverg","pluto_mediatomb","pluto_myth","pluto_security","pluto_telecom","pluto_vdr");

foreach $dbname (@databases) {
        # Select all tbles that don't have utf8_general_ci as there collation
	$sql = "SELECT TABLE_NAME FROM information_schema.TABLES Where TABLE_SCHEMA = '$dbname' AND TABLE_NAME != 'Schema' AND TABLE_COLLATION != 'utf8_general_ci';";
#	print "$sql\n";
	$st = $db->prepare($sql) or die "Error in prepare $sql\n";
	$st->execute() or die "Error on execute $sql\n";
	while($local_row = $st->fetchrow_hashref()) {
		$table_name = $local_row->{'TABLE_NAME'};
		$sql = "ALTER TABLE $dbname.$table_name CHARACTER SET utf8;";
#		print "$sql\n";
		$st2 = $db->prepare($sql) or die "Error in prepare $sql";
		$st2->execute() or die "Error on execute $sql";
		$st2->finish();
	}
	$st->finish();

        # Select all columns that have a specific character set, and reset them to default
	$sql_column = "SELECT TABLE_NAME,COLUMN_NAME, COLUMN_TYPE, IS_NULLABLE, COLUMN_DEFAULT, COLUMN_KEY FROM information_schema.COLUMNS where TABLE_SCHEMA = '$dbname' ";
	$sql_column .= " AND (DATA_TYPE = 'varchar' OR DATA_TYPE = 'text' OR DATA_TYPE = 'char') ";
	$sql_column .= " AND CHARACTER_SET_NAME IS NOT NULL;";
	$st_column = $db->prepare($sql_column);
	$st_column->execute() or die "Error on execute $sql_column";
	while ($local_column = $st_column->fetchrow_hashref()) {
	        $table_name = $local_column->{'TABLE_NAME'};
	        $column_name = $local_column->{'COLUMN_NAME'};
		$column_type = $local_column->{'COLUMN_TYPE'};
		$sql = "ALTER TABLE $dbname.$table_name MODIFY COLUMN $column_name $column_type";
		# print $sql . "\n";
		$st2 = $db->prepare($sql) or die "Error in prepare $sql";
		$st2->execute() or die "Error on execute $sql";
		$st2->finish();
        }

}

# Add new field to CommandGroup
$sql = "ALTER TABLE pluto_main.CommandGroup ADD FK_Text INT(11) DEFAULT NULL AFTER TemplateParm2";
$st2 = $db->prepare($sql);
# or die "Error in prepare $sql\n";
$st2->execute();
# or die "Error on execute $sql\n";
                                 
$db->disconnect();

print "[DONE]\n";
