<h1><? echo _("Add user") ?></h1>
<FORM METHOD="POST" ACTION="?site=users/add">
<? echo _("Username").": "; ?><INPUT TYPE="text" NAME="Username"><BR>
<? echo _("MD5 Hash").": "; ?><INPUT TYPE="text" NAME="Password"><BR>
<? echo _("Email").": "; ?><INPUT TYPE="text" NAME="Email"><P>
<? echo _("User type").": "; ?><BR>
<INPUT TYPE="radio" NAME="sqlCvsAdmin" VALUE="0">User<BR>
<INPUT TYPE="radio" NAME="sqlCvsAdmin" VALUE="1">Admin<BR>
<INPUT TYPE="submit" VALUE="<? echo _("Submit") ?>" NAME="submit">
</FORM>

