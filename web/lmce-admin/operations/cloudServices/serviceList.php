<?php

function init_services($mediadbADO, $dbADO, $output){
	
	$authfile = '/tmp/user'.$_SESSION['userID'].'.json';
	$check = file_exists($authfile);			
    $gmailBox='<th class="tg-031e"> <a href="index.php?section=cloudServices&service=gmail&action=auth" >
    <img src="operations/cloudServices/images/gmail.gif" alt="gmail" >
      <br> Google Services are not enabled for '.$_SESSION['hh_username'].'. <br> Click to Authorize <br></a>
    </th>';		
	
	if( $check){	
		$gmailBox='<th class="tg-031e"> 
   <img src="operations/cloudServices/images/gmail.gif" alt="gmail" >
     <br> Google Services are enabled. <a href="index.php?section=googleFiles">View Files.</a> <br>
   </th>';
	} else {
		
	}
	
	$out='<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;margin:0px auto;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
</style>
<table class="tg">
  <tr>';
  
 $out.=$gmailBox;
 
   $out.='<th class="tg-031e"> <img src="operations/cloudServices/images/facebook.png" alt="facebook" > </th>';
  $out.='</tr>
  <tr>
    <td class="tg-031e"> <img src="operations/cloudServices/images/dropbox.jpg" alt="Dropbox" ></td>
    <td class="tg-031e"> <img src="operations/cloudServices/images/google-drive.png" alt="google drive" > </td>
  </tr> 
</table>';
	
	//$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services ');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	
}
   
?>