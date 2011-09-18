<?php /* $Id: functions.inc.php 2188 2006-07-27 02:21:52Z p_lindheimer $ */

/* 	Generates dialplan for parkinglot
	We call this with retrieve_conf
*/

function parking_get_config($engine) {
	global $db;
	global $amp_conf;
	global $ext;  // is this the best way to pass this?
	global $asterisk_conf;
	global $core_conf;
	global $version;

	switch($engine) {
	case "asterisk":

    $ast_lt_16 = version_compare($version,'1.6','lt');
    $ast_lt_162 = version_compare($version,'1.6.2','lt');
    $ast_ge_18 = version_compare($version,'1.8','ge');

		$contextname = 'park-dial';

		$parkinglot_id = 1; // only 1 parking lot, but prepare for future
		$results = parking_getconfig($parkinglot_id);

		// Got the array, let's go work out the required variables
		//
		$parkingenabled	= isset($results['parkingenabled'])?$results['parkingenabled']:'';
		$parkext 	= isset($results['parkext'])?$results['parkext']:70;
		$numslots  	= isset($results['numslots'])?$results['numslots']:8;
		$parkingtime 	= isset($results['parkingtime'])?$results['parkingtime']:'';
		$parkingcontext	= isset($results['parkingcontext'])?$results['parkingcontext']:'parkedcalls';
		$parkalertinfo 	= isset($results['parkalertinfo'])?$results['parkalertinfo']:'';
		$parkcid 	= isset($results['parkcid'])?trim($results['parkcid']):'';
		$parkingannmsg_id 	= isset($results['parkingannmsg_id'])?$results['parkingannmsg_id']:'';
		$goto	 	= isset($results['goto'])?$results['goto']:'';

    $parkinghints = isset($results['parkinghints'])?$results['parkinghints']:'no';

		$parkpos1	= $parkext + 1;
		$parkpos2	= $parkpos1 + $numslots -1;

		if ($parkingenabled) {

			$core_conf->addFeatureGeneral('parkext',$parkext);
			$core_conf->addFeatureGeneral('parkpos',$parkpos1."-".$parkpos2);
			$core_conf->addFeatureGeneral('context',$parkingcontext);

      if (!$ast_ge_18) {
        $ext->addInclude('from-internal-additional', $parkingcontext);
      }

			if ($parkingtime) {
				$core_conf->addFeatureGeneral('parkingtime',$parkingtime);
			}

			// If we have an appropriate Asterisk patch, set paraemters for Asterisk
			//
			if ($amp_conf["PARKINGPATCH"] && $ast_lt_16) {
				if ($parkalertinfo) {
					$core_conf->addFeatureGeneral('parkreturnalertinfo',$parkalertinfo);
				}
				if ($parkcid) {
					$core_conf->addFeatureGeneral('parkreturncidprefix',$parkcid);
				}
			  // Now generate dialplan
			  $ext->add($contextname, "t", '', new ext_noop('Parked Call Timed Out and Got Orphaned'));
			  $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_noop('Parked Call Timed Out and Got Orphaned'));

			  if ($parkingannmsg_id != '') {
				  $parkingannmsg = recordings_get_file($parkingannmsg_id);
				  $ext->add($contextname, "t", '', new ext_playback($parkingannmsg));
				  $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_playback($parkingannmsg));
			  }

			} else if (!$ast_ge_18) {

				if ($parkalertinfo) {
					$ext->add($contextname, "t", '', new ext_alertinfo(str_replace(';', '\;', $parkalertinfo)));
					$ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_alertinfo(str_replace(';', '\;', $parkalertinfo)));
				}
				if ($parkcid) {
					$ext->add($contextname, "t", '', new ext_setvar('CALLERID(name)', $parkcid.'${CALLERID(name)}'));
					$ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_setvar('CALLERID(name)', $parkcid.'${CALLERID(name)}'));
				}
			  if ($parkingannmsg_id != '') {
				  $parkingannmsg = recordings_get_file($parkingannmsg_id);
				  $ext->add($contextname, "t", '', new ext_playback($parkingannmsg));
				  $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_playback($parkingannmsg));
			  }

			} else {

        $parking_dest = isset($results['parking_dest'])?$results['parking_dest']:'device';

        // in 1.8 we never come back to the origin, we always run it through our processing and then send it
        // either to the original caller through the generated context in park-dial, or to the default destination
        //
				$core_conf->addFeatureGeneral('comebacktoorigin','no');

				$core_conf->addFeatureGeneral('parkedplay',isset($results['parkedplay']) ? $results['parkedplay'] : 'both');
				$core_conf->addFeatureGeneral('parkedcalltransfers',isset($results['parkedcalltransfers']) ? $results['parkedcalltransfers'] : 'caller');
				$core_conf->addFeatureGeneral('parkedcallrepark',isset($results['parkedcallrepark']) ? $results['parkedcallrepark'] : 'caller');
				$core_conf->addFeatureGeneral('parkedcallhangup',isset($results['parkedcallhangup']) ? $results['parkedcallhangup'] : 'no');
				$core_conf->addFeatureGeneral('parkedcallrecording',isset($results['parkedcallrecording']) ? $results['parkedcallrecording'] : 'no');
				$core_conf->addFeatureGeneral('parkedmusicclass',isset($results['parkedmusicclass']) ? $results['parkedmusicclass'] : 'default');
				$core_conf->addFeatureGeneral('adsipark',isset($results['adsipark']) ? $results['adsipark'] : 'no');
				$core_conf->addFeatureGeneral('findslot',isset($results['findslot']) ? $results['findslot'] : 'first');

        $contextname = 'parkedcallstimeout';
        $exten = $parking_dest == 'dest' && $goto ? 's' : '_[0-9a-zA-Z*#].';
			  if ($parkalertinfo) {
				  $ext->add($contextname, $exten, '', new ext_sipremoveheader('Alert-Info:'));
				  $ext->add($contextname, $exten, '', new ext_alertinfo(str_replace(';', '\;', $parkalertinfo)));
         }
			  if ($parkcid) {
				  $ext->add($contextname, $exten, '', new ext_execif('$["${CALLERID(name):0:' .strlen($parkcid). '}" != "' .$parkcid. '"]','Set','CALLERID(name)=' .$parkcid. '${CALLERID(name)}'));
			  }
		    if ($parkingannmsg_id != '') {
			    $parkingannmsg = recordings_get_file($parkingannmsg_id);
			    $ext->add($contextname, $exten, '', new ext_playback($parkingannmsg));
		    }
			  $ext->add($contextname, $exten, '', new ext_goto($parking_dest == 'dest' && $goto ? $goto : 'park-dial,${EXTEN},1'));
      }

      // In all cases, add a timeout and catchall in the park-dial context to revert to the
      // $goto destination if all else fails
		  //
      $contextname = 'park-dial';
      if ($goto) {
		    $ext->add($contextname, "t", '', new ext_goto($goto));
		    $ext->add($contextname, "t", '', new ext_noop('WARNING: PARKRETURN to: [${EXTEN}] failed with: [${DIALSTATUS}]. Trying Alternate Dest'));
		    $ext->add($contextname, "t", '', new ext_goto($goto));
		    $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_goto($goto));
		    $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_noop('WARNING: PARKRETURN to: [${EXTEN}] failed with: [${DIALSTATUS}]. Trying Alternate Dest'));
		    $ext->add($contextname, "_[0-9a-zA-Z*#].", '', new ext_goto($goto));
      }

      // Although 1.6.2+ can generate the hints, we are going to do it since we need to make some other changes
      // and thus don't want to include the default context in 1.8. Also, when we get to multi-parking lots we
      // need to generate their hints anyhow.
      //
      if ($parkinghints == 'yes' || $ast_ge_18) {
        $parkhints = 'park-hints';
        $ext->addInclude('from-internal-additional', $parkhints);
        $ext->addInclude($parkhints, $parkingcontext);
        $hv_all = '';
        for ($slot = $parkpos1; $slot <= $parkpos2; $slot++) {
          if ($ast_ge_18) {
				    $ext->add($parkhints, $slot, '', new ext_macro('parked-call',$slot));
          } else {
				    $ext->add($parkhints, $slot, '', new ext_parkedcall($slot));
          }
          if ($parkinghints == 'yes') {
            $hv = "park:$slot@$parkingcontext";
            $hv_all .= $hv.'&';
				    $ext->addHint($parkhints, $slot, $hv);
          }
        }

        $fcc = new featurecode('parking', 'parkedcall');
        $pc_code = $fcc->getCodeActive();
        unset($fcc);

        if ($pc_code != '') {
          if ($ast_ge_18) {
				    $ext->add($parkhints, $pc_code, '', new ext_macro('parked-call',''));
          } else {
				    $ext->add($parkhints, $pc_code, '', new ext_parkedcall());
          }
          if ($parkinghints == 'yes') {
            $hv = rtrim($hv_all,'&');
				    $ext->addHint($parkhints, $pc_code, $hv);
          }
        }
        $pc = 'macro-parked-call';
        $exten = 's';
				$ext->add($pc, $exten, '', new ext_set('CCSS_SETUP','TRUE'));
				$ext->add($pc, $exten, '', new ext_macro('user-callerid'));
				$ext->add($pc, $exten, '', new ext_gotoif('$["${ARG1}" = "" | ${DIALPLAN_EXISTS(parkedcalls,${ARG1},1)} = 1]','parkedcall'));
				$ext->add($pc, $exten, '', new ext_resetcdr(''));
				$ext->add($pc, $exten, '', new ext_nocdr(''));
				$ext->add($pc, $exten, '', new ext_wait('1'));
				$ext->add($pc, $exten, '', new ext_noop_trace('User: ${CALLERID(all)} tried to pickup non-existent Parked Call Slot ${ARG1}'));
				$ext->add($pc, $exten, '', new ext_playback('pbx-invalidpark'));
				$ext->add($pc, $exten, '', new ext_wait('1'));
				$ext->add($pc, $exten, '', new ext_hangup(''));
				$ext->add($pc, $exten, 'parkedcall', new ext_noop_trace('User: ${CALLERID(all)} picking up Parked Call Slot ${ARG1}'));
				$ext->add($pc, $exten, 'parkedcall', new ext_parkedcall('${ARG1}'));
		  }
		break;
    }
	}
}

