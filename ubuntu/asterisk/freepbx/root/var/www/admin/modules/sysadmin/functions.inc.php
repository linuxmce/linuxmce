<?php

if(file_exists("/var/lib/asterisk/agi-bin/LoadLicenseIfExists.php")) {
    include_once("/var/lib/asterisk/agi-bin/LoadLicenseIfExists.php");
}

require_once(dirname(__FILE__) . '/functions.inc/general.php');

?>