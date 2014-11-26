<?php
function photoBrowser($output,$mediadbADO,$dbADO) {
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');
	include('photoUtils.php');

	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	$out.=getJS();
	$out.=getCSS();
	$out.='
<script>
document.observe("dom:loaded", function() {
    loadAttributesToSelect("attrname");
});
</script>
<form action="index.php?section=photoBrowser" method="POST" name="photoBrowserForm">
<input type="hidden" name="section" value="photoBrowser" />
<input type="hidden" name="page" value="'.$page.'" />
Attribute: <select id="attrname" name="attrname" size="1" ><option value="0"></option></select>';
$out.='
<input name="submit" type="submit" value="Search" />
<select id="newkw_lookup" size="8" style="display: none;"><option value="0"></option></select>
';
if (isset($_REQUEST['attrname']) && !empty($_REQUEST['attrname'])) {
   $attrname = $_REQUEST['attrname'];
//   $attributes = getAttributesByName($mediadbADO, $attrname);
//   if (sizeof($attributes) > 0) {
      $attribute = $attrname;//$attributes[0]['PK_Attribute'];
      $out.='
	<table cellpading="0" cellspacing="0">
		<tr>
			<td>'.getPhotoFiles($attribute,$mediadbADO,1,100).'</td>
		</tr>
	</table>';
//   }
}
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle("Photo Browser");
	$output->setReloadLeftFrame(false);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>