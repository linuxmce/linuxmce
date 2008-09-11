<?
// @dl("mysql.so");

function db_connect($dbhost,$dbuser,$dbpass) {
	$conn = mysql_connect($dbhost,$dbuser,$dbpass);
	if (!$conn) {
		echo "Database error";
		return -1;
	}
	return $conn;
}
function db_select($dbname) {
	return mysql_select_db($dbname);
}
function db_query($querystring) {
	return mysql_query($querystring);
}
function db_print_result($dbresult) {

	if (@mysql_num_rows($dbresult)) {
		
		print "<TABLE>";
		for ($x = 0;$x < mysql_num_fields($dbresult);$x++) {
			print "<TH>".mysql_field_name($dbresult,$x)."</TH>";
		}
		while ($row=@mysql_fetch_array($dbresult)) {
			print "<TR>";
			for ($x = 0;$x < mysql_num_fields($dbresult);$x++) {
				print "<TD>".$row[$x]."</TD>";
			}
			print "</TR>\n";
		}
		print "</TABLE>\n";
	}
}

function db_num_rows($dbresult) {
	return @mysql_num_rows($dbresult);
}
function db_fetch_row($dbresult) {
	return @mysql_fetch_row($dbresult);
}
function db_insert_id($dbresult) {
	return @mysql_insert_id($dbresult);
}
?>