function parking_add($parkingenabled, $parkext, $numslots, $parkingtime, $parkingcontext, $parkalertinfo, $parkcid, $parkingannmsg_id, $goto) {
  $pargs['parkingenabled'] = $parkingenabled;
  $pargs['parkext'] = $parkext;
  $pargs['numslots'] = $numslots;
  $pargs['parkingtime'] = $parkingtime;
  $pargs['parkingcontext'] = $parkingcontext;
  $pargs['parkalertinfo'] = $parkalertinfo;
  $pargs['parkcid'] = $parkcid;
  $pargs['parkingannmsg_id'] = $parkingannmsg_id;
  $pargs['goto'] = $goto;
  
  return parking_update($pargs);
}

function parking_update($pargs) {
	global $db;
	global $amp_conf;

  $ast_ge_18 = version_compare($amp_conf['ASTVERSION'],'1.8','ge');

  $parkingenabled = isset($pargs['parkingenabled']) ? $pargs['parkingenabled'] : '';
  $parkext = isset($pargs['parkext']) ? $pargs['parkext'] : '70';
  $numslots = isset($pargs['numslots']) ? $pargs['numslots'] : '8';
  $parkingtime = isset($pargs['parkingtime']) ? $pargs['parkingtime'] : '45';
  $parkingcontext = isset($pargs['parkingcontext']) ? $pargs['parkingcontext'] : 'parkedcalls';
  $parkalertinfo = isset($pargs['parkalertinfo']) ? $pargs['parkalertinfo'] : '';
  $parkcid = isset($pargs['parkcid']) ? $pargs['parkcid'] : '';
  $parkingannmsg_id = isset($pargs['parkingannmsg_id']) ? $pargs['parkingannmsg_id'] : '';
  $goto = isset($pargs['goto']) ? $pargs['goto'] : '';
  $parkinghints = isset($pargs['parkinghints']) ? $pargs['parkinghints'] : 'yes';

	$parkinglot_id 	= 1; // only 1 parkinglot but prepare for future

	// in future will do in a parking_del but not needed for now
	//
	$sql = "DELETE FROM parkinglot WHERE id = '$parkinglot_id'";
	$result = $db->query($sql);
	if(DB::IsError($result)) {
		die_freepbx($result->getMessage().$sql);
	}

	// Check for interger only inputs and set to default if not
	//  and set goto to default if not set for some reason
	//
	$parkext	= ctype_digit($parkext)		? $parkext	: 70;
	$numslots		= ctype_digit($numslots)		? $numslots	: 8;
	$parkingtime	= ctype_digit($parkingtime)	? $parkingtime	: '';

	$parkfields =	array(array($parkinglot_id, 'parkingenabled', "$parkingenabled"),
			array($parkinglot_id, 'parkext', "$parkext"),
			array($parkinglot_id, 'numslots', "$numslots"),
			array($parkinglot_id, 'parkingtime', "$parkingtime"),
			array($parkinglot_id, 'parkingcontext', trim("$parkingcontext")),
			array($parkinglot_id, 'parkalertinfo', trim("$parkalertinfo")),
			array($parkinglot_id, 'parkcid', trim("$parkcid")),
			array($parkinglot_id, 'parkingannmsg_id', "$parkingannmsg_id"),
      array($parkinglot_id, 'goto', "$goto"),
      array($parkinglot_id, 'parkinghints', "$parkinghints"),
    );

  if ($ast_ge_18) {
    $parkfields[] = array($parkinglot_id, 'parking_dest', isset($pargs['parking_dest'])?$pargs['parking_dest']:'device');
    $parkfields[] = array($parkinglot_id, 'parkedplay', isset($pargs['parkedplay'])?$pargs['parkedplay']:'both');
    $parkfields[] = array($parkinglot_id, 'parkedcalltransfers', isset($pargs['parkedcalltransfers'])?$pargs['parkedcalltransfers']:'caller');
    $parkfields[] = array($parkinglot_id, 'parkedcallreparking', isset($pargs['parkedcallreparking'])?$pargs['parkedcallreparking']:'caller');
    $parkfields[] = array($parkinglot_id, 'parkedcallhangup', isset($pargs['parkedcallhangup'])?$pargs['parkedcallhangup']:'no');
    $parkfields[] = array($parkinglot_id, 'parkedcallrecording', isset($pargs['parkedcallrecording'])?$pargs['parkedcallrecording']:'no');
    $parkfields[] = array($parkinglot_id, 'parkedmusicclass', isset($pargs['parkedmusicclass'])?$pargs['parkedmusicclass']:'default');
    $parkfields[] = array($parkinglot_id, 'findslot', isset($pargs['findslot'])?$pargs['findslot']:'first');
    $parkfields[] = array($parkinglot_id, 'adsipark', isset($pargs['adsipark'])?$pargs['adsipark']:'no');
  }

	$compiled = $db->prepare('INSERT INTO parkinglot (id, keyword, data) values (?,?,?)');

	$result = $db->executeMultiple($compiled,$parkfields);
	if(DB::IsError($result)) {
		die_freepbx($result->getDebugInfo()."<br><br>".'error adding to PARKING table');
	}
}

