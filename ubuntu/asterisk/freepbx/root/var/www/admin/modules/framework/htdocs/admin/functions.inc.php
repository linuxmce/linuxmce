<?php /* $id$ */
//Copyright (C) 2004 Coalescent Systems Inc. (info@coalescentsystems.ca)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

$dirname = dirname(__FILE__);
//PLEASE NOTE: for performance reasons, these are hardcoded 
//and dont get included dynamically

//TODO: include these dynamically as needed/use __autoload()


//----------include classes----------


//guieleemnts class for dynamicly generating gui
require_once($dirname . '/libraries/components.class.php');

//php4 parser for xml's
require_once($dirname . '/libraries/xml2Array.class.php');

//freepbx class to manage cron
require_once($dirname . '/libraries/cronmanager.class.php');

//hooks class
require_once($dirname . '/libraries/moduleHook.class.php');

//freepbx notification engine
require_once($dirname . '/libraries/notifications.class.php');

//class to enforce extension/view restrictions amongst freepbx admins
require_once($dirname . '/libraries/ampuser.class.php');

//module related class. TODO: update this line if you know what it dose
require_once($dirname . '/libraries/modulelist.class.php');

//class that handels freepbx global setting
require_once($dirname . '/libraries/freepbx_conf.class.php');

//class for handeling/hooking feature codes
require_once($dirname . '/libraries/featurecodes.class.php');


//----------include function files----------

//freepbx helpers for debuggin/logging/comparing
require_once($dirname . '/libraries/utility.functions.php');

//module state manipulation functions
require_once($dirname . '/libraries/module.functions.php');

//dynamic registry of which exten's are in use and by whom
require_once($dirname . '/libraries/usage_registry.functions.php');

//emulated compatability for older versions of freepbx
require_once($dirname . '/libraries/php-upgrade.functions.php');

//lightweight query functions
require_once($dirname . '/libraries/sql.functions.php');

//functions for view related activities
require_once($dirname . '/libraries/view.functions.php');

//functions for reding writing voicemail files
require_once($dirname . '/libraries/voicemail.function.php');


//----------include helpers----------

//freepbx specific gui helpers
require_once($dirname . '/helpers/freepbx_helpers.php');

//general html helpers
require_once($dirname . '/helpers/html_helper.php');

//table generation class
function log_message(){} define('BASEPATH', '');//make upstream scripts happy
require_once($dirname . '/helpers/Table.php');


/**
 * returns true if asterisk is running with chan_dahdi
 *
 * @return bool
 */
function ast_with_dahdi() {
	global $version;
	global $astman;
	global $amp_conf;
	global $chan_dahdi_loaded;

  // determine once, subsequent calls will use this
  global $ast_with_dahdi;

  if (isset($ast_with_dahdi)) {
    return $ast_with_dahdi;
  }
	
	if (empty($version)) {
		$engine_info = engine_getinfo();
		$version = $engine_info['version'];
	}
		
	if (version_compare($version, '1.4', 'ge') && $amp_conf['AMPENGINE'] == 'asterisk') {		
    if ($amp_conf['ZAP2DAHDICOMPAT']) {
      $ast_with_dahdi = true;
      $chan_dahdi_loaded = true;
      return true;
    } else if (isset($astman) && $astman->connected()) {
			// earlier revisions of 1.4 and dahdi loaded but still running as zap, so if ZapScan is present, we assume
      // that is the mode it is running in.
			$response = $astman->send_request('Command', array('Command' => 'show applications like ZapScan'));
			if (!preg_match('/1 Applications Matching/', $response['data'])) {
        $ast_with_dahdi = true;
        $chan_dahdi_loaded = true;
				return $ast_with_dahdi;
			} else {
        $chan_dahdi_loaded = false;
			}
		}
	}
  $ast_with_dahdi = false;
  return $ast_with_dahdi;
}

