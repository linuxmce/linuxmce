<?php
include_once "config.inc";

function insertphones($fd,$cid,$link){
	while ((trim($buffer=fgets($fd,4096))!= "") && (!feof($fd)) && ($cid!="")){
		$sql="select PK_PhoneType from PhoneType where ucase(Description)='". strtoupper(trim($buffer))."'";
		$result=mysql_query($sql,$link);
		if (mysql_num_rows($result)>0){
			$row=mysql_fetch_row($result);
			$phonetype=$row[0];	
			$buffer=fgets($fd,4096);
			$sql="insert into PhoneNumber(FK_Contact,FK_PhoneType,Countrycode,AreaCode,PhoneNumber,Extension) values ('". $cid ."','".$phonetype . "',".$buffer .")" ;
			mysql_query($sql,$link);
		}else {
			echo "Error : No phone type found ".$sql."|";
		}
	}

}


$i=0;
if (!($link = mysql_pconnect($DB_SERVER,$DB_LOGIN,$DB_PASSWORD)))
  {  echo("Error : Internal error.Failed to connect to database|");
  exit();
  }
 if (!($nresult=mysql_select_db($DB,$link)))
  {
  echo("Error : in connecting to the database |");
  exit();
  } 

$userfiletemp=$_FILES['fileinsert']['tmp_name'];	
$fd=fopen($userfiletemp,"r");
$e=0;
$st="";
if (!feof($fd))  $buffer = fgets($fd, 4096);
if (!feof($fd))
while (!feof($fd)) {
   if ($i==0)
   {	$i++;
	if (trim($buffer)!="xcevw12e9f5kj"){
		fclose($fd);
		exit();
	}
   }
	while((($buffer=fgets($fd,4096))) && (!feof($fd))){if (trim($buffer)!='') break;}
	if (!(feof($fd))){
		$buffer=mysql_real_escape_string($buffer);
		$arrfields=explode("~",$buffer);
		$sql="select PK_Contact from Contact where EntryID='".$arrfields[0]."'";

		$rowset=mysql_query($sql,$link);
		if (mysql_num_rows($rowset)>0){

		// update

			$row=mysql_fetch_array($rowset);
			$cid=$row['PK_Contact'];


			$sql="update Contact set Name='".$arrfields[1]."',";
			$sql.="Company='".$arrfields[2]."',";
			$sql.="JobDescription='".$arrfields[3]."',";
			$sql.="Title='".$arrfields[4]."',";
			$sql.="Email='".$arrfields[5]."' where EntryID='".$arrfields[0]."'";

			mysql_query($sql,$link);
		
			mysql_query("delete from PhoneNumber where FK_Contact=$cid",$link);

			insertphones($fd,$cid,$link);
			
			
			
		}else{		

		// insert	

			$buffer="";
			for ($ctr=0;$ctr<count($arrfields);$ctr++){
				$buffer.="'".$arrfields[$ctr]."',";
			}

			$buffer=substr($buffer,0,strlen($buffer)-1);

			$sql="insert into Contact(EntryID,Name,Company,JobDescription,Title,Email) values (" . $buffer . ")";
			$result=mysql_query($sql,$link);
			$cid=mysql_insert_id($link);
			if(!($result))
			{ $e++;
				echo "Error ID: ".$i. "  Execution Query ".$sql .". ErrorNo =". mysql_errno()." Error=".mysql_error() . "|";
				$cid="";
			}
			insertphones($fd,$cid,$link);
		}
	}
	
}
fclose($fd);


?>