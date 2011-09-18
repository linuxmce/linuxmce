<?php

// HELPER FUNCTIONS:

function framework_print_errors($src, $dst, $errors) {
	echo "error copying framework files:<br />'cp -rf' from src: '$src' to dst: '$dst'...details follow<br />";
	foreach ($errors as $error) {
		echo "$error<br />";
	}
}

// These are need to handle 2.8 to 2.9 upgrades
//
if (! function_exists('out')) {
	function out($text) {
		echo $text."<br>";
	}
}

if (! function_exists('outn')) {
	function outn($text) {
		echo $text;
	}
}

if (! function_exists('error')) {
	function error($text) {
		echo "[ERROR] ".$text."<br>";
	}
}

if (! function_exists('fatal')) {
	function fatal($text) {
		echo "[FATAL] ".$text."<br>";
		exit(1);
	}
}

if (! function_exists('debug')) {
	function debug($text) {
		global $debug;
		
		if ($debug) echo "[DEBUG-preDB] ".$text."<br>";
	}
}

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

	if (!file_exists(dirname($htdocs_source))) {
    out(sprintf(_("No directory %s, install script not needed"),dirname($htdocs_source)));
    return true;
  }

	// These are required by libfreepbx.install.php library for upgrade routines
	//
	define("UPGRADE_DIR", dirname(__FILE__)."/upgrades/");
	define("MODULE_DIR",  $amp_conf['AMPWEBROOT'].'/modules/');

	$htdocs_dest = $amp_conf['AMPWEBROOT'];
	$bin_dest    = isset($amp_conf['AMPBIN']) ? $amp_conf['AMPBIN'] : '/var/lib/asterisk/bin';
	$agibin_dest = isset($asterisk_conf['astagidir']) ? $asterisk_conf['astagidir']:'/var/lib/asterisk/agi-bin';

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

  /*TODO: (Requirment for #4733)
   *
   * 1. Update publish.pl to grab a copy of amportal and put it somehwere.
   * 2. If we have access to do an md5sum on AMPSBIN/amportal do it and
   *    compare to the local copy.
   * 3. If the md5sum is different or we couldn't check, put amportal in AMPBIN
   * 4. If we decided they need a new one, then write out a message that they
   *    should run amportal to update it.
   */

	if (function_exists('upgrade_all')) {
		upgrade_all(getversion());
    // We run this each time so that we can add settings if need be
    // without requiring a major version bump
    //
    freepbx_settings_init(true);
	} else {
		echo ("[ERROR] Function: 'upgrade_all' not present, libfreepbx.install.php seems not to be installed<br>");
	}

?>