function engine_getinfo($force_read=false) {
	global $amp_conf;
	global $astman;
  static $engine_info;

  $gotinfo = false;

  if (!$force_read && isset($engine_info) && $engine_info != '') {
    return $engine_info;
  }

	switch ($amp_conf['AMPENGINE']) {
		case 'asterisk':
			if (isset($astman) && $astman->connected()) {
				//get version (1.4)
				$response = $astman->send_request('Command', array('Command'=>'core show version'));
				if (preg_match('/No such command/',$response['data'])) {
					// get version (1.2)
					$response = $astman->send_request('Command', array('Command'=>'show version'));
				}
				$verinfo = $response['data'];
			} else {
				// could not connect to asterisk manager, try console
				$verinfo = exec('asterisk -V');
			}
			
			if (preg_match('/Asterisk (\d+(\.\d+)*)(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => $matches[1], 'additional' => $matches[4], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk SVN-(\d+(\.\d+)*)(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => $matches[1], 'additional' => $matches[4], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk SVN-branch-(\d+(\.\d+)*)-r(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => $matches[1].'.'.$matches[4], 'additional' => $matches[4], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk SVN-trunk-r(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => '1.8', 'additional' => $matches[1], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk SVN-.+-(\d+(\.\d+)*)-r(-?(\S*))-(.+)/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => $matches[1], 'additional' => $matches[3], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk [B].(\d+(\.\d+)*)(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => '1.2', 'additional' => $matches[3], 'raw' => $verinfo);
        $gotinfo = true;
			} elseif (preg_match('/Asterisk [C].(\d+(\.\d+)*)(-?(\S*))/', $verinfo, $matches)) {
				$engine_info = array('engine'=>'asterisk', 'version' => '1.4', 'additional' => $matches[3], 'raw' => $verinfo);
        $gotinfo = true;
			}

      if (!$gotinfo) {
			  $engine_info = array('engine'=>'ERROR-UNABLE-TO-PARSE', 'version'=>'0', 'additional' => '0', 'raw' => $verinfo);
      }
      if ($amp_conf['FORCED_ASTVERSION']) {
        $engine_info['engine'] = $amp_conf['AMPENGINE'];
        $engine_info['version'] = $amp_conf['FORCED_ASTVERSION'];
      }

      // Now we make sure the ASTVERSION freepbx_setting/amp_conf value is defined and set

      // this is not initialized in the installer because I think there are scenarios where
      // Asterisk may not be running and we may some day not need it to be so just deal
      // with it here.
      //
      $freepbx_conf =& freepbx_conf::create();
      if (!$freepbx_conf->conf_setting_exists('ASTVERSION')) {
        // ASTVERSION
        //
        $set['value'] = $engine_info['version'];
        $set['defaultval'] = '';
        $set['options'] = '';
        $set['readonly'] = 1;
        $set['hidden'] = 1;
        $set['level'] = 10;
        $set['module'] = '';
        $set['category'] = 'Internal Use';
        $set['emptyok'] = 1;
        $set['name'] = 'Asterisk Version';
        $set['description'] = "Last Asterisk Version detected (or forced)";
        $set['type'] = CONF_TYPE_TEXT;
        $freepbx_conf->define_conf_setting('ASTVERSION',$set,true);
        unset($set);
        $amp_conf['ASTVERSION'] = $engine_info['version'];
      }

      if ($engine_info['version'] != $amp_conf['ASTVERSION']) {
        $freepbx_conf->set_conf_values(array('ASTVERSION' => $engine_info['version']), true, true);
      }

      return $engine_info;
		break;
	}
	$engine_info = array('engine'=>'ERROR-UNSUPPORTED-ENGINE-'.$amp_conf['AMPENGINE'], 'version'=>'0', 'additional' => '0', 'raw' => $verinfo);
  return $engine_info;
}

function do_reload() {
	global $amp_conf, $asterisk_conf, $db, $astman, $version;

	if (empty($version)) {
		$engine_info = engine_getinfo();
		$version = $engine_info['version'];
	}
	
	$notify =& notifications::create($db);
	
	$return = array('num_errors'=>0,'test'=>'abc');
	$exit_val = null;
	
	if (isset($amp_conf["PRE_RELOAD"]) && !empty($amp_conf['PRE_RELOAD']))  {
		exec( $amp_conf["PRE_RELOAD"], $output, $exit_val );
		
		if ($exit_val != 0) {
			$desc = sprintf(_("Exit code was %s and output was: %s"), $exit_val, "\n\n".implode("\n",$output));
			$notify->add_error('freepbx','reload_pre_script', sprintf(_('Could not run %s script.'), $amp_conf['PRE_RELOAD']), $desc);
			
			$return['num_errors']++;
		} else {
			$notify->delete('freepbx', 'reload_pre_script');
		}
	}
	
	$retrieve = $amp_conf['AMPBIN'].'/retrieve_conf 2>&1';
	//exec($retrieve.'&>'.$asterisk_conf['astlogdir'].'/freepbx-retrieve.log', $output, $exit_val);
	exec($retrieve, $output, $exit_val);
	
	// retrieve_conf html output
	$return['retrieve_conf'] = 'exit: '.$exit_val.'<br/>'.implode('<br/>',$output);
	
	if ($exit_val != 0) {
		$return['status'] = false;
		$return['message'] = sprintf(_('Reload failed because retrieve_conf encountered an error: %s'),$exit_val);
		$return['num_errors']++;
		$notify->add_critical('freepbx','RCONFFAIL', _("retrieve_conf failed, config not applied"), $return['message']);
		return $return;
	}
	
	if (!isset($astman) || !$astman) {
		$return['status'] = false;
		$return['message'] = _('Reload failed because FreePBX could not connect to the asterisk manager interface.');
		$return['num_errors']++;
		$notify->add_critical('freepbx','RCONFFAIL', _("retrieve_conf failed, config not applied"), $return['message']);
		return $return;
	}
	$notify->delete('freepbx', 'RCONFFAIL');
	
	//reload MOH to get around 'reload' not actually doing that.
	$astman->send_request('Command', array('Command'=>'moh reload'));
	
	//reload asterisk
  if (version_compare($version,'1.4','lt')) {
	  $astman->send_request('Command', array('Command'=>'reload'));	
  } else {
	  $astman->send_request('Command', array('Command'=>'module reload'));	
  }
	
	$return['status'] = true;
	$return['message'] = _('Successfully reloaded');
	
	if ($amp_conf['FOPRUN'] && !$amp_conf['FOPDISABLE']) {
    unset($output);
		exec('killall -HUP op_server.pl 2>&1', $output, $exit_val);
		if ($exit_val != 0) {
			$desc = _('Could not reload the FOP operator panel server using killall -HUP op_server.pl. Configuration changes may not be reflected in the panel display.');
			$notify->add_error('freepbx','reload_fop', _('Could not reload FOP server'), $desc);
      // send the error output to dbug log if enabled.
      dbug($output);
			
			$return['num_errors']++;
		} else {
			$notify->delete('freepbx','reload_fop');
		}
	}
	
	//store asterisk reloaded status
	$sql = "UPDATE admin SET value = 'false' WHERE variable = 'need_reload'"; 
	$result = $db->query($sql);
	if(DB::IsError($result)) {
		$return['message'] = _('Successful reload, but could not clear reload flag due to a database error: ').$db->getMessage();
		$return['num_errors']++;
	}
	
	if (isset($amp_conf["POST_RELOAD"]) && !empty($amp_conf['POST_RELOAD']))  {
		exec( $amp_conf["POST_RELOAD"], $output, $exit_val );
		
		if ($exit_val != 0) {
			$desc = sprintf(_("Exit code was %s and output was: %s"), $exit_val, "\n\n".implode("\n",$output));
			$notify->add_error('freepbx','reload_post_script', sprintf(_('Could not run %s script.'), 'POST_RELOAD'), $desc);
			
			$return['num_errors']++;
		} else {
			$notify->delete('freepbx', 'reload_post_script');
		}
	}
	
	return $return;
}


// draw list for users and devices with paging
// $skip has been deprecated, used to be used to page-enate
function drawListMenu($results, $skip, $type, $dispnum, $extdisplay, $description=false) {
	
	$index = 0;
	echo "<ul>\n";
	if ($description !== false) {
 		echo "\t<li><a ".($extdisplay=='' ? 'class="current"':'')." href=\"config.php?type=".$type."&display=".$dispnum."\">"._("Add")." ".$description."</a></li>\n";
	}
	if (isset($results)) {
		foreach ($results as $key=>$result) {
			$index= $index + 1;
			echo "\t<li><a".($extdisplay==$result[0] ? ' class="current"':''). " href=\"config.php?type=".$type."&display=".$dispnum."&extdisplay={$result[0]}\">{$result[1]} &lt;{$result[0]}&gt;</a></li>\n";
		}
	}
	echo "</ul>\n";
}

// this function returns true if $astman is defined and set to something (implying a current connection, false otherwise.
// this function no longer puts out an error message, it is up to the caller to handle the situation. 
// Should probably be changed (at least name) to check if a connection is available to the current engine)
//
function checkAstMan() {
	global $astman;

	return ($astman)?true:false;
}

/* merge_ext_followme($dest) {
 * 
 * The purpose of this function is to take a destination
 * that was either a core extension OR a findmefollow-destination
 * and convert it so that they are merged and handled just like
 * direct-did routing
 *
 * Assuming an extension number of 222:
 *
 * The two formats that existed for findmefollow were:
 *
 * ext-findmefollow,222,1
 * ext-findmefollow,FM222,1
 *
 * The one format that existed for core was:
 *
 * ext-local,222,1
 *
 * In all those cases they should be converted to:
 *
 * from-did-direct,222,1
 *
 */
function merge_ext_followme($dest) {

	if (preg_match("/^\s*ext-findmefollow,(FM)?(\d+),(\d+)/",$dest,$matches) ||
	    preg_match("/^\s*ext-local,(FM)?(\d+),(\d+)/",$dest,$matches) ) {
				// matches[2] => extn
				// matches[3] => priority
		return "from-did-direct,".$matches[2].",".$matches[3];
	} else {
		return $dest;
	}
}

function get_headers_assoc($url) {
	$url_info=parse_url($url);
  $host = isset($url_info['host']) ? $url_info['host'] : '';
	if (isset($url_info['scheme']) && $url_info['scheme'] == 'https') {
		$port = isset($url_info['port']) ? $url_info['port'] : 443;
		@$fp=fsockopen('ssl://'.$host, $port, $errno, $errstr, 10);
	} else {
		$port = isset($url_info['port']) ? $url_info['port'] : 80;
		@$fp=fsockopen($host, $port, $errno, $errstr, 10);
	}
	if ($fp) {
		stream_set_timeout($fp, 10);
    $query = isset($url_info['query']) ? $url_info['query'] : '';
		$head = "HEAD ".@$url_info['path']."?".$query;
		$head .= " HTTP/1.0\r\nHost: ".$host."\r\n\r\n";
		fputs($fp, $head);
		while(!feof($fp)) {
			if($header=trim(fgets($fp, 1024))) {
				$sc_pos = strpos($header, ':');
				if ($sc_pos === false) {
					$headers['status'] = $header;
				} else {
					$label = substr( $header, 0, $sc_pos );
					$value = substr( $header, $sc_pos+1 );
					$headers[strtolower($label)] = trim($value);
				}
			}
		}
		return $headers;
	} else {
		return false;
	}
}


// Dragged this in from page.modules.php, so it can be used by install_amp. 
function runModuleSQL($moddir,$type){
	trigger_error("runModuleSQL() is depreciated - please use _module_runscripts(), or preferably module_install() or module_enable() instead", E_USER_WARNING);
	_module_runscripts($moddir, $type);
}

//This function calls modulename_contexts()
//expects a returned array which minimally includes 'context' => the actual context to include
//can also define 'description' => the display for this context - if undefined will be set to 'context'
//'module' => the display for the section this should be listed under defaults to module display (can be used to group subsets within one module)
//'parent' => if including another context automatically includes this one, list the parent context
//'priority' => default sort order for includes range -50 to +50, 0 is default
//'enabled' => can be used to flag a context as disabled and it won't be included, but will not have its settings removed.
//'extension' => can be used to tag with an extension for checkRange($extension)
//'dept' => can be used to tag with a department for checkDept($dept)
//	this defaults to false for disabled modules.
function freepbx_get_contexts() {
	$modules = module_getinfo(false, array(MODULE_STATUS_ENABLED, MODULE_STATUS_DISABLED, MODULE_STATUS_NEEDUPGRADE));
	
	$contexts = array();
	
	foreach ($modules as $modname => $mod) {
                $funct = strtolower($modname.'_contexts');
		if (function_exists($funct)) {
			// call the  modulename_contexts() function
			$contextArray = $funct();
			if (is_array($contextArray)) {
				foreach ($contextArray as $con) {
					if (isset($con['context'])) {
						if (!isset($con['description'])) {
							$con['description'] = $con['context'];
						}
						if (!isset($con['module'])) {
							$con['module'] = $mod['displayName'];
						}
						if (!isset($con['priority'])) {
							$con['priority'] = 0;
						}
						if (!isset($con['parent'])) {
							$con['parent'] = '';
						}
						if (!isset($con['extension'])) {
							$con['extension'] = null;
						}
						if (!isset($con['dept'])) {
							$con['dept'] = null;
						}
						if ($mod['status'] == MODULE_STATUS_ENABLED) {
							if (!isset($con['enabled'])) {
								$con['enabled'] = true;
							}
						} else {
							$con['enabled'] = false;
						}
						$contexts[ $con['context'] ] = $con;
					}
				}
			}
		}
	}
	return $contexts;
}

?>
