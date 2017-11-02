<?php
session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/template.class.inc.php');

$table=$_REQUEST['table'];
$where=stripslashes($_REQUEST['where']);

// Don't double quote
if ( ! substr($table,0,1) == "`") {
        $table = "`" . $table . "`"; // To circumvent problems with reserved words in tab
}
$cols=$dbADO->MetaColumnNames( $table );
$fieldsNo=count($cols);
$out='
	<table width="600" cellpadding="3" cellspacing="0" style="border-style:inset; border-width:1px;">
		<tr bgcolor="#DFEBFF">
			<td colspan="'.$fieldsNo.'"><B>Table: '.$table.'</B></td>
		</tr>
		<tr>
			<td colspan="'.$fieldsNo.'">&nbsp;</td>
		</tr>
		<tr bgcolor="#DFEBFF">';
foreach ($cols AS $column){
	$out.='<td><b>'.$column.'</b></td>';
}
$out.='</tr>';

$rs=$dbADO->Execute('SELECT `'.join('`,`',$cols).'` FROM  '.$table.' '.$where);
while($row=$rs->FetchRow()){
	$pos=0;
	foreach ($cols AS $column){
		$pos++;
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='<td bgcolor="'.$color.'">'.$row[$column].'</td>';
	}	
}
$out.='</table><br>
<div align="center"><input type="button" class="button" value="Close" onClick="self.close();"></div>
';

$output = new Template($dbADO);
$output->setTemplateFileType('small');
$output->setScriptCalendar('null');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: Show record');
$output->output();

?>