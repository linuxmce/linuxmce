<?php
include_once "config.inc";

if (!($link = mysql_pconnect($DB_SERVER,$DB_LOGIN,$DB_PASSWORD)))
  {  echo("Internal error.Failed to connect to database");
  exit();
  }
 if (!($nresult=mysql_select_db($DB,$link)))
  {
  DisplayErrMsg("Error in connecting to the database ");
  exit();
  } 

$sql="select A.PK_Contact,A.Name,A.Company,A.JobDescription,A.Title,Email, B.CountryCode,B.AreaCode,B.PhoneNumber,B.Extension,C.Description from Contact as A, PhoneNumber as B,PhoneType as C where A.PK_Contact=B.FK_Contact and B.FK_PhoneType=C.PK_PhoneType and A.EntryID is null order by A.PK_Contact";
$result=mysql_query($sql,$link);
while ($row=mysql_fetch_row($result)){
	$fldstring="";
	for($i=0;$i<=count($row);$i++) {
	$fldstring.=$row[$i]."~";
	}
	echo $fldstring."|";
}
if (mysql_num_rows($result)==0){
	echo "End";
}
?>