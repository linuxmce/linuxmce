<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');

$path=$GLOBALS['save_loading_time'].'loadingTimes';
$GLOBALS['maximum_allowed_loading_time']=1.000;

if(!file_exists($path)){
	die('Loading time reports are not available');
}

$timePerSection=array();
$linesArray=file($path);
foreach ($linesArray AS $line){
	$parts=explode("\t",$line);
	$timePerSection[$parts[0]][]=$parts[1];
}

ksort($timePerSection);
$out='
<style>
td {
   font-family: Arial, Verdana, Helvetica, sans-serif;
   font-size: 12px;
   color: #4E6DA1;
}
/* Sortable tables */
table.sortable a.sortheader {
    background-color:#E0E6EF;
    color:#4E6DA1;
    font-weight: bold;
    text-decoration: none;
}
table.sortable span.sortarrow {
    color: black;
    text-decoration: none;
}
</style>

<script src="scripts/sorttable.js"></script>
<b>Loading time reports</b>
	<table cellpadding="3" cellspacing="0" class="sortable" id="unique_id">
	<tr bgcolor="#E0E6EF">
		<td><B>Section</B></td>
		<td><B>Average time</B></td>
		<td><B>Min time</B></td>
		<td><B>Max time</B></td>
		<td><B>No. of views</B></td>
	</tr>';
$pos=0;
foreach ($timePerSection AS $section=>$values){
	$pos++;
	$averageLoadingTime=round(array_sum($values)/count($values),3);
	$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
	$color=($averageLoadingTime>$GLOBALS['maximum_allowed_loading_time'])?'#FFAFAF':$color;
	$out.='
	<tr bgcolor="'.$color.'">
		<td><B>'.$section.'</B></td>
		<td align="center">'.$averageLoadingTime.'</td>
		<td align="center">'.min($values).'</td>
		<td align="center">'.max($values).'</td>
		<td align="center">'.count($values).'</td>
	</tr>';
}
$out.='</table>';

print $out;
?>