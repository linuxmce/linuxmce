<h1><? echo _("View batches") ?></h1>
<FORM METHOD="POST" ACTION="?site=batch/view">
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
	$result = db_query('SELECT * FROM psc_'.$_POST['repository'].'_bathdr ORDER BY date DESC');
	if (@mysql_num_rows($result)) {
		
		print "<TABLE>";
		print "<TH>"._("Batch ID")."</TH><TH>"._("IP Address")."</TH><TH>"._("Date")."</TH><TH>"._("Comments")."</TH>";
		while ($row=@mysql_fetch_array($result)) {
			print "<TR><TD><A HREF=\"?site=batch/detail&repository=".$_POST['repository']."&batchid=".$row[0]."\">".$row[0]."</A></TD>";
			print "<TD>".$row[1]."</TD><TD>".$row[2]."</TD><TD>".$row[3]."</TD></TR>";
		}
		print "</TABLE>\n";
	}
} ?>
