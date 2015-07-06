<?php
function init_services($mediadbADO, $dbADO, $output){
	
	$googleCheck=' SELECT 
				DeviceData.Description AS dd_Description, 
				Device_DeviceData.FK_DeviceData,
                                Device.PK_Device,
				ParameterType.Description AS typeParam, 
				Device_DeviceData.IK_DeviceData
			FROM DeviceData 
			INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
			INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
			INNER JOIN Device ON FK_Device=PK_Device
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE Device.FK_DeviceTemplate=2316 AND Device_DeviceData.FK_DeviceData=59  ';
			
	$gmailBox.='<th class="tg-031e"> <a href="index.php?section=cloudServices&service=gmail&action=auth" > 
   <img src="operations/cloudServices/images/gmail.gif" alt="gmail" > 
   </a> 
   </th>';		
	$res = $dbADO->Execute($googleCheck);
	$ddForDevice = $res->FetchRow();
	
	$confData=json_decode($ddForDevice['IK_DeviceData'], true);
	$userDataArray = $confData[$_SESSION['userID']];	

	if( !empty($userDataArray)){					
		
		$gmailBox='<th class="tg-031e"> <a href="index.php?section=cloudServices&service=gmail&action=auth" > 
   <img src="operations/cloudServices/images/gmail.gif" alt="gmail" > </a> 
     <br> Google Services are enabled. <br>
   </th>';
		//if($confData[ $_SESSION['userID'] ]){
			
	//	}
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
	
	$output -> setScriptInHead($scriptInHead);
	$output -> setReloadLeftFrame(false);
	$output -> setMenuTitle('Cloud Services ');
	$output -> setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
	$output -> setBody($out);
	$output -> setTitle(APPLICATION_NAME);
	$output -> output();
	
}
   
?>