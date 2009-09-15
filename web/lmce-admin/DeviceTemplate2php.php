<?php
/* 
 * DeviceTemplate2PHP is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * DeviceTemplate2PHP is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA
 *
 *  Copyright 2008 posde@web.de
 *
 * 	v0.01 - 2008-09-23 - Sharm el-Sheikh version
 * 2009-05-18 - Rewrite by Jason Richardson <merkur2k@gmail.com>
 * - Change export data to be a serialized php associative array
 * - Add functions to web-enable the entire shebang
 
 */
error_reporting(E_ALL);


//The device template to export
$pk_value = @$_REQUEST['DeviceTemplateID'];
//Used to override the warning for exporting a modified stock template
$force_export = @$_REQUEST["forceExport"] || false;
//Setting forceExportAll will force the script to output all related devices even if
//they have not been modified. There is no user interface option for this as it is just for testing.
$force_export_all = @$_REQUEST["forceExportAll"] || false;
//Also used when overriding the warning for exporting a modified stock template.
$force_export_type = @$_REQUEST["forceExportType"];
//import or export...
$mode = @$_REQUEST["mode"];
//Result of confirmation request
$confirm = @$_REQUEST["confirm"];
if (!$mode) {
  print "Please enter the device template ID of a device template to export:
<form method=GET>
<input type=text name=DeviceTemplateID value=$pk_value>
<input type=submit value=Export>
<input type=hidden name=mode value=export>
</form>
<P>OR<P>
Please select a file to import:
<form method=POST enctype='multipart/form-data'>
<input type=file name=importFile>
<input type=submit value=Import>
<input type=hidden name=mode value=import>
</form>
";
  die ();
}
$link = mysql_connect("localhost", "root");
mysql_select_db("pluto_main", $link);
if ($mode == "export") {
  $pk_value = intval($pk_value);

  $query = "SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate = ".$pk_value;
  If(!($result = mysql_query($query)))
  {
    $noOfRows = 0;
  }
  else
  {
    $noOfRows = mysql_num_rows($result);
  }
  if ($noOfRows <> 1) {
    if ($noOfRows == 0)
    {
      print "<p>There is no DeviceTemplate with ID number $pk_value</p>";
    }
    else
    {
      print "<p>There is more than one DeviceTemplate with ID number $pk_value. THIS IS WRONG!</p>";
    }
    die ("<p>Export aborted!</p>");
  }
  $row = mysql_fetch_object($result);

  if (!$confirm) {
    if ($row->psc_mod == '0000-00-00 00:00:00')
    {
      //Template came with the original install or from a SQLcvs update and has not been modified
      //No need to do anything with this case
      print "DeviceTemplate ".$row->PK_DeviceTemplate." (".$row->Description.") has not been modified. There are no modifications to export.<P>Export Aborted!<BR>";
      $type = "old";
      $force_export || die ();
    }
    else
    if ($row->psc_id > 0)
    {
      //Template came with the original install or from a SQLcvs update and has been modified.
      //Will export.
      print "DeviceTemplate ".$row->PK_DeviceTemplate." (".$row->Description.") is an original template that has been modified.<BR>Replacing a device template from the original install will most likely cause serious problems.
	It is recommended that you export this as a new device.
	<form method=GET>
	<input type=hidden name=DeviceTemplateID value=$pk_value>
	<input type=hidden name=mode value=$mode>
	<input type=radio name=forceExportType value='old'> I know what I am doing, continue export as is.<BR>
	<input type=radio name=forceExportType value='new' checked> Export as a new device.<BR>
	<input type=hidden name=forceExport value=true>
	<input type=hidden name=confirm value=true>
	<input type=submit value='Continue export'>
	</form>";
      $type = "old";
      $force_export || die ();
    }
    else
    {
      //Template was added by the user after install.
      //Will export.
      print "DeviceTemplate ".$row->PK_DeviceTemplate." (".$row->Description.") is a custom template.<BR>
	  <form method=GET>
	<input type=hidden name=DeviceTemplateID value=$pk_value>
	<input type=hidden name=mode value=$mode>
	  <input type=hidden name=forceExport value=true>
	<input type=hidden name=confirm value=true>
	<input type=submit value='Continue export'>
	</form>";
      $type = "new";
    }

    //start the actual export
    print "Exporting DeviceTemplate ".$row->PK_DeviceTemplate." (".$row->Description.").<BR>";
  }
  //Create a container object to hold all the data related to this device.
  $device = array ();
  $device["id"] = $row->PK_DeviceTemplate;
  $device["type"] = $force_export_type?$force_export_type:$type;
  $device["data"] = array ();
  $device["related"] = array ();
  $related = array ();

  //print "<pre>";

  function CreateInsert( & $device, $link, $pk_value, $table, $keyColumn = "", $newKey = "")
  {
    //
    //	This function creates a PHP script to insert a single record into
    // a database based upon the record identified by the Primary Key.
    //
    if ($keyColumn == "")
    {
      $keyColumn = "PK_".$table;
    }
    // We take all data from the original table.
    $query = "SELECT * FROM $table WHERE $keyColumn = $pk_value ";
    //Ignore related device entries that have not been modified.
    if ($table != 'DeviceTemplate' && !$GLOBALS["force_export_all"]) {
      $query .= "AND psc_mod>0";
    }
    $result = mysql_query($query, $link);
    if (!$result) {
      return;
    }
    $noOfRows = mysql_num_rows($result);
    $externalTables = array ();
    //		print $query . "\n";
    //		print $result . "\n";
    if ($noOfRows == 0)
    {
      $externalTables = array ("-1", "-1");
    }
    else
    {
      // Get the relevant record(s)
      while ($line = mysql_fetch_array($result, MYSQL_ASSOC))
      {
        //print serialize($line);
        $newrow = array ();

        $counter = 0;
        $first = 0;
        foreach ($line as $field)
        {
          $fieldType = mysql_field_type($result, $counter);
          $fieldName = mysql_field_name($result, $counter);
          $prefix = substr($fieldName, 0, 3);
          // Usually we insert the field value
          $insertField = True;
          // Except, if the value is NULL, a primary key, or one of the psc_ variables for sqlCVS.
          if (is_null($field))
          {
            $insertField = False;
          }
          if ($prefix == "PK_")
          {
            $insertField = False;
          }
          if ($prefix == "psc")
          {
            $insertField = False;
          }
          //
          if ($insertField)
          {
            $value = $field;

            $withoutPrefix = substr($fieldName, 3);
            if ($fieldName == $keyColumn)
            {
              $value = -1;
            }
            // We need to take all the foreign key associated tables with us.
            if (($prefix == "FK_") and ($table == "DeviceTemplate") and ($insertField))
            {
              //
              // Question is, do  we insert the child table now, and have the new
              // ID value for the current insert available. Hmm, this work as long
              // as we dont have cicrular relation.



              $returnValue = CreateInsert($device, $link, $field, $withoutPrefix);
              // Only create a special entry, IF we got a result
              $specialEntryRequired = False;
              If(@$returnValue[0] != -1)
              {
                $specialEntryRequired = True;
              }
              // We only add new PK_s for children connections to the DeviceTemplate table.
              If($table <> "DeviceTemplate")
              {
                $specialEntryRequired = False;
              }
              if ($specialEntryRequired)
              {
                if ($withoutPrefix == "InfraredGroup")
                {
                  CreateInsert($device, $link, $field, "InfraredGroup_Command", "FK_InfraredGroup");
                }
              }
            }
            $newrow[$fieldName] = $value;
            $first = 1;
          }
          $counter++;
        }
        //print "Found data in table $table<BR>";
        if ($table == 'DeviceTemplate') {
          $device["data"] = $newrow;
        }
        else {
          $device["related"][$table] = $newrow;
        }
      }
    }
    return $externalTables;
  }

  // Create the INSERT code for the maintable and its children.
  CreateInsert($device, $link, $pk_value, "DeviceTemplate");

  // Create all other tables that may contain additional device template information.
  $tables = array ("CommandGroup_D_Command",
  "DHCPDevice",
  "DeviceTemplate_AV",
  "DeviceTemplate_DSPMode",
  "DeviceTemplate_DeviceCategory_ControlledVia",
  "DeviceTemplate_DeviceCommandGroup",
  "DeviceTemplate_DeviceData",
  "DeviceTemplate_DeviceTemplate_ControlledVia",
  "DeviceTemplate_DeviceTemplate_Related",
  "DeviceTemplate_Event",
  "DeviceTemplate_InfraredGroup",
  "DeviceTemplate_Input",
  "DeviceTemplate_MediaType",
  "DeviceTemplate_Output",
  "DeviceTemplate_PageSetup",
  "InstallWizard",
  "Screen_DesignObj",
  "StartupScript"
  );

  foreach ($tables as $table)
  {
    CreateInsert($device, $link, $pk_value, $table, "FK_DeviceTemplate");
  }


  // TODO - Copy any script that might be part of the DHCPDevice definitions into the package.

  //mysql_close($link);
  //print_r($device);
  $data = serialize($device);
  header("Content-type: application/octet-stream");
  header("Content-Disposition: attachment; filename=\"device$pk_value.dt\"");
  print $data;
} else if ($mode == "import") {
  $file = fopen($_FILES["importFile"]["tmp_name"], "r");
  $data = fread($file, filesize($_FILES["importFile"]["tmp_name"]));
  //Now for the insert
  $newdevice = unserialize($data);
  if ($newdevice["type"] == "new") {
    print "<P>Will create new template<BR>";
	print "Creating DeviceTemplate entry<BR>";
    $query = "insert into DeviceTemplate ('";
    $query .= join("', '", array_keys($newdevice["data"]));
    $query .= "') values ('";
    $query .= join("', '", array_values($newdevice["data"]));
    $query .= "')";
    print $query."<BR>";
    $result = mysql_query($query);
    if (!$result) {
      print mysql_error();
      die ();
    }
    $newID = mysql_insert_id();
    foreach ($newdevice["related"] as $table=>$tabledata) {
      //Replace foreign key placeholder with correct value
      foreach ($tabledata as $key=>$value) {
        if ($value == -1) {
          $tabledata[$key] = $newID;
        }
      }
	  print "Creating $table entry<BR>";
      $query = "insert into $table ('";
      $query .= join("', '", array_keys($tabledata));
      $query .= "') values ('";
      $query .= join("', '", array_values($tabledata));
      $query .= "')";
      print $query."<BR>";
      $result = mysql_query($query);
      if (!$result) {
        print mysql_error();
        die ();
      }
    }
  }
  else if ($newdevice["type"] == "old") {
    print "<P>Will replace old template<BR>";
	print "Replacing DeviceTemplate entry<BR>";
    $query = "delete from DeviceTemplate where PK_DeviceTemplate = ".$newdevice["id"];
    print $query."<BR>";
    $newdevice["data"]["PK_DeviceTemplate"] = $newdevice["id"];
    $query = "insert into DeviceTemplate ('";
    $query .= join("', '", array_keys($newdevice["data"]));
    $query .= "') values ('";
    $query .= join("', '", array_values($newdevice["data"]));
    $query .= "')";
    print $query."<BR>";
    $result = mysql_query($query);
    if (!$result) {
      print mysql_error();
      die ();
    }
    $newID = mysql_insert_id();
    foreach ($newdevice["related"] as $table=>$tabledata) {
      //Replace foreign key placeholder with correct value
      foreach ($tabledata as $key=>$value) {
        if ($value == -1) {
          $tabledata[$key] = $newID;
        }
      }
	  print "Creating $table entry<BR>";
      $query = "insert into $table ('";
      $query .= join("', '", array_keys($tabledata));
      $query .= "') values ('";
      $query .= join("', '", array_values($tabledata));
      $query .= "')";
      print $query."<BR>";
      $result = mysql_query($query);
      if (!$result) {
        print mysql_error();
        die ();
      }
    }
  }
  print "Done!";
}
?>
