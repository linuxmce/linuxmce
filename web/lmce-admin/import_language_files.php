<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
$start_time=getmicrotime();

$path='/home/users/jondecker76/work/web/lmce-admin/languages/en/';
$cmd="grep -H '^\\$' $path*.php";


// hard-coded
$PlutoAdminCategory=209;
$subCategories=array(''=>0,)+getAssocArray('TextCategory','Description','PK_TextCategory',$dbADO,'WHERE FK_TextCategory_Parent='.$PlutoAdminCategory);
//$plutoAdminFilenames=getAssocArray('Text','CONCAT(Description,FK_TextCategory) AS Description','PK_Text',$dbADO,'WHERE FK_TextCategory IN ('.join(',',$subCategories).')');
$plutoAdminFilenames=getTextFilenames($subCategories,$dbADO);
$englishValues=(count($plutoAdminFilenames)>0)?getAssocArray('Text_LS','FK_Text','Description',$dbADO,'WHERE FK_Text IN ('.join(',',array_values($plutoAdminFilenames)).') AND FK_Language=1'):array();

exec($cmd,$retArr);
$len=strlen($path);
foreach ($retArr AS $line){
	//echo $line.'<br>';
	$pattern='/'.str_replace('/','\/',$path).'(.*).lang.php:\$TEXT_(.*)_CONST=\'(.*)\';/';
	preg_match($pattern,$line,$matches);
	if(isset($matches) && count($matches)==4){
		$filename=$matches[1];
		$value=$matches[3];
		
		// add record in TextCategory if it does not exist
		if(!isset($subCategories[$filename])){
			$dbADO->Execute('INSERT INTO TextCategory (FK_TextCategory_Parent,Description) VALUES (?,?)',array($PlutoAdminCategory,$filename));
			$subCategories[$filename]=$dbADO->Insert_ID();
		}
		$variable=$matches[2].'_'.$subCategories[$filename];

		// add record in text table if does not exist
		if(!isset($plutoAdminFilenames[$variable])){
			$dbADO->Execute('INSERT INTO Text (FK_TextCategory,Description) VALUES (?,?)',array($subCategories[$filename],str_replace('_'.$subCategories[$filename],'',$variable)));
			$plutoAdminFilenames[$variable]=$dbADO->Insert_ID();
		}
		if(isset($englishValues[$plutoAdminFilenames[$variable]])){
			$dbADO->Execute('UPDATE Text_LS SET Description=? WHERE FK_Text=? AND FK_Language=?',array($value,$plutoAdminFilenames[$variable],1));
		}else{
			//echo $variable.' '.$plutoAdminFilenames[$variable].'<br>';
			$dbADO->Execute('INSERT IGNORE INTO Text_LS (FK_Text,FK_Language,Description) VALUES (?,?,?)',array($plutoAdminFilenames[$variable],1,$value));		
		}
	}else{
		echo $line.'<br>';
	}

}

$end_time=getmicrotime();
print '<span class="normaltext">Import finished in '.round(($end_time-$start_time),3).' s.</span>';


function getTextFilenames($subCategories,$dbADO){
	$res=$dbADO->Execute('SELECT CONCAT(Description,\'_\',FK_TextCategory) AS Description,PK_Text FROM Text WHERE FK_TextCategory IN ('.join(',',$subCategories).')');
	$filenames=array();
	while($row=$res->FetchRow()){
		$filenames[$row['Description']]=$row['PK_Text'];
	}
	
	return $filenames;
}
?>
