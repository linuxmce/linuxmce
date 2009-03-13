<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/template.class.inc.php');

$start_time=getmicrotime();	
/* @var $mediadbADO ADOConnection */
/* @var $rs ADORecordSet */
$mediaTypes=array(4=>'Music',3=>'DVD',5=>'DVD');
	
$out='';
$from=(int)@$_REQUEST['from'];
$range=(isset($_REQUEST['range']))?(int)@$_REQUEST['range']:100;

/* Complete query:

	SELECT PK_File,CONCAT(Path,Filename) AS Filename
	FROM File
	LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File
	LEFT JOIN CoverArtScan ON CoverArtScan.FK_File=PK_File
	WHERE EK_MediaType IN (3,4,5) AND (Scanned IS NULL OR Scanned=0) AND Missing=0 AND IsDirectory=0
	GROUP BY PK_File
	HAVING count(FK_Picture)=0 
			
*/

$dataArray=getAssocArray('File','PK_File','CONCAT(EK_MediaType,\'|\',Filename)',$mediadbADO,'LEFT JOIN Picture_File ON  Picture_File.FK_File=PK_File LEFT JOIN CoverArtScan ON CoverArtScan.FK_File=PK_File WHERE EK_MediaType IN (3,4,5) AND (Scanned IS NULL OR Scanned=0) AND Missing=0 AND IsDirectory=0 GROUP BY PK_File HAVING count(FK_Picture)=0 ORDER BY PK_File DESC LIMIT '.$from.','.$range);

$filters=array();
$rs=$mediadbADO->Execute('SELECT * FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType WHERE FK_AttributeType IN (2,3) AND FK_File IN ('.join(',',array_keys($dataArray)).')');
while($row=$rs->FetchRow()){
	$filters[$row['FK_File']][$row['FK_AttributeType']]=$row['Name'];
}


$toScan='';
foreach ($dataArray AS $id=>$info){
	$parts=explode('|',$info);
	$mediaType=$parts[0];
	$toSearch=$parts[1];
	$toSearch=substr($toSearch,0,strrpos($toSearch,'.'));
	$toSearch=str_replace(array('-','(',')'),' ',$toSearch);
	
	
	$searchParms='searchIndex='.$mediaTypes[$mediaType];

	$filterSelected1='Keywords';
	$filter2='';
	$filterSelected2='';
	$filter3='';
	$filterSelected3='';
	if(isset($filters[$id][2]) && isset($filters[$id][3]) && $mediaType==4){

		if(@trim($filters[$id][2])!=''){
			$filter2=@$filters[$id][2];
			$filterSelected2='Artist';
		}
		if(@trim($filters[$id][3])!=''){
			$filter3=@$filters[$id][3];
			$filterSelected3='Title';
		}
		$toSearch='';
	}
	$searchParms.=($toSearch!='')?'&Keyword1Type='.$filterSelected1.'&Keyword1Search='.$toSearch:'';
	$searchParms.=($filter2!='')?'&Keyword2Type='.$filterSelected2.'&Keyword2Search='.$filter2:'';
	$searchParms.=($filter3!='')?'&Keyword3Type='.$filterSelected3.'&Keyword3Search='.$filter3:'';
	
	// add the record to the list of items to scan
	$scanUrl='coverArtExtractor.php?fileID='.$id.'&'.$searchParms;
	$toScan.=$scanUrl."\n";

	// todo: remove after Mircea make his scripts
	$cmd='wget \'http://'.$_SERVER['HTTP_HOST'].'/pluto-admin/'.$scanUrl.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	//echo $cmd.'<br>';
	//exec_batch_command($cmd);
}


echo nl2br($toScan);
$end_time=getmicrotime();
//print '<br>Finished in '.round(($end_time-$start_time),3).'s.';

?>

