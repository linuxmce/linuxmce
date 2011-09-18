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
set_time_limit(3000);

function bdid_fatal($text)  {
	$clean = str_replace("</script>","",str_replace("<script>javascript:alert('","",$text));
	return "\t".$clean."\n";
}

// $change is used as a flag whether or not a reload is needed. If no changes
// are made, no reload will be prompted.
$change = false;
$output = "";

if ($_REQUEST["csv_type"] == "output") {
	exportdids_all();
} elseif ($_REQUEST["csv_type"] == "input") {

 if (!$_SESSION["AMP_user"]->checkSection("did")) { 
  $output = "<h3>Access denied due to Administrator restrictions</h3>";
  } else {  

    $aFields = array (
      "action" => array(false, -1),
      "DID" => array(false, -1),
      "description" => array(false, -1),
      "destination" => array(false, -1),
      "cidnum" => array(false, -1),
      "pricid" => array(false, -1),
      "alertinfo" => array(false, -1),
      "grppre" => array(false, -1),
      "mohclass" => array(false, -1),
      "ringing" => array(false, -1),
      "delay_answer" => array(false, -1),
      "privacyman" => array(false, -1),
      "pmmaxretries" => array(false, -1),
      "pmminlength" => array(false, -1)
      );

      $fh = fopen($_FILES["csvFile"]["tmp_name"], "r");
      if ($fh == NULL) {
	      $file_ok = FALSE;
      } else {
	      $file_ok = TRUE;
      }

      $k = 0;
      $i = 0;

      while ($file_ok && (($aInfo = fgetcsv($fh, 2000, ",", "\"")) !== FALSE)) {
              $k++;
	      if (empty($aInfo[0])) {
		      continue;
	      }

	      // If this is the first row then we need to check each field listed (these are the headings)
	      if ($i==0) {
		      for ($j=0; $j<count($aInfo); $j++) {
			      $aKeys = array_keys($aFields);
			      foreach ($aKeys as $sKey) {
				      if ($aInfo[$j] == $sKey) {
					      $aFields[$sKey][0] = true;
					      $aFields[$sKey][1] = $j;
				      }
			      }
		      }
		      $i++;
		      $output .= "<BR><BR>Row $k: Headers parsed. <BR>";
		      continue;
	      }

	      if ($aFields["action"][0]) {
		      $vars["action"] = trim($aInfo[$aFields["action"][1]]);
	      }

	      if ($aFields["DID"][0]) {
		      $vars["extension"]  = trim($aInfo[$aFields["DID"][1]]);
	      }

	      if ($aFields["description"][0]) {
		      $vars["description"] = trim($aInfo[$aFields["description"][1]]);
	      }

	      if ($aFields["destination"][0]) {
		      $vars["destination"] = trim($aInfo[$aFields["destination"][1]]);
	      }

	      if ($aFields["cidnum"][0]) {
		      $vars["cidnum"] = trim($aInfo[$aFields["cidnum"][1]]);
	      }

	      if ($aFields["pricid"][0]) {
		      $vars["pricid"] = trim($aInfo[$aFields["pricid"][1]]);
	      }

	      if ($aFields["alertinfo"][0]) {
		      $vars["alertinfo"] = trim($aInfo[$aFields["alertinfo"][1]]);
	      }

	      if ($aFields["grppre"][0]) {
		      $vars["grppre"] = trim($aInfo[$aFields["grppre"][1]]);
	      }

      	if ($aFields["mohclass"][0] && $aInfo[$aFields["mohclass"][1]]) {
		      $vars["mohclass"] = trim($aInfo[$aFields["mohclass"][1]]);
	      }
	      else  {
		      $vars["mohclass"] = "default";
	      }
	      if ($aFields["ringing"][0]) {
		      $vars["ringing"] = trim($aInfo[$aFields["ringing"][1]]);
	      }

	      if ($aFields["delay_answer"][0]) {
		      $vars["delay_answer"] = trim($aInfo[$aFields["delay_answer"][1]]);
	      }
// If privacyman is enabled then check pmmaxretries and pmminlength
	      if ($aFields["privacyman"][0]) {
		      $vars["privacyman"] = trim($aInfo[$aFields["privacyman"][1]]);
		      
		      if ($aFields["pmmaxretries"][0]) {
		        $vars["pmmaxretries"] = trim($aInfo[$aFields["pmmaxretries"][1]]);
		        if($vars["pmmaxretries"] > "10") $vars["pmmaxretries"] = "10";
	        }

	        if ($aFields["pmminlength"][0]) {
		        $vars["pmminlength"] = trim($aInfo[$aFields["pmminlength"][1]]);
		        if($vars["pmminlength"] > "15") $vars["pmminlength"] = "15";
	        }
	      }

	      $vars["faxexten"] = "default";
	      $vars["display"]	= "bulkdids";
	      $vars["type"]	= "tool";

	      $_REQUEST = $vars;

		      switch ($vars["action"]) {
		      	case "add":
				ob_start("bdid_fatal");
				if(!core_did_add($vars,($vars["destination"]?$vars["destination"]:false)))  {
					$output .= "ERROR: ".$vars["extension"]." ".$vars["description"].". See error above<br>";

				}
				else  {		
					$output .= "Row $k: Added: " . $vars["extension"];
					$output .= "<br />";
				}
				ob_end_flush();

				// begin status output for this row
				$change = true;
				break;
			case "edit":
				if (core_did_get($vars["extension"],$vars["cidnum"])) {
					core_did_del($vars["extension"],$vars["cidnum"]);
					$error = ob_start("bdid_fatal");
					if(!core_did_add($vars,($vars["destination"]?$vars["destination"]:false)))  {
						$output .= "ERROR: ".$vars["extension"]." ".$vars["description"].". See error above<br>";

					}
					else  {
						$output .= "Row $k: Edited: " . $vars["extension"] . "<BR>";
					}
					ob_end_flush();
					$change = true;
				}
				break;
			case "del":
				if (core_did_get($vars["extension"],$vars["cidnum"])) {
					core_did_del($vars["extension"],$vars["cidnum"]);
					$change = true;
				}
				$output .= "Row $k: Deleted: " . $vars["extension"] . "<BR>";
				break;
			default:
				$output .= "Row $k: Unrecognized action: the only actions recognized are add, edit, del.\n";
				break;
		      }
	
		      if ($change) {
			  needreload();
		      }
      } // while loop
     }
     print $output;

} else
{
	$table_output = "";
	$table_rows = bulkdids_generate_table_rows();
	if ($table_rows === NULL) {
		$table_output = "Table unavailable";
	} else {
		$table_output .=	"<table cellspacing='0' cellpadding='4' rules='rows'>";
		$table_output .=	"<tr valign='top'>
						<th align='left' valign='top'>#</th>
						<th align='left' valign='top'>Field</th>
						<th align='left' valign='top'>Default</th>
						<th align='left' valign='top'>Allowed</th>
						<th align='left' valign='top'>Field Details</th>
						<th align='left' valign='top'>Description</th>
					</tr>";
		$i = 1;
		foreach ($table_rows as $row) {
			$table_output .= "<tr>";
			$table_output .= "<td valign='top'>" . $i . "</td>";
			$i++;
			foreach ($row as $col) {
				$table_output .= "<td valign='top'>" . $col . "</td>";
			}
			$table_output .= "</tr>";
		}
		$table_output .= "</table>";
	}

?>
<h1>Bulk DIDs</h1>

<h2>Manage DIDs in bulk using CSV files.</h2>

<p>
Start by downloading the
<a href="modules/bulkdids/template.csv">Template CSV file</a>
(right-click > save as) or clicking the Export DIDs button.
</p>
<p>
Modify the CSV file to add, edit, or delete DIDs as desired. Then load
the CSV file. After the CSV file is processed, the action taken for each row
will be displayed.
</p>
<p>

<form action="<?php $_SERVER["PHP_SELF"] ?>" name="uploadcsv" method="post" enctype="multipart/form-data">
<input id="csv_type" name="csv_type" type="hidden" value="none" />
<input type="submit" onclick="document.getElementById('csv_type').value='output';" value="Export DIDs" />
&nbsp;&nbsp;CSV File to Load: <input name="csvFile" type="file" />
<input type="submit" onclick="document.getElementById('csv_type').value='input';"  value="Load File" />
<hr />
</form>
<hr />
<h3>Bulk DIDs CSV File Columns</h3>
<p>
The table below explains each column in the CSV file. You can change the column
order of the CSV file as you like, however, the column names must be preserved.
</p>
<?php
	print $table_output;
}
?>
