<?
  	//debuging
  	$GLOBALS['inDebug']=0;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		$GLOBALS['globalConfigPath']='/var/www/globalconfig/';
  	}else{
  		error_reporting(E_ALL);
  		$GLOBALS['globalConfigPath']='/home/users/vali/work/web/globalconfig/';
  	}

  	include_once($GLOBALS['globalConfigPath'].'globalconfig.inc.php');

  	$conn=mysql_connect($dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass) or die('could not connect to database');
  	$db=mysql_selectdb($dbPlutoAdminDatabase,$conn) or die("could not select $dbPlutoAdminDatabase");  

	$ScenariosArray=getAssocArray('Template','PK_Template','Description',$conn,'','');
	$ScenariosArray['']='Undefined';

	$msg=($_SERVER['QUERY_STRING']=='')?'No parameters specified':'';

	if(isset($_REQUEST['cgid'])){
		$cgid=(int)$_REQUEST['cgid'];
		if($cgid>0){
			 $res=mysql_query('SELECT Description,FK_Template FROM CommandGroup WHERE PK_CommandGroup='.$cgid,$conn) or die ('Query failed: '.mysql_error());
			if(mysql_num_rows($res)>0){
				$row=mysql_fetch_assoc($res);
				$commandToSend='/usr/pluto/bin/MessageSend localhost 0 0 10 '.$cgid;
				//exec($commandToSend);
				$msg='Scenario <b>'.$row['Description'].'</b> was executed.';
			}else {
				$msg='Invalid scenario.';
			}
		}else{
			$msg='Invalid scenario ID.';
		}
	}else{
		$msg='Parameter not recognised.';
	}

print $msg;



// function definitions

function getAssocArray($table,$keyField,$labelField,$conn,$whereClause='',$orderClause='')
{
	$retArray=array();
	$res=mysql_query("SELECT $keyField,$labelField FROM $table $whereClause $orderClause",$conn) or die ('Query failed: '.mysql_error());
	while($row=mysql_fetch_assoc($res)){
		$retArray[$row[$keyField]]=$row[$labelField];
	}
	return $retArray;
}

?>