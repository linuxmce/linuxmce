<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
Header( "Content-type: text/xml");

$fileID=(int)@$_REQUEST['fileID'];
$attID=(int)@$_REQUEST['attID'];
$attID=($attID==0)?13:$attID;
	
$xml="<?xml version='1.0'?>\n";
$xml.="<files>\n";


$attributeTypes=getAssocArray('AttributeType','PK_AttributeType','Description',$mediadbADO);
	
$where='
LEFT JOIN File_Attribute ON File_Attribute.FK_File=PK_File 
LEFT JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType='.$attID.' 
LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File
LEFT JOIN Picture ON FK_Picture=PK_Picture
WHERE IsDirectory=0 AND Missing=0
GROUP BY FK_AttributeType,FK_Picture';

$files=getFields('File','PK_File,Name,Url,FK_AttributeType,FK_Picture',$mediadbADO,$where);
if(count($files)==0){
	$xml.="<error>No files.</error>\n";
}else{
	for($i=0;$i<count($files);$i++){
		$xml.="<file>\n";
		$xml.="<fileID>{$files[$i]['PK_File']}</fileID>\n";
		
		if(isset($attributeTypes[$files[$i]['FK_AttributeType']])){
			$attributeTag=$attributeTypes[$files[$i]['FK_AttributeType']];
			$attributeName=$files[$i]['Name'];
			$xml.="<$attributeTag>$attributeName</$attributeTag>\n";
		}
		
		if((int)$files[$i]['FK_Picture']!=0){
			$xml.="<coverart>{$files[$i]['Url']}</coverart>\n";
		}
		$xml.="</file>\n";
	}
}



$xml.="</files>";

print $xml;
?>