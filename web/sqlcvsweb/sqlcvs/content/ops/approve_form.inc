<h1><? echo _("Batch approval form") ?></h1>
<FORM METHOD="POST" ACTION="?site=ops/approve">
<? echo _("Repository").":"; ?>
<SELECT NAME="repository">
<!--- pluto_main repos -->
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
<br>
<? echo _("Batch ID").": "; ?><INPUT TYPE="text" NAME="batchid"><br>
<? echo _("Comments").": "; ?><INPUT TYPE="text" NAME="comments"><br>
<? echo _("Credentials").": "; ?><INPUT TYPE="text" NAME="credentials"><br>
<INPUT TYPE="submit" VALUE="<? echo _("Approve"); ?>" NAME="submit">
</FORM>
