<?
// @dl("pgsql.so");

function db_connect($dbconnstring) {
	$conn = pg_connect($dbconnstring);
	if (!$conn) {
		echo "Database error";
		return -1;
	}
	return $conn;
}

?>


