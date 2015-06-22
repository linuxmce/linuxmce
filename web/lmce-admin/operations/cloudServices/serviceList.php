<?php
function init_services($mediadbADO, $dbADO, $output){
	
	$out='<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;border-color:#aaa;margin:0px auto;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#aaa;color:#333;background-color:#fff;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#aaa;color:#fff;background-color:#f38630;}
.tg .tg-7h0m{background-color:#9b9b9b}
.tg .tg-depp{background-color:#FCFBE3;font-family:"Trebuchet MS", Helvetica, sans-serif !important;}
.tg .tg-z2zr{background-color:#FCFBE3}
</style>
<table class="tg">
  <tr>
    <th class="tg-7h0m">Service</th>
    <th class="tg-7h0m">Type of Services</th>
    <th class="tg-7h0m">Status</th>
    <th class="tg-7h0m">Last Checked</th>
  </tr>
  <tr>
    <td class="tg-depp">Gmail</td>
    <td class="tg-z2zr">Email Integration</td>
    <td class="tg-z2zr"></td>
    <td class="tg-z2zr"></td>
  </tr>
  <tr>
    <td class="tg-031e">Facebook</td>
    <td class="tg-031e">Status Updates<br>Photo Uploads<br>Photo Viewing<br>Location</td>
    <td class="tg-031e"></td>
    <td class="tg-031e"></td>
  </tr>
  <tr>
    <td class="tg-z2zr">Dropbox</td>
    <td class="tg-z2zr">File Storage<br>Photo Uploads<br>Photo Viewing</td>
    <td class="tg-z2zr"></td>
    <td class="tg-z2zr"></td>
  </tr>
  <tr>
    <td class="tg-031e">Google Drive</td>
    <td class="tg-031e">File Storage</td>
    <td class="tg-031e"></td>
    <td class="tg-031e"></td>
  </tr>
</table>';
	
	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	
}
   
?>