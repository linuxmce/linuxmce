<?php
//This file is part of FreePBX.
//
//    FreePBX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 2 of the License, or
//    (at your option) any later version.
//
//    FreePBX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with FreePBX.  If not, see <http://www.gnu.org/licenses/>.
//
//    Copyright 2006 Seth Sargent, Steven Ward
//    Portions Copyright 2009 Mikael Carlsson, mickecamino@gmail.com
//    Portions Copyright 2009 Schmooze Communications LLC
//
/* functions.inc.php - functions for BulkDIDs module. */

function exportdids_all() {
	global $db;
	$action		= "edit";
	$fname		= "bulkdids__" .  (string) time() . $_SERVER["SERVER_NAME"] . ".csv";
	$csv_header 	= "action,DID,description,destination,cidnum,pricid,alertinfo,grppre,mohclass,ringing,delay_answer,privacyman,pmmaxretries,pmminlength\n";
	$data 		= $csv_header;
	$exts 		= get_all_dids();
	foreach ($exts as $ext) {

		$e 	= $ext;
		$did_info = core_did_get($e['extension'],$e['cidnum']);
		$csv_line[0] 	= $action;
		$csv_line[1] 	= isset($did_info["extension"])?$did_info["extension"]:"";
		$csv_line[2] 	= isset($did_info["description"])?$did_info["description"]:"";
		$csv_line[3] 	= isset($did_info["destination"])?$did_info["destination"]:"";
		$csv_line[4] 	= isset($did_info["cidnum"])?$did_info["cidnum"]:"";
		$csv_line[5]	= isset($did_info["pricid"])?$did_info["pricid"]:"";
		$csv_line[6] 	= isset($did_info["alertinfo"])?$did_info["alertinfo"]:"";
		$csv_line[7] 	= isset($did_info["grppre"])?$did_info["grppre"]:"";
		$csv_line[8]	= isset($did_info["mohclass"])?$did_info["mohclass"]:"";
		$csv_line[9]	= isset($did_info["ringing"])?$did_info["ringing"]:"0";
		$csv_line[10]	= isset($did_info["delay_answer"])?$did_info["delay_answer"]:"";
		$csv_line[11]	= isset($did_info["privacyman"])?$did_info["privacyman"]:"";
		$csv_line[12]	= isset($did_info["pmmaxretries"])?$did_info["pmmaxretries"]:"";
		$csv_line[13]	= isset($did_info["pmminlength"])?$did_info["pmminlength"]:"";

		for ($i = 0; $i < count($csv_line); $i++) {
			/* If the string contains a comma, enclose it in double-quotes. */
			if (strpos($csv_line[$i], ",") !== FALSE) {
				$csv_line[$i] = "\"" . $csv_line[$i] . "\"";
			}
			if ($i != count($csv_line) - 1) {
				$data = $data . $csv_line[$i] . ",";
			} else {
				$data = $data . $csv_line[$i];
			}
		}
		$data = $data . "\n";
		unset($csv_line);
	}
	bulkdids_force_download($data, $fname);
	return;
}

function get_all_dids() {
	$sql 	= "SELECT extension,cidnum FROM incoming ORDER BY extension";
	//$extens = sql($sql,"getAll");
	$extens = sql($sql,"getAll",DB_FETCHMODE_ASSOC);
	if (isset($extens)) {
		return $extens;
	} else {
		return null;
	}
}

function bulkdids_force_download ($data, $name, $mimetype="", $filesize=false) {
    // File size not set?
    if ($filesize == false OR !is_numeric($filesize)) {
        $filesize = strlen($data);
    }
    // Mimetype not set?
    if (empty($mimetype)) {
        $mimetype = "application/octet-stream";
    }
    // Make sure there's not anything else left
    bulkdids_ob_clean_all();
    // Start sending headers
    header("Pragma: public"); // required
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Cache-Control: private",false); // required for certain browsers
    header("Content-Transfer-Encoding: binary");
    header("Content-Type: " . $mimetype);
    header("Content-Length: " . $filesize);
    header("Content-Disposition: attachment; filename=\"" . $name . "\";" );
    // Send data
    echo $data;
    die();
}

function bulkdids_ob_clean_all () {
    $ob_active = ob_get_length () !== false;
    while($ob_active) {
        ob_end_clean();
        $ob_active = ob_get_length () !== false;
    }
    return true;
}

function bulkdids_generate_table_rows() {
	$fh = fopen("modules/bulkdids/table.csv", "r");
	if ($fh == NULL) {
		return NULL;
	}
	$k = 0;
	$table = "";
	while (($csv_data = fgetcsv($fh, 1000, ",", "\"")) !== FALSE) {
		$k++;
		for ($i = 0; $i < 5; $i++) {
			if (isset($csv_data[$i])) {
				$table[$k][$i] = $csv_data[$i];
			} else {
				$table[$k][$i] = "";
			}
		}
	}
	fclose($fh);
	return $table;
}
?>
