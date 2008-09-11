<h1><? echo _("Batch details") ?></h1>
<h3><? echo _("Repository").": ".$_GET['repository'].", "._("Batch ID").": ".$_GET['batchid']; ?></h3>

<?
$result = db_query('SELECT * FROM psc_'.$_GET['repository'].'_bathdr where PK_psc_'.$_GET['repository'].'_bathdr = '.$_GET['batchid']);
if (@mysql_num_rows($result)) {
	print "<h3>"._("Batch header")."</h3>";
	print "<TABLE>";
	print "<TH>"._("Batch ID")."</TH><TH>"._("IP Address")."</TH><TH>"._("Date")."</TH><TH>"._("Comments")."</TH>";
	while ($row=@mysql_fetch_array($result)) {
		print "<TR><TD>".$row[0]."</TD>";
		print "<TD>".$row[1]."</TD><TD>".$row[2]."</TD><TD>".$row[3]."</TD></TR>";
	}
	print "</TABLE>\n";
}
$result = db_query('SELECT * FROM psc_'.$_GET['repository'].'_batdet where FK_psc_'.$_GET['repository'].'_bathdr = '.$_GET['batchid']);

if (@mysql_num_rows($result)) {
	print "<h3>"._("Batch details")."</h3>";
	print "<TABLE>";
	print "<TH>"._("PK")."</TH>" .
		"<TH>"._("Tablename")."</TH>" . 
		"<TH>"._("New")."</TH>" .
		"<TH>"._("Deleted")."</TH>" .
		"<TH>"._("Modified")."</TH>" .
		"<TH>"._("bathdr_orig")."</TH>".
		"<TH>"._("bathdr_auth")."</TH>".
		"<TH>"._("bathdr_unauth")."</TH>";
	while ($row=@mysql_fetch_array($result)) {
		$tablelist[] = $row[2];
		print "<TR>" . 
			'<TD><A HREF="?site=row/view&batchid='.$_GET['batchid'].'&table='.$row[2].'">'.$row[0].'</A></TD>';
		print "<TD>".$row[2]."</TD>" .
			"<TD>".$row[3]."</TD>" .
			"<TD>".$row[4]."</TD>" .
			"<TD>".$row[5]."</TD>" .
			'<TD><A HREF="?site=batch/detail&repository='.$_GET['repository'].'&batchid='.$row[6].'">'.$row[6].'</A></TD>' .
			'<TD><A HREF="?site=batch/detail&repository='.$_GET['repository'].'&batchid='.$row[7].'">'.$row[7].'</A></TD>' .
			'<TD><A HREF="?site=batch/detail&repository='.$_GET['repository'].'&batchid='.$row[8].'">'.$row[8].'</A></TD>' .
			"</TR>";
	}
	print "</TABLE>\n";
	print "<h3>"._("Batch contents")."</h3>";
	foreach ($tablelist as $key => $tablename) {
		print "<H4>"._("Table")." ".$tablename."</h4>\n";
		$query = 'select * from '.$tablename.' where psc_batch='.$_GET['batchid'];
		print $query."<br>";
		$result = db_query($query);
		db_print_result($result);
		print "<H4>"._("Table history")." ".$tablename."</h4>\n";
		$query = 'select * from '.$tablename.'_pschist where psc_batch='.$_GET['batchid'];
		print $query."<br>";
		$result = db_query($query);
		db_print_result($result);
	}
}
?>
<div class="pre">
<pre>
<?

if ($_GET['debug'] == 1) {
	system('/usr/bin/sudo /usr/sbin/chroot /opt/pluto/chroot sqlCVS -h "127.0.0.1" -u "root" -p "" -D "pluto_main" -P "3306" -r "'.$_GET['repository'].'" -t "" -U "" -n -b "'.$_GET['batchid'].'" batch-contents');
}

?>
</pre>
</div>
