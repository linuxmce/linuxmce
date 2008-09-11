<h1><? echo _("View row in table") ?></h1>
<? if ($_GET['batchid']  && $_GET['table']) {
	print "<h3>"._("Results")."</H3>\n";
	$query = 'SELECT * FROM '.$_GET['table'].' where psc_batch = '.$_GET['batchid'];
	print $query."<br>";
	$result = db_query($query);
	db_print_result($result);
} ?>
