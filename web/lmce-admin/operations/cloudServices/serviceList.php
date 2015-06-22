<?php
function init_services($mediadbADO, $dbADO, $output){
	
	$out='<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;margin:0px auto;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
</style>
<table class="tg">
  <tr>
    <th class="tg-031e"> <a href="index.php?section=cloudServices&app=gmail" > <img src="operations/cloudServices/images/gmail.gif" alt="gmail" > </a> </th>
    <th class="tg-031e"> <img src="operations/cloudServices/images/facebook.png" alt="facebook" > </th>
  </tr>
  <tr>
    <td class="tg-031e"> <img src="operations/cloudServices/images/dropbox.jpg" alt="Dropbox" ></td>
    <td class="tg-031e"> <img src="operations/cloudServices/images/google-drive.png" alt="google drive" > </td>
  </tr> 
</table>';
	
	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services ');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	
}
   
?>