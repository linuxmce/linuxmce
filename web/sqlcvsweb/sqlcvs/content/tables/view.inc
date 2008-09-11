<h1><? echo _("View tables") ?></h1>
<FORM METHOD="POST" ACTION="?site=tables/view">
<SELECT NAME="repository">
<!--- pluto_main repos --->
<OPTION VALUE="constants">constants
<OPTION VALUE="dce">dce
<OPTION VALUE="designer">designer
<OPTION VALUE="document">document
<OPTION VALUE="ir">ir
<OPTION VALUE="local">local
<OPTION VALUE="media">media
<OPTION VALUE="myth">myth
<OPTION VALUE="security">security
<OPTION VALUE="telecom">telecom
<OPTION VALUE="website">website

</SELECT>
<INPUT TYPE="hidden" NAME="action" VALUE="view">
<INPUT TYPE="submit" VALUE="<? echo _("Submit") ?>" NAME="submit">
</FORM>
<? if ($_POST['action']=='view') {
	print "<h3>"._("Results")."</H3>\n";
	$result = db_query('SELECT * FROM psc_'.$_POST['repository'].'_tables ORDER BY PK_psc_'.$_POST['repository'].'_tables');
	if (@mysql_num_rows($result)) {
		
		print "<TABLE>";
		print "<TH>"._("Table ID")."</TH><TH>"._("Tablename")."</TH><TH>"._("last_psc_id")."</TH><TH>"._("last_batch_id")."</TH>";
		while ($row=@mysql_fetch_array($result)) {
			print "<TR><TD>".$row[0]."</TD>";
			print "<TD>".$row[1]."</TD>";
			print "<TD>".$row[5]."</TD>";
			print "<TD><A HREF=\"?site=batch/detail&repository=".$_POST['repository']."&batchid=".$row[6]."\">".$row[6]."</a></TD></TR>";
		}
		print "</TABLE>\n";
	}
} ?>
