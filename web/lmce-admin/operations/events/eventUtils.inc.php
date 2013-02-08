<?php
function reloadEventPlugin($installationID, $dbADO) {
	 $DEVICE_TEMPLATE_EVENT_PLUGIN_CONST=52;
	 $COMMAND_DOWNLOAD_CONFIGURATION_CONST=757;
	$PK_Device = getDeviceFromDT($installationID, $DEVICE_TEMPLATE_EVENT_PLUGIN_CONST, $dbADO);

	$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$PK_Device.' 1 '.$COMMAND_DOWNLOAD_CONFIGURATION_CONST;
	exec($commandToSend);
}

?>