<?php
$restrict_mods = true;
if (!@include_once(getenv('FREEPBX_CONF') ? getenv('FREEPBX_CONF') : '/etc/freepbx.conf')) { 
	  include_once('/etc/asterisk/freepbx.conf'); 
}

//re #872 for why we call global $amp_conf here
global $amp_conf;
$lic_file 	= '';
$dir 		= scandir($amp_conf['ASTAGIDIR']);
//look for license file in the dir
foreach ($dir as $f) {
	if (substr($f, 0, 8) == 'license-' && substr($f, -3) == '.zl' && strlen($f) > 11) {
		$lic_file = $amp_conf['ASTAGIDIR']  . '/' . $f;
		break;
	}
}

if($lic_file) {
	$load_license = zend_loader_install_license($lic_file);
}