function parking_getconfig($parkinglot_id=1) {
	global $db;
	$sql = "SELECT keyword,data FROM parkinglot WHERE id = '$parkinglot_id'";
	$results = $db->getAssoc($sql);
	if(DB::IsError($results)) {
		$results = null;
	}
	return $results;
}

function parking_check_destinations($dest=true) {
	global $active_modules;

	$destlist = array();
	if (is_array($dest) && empty($dest)) {
		return $destlist;
	}
	$sql = "SELECT keyword, data FROM parkinglot WHERE keyword = 'goto' ";
	if ($dest !== true) {
		$sql .= "AND data in ('".implode("','",$dest)."')";
	}
	$results = sql($sql,"getAll",DB_FETCHMODE_ASSOC);

	$type = isset($active_modules['parking']['type'])?$active_modules['parking']['type']:'setup';

	foreach ($results as $result) {
		$thisdest = $result['data'];
		$destlist[] = array(
			'dest' => $thisdest,
			'description' => _('Parking Lot'),
			'edit_url' => 'config.php?type='.$type.'&display=parking',
		);
	}
	return $destlist;
}

function parking_change_destination($old_dest, $new_dest) {
	$sql = 'UPDATE parkinglot SET data = "' . $new_dest . '" WHERE data = "' . $old_dest . '" AND keyword = "goto"';
	sql($sql, "query");
}

