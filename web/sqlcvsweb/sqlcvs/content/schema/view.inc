<h1><? echo _("View schema") ?></h1>
<FORM METHOD="POST" ACTION="?site=schema/view">
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
	$result = db_query('SELECT * FROM psc_'.$_POST['repository'].'_schema ORDER BY PK_psc_'.$_POST['repository'].'_schema DESC');
	if (@mysql_num_rows($result)) {
		
		print "<TABLE>";
		print "<TH>"._("Schema ID")."</TH><TH>"._("Value")."</TH>";
		while ($row=@mysql_fetch_array($result)) {
			print "<TR><TD>".$row[0]."</TD>";
			print "<TD>".$row[1]."</TD></TR>";
		}
		print "</TABLE>\n";
	}
} ?>
