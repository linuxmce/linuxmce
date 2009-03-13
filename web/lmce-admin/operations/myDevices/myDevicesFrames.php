<?
//se iau intai cele selectate, din baza de date si apoi se verifica array-ul din DeviceTemplate_DeviceParameter!
function myDevicesFrames($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
$output->setScriptCalendar('null');

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
}