function parking_recordings_usage($recording_id) {
	global $active_modules;

	$my_id = sql("SELECT `data` FROM `parkinglot` WHERE `id` = '1' AND `keyword` = 'parkingannmsg_id' AND `data` = '$recording_id'","getOne");
	if (!isset($my_id) || $my_id == '') {
		return array();
	} else {
		$type = isset($active_modules['parking']['type'])?$active_modules['parking']['type']:'setup';
		$usage_arr[] = array(
			'url_query' => 'config.php?type='.$type.'&display=parking',
			'description' => _("Parking Lot"),
		);
		return $usage_arr;
	}
}

// Duly stolen from the queues module (since I can't count on it being there, but would not be bad to stuff back in the common include
//
function parking_timeString($seconds, $full = false) {
        if ($seconds == 0) {
                return "0 ".($full ? "seconds" : "s");
        }

        $minutes = floor($seconds / 60);
        $seconds = $seconds % 60;

        $hours = floor($minutes / 60);
        $minutes = $minutes % 60;

        $days = floor($hours / 24);
        $hours = $hours % 24;

        if ($full) {
                return substr(
				($days ? $days." "._("day").(($days == 1) ? "" : "s").", " : "").
                              	($hours ? $hours." ".(($hours == 1) ? _("hour") : _("hours")).", " : "").
                              	($minutes ? $minutes." ".(($minutes == 1) ? _("minute") : _("minutes")).", " : "").
                              	($seconds ? $seconds." ".(($seconds == 1) ? _("second") : _("seconds")).", " : ""),
                              	0, -2);
        } else {
                return substr(($days ? $days."d, " : "").($hours ? $hours."h, " : "").($minutes ? $minutes."m, " : "").($seconds ? $seconds."s, " : ""), 0, -2);
        }
}

?>
