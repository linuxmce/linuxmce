<?PHP
 include('include/adodb/adodb-errorhandler.inc.php');
 require('include/adodb/adodb.inc.php');
 include('include/adodb/tohtml.inc.php');
 
 
  $ADODB_COUNTRECS=true; //for speed up, set it to false, but it will return -1 for RecordCount()
  

  //define('ADODB_ERROR_LOG_TYPE',3);
  //define('ADODB_ERROR_LOG_DEST','C:/mysql_errors.log');

//  $dsn = $dbPlutoMainType.'://'.urlencode($dbPlutoMainUser).':'.urlencode($dbPlutoMainPass).'@'.$dbPlutoMainServer.'/'.urlencode($dbPlutoMainDatabase); 
//  $dbADO = &ADONewConnection($dsn);
  $dbADO = &ADONewConnection('mysql');
  $dbADO->NConnect($dbPlutoMainServer,urlencode($dbPlutoMainUser),urlencode($dbPlutoMainPass),urlencode($dbPlutoMainDatabase)); 
  
//  $mediaDSN = $dbPlutoMediaType.'://'.urlencode($dbPlutoMediaUser).':'.urlencode($dbPlutoMediaPass).'@'.$dbPlutoMediaServer.'/'.urlencode($dbPlutoMediaDatabase); 
//  $mediadbADO = &ADONewConnection($mediaDSN);
  $mediadbADO = &ADONewConnection($dbPlutoMediaType);
  $mediadbADO->NConnect($dbPlutoMediaServer,urlencode($dbPlutoMediaUser),urlencode($dbPlutoMediaPass),urlencode($dbPlutoMediaDatabase)); 

  
//  $securityDSN = $dbPlutoSecurityType.'://'.urlencode($dbPlutoSecurityUser).':'.urlencode($dbPlutoSecurityPass).'@'.$dbPlutoSecurityServer.'/'.urlencode($dbPlutoSecurityDatabase); 
//  $securityADO = &ADONewConnection($securityDSN);
  $securityADO = &ADONewConnection($dbPlutoSecurityType);
  $securityADO->NConnect($dbPlutoSecurityServer,urlencode($dbPlutoSecurityUser),urlencode($dbPlutoSecurityPass),urlencode($dbPlutoSecurityDatabase)); 

  //for sqlite
  //$db = &ADONewConnection('sqlite');
  //$db->NConnect($GLOBALS['appRoot'].'sqlite.db'); # sqlite will create if does not exist

  /* @var $db ADOConnection */

  if (!$dbADO) {
	die("Connection failed");
  }
  
 
  $dbADO->SetFetchMode(ADODB_FETCH_ASSOC);
  $ADODB_FORCE_TYPE=3;

  //$dbADO->LogSQL(true);

  //-you must not modify anything bellow this line
  /*$conn=mysql_connect($dbServer,$dbUser,$dbPass) or die('could not connect to database');
  $db=mysql_selectdb($dbDatabase,$conn) or die("could not select $dbDatabase");*/   
?>
