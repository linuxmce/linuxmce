<?php
include('photoUtils.php');

function getCriteriaField($mediadbADO, $type, $index, $value) {
	 $html = "<p id='criteria_p_".$index."'>";
	 if ($type == "attr") {
	     	  $html.="Keyword: <select id=\"criteria_".$index."\" name=\"criteria_".$index."\" size=\"1\" >";

		  $query='SELECT DISTINCT PK_Attribute, Name
		  		 FROM File_Attribute INNER JOIN File ON File.PK_File=File_Attribute.FK_Attribute 
				 INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute
				 WHERE EK_MediaType = ? AND FK_AttributeType = ? ORDER BY Name';
				 $res=$mediadbADO->Execute($query,array(7, 29));
		  while($row=$res->FetchRow()) {
		      $html.="<option value=\"".$row['PK_Attribute']."\"";
		      if ($row['PK_Attribute'] == $value) {
		          $html.=" selected";
		      }
		      $html.=">".$row['Name']."</option>";
		  }
		  $html.="</select>";
	 } else if ($type == "date") {
	     	  $html.="Date (example: >2000-01-01): <input type=\"text\" id=\"criteria_".$index."\" name=\"criteria_".$index."\" size=\"10\" value=\"".$value."\"/>";
	 }
	 $html.="<input type=\"hidden\" name=\"criteria_type_".$index."\" value=\"".$type."\" /><input type=\"button\" value=\"Remove\" onclick=\"$('criteria_p_".$index."').remove();\" /></p>";
	 return $html;
}

function photoBrowser($output,$mediadbADO,$dbADO) {
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	$PROTOCOL="smb:";
	$DCESHARE="//dcerouter";
	$TMP_DIR="/home/public/.tmp_pictures";

	if ($_REQUEST['action'] == "loadCriteria") {
	       header('Content-type: text/html');
               print(getCriteriaField($mediadbADO, $_REQUEST['type'], $_REQUEST['index'], ""));
	       exit();
	}
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	$i = 0;
	while (!empty($_REQUEST['criteria_type_'.$i])) {
	      if (!empty($_REQUEST['criteria_'.$i])) {
	      	 $item['type'] = $_REQUEST['criteria_type_'.$i];
		 $item['value'] = $_REQUEST['criteria_'.$i];
	      	 $criteria[] = $item;
	      }
	      $i++;
	}
	if ($_REQUEST['action'] == "preparefolder") {
	   exec('sudo -u root /usr/pluto/bin/linkPhotosToDir.sh '.$TMP_DIR.' '.getPhotoFiles($criteria,$mediadbADO,1,-1,true));
	}
	$out.=getJS();
	$out.=getCSS();
	$out.='
<script>
function addCriteria(type) {
	 var i = 0;
	 while (document.getElementById("criteria_"+i) != null) {
	       i++;
	 }
	 new Ajax.Request("index.php?section=photoBrowser", {
	 	method:"get",
		parameters:{type: type, action:"loadCriteria", index: i },
		onSuccess: function(transport) {
			   var response = transport.responseText || "";
			   $("attributeDiv").insert(response);
		} });
}
function prepareFolder() 
{
	$("action").value = "preparefolder";
	$("photoBrowserForm").submit();
}';
$out.='
</script>
<form action="index.php?section=photoBrowser" method="POST" name="photoBrowserForm" id="photoBrowserForm">
<input type="hidden" name="section" value="photoBrowser" />
<input type="hidden" name="page" id="page" value="'.$page.'" />
<input type="hidden" name="action" id="action" value="" />
<div id="attributeDiv">';
for ($i = 0; $i < sizeof($criteria); $i++) {
    $out.=getCriteriaField($mediadbADO, $criteria[$i]['type'], $i, $criteria[$i]['value']);
}
$out.='
</div>
<input name="submitbt" type="submit" value="Search" />
<input type="button" name="addAttribute" value="New Attribute" onclick="addCriteria(\'attr\');" />
<input type="button" name="addDate" value="New Date" onclick="addCriteria(\'date\');" />
<input type="button" name="linkFilesToDir" value="Prepare Folder" onclick="prepareFolder();" />
';
if ($_REQUEST['action'] == "preparefolder") {
   $out.='<a href="'.$PROTOCOL.''.$DCESHARE.''.$TMP_DIR.'">'.$DCESHARE.''.$TMP_DIR.'</a>(You will have to copy the link due to browser limitations)';
}

$out.='
<select id="newkw_lookup" size="8" style="display: none;"><option value="0"></option></select>
';
$page = 1;
if ($_REQUEST['page'] > 1) {
   $page = $_REQUEST['page'];
}
if (sizeof($criteria) > 0) {
      $out.='
	<table cellpading="0" cellspacing="0">
		<tr>
			<td>'.getPhotoFiles($criteria,$mediadbADO,$page,50, false).'</td>
		</tr>
	</table>';
}
$out.='</form>';

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle("Photo Browser");
	$output->setReloadLeftFrame(false);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>