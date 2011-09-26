<?php

include dirname(__FILE__)."/libfreepbx.install.php";

global $amp_conf;
global $asterisk_conf;

$debug = false;
$dryrun = false;

/** verison_compare that works with freePBX version numbers
 *  included here because there are some older versions of functions.inc.php that do not have
 *  it included as it was added during 2.3.0beta1
 */
if (!function_exists('version_compare_freepbx')) {
	function version_compare_freepbx($version1, $version2, $op = null) {
		$version1 = str_replace("rc","RC", strtolower($version1));
		$version2 = str_replace("rc","RC", strtolower($version2));
		if (!is_null($op)) {
			return version_compare($version1, $version2, $op);
		} else {
			return version_compare($version1, $version2);
		}
	}
}

/*
 * Framework install script
 */

	$htdocs_source = dirname(__FILE__)."/htdocs/*";
	$bin_source = dirname(__FILE__)."/bin/*";
	$agibin_source = dirname(__FILE__)."/agi-bin/*";
	$htdocs_panel_source = dirname(__FILE__)."/htdocs_panel/*";

	// These are required by libfreepbx.install.php library for upgrade routines
	//
	define("UPGRADE_DIR", dirname(__FILE__)."/upgrades/");
	define("MODULE_DIR",  $amp_conf['AMPWEBROOT'].'/modules/');

	$htdocs_dest = $amp_conf['AMPWEBROOT'];
	$bin_dest    = isset($amp_conf['AMPBIN']) ? $amp_conf['AMPBIN'] : '/var/lib/asterisk/bin';
	$agibin_dest = isset($asterisk_conf['astagidir']) ? $asterisk_conf['astagidir']:'/var/lib/asterisk/agi-bin';

	// There was a past bug where FOPWEBROOT was pointing to AMPWEBROOT so if that is the case then hardcode
	// and force to panel here.
	//
	$htdocs_panel_dest = $amp_conf['FOPWEBROOT'];
	if ($htdocs_panel_dest == $amp_conf['AMPWEBROOT']) {
		$htdocs_panel_dest .= "/panel";
	}

	exec("cp -rf $htdocs_source $htdocs_dest 2>&1",$out,$ret);
	if ($ret != 0) {
		framework_print_errors($htdocs_source, $htdocs_dest, $out);
	}

	exec("cp -rf $bin_source $bin_dest 2>&1",$out,$ret);
	if ($ret != 0) {
		framework_print_errors($bin_source, $bin_dest, $out);
	}

	exec("cp -rf $agibin_source $agibin_dest 2>&1",$out,$ret);
	if ($ret != 0) {
		framework_print_errors($agibin_source, $agibin_dest, $out);
	}

	exec("cp -rf $htdocs_panel_source $htdocs_panel_dest 2>&1",$out,$ret);
	if ($ret != 0) {
		framework_print_errors($htdocs_panel_source, $htdocs_panel_dest, $out);
	}

	if (function_exists('upgrade_all')) {
		upgrade_all(getversion());
	} else {
		echo ("[ERROR] Function: 'upgrade_all' not present, libfreepbx.inc.php must not be isntall<br>");
	}


// HELPER FUNCTIONS:

function framework_print_errors($src, $dst, $errors) {
	echo "error copying framework files:<br />'cp -rf' from src: '$src' to dst: '$dst'...details follow<br />";
	foreach ($errors as $error) {
		echo "$error<br />";
	}
}

function out($text) {
	echo $text."<br>";
}

function outn($text) {
	echo $text;
}

function error($text) {
	echo "[ERROR] ".$text."<br>";
}

function fatal($text) {
	echo "[FATAL] ".$text."<br>";
	exit(1);
}

function debug($text) {
	global $debug;
	
	if ($debug) echo "[DEBUG-preDB] ".$text."<br>";
}
?>
