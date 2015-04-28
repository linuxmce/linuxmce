<?php 
/* @var $dbADO ADOConnection */

// include language files
includeLangFile('common.lang.php');
includeLangFile('utils.lang.php');

function eval_buffer($string) {
   ob_start();
   eval("$string[2];");
   $return = ob_get_contents();
   ob_end_clean();
   return $return;
}

function eval_print_buffer($string) {
   ob_start();
   eval("print $string[2];");
   $return = ob_get_contents();
   ob_end_clean();
   return $return;
}

function showSectionContentWithPhp($string) {
	
   $string = preg_replace_callback("/(<\?=)(.*?)\?>/si",
                                   "eval_print_buffer",$content);
   $string=preg_replace_callback("/(<\?php|<\?)(.*?)\?>/si",
                                 "eval_buffer",$string);
   return stripslashes($string);                                 	
} 

// display array nicely formated - used for debug only
function print_array($arr)
{
	print '<pre>';
	print_r($arr);
	print '</pre>';
}

function array_concat($arr1,$arr2){
	$result=$arr1;
	foreach ($arr2 AS $val){
		$result[]=$val;
	}
	
	return $result;
}

//---------------------------------------------------------------------------------------
// i18n functions 
//---------------------------------------------------------------------------------------
function includeLangFile($langFile)
{
	// get existing variables
    $before_eval_vars = get_defined_vars();
	
	// include fallback language file
    if(!isset($GLOBALS['fallbacklang'])) $GLOBALS['fallbacklang']='en';
    if(!defined("APPROOT")) define("APPROOT",$_SERVER['DOCUMENT_ROOT']."lmce-admin/");
    $file = APPROOT.'languages/'.$GLOBALS['fallbacklang'].'/'.$langFile;
    $evalt = "require_once '$file';";
    eval($evalt);

	// include user language file to overwrite existing variables from fallback language
    $file = APPROOT.'languages/'.$GLOBALS['userlang'].'/'.$langFile;
    if( file_exists($file))
    {
    	$evalt = "require_once '$file';";
    	eval($evalt);
    }

    // Create array entry in $GLOBALS for not yet existing variables
    $function_variable_names = array("function_variable_names" => 0, "before_eval_vars" => 0, "created" => 0);
    $created = array_diff_key(get_defined_vars(), $GLOBALS, $function_variable_names, $before_eval_vars);
    foreach ($created as $created_name => $created_value)
    	$GLOBALS[$created_name]=$created_value;
}

function translate($CONST)
{
	return $GLOBALS[$CONST];
}

//---------------------------------------------------------------------------------------
//customized data format
//---------------------------------------------------------------------------------------
function formatDate($strData)
{
  $result = trim( $strData);
  $result = str_replace( "  ", " ", $result );
  //list( $aMonth, $aDay, $aYear, $theRest ) = explode( " ", $result, 4 );
  $result = date( "j M, Y",strtotime($result));
  return $result;
}
function formatTime($strData)
{
  $result = trim( $strData);
  $result = str_replace( "  ", " ", $result );
  //list( $aMonth, $aDay, $aYear, $theRest ) = explode(" ", $result, 4);
  $result = date( "G:i:s A", strtotime($result));
  return $result;
}
function formatNumber($nr) {return number_format($nr,0,',','.');}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//listing functions
//---------------------------------------------------------------------------------------
function getPagesMenu($query,$start,$items,$orderBy,$orderType,$url)
{
 $pages_menu = '';
 $urlStr = $GLOBALS['vRoot'].$url;
 $_SESSION['itemsPerPage'] = $items;

 if(($orderBy!='')&&(($orderType!=''))) {
   $_SESSION['sortOrder'] = $orderBy;
   $_SESSION['sortType']  = $orderType;
 }
 
 $res = @mysql_query($query);
 $rows = @(int)mysql_num_rows($res);
 if( ($rows>0) && ($rows>$items) ){
   
     $pages = $rows/$items;
     
     $prev = $start - $items;
     $crt_page = $start / $items;
     $first_page = $crt_page - $GLOBALS["prevNo"] + 1;
     $last_page  = $crt_page + $GLOBALS["nextNo"];
     
     //initialize if necessary
     if ($prev < 0) {$prev = 0;}
     if ($first_page < 1) {$first_page= 1;}
     if ($last_page > $pages) {$last_page = $pages;}
     
     if ($start!=0) {$pages_menu .= '<a class="smallb" href="'.$urlStr.'start='.$prev.'&orderBy='.$orderBy.'&orderType='.$orderType.'">&#171;</a>';}
     for ($i=$first_page; $i<=(int)$last_page; $i++)
     {
      if (($i-1)*$items==$start) {//crt page
       $crt_pos = $i;
       $pages_menu.= ' [<a class="smallnote" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'"><b>'.$i.'</b></a>]';
      } 
      else {$pages_menu.= ' [<a class="small" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'">'.$i.'</a>]';}
     }
     //last items page
     if (($items*($i-1)) != $rows)
      {
       if (($i-1)*$items==$start){
          $pages_menu.= ' [<a class="smallnote" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'"><b>'.$i.'</b></a>]';
       }else{
          $pages_menu.= ' [<a class="small" href="'.$urlStr.'start='.$items*($i-1).'&orderBy='.$orderBy.'&orderType='.$orderType.'">'.$i.'</a>]';
       }
      }
     
    if (((@$crt_pos * $items) != $rows) && ($start!=($items*($i-1)))) {
      $pages_menu.= ' <a class="smallb" href="'.$urlStr.'start='.$items*(@$crt_pos).'&orderBy='.$orderBy.'&orderType='.$orderType.'">&#187;</a>';}
      
    $pages_menu = '<table cellpadding="3" cellspacing="0" border="0"><tr><td class="small">
                  '.$pages_menu.'</td></tr></table>';
 } 
 
 return $pages_menu;
}

//---------------------------------------------------------------------------------------

function die_admin($output,$message,$sourceScript,$query) {
$out=$message."<br />"."query:".$query;
$output->set_title(ApplicationName."::Error");
$output->set_body($out);
$output->do_output();
die();
}

function die_mesg_public($output,$message,$sourceScript,$query) {
	
	
	if ($GLOBALS["inDebug"]==1) {
		$queryTemp=explode("::",$query);
		if (count($queryTemp)==2) {
				$out="<h2>".$message."</h2><h3>Source:$sourceScript</h3><h4>SQL Error:".$queryTemp[0]."</h4><h4>".$queryTemp[1]."</h4>";
		}else{
				$out="<h2>".$message."</h2><h3>Source: $sourceScript</h3><h4>Error:".$query."</h4>";
		}		
		$output->setBody($out);
		$output->output();
	} else {
		$queryTemp=explode("::",$query);
		if (count($queryTemp)==2) {
				$mailBody=$message."\n\n Source:$sourceScript \n\n SQL Error:".$queryTemp[0]." \n\n ".$queryTemp[1]."";
		}else{
				$mailBody=$message." \n\n Source: $sourceScript \n\n ".$query;
		}	
		if (strlen($GLOBALS['sendErrorsTo'])>2) {
			mail($GLOBALS['sendErrorsTo'],"Error on site:".$GLOBALS['vRoot'],$mailBody);
		}
		$out="An error occured and an email about it was sent to the webadmin.<br />Sorry for the incovenience!";
		$output->setBody($out);
		$output->output();
	}
//$output->do_output();
die();
}


function getPagesNextPrev($pagesNumber,$currentPage) {
$output = "";
        if ($pagesNumber > 1) {
            $output.= "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"> ";
            $output.= "<tr>";
            $output.= "<td class=\"small\" width=\"25%\" nowrap>";
            if ($currentPage-5>1) {
             $startShowPrevNext  = $currentPage - 5;
            } else {
             $startShowPrevNext  = 1;
            }
            if ($currentPage+5<=$pagesNumber) {
             $endShowPrevNext = $currentPage+5;
            } else {
             $endShowPrevNext = $pagesNumber;
            }

            for ($jk=$startShowPrevNext;$jk<=$endShowPrevNext;$jk++) {
                if ($currentPage==$jk) {
                    $output.="[<b>" + $jk + "</b>]";
                } else {
                    $output .= "[<a class=\"small\" href=\"javascript:changePage(".$jk.");\">".$jk."</a>]";
                }
                $output .= "&nbsp;";
            }
            $output .= "</td>";
            $output .= "<td width=\"19%\" class=\"small\" align=\"right\" nowrap>Go to page:
                    <input class=\"small\" type=\"text\" name=\"gotoPageNo\" maxlength=\"3\" size=\"2\">&nbsp;<input type=\"button\" class=\"small\" value=\"Go\" onClick=\"javascript:formSubmit(this.form);\" id=button1 name=button1>
                    </td>
                    <td class=\"small\" align=\"right\" width=\"1%\"><img src=\"images/spacer.gif\" width=\"29\" height=\"8\"></td>
                    </tr>
                    </table>";
        } else {
            $output = "&nbsp;";
        }
        return $output;
} 

function cleanString($str,$len=10000) {
	//magic quotes is always off now; this is deprecated
	return $str;
}

function cleanStringWithTags2Show($string,$len=100000) {
	return trim($string);
}

function cleanStringWithTags4DB($string,$len=100000) {
	return mysql_escape_string(trim($string));
}

function cleanInteger($number) {
	return (int)$number;
}

function cleanFloat($number) {
	return (float)$number;
}

function cleanArray($array2Parse) {
	$resultArray = array();
	foreach ($array2Parse as $elem) {
		if (cleanString((trim($elem)))!="") {
			$resultArray[]=$elem;			
		}
	}
	return $resultArray;
}



function getChildsNo($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = ?";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	$childsNo=0;
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$childsNo++;
				$childsNo+=getChildsNo($row['PK_Device'],$dbADO);
		}
	}
	return $childsNo;
}

$GLOBALS['childsArray'] = array();

function getDeviceChildsArray($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = ?";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsArray'][]=$row['PK_Device'];
				getDeviceChildsArray($row['PK_Device'],$dbADO);
		}
	}
}


$GLOBALS['childsDeviceCategoryNo']=0;
function getDeviceCategoryChildsNo($parentID,$dbADO) {
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = ?";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsDeviceCategoryNo']++;
				getDeviceCategoryChildsNo($row['PK_DeviceCategory'],$dbADO);
		}
	}	
}

$GLOBALS['childsDeviceCategoryArray'] = array();

function getDeviceCategoryChildsArray($parentID,$dbADO) {
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = ?";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$GLOBALS['childsDeviceCategoryArray'][]=$row['PK_DeviceCategory'];				
				getDeviceCategoryChildsArray($row['PK_DeviceCategory'],$dbADO);
		}
	}
}

function getDeviceCategoryChildsOptions($parentID,$parentName,$selectedValue,$notIn,$dbADO) {
	$whereNotIn = '';
	if (strlen($notIn)!='') {
		$whereNotIn=' and PK_DeviceCategory not in ('.$notIn.')';
	}
	$queryGP = "select * from DeviceCategory where FK_DeviceCategory_Parent = ? $whereNotIn order by Description Asc";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	$options='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
			@$GLOBALS['categoriesArray'][$row['PK_DeviceCategory']]=$parentName.' - '.$row['Description'];
			$options.= '<option '.($selectedValue==$row['PK_DeviceCategory']?' selected="selected" ':'').' value="'.$row['PK_DeviceCategory'].'">'.$parentName.' - '.$row['Description'].' #'.$row['PK_DeviceCategory'].'</option>';
			$options.= getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$parentName.' - '.$row['Description'],$selectedValue,$notIn,$dbADO);
		}
		$resGP->Close();
	}
	return $options;
}


function getEventCategoryChildsArray($parentID,$parentName,$selectedValue,$dbADO) {
	$queryGP = "select PK_EventCategory,Description from EventCategory where  FK_EventCategory_Parent = ? order by Description Asc";
	$resGP = $dbADO->Execute($queryGP, array($parentID));
	$EventCategoryOptions = '';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {				
				$EventCategoryOptions.= '<option '.($selectedValue==$row['PK_EventCategory']?' selected="selected" ':'').' value="'.$row['PK_EventCategory'].'">'.$parentName.' - '.$row['Description'].'</option>';
				$EventCategoryOptions.= getEventCategoryChildsArray($row['PK_EventCategory'],$parentName.' - '.$row['Description'],$selectedValue,$dbADO);
		}
	}	
	return $EventCategoryOptions;
}

/*
check if the user has permission for this field
*/

function checkOwner($userIDField,$pkField,$pkValue,$table,$dbADO,$fkField='',$fkValue='') {
	$currentUser = (int)@$_SESSION['userID'];
	
	if ($currentUser!=0) {
		if (trim($fkField)!='') {
			$querySelOwner = 'SELECT '.$userIDField.' FROM '. $table .' WHERE '.$userIDField.'= '.$currentUser." AND $pkField='$pkValue' AND $fkField='$fkValue'";
		} else {
			$querySelOwner = 'SELECT '.$userIDField.' FROM '. $table .' WHERE '.$userIDField.'= '.$currentUser." AND $pkField='$pkValue'";
		}
		
		$resSelOwner = $dbADO->_Execute($querySelOwner);

		if ($resSelOwner && $resSelOwner->RecordCount()==1) {
			return true;
		}
	} else {
		return false;
	}
	
	return false;
}

function getUserCanModifyInstallation($userID,$installationID,$dbADO) {
	$query = "SELECT userCanModifyInstallation from Installation_Users where FK_Installation = ? and FK_Users = ?";
	$res=$dbADO->Execute($query,array($installationID,$userID));
	 if ($res) {
	 	$row = $res->FetchRow();
	 	if ($row['userCanModifyInstallation']==1) {
	 		return true;
	 	}
	 }
	 return false;
}

function getValidOrbitersArray($installationID,$dbADO) {
		$orbiterID = $GLOBALS['rootOrbiterID'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($orbiterID,$dbADO);
		$validDeviceCategoryOrbiters=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryOrbiters[]=$orbiterID; //add orbiter root id to array
		
		$getOrbitersFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryOrbiters).')';
		$resOrbitersFromMasterDeviceList = $dbADO->_Execute($getOrbitersFromMasterDeviceList);
		$arrayOrbitersFromMasterDeviceList = array();
		 if ($resOrbitersFromMasterDeviceList) {
		 	while ($rowOrbitersFromMasterDeviceList = $resOrbitersFromMasterDeviceList->FetchRow()) {
		 		$arrayOrbitersFromMasterDeviceList[]=$rowOrbitersFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayOrbitersFromMasterDeviceList=cleanArray($arrayOrbitersFromMasterDeviceList);
		$queryOrbitersFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayOrbitersFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resOrbitersFromDevices = $dbADO->_Execute($queryOrbitersFromDevices);
		
		$orbitersFromDevicesArray = array();
		if ($resOrbitersFromDevices) {
			while ($rowOrbitersFromDevices = $resOrbitersFromDevices->FetchRow()) {
				$orbitersFromDevicesArray[] = $rowOrbitersFromDevices['PK_Device'];
			}
		}
		
		return $orbitersFromDevicesArray;
}

function getValidComputersArray($installationID,$dbADO) {
		$computerID = $GLOBALS['rootComputerID'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($computerID,$dbADO);
		$validDeviceCategoryComputers=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryComputers[]=$computerID; //add Computer root id to array
		
		$getComputersFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryComputers).')';
		$resComputersFromMasterDeviceList = $dbADO->_Execute($getComputersFromMasterDeviceList);
		$arrayComputersFromMasterDeviceList = array();
		 if ($resComputersFromMasterDeviceList) {
		 	while ($rowComputersFromMasterDeviceList = $resComputersFromMasterDeviceList->FetchRow()) {
		 		$arrayComputersFromMasterDeviceList[]=$rowComputersFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayComputersFromMasterDeviceList=cleanArray($arrayComputersFromMasterDeviceList);
		$queryComputersFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayComputersFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resComputersFromDevices = $dbADO->_Execute($queryComputersFromDevices);
		
		$ComputersFromDevicesArray = array();
		if ($resComputersFromDevices) {
			while ($rowComputersFromDevices = $resComputersFromDevices->FetchRow()) {
				$ComputersFromDevicesArray[] = $rowComputersFromDevices['PK_Device'];
			}
		}
		
		return $ComputersFromDevicesArray;
}

function getValidLightingObjectsArray($installationID,$dbADO) {
		$lightingID = $GLOBALS['rootLights'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($lightingID,$dbADO);
		$validDeviceCategoryLighting=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryLighting[]=$lightingID; //add lighting root id to array
		
		$getLightingsFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryLighting).')';
		$resLightingsFromMasterDeviceList = $dbADO->_Execute($getLightingsFromMasterDeviceList);
		$arrayLightingsFromMasterDeviceList = array();
		 if ($resLightingsFromMasterDeviceList) {
		 	while ($rowLightingsFromMasterDeviceList = $resLightingsFromMasterDeviceList->FetchRow()) {
		 		$arrayLightingsFromMasterDeviceList[]=$rowLightingsFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayLightingsFromMasterDeviceList=cleanArray($arrayLightingsFromMasterDeviceList);
		$queryLightingsFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayLightingsFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resLightingsFromDevices = $dbADO->_Execute($queryLightingsFromDevices);
		
		$lightingsFromDevicesArray = array();
		if ($resLightingsFromDevices) {
			while ($rowLightingsFromDevices = $resLightingsFromDevices->FetchRow()) {
				$lightingsFromDevicesArray[] = $rowLightingsFromDevices['PK_Device'];
			}
		}
		
		return $lightingsFromDevicesArray;
}

function getValidSecurityObjectsArray($installationID,$dbADO) {
		$securityID = $GLOBALS['rootSecurity'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($securityID,$dbADO);
		$validDeviceCategorySecurity=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategorySecurity[]=$securityID; //add security root id to array
		
		$getSecuritysFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategorySecurity).')';
		$resSecuritysFromMasterDeviceList = $dbADO->_Execute($getSecuritysFromMasterDeviceList);
		$arraySecuritysFromMasterDeviceList = array();
		 if ($resSecuritysFromMasterDeviceList) {
		 	while ($rowSecuritysFromMasterDeviceList = $resSecuritysFromMasterDeviceList->FetchRow()) {
		 		$arraySecuritysFromMasterDeviceList[]=$rowSecuritysFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arraySecuritysFromMasterDeviceList=cleanArray($arraySecuritysFromMasterDeviceList);
		$querySecuritysFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arraySecuritysFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resSecuritysFromDevices = $dbADO->_Execute($querySecuritysFromDevices);
		
		$securitysFromDevicesArray = array();
		if ($resSecuritysFromDevices) {
			while ($rowSecuritysFromDevices = $resSecuritysFromDevices->FetchRow()) {
				$securitysFromDevicesArray[] = $rowSecuritysFromDevices['PK_Device'];
			}
		}
		
		return $securitysFromDevicesArray;
}

function getValidClimateObjectsArray($installationID,$dbADO) {
		$climateID = $GLOBALS['rootClimate'];
		$GLOBALS['childsDeviceCategoryArray'] = array();
		getDeviceCategoryChildsArray($climateID,$dbADO);
		$validDeviceCategoryClimate=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$validDeviceCategoryClimate[]=$climateID; //add climate root id to array
		
		$getClimatesFromMasterDeviceList = 'SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory IN ('.join(",",$validDeviceCategoryClimate).')';
		$resClimatesFromMasterDeviceList = $dbADO->_Execute($getClimatesFromMasterDeviceList);
		$arrayClimatesFromMasterDeviceList = array();
		 if ($resClimatesFromMasterDeviceList) {
		 	while ($rowClimatesFromMasterDeviceList = $resClimatesFromMasterDeviceList->FetchRow()) {
		 		$arrayClimatesFromMasterDeviceList[]=$rowClimatesFromMasterDeviceList['PK_DeviceTemplate'];
		 	}
		 }
		
		 
		$arrayClimatesFromMasterDeviceList=cleanArray($arrayClimatesFromMasterDeviceList);
		$queryClimatesFromDevices = 'SELECT PK_Device FROM Device WHERE FK_DeviceTemplate in ('.join(",",$arrayClimatesFromMasterDeviceList).') and FK_Installation = '.$installationID;
		$resClimatesFromDevices = $dbADO->_Execute($queryClimatesFromDevices);
		
		$climatesFromDevicesArray = array();
		if ($resClimatesFromDevices) {
			while ($rowClimatesFromDevices = $resClimatesFromDevices->FetchRow()) {
				$climatesFromDevicesArray[] = $rowClimatesFromDevices['PK_Device'];
			}
		}
		
		return $climatesFromDevicesArray;
}


$GLOBALS['childsFromAnyCategory']=array();
function getChildsForCategory($tableName,$columnPK,$columnFK,$parentID,$dbADO) {
	
	$selectChildsFromCategories = "SELECT Description,$columnPK FROM $tableName WHERE $columnFK = {$parentID} ORDER BY Description ASC";
		$rs2 = $dbADO->_Execute($selectChildsFromCategories);
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$GLOBALS['childsFromAnyCategory'][] = $row2[$columnPK];
				$jsTree.= '
					auxS'.$row2[$columnPK].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].'", "javascript:manageCategories.categSelected.value='.$row2[$columnPK].';manageCategories.actionX.value=null;manageCategories.submit();"))
					auxS'.$row2[$columnPK].'.xID = '.$row2[$columnPK].';
				';
				$jsTree.=getChildsForCategory($tableName,$columnPK,$columnFK,$row2[$columnPK],$dbADO);
			}

	return $jsTree;
}

function calculatePlutoID($PK_PlutoId)
{
	$str = PRV_plutoid_base35_char($mod = $PK_PlutoId%35);
	while ($PK_PlutoId = ($PK_PlutoId-$mod)/35)
	{
		$str .= PRV_plutoid_base35_char($mod = $PK_PlutoId%35);
	}
	return strrev(str_pad($str,6,'0'));
}

function PRV_plutoid_base35_char($int)
{
	if ($int < 10) return $int;
	if ($int < 24) return chr($int+55); // A-N: 65-78
	return chr($int+56); // P-Z: 80-90
}

function calculatePIN()
{
	$pin='';
	for ($i=0; $i<3; $i++)
	$pin.= PRV_plutoid_base35_char(rand(0,34));
	return $pin;
}

function getTopMenu($website,$dbADO,$package=0) {
		$selectMenu = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website=?";
		$resSelectMenu = $dbADO->Execute($selectMenu, array($website));
		$menuPages='';	
/*		$menuPages='
				AddMenu("1"  ,  "1"   ,  "Wizard"       ,  ""  ,  ""  , "");
			';
*/		
		while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
			
			$menuPages.='
				AddMenu("'.$rowSelectMenu['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectMenu['Description'].'"       ,  ""  ,  ""  , "'.$rowSelectMenu['pageURL'].'");
			';
			//get childs (ne-recursive method)
			$selectSubMenu1 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectSubMenu1 = $dbADO->Execute($selectSubMenu1);
			while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {
				//get level 2 childs (ne-recursive method)
					$selectSubMenu2 = "SELECT * FROM PageSetup WHERE FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";
					$resSelectSubMenu2 = $dbADO->Execute($selectSubMenu2);
					
				$menuPages.='
					AddMenu("'.$rowSelectSubMenu1['PK_PageSetup'].'"  ,  "'.$rowSelectMenu['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectSubMenu1['Description'].($resSelectSubMenu2->RecordCount()>0?" &gt;&gt;":"").'"       ,  ""  ,  ""  , "'.$rowSelectSubMenu1['pageURL'].'");
				';
				
					
					while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
						$menuPages.='
							AddMenu("'.$rowSelectSubMenu2['PK_PageSetup'].'"  ,  "'.$rowSelectSubMenu1['PK_PageSetup'].'"   ,  "&nbsp;&nbsp;'.$rowSelectSubMenu2['Description'].'"       ,  ""  ,  ""  , "'.$rowSelectSubMenu2['pageURL'].'");
						';
					}
					$resSelectSubMenu2->Close();		
			}
			$resSelectSubMenu1->Close();					
		}
		$resSelectMenu->Close();
return $menuPages;
}

function InheritDeviceData($masterDeviceID,$insertID,$dbADO)
{
	$getDeviceTemplateDeviceData='
		INSERT INTO Device_DeviceData
			(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT 
			PK_Device, DeviceTemplate_DeviceData.FK_DeviceData, DeviceTemplate_DeviceData.IK_DeviceData
		FROM Device
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceTemplate_DeviceData on DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceTemplate_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getDeviceTemplateDeviceData,array($insertID));

	$getDeviceCategoryDeviceData='
	INSERT INTO Device_DeviceData
		(FK_Device, FK_DeviceData, IK_DeviceData)
	SELECT 
		PK_Device, DeviceCategory_DeviceData.FK_DeviceData, DeviceCategory_DeviceData.IK_DeviceData
	FROM Device
		INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		INNER JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceTemplate.FK_DeviceCategory
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
	WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getDeviceCategoryDeviceData,array($insertID));

	$getParentCategoryDeviceData='
		INSERT INTO Device_DeviceData
			(FK_Device, FK_DeviceData, IK_DeviceData)
		SELECT 
			PK_Device, DeviceCategory_DeviceData.FK_DeviceData, DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
			LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=?';
	$dbADO->Execute($getParentCategoryDeviceData,array($insertID));
}


function createChildsForControledViaDeviceTemplate($masterDeviceID,$installationID,$insertID,$dbADO)
{
	// check if DeviceTemplate controll anything
	$queryDeviceTemplate_DeviceTemplate_ControlledVia='SELECT * FROM DeviceTemplate_DeviceTemplate_ControlledVia
				WHERE FK_DeviceTemplate_ControlledVia = ? AND AutoCreateChildren = 1';
	$resDeviceTemplate_DeviceTemplate_ControlledVia=$dbADO->Execute($queryDeviceTemplate_DeviceTemplate_ControlledVia,array($masterDeviceID));
	if($resDeviceTemplate_DeviceTemplate_ControlledVia->RecordCount()>0){
		// insert the children
		while($row=$resDeviceTemplate_DeviceTemplate_ControlledVia->FetchRow()){
			$queryDeviceTemplate='SELECT Description FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
			$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,array($row['FK_DeviceTemplate']));
			$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
			$queryInsertDevice = "INSERT INTO Device(FK_Installation, Description, FK_Device_ControlledVia, FK_DeviceTemplate)
										values(?,?,?,?)";
			$dbADO->Execute($queryInsertDevice,array($installationID,$rowDeviceTemplate['Description'],$insertID,$row['FK_DeviceTemplate']));
			$insertChildID = $dbADO->Insert_ID();
			InheritDeviceData($row['FK_DeviceTemplate'],$insertChildID,$dbADO);
			setDCERouterNeedConfigure($installationID,$dbADO);
		}
	}
}

function createChildsForControledViaDeviceCategory($masterDeviceID,$installationID,$insertID,$dbADO)
{
	// get Device Template Category
	$queryDeviceTemplate='SELECT FK_DeviceCategory FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,array($masterDeviceID));
	$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
	$FK_DeviceCategory=$rowDeviceTemplate['FK_DeviceCategory'];

	// query DeviceTemplate_DeviceCategory_ControlledVia if his category controll anything
	$queryDeviceTemplate_DeviceCategory_ControlledVia='SELECT * FROM DeviceTemplate_DeviceCategory_ControlledVia
				WHERE FK_DeviceCategory = ? AND AutoCreateChildren = 1';
	$res=$dbADO->Execute($queryDeviceTemplate_DeviceCategory_ControlledVia,array($FK_DeviceCategory));
	if($res->RecordCount()>0){
		
		while($row=$res->FetchRow()){
			$queryDeviceTemplate='SELECT Description FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
			$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate,array($row['FK_DeviceTemplate']));
			$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
			$queryInsertDevice = "INSERT INTO Device(FK_Installation, Description, FK_Device_ControlledVia, FK_DeviceTemplate)
										values(?,?,?,?)";
			$dbADO->Execute($queryInsertDevice,array($installationID,$rowDeviceTemplate['Description'],$insertID,$row['FK_DeviceTemplate']));
			$insertChildID = $dbADO->Insert_ID();
			InheritDeviceData($row['FK_DeviceTemplate'],$insertChildID,$dbADO);
			setDCERouterNeedConfigure($installationID,$dbADO);
		}
	}
}

$GLOBALS['DeviceIDControlledVia']=array();
$GLOBALS['DeviceControlledVia']=array();
function GetDeviceControlledVia($deviceID, $dbADO)
{
	$whereClause='';
	if ($deviceID!=0) {
		$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
		$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$_SESSION['installationID']));
		if ($resSelectMasterDeviceForParent) {
			$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
			$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];

			$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
			$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($DeviceTemplateForParent));

			$GLOBALS['childsDeviceCategoryArray'] = array();
			if ($resSelectControlledViaCategory) {
				while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
					$GLOBALS['childsDeviceCategoryArray'][]=$rowSelectControlledVia['FK_DeviceCategory'];
					getDeviceCategoryChildsArray($rowSelectControlledVia['FK_DeviceCategory'],$dbADO);
				}
			}
			$controlledViaCategoryArray = cleanArray($GLOBALS['childsDeviceCategoryArray']);

			$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
			$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

			$childsDeviceTemplateArray = array();
			if ($resSelectControlledViaDeviceTemplate) {
				while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
					$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];
				}
			}



			if (count($controlledViaCategoryArray)>0) {
				$whereClause.=' and (FK_DeviceCategory in ('.join(",",$controlledViaCategoryArray).')';
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
				$whereClause.=')';
			} else {
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
			}

		}
	}

	$queryDeviceTemplate = "SELECT DISTINCT Device.Description,Device.PK_Device
											FROM
											Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
											WHERE 1=1 $whereClause AND FK_Installation='".$_SESSION['installationID']."' order by Device.Description asc";
	$resDeviceTemplate = $dbADO->_Execute($queryDeviceTemplate);
	$DeviceTemplateTxt = '';
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$GLOBALS['DeviceIDControlledVia'][]=$row['PK_Device'];
			$GLOBALS['DeviceControlledVia'][]=$row['Description'];
		}
	}
}

function isMediaDirector($deviceID,$dbADO,$mdOnly=0)
{
	if($deviceID!=0){
		$resCategory=$dbADO->Execute('
			SELECT FK_Device_ControlledVia,FK_DeviceCategory
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_Device=?',array($deviceID));

	        if($resCategory->RecordCount()>0){
			$row=$resCategory->FetchRow();
			$DeviceCategory=$row['FK_DeviceCategory'];
		}else
			return false;

		if($DeviceCategory==$GLOBALS['rootMediaDirectors'] ){
			return true;
		}
	}
	elseif($mdOnly==1){
		return false;
	}elseif($row['FK_Device_ControlledVia']!='' )
		return isMediaDirector($row['FK_Device_ControlledVia'],$dbADO);
	else{
		return false;
	}
}

function isCore($deviceID,$dbADO)
{
	$getTemplate='
		SELECT FK_DeviceCategory,FK_Device_ControlledVia 
		FROM Device 
		INNEr JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device=?';
	$resTemplate=$dbADO->Execute($getTemplate,array($deviceID));
	if($resTemplate->RecordCount()>0){
		$row=$resTemplate->FetchRow();
		$DeviceCategory=$row['FK_DeviceCategory'];
	}else
		return false;
	if($DeviceCategory==$GLOBALS['CategoryCore']){
		return true;
	}
	return false;
}

function isOrbiter($deviceID,$dbADO)
{
	$orbitersArray=array();
	$orbitersArray=getValidOrbitersArray($_SESSION['installationID'],$dbADO);
	if(in_array($deviceID,$orbitersArray))
		return true;
	return false;
}


function deleteCommandGroup($PK_CommandGroup,$dbADO)
{
	$deleteParameters='
		DELETE CommandGroup_Command_CommandParameter
		FROM CommandGroup_Command_CommandParameter
		JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteParameters,array($PK_CommandGroup));
	
	$deleteCommandGroup_Command='
		DELETE FROM CommandGroup_Command
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_Command,array($PK_CommandGroup));

	$deleteCommandGroup_EntArea='
		DELETE FROM CommandGroup_EntertainArea
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_EntArea,array($PK_CommandGroup));

	$deleteCommandGroup_Room='
		DELETE FROM CommandGroup_Room
		WHERE FK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup_Room,array($PK_CommandGroup));
	
	$deleteCommandGroup='
		DELETE FROM CommandGroup 
		WHERE PK_CommandGroup=?';
	$dbADO->Execute($deleteCommandGroup,array($PK_CommandGroup));
}

function grabDirectory ($path, $depth) {
	if (($d = @opendir ($path)) === false) 
		return;
	else {
		while ($f = readdir ($d)) {
			if ($f != "." && $f != "..") {
				if (@is_dir ($path . "/" . $f)) {
					$GLOBALS['physicalPathsArray'][]=$path . "/" . $f;
					$depth++;
					grabDirectory ($path . "/" . $f, $depth);
					$depth--;
				} 
			}
		}
		closedir ($d);
	}
}


function grabFiles($path,$fileParm='-type f',$startingWith='') {
	$filesArray=array();
	// required to read files larger than 2G
	$escapedPath=str_replace(array('\\','"','$'),array('\\\\','\"','\$'),$path);
	
	$PathParm=($startingWith!='')?'"'.$escapedPath.'/'.$startingWith.'"*':'"'.$escapedPath.'/"';
	//return all files except special *.id3 files *.keys.tar.gz *.mpg.png *.mpg.png.tnj *.jpg.tnj
	$cmd='sudo -u root find '.$PathParm.' '.$fileParm.' -maxdepth 1 -not -name \'*.id3\' -not -name \'*.keys.tar.gz\' -not -name \'*.mpg.png\' -not -name \'*.mpg.png.tnj\' -not -name \'*.jpg.tnj\' -not -name \'*.JPG.tnj\'';
	//echo $cmd;
	exec($cmd,$retArray);
	//print_array($retArray);
	foreach ($retArray AS $file){
		if($file!=$path && is_dir64($path)!==false){
			$cleanFile=str_replace($path.'/','',$file);
			if($cleanFile!=''){
				$filesArray[]=$cleanFile;
			}
		}
	}
	sort($filesArray,SORT_STRING);
	reset($filesArray);

	return $filesArray;
}

function resizeImage($source, $destination, $new_width, $new_height,$forcedPNG=0){
	$cmd='/usr/bin/convert -sample '.$new_width.'x'.$new_height.' '.$source.' '.$destination;
	exec_batch_command($cmd); 	
}

function resizeImage_old($source, $destination, $new_width, $new_height,$forcedPNG=0)
{
	if(!file_exists($source))
		return 1;	// Source file does not exists

	$arr=getimagesize($source);
	$old_w=$arr[0];
	$old_h=$arr[1];
	if($new_width==0 || $new_height==0){
		$new_width=$old_w;
		$new_height=$old_h;
	}
	if($new_height<0 || $new_width<0)
		return 2;	// invalid width or height
	$aspect_ratio=$new_width/$new_height;
	if($old_w/$old_h==$aspect_ratio)
	{
		$src_x=0;
		$src_y=0;
		$src_w=floor($old_w*$aspect_ratio);
		$src_h=floor($old_h*$aspect_ratio);
	}
	elseif($old_w/$old_h>$aspect_ratio)
	{
		$src_x=floor(($old_w-$old_h*$aspect_ratio)/2);
		$src_y=0;
		$src_w=floor($old_h*$new_width/$new_height);
		$src_h=floor($old_h);
	}
	else
	{
		$src_x=0;
		$src_y=floor(($old_h-$old_w/$aspect_ratio)/2);
		$src_w=floor($old_w);
		$src_h=floor($old_w*$new_height/$new_width);
	}
	$new_w=$new_width;
	$new_h=$new_height;	 

	$tmpArray=@getimagesize($source);
	$destinationType=substr($destination,strrpos($destination,'.')+1);
	
	$dst_img=imagecreatetruecolor($new_w,$new_h);
	$src_img=(@$tmpArray[2]==2)?ImageCreateFromJpeg($source):imagecreatefrompng($source);
	if(!@imagecopyresized($dst_img,$src_img,0,0,$src_x,$src_y,$new_w,$new_h,$src_w,$src_h))
		return 3; // resize error
	if($forcedPNG==1){
		
		if(!imagepng($dst_img, str_replace('.jpg','.png',$destination))){
			return 5; // writing png thumbnail error
		}
		
	}else{
		if(($destinationType=='jpg') && !imagejpeg($dst_img, $destination, 100))
			return 4; // writing jpg thumbnail error
		elseif(@$destinationType=='png' && !@imagepng($dst_img, $destination))
			return 5;	// writing png thumbnail error
	}

	
	return 0;
}

function multi_page($query, $params,$url, $page_no, $art_pagina,$dbADO)
{
	$res=$dbADO->Execute($query,$params);
	$total=$res->RecordCount();
	$max_pages = $total/$art_pagina;
	if( $max_pages - floor($max_pages) >= 0 )
		$max_pages = floor($max_pages) + 1;
	else
		$max_pages = floor( $max_pages );

	if($total!=0){
		$output= '<div align="center">';
		for($i=0;$i<$max_pages;$i++){
			$output.=($page_no==$i)?'<font color="red">'.($i+1).'</font> ':'<a href='.$url.'&page_no='.($i+1).'>[ '.($i+1).' ]</a> ';
		}
		$output.='</div>';
	}
	if($total==0){
		$output=translate('TEXT_NO_RECORDS_CONST');
	}
	else{
		$output.='<table>
					<tr class="tablehead">
						<td align="center"><B>'.translate('TEXT_FILES_CONST').'</B></td>
						<td align="center">&nbsp;</td>
					</tr>';
		$art_index=0;
		$start = $page_no * $art_pagina;
		$res=$dbADO->Execute($query." LIMIT  $start,$art_pagina",$params);
	    while($row = $res->FetchRow()){
			$art_index++;
			if($art_index-1 == $art_pagina)
		        break;
			$output.=multi_page_format($row, $art_index+$start,$dbADO);
		}
	}
	$output.=translate('TEXT_FOUND_CONST').': '.$total.'. ';
	if($total!=0)
		$output.= translate('TEXT_PAGE_CONST').' '.($page_no+1).'l '.translate('TEXT_TOTAL_PAGES_CONST').': '.$max_pages.'.<br>';
	return $output;
}
function multi_page_format($row, $art_index,$mediadbADO)
{
	$queryOtherAttributes='
		SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name
		FROM File_Attribute
			INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
			INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
		WHERE FK_File=? AND FK_AttributeType!=? ORDER BY AttributeType.Description ASC
		';

	$otherAttributes='';
	$resOtherAttributes=$mediadbADO->Execute($queryOtherAttributes,array($row['FK_File'],$GLOBALS['attrType']));
	while($rowOtherAttributes=$resOtherAttributes->FetchRow()){
		$otherAttributes.='<b>'.$rowOtherAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowOtherAttributes['PK_Attribute'].'&action=properties" target="_self">'.$rowOtherAttributes['Name'].'</a> ';
	}

	$out='
		<tr style="background-color:'.(($art_index%2==0)?'#EEEEEE':'#EBEFF9').';">
			<td title="'.$row['Path'].'" align="left">'.(($row['Missing']!=0)?'<img src="include/images/missing.gif" align="top"> ':'').'<b>'.translate('TEXT_FILENAME_CONST').':</b> <a href="index.php?section=editMediaFile&fileID='.$row['PK_File'].'">'.$row['Filename'].'</a><br><B>'.translate('TEXT_PATH_CONST').':</B> <a href="index.php?section=mainMediaFilesSync&path='.urlencode($row['Path']).'&filename='.urlencode($row['Filename']).'">'.$row['Path'].'</a></td>
			<td rowspan="2"><a href="#" onClick="if(confirm(\''.translate('TEXT_DELETE_MEDIA_FILES_CONFIRMATION_CONST').'\'))self.location=\'index.php?section=mainMediaBrowser&attributeID='.$GLOBALS['attributeID'].'&action=properties&fileID='.$row['PK_File'].'\';">'.translate('TEXT_DELETE_CONST').'</a></td>
		</tr>
		<tr style="background-color:'.(($art_index%2==0)?'#EEEEEE':'#EBEFF9').';">
			<td align="left">'.$otherAttributes.'</td>
		</tr>
		';
	return $out;
}

// return the devices in selected category and its childs
function getDevicesArrayFromCategory($categoryID,$dbADO,$extra='')
{
	$categories=getDescendantsForCategory($categoryID,$dbADO);
	
	$label=(@$GLOBALS['DT_&_Room']==1)?'IF(Device.FK_Room IS NOT NULL,CONCAT(Device.Description,\' (\',DeviceTemplate.Description,\') \',Room.Description),CONCAT(Device.Description,\' (\',DeviceTemplate.Description,\')\')) AS Description':'Device.Description AS Description';
	$label=(@$GLOBALS['Room_Name']==1)?'IF(Device.FK_Room IS NOT NULL,CONCAT(Device.Description,\' (\',Room.Description,\')\'),Device.Description) AS Description':$label;
	$label=(@$GLOBALS['Room_Name']==2)?'IF(Device.FK_Room IS NOT NULL,CONCAT(Room.Description,\' - \',Device.Description),Device.Description) AS Description':$label;
	$devicesList=getAssocArray('Device','PK_Device',$label,$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate	LEFT JOIN Room ON FK_Room=PK_Room WHERE FK_DeviceCategory IN ('.join(',',$categories).') AND Device.FK_Installation='.(int)@$_SESSION['installationID'].$extra,'ORDER BY Description ASC');
	
	return $devicesList;
}

function deleteDevice($PK_Device,$dbADO)
{
	$dbADO->Execute('BEGIN');
	// get auto-generated scenarios using this device
	$res=$dbADO->Execute('
		SELECT PK_CommandGroup 
		FROM CommandGroup_Command_CommandParameter
		INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
		INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup
		WHERE AutoGeneratedDate>0 AND FK_CommandParameter=2 AND IK_CommandParameter=?',array($PK_Device));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		$dbADO->Execute('DELETE FROM CommandGroup WHERE PK_CommandGroup=?',array($row['PK_CommandGroup']));
	}
	
	getDeviceChildsArray($PK_Device,$dbADO);
	
	$toDelete = cleanArray($GLOBALS['childsArray']);
	$toDelete[]=$PK_Device;
	if (!is_array($toDelete)) {
		$toDelete=array();
	}
	foreach ($toDelete as $elem) {
	
		$arrayFKDeviceTables=array('CommandGroup_Command','Device_Command','Device_CommandGroup','Device_DeviceData','Device_DeviceGroup','Device_Device_Related','Device_EntertainArea','Device_HouseMode','Device_Orbiter','Device_StartupScript','Device_Users');
		foreach($arrayFKDeviceTables AS $tablename){	
			$queryDelFromTable='DELETE FROM '.$tablename.' WHERE FK_Device=?';
			$dbADO->Execute($queryDelFromTable, array($elem));
		}		
		$queryDelDevice = 'DELETE FROM Device WHERE PK_Device = ?';
		$dbADO->Execute($queryDelDevice, array($elem));
	}
	//Delete extra Orbiter-related data
	$arrayFKDeviceTables=array('Device_Orbiter','Orbiter_Users_PasswordReq','Orbiter_Variable', 'CachedScreens', 'RemoteControl', 'Room_Users');
	foreach($arrayFKDeviceTables AS $tablename){
		$queryDelFromTable='DELETE FROM '.$tablename.' WHERE FK_Orbiter='.$PK_Device;
		$dbADO->Execute($queryDelFromTable);
	}
	$queryDelDevice = 'DELETE FROM Orbiter WHERE PK_Orbiter = '.$PK_Device;
	$dbADO->Execute($queryDelDevice);

	writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\tDevice deleted: $PK_Device\n",'a+');
	$dbADO->Execute('COMMIT');
}

function addDeviceToEntertainArea($deviceID,$entArea,$dbADO)
{
	$queryDE='SELECT * FROM Device_EntertainArea WHERE FK_Device=? AND FK_EntertainArea=?';
	$resDE=$dbADO->Execute($queryDE,array($deviceID,$entArea));
	if($resDE->RecordCount()==0){
		$insertDeviceEntertainArea='INSERT INTO Device_EntertainArea (FK_Device, FK_EntertainArea) VALUES (?,?)';
		$dbADO->Execute($insertDeviceEntertainArea,array($deviceID,$entArea));
	}
					
	if(isOrbiter($deviceID,$dbADO) || isMediaDirector($deviceID,$dbADO)){
		$queryChilds='SELECT * FROM Device WHERE FK_Device_ControlledVia=? AND FK_Installation=?';
		$resChilds=$dbADO->Execute($queryChilds,array($deviceID,$_SESSION['installationID']));
		if($resChilds->RecordCount()>0){
			while($rowChilds=$resChilds->FetchRow()){
				addDeviceToEntertainArea($rowChilds['PK_Device'],$entArea,$dbADO);
			}
		}
	}
}

function getChilds($parentID,$dbADO) {
	
	$selectDevicesFromCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent = ? ORDER BY Description";
		$rs2 = $dbADO->_Execute($selectDevicesFromCategories, array($parentID));
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$jsTree.= '
					auxS'.$row2['PK_DeviceCategory'].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].' #'.$row2['PK_DeviceCategory'].'", "javascript:setPicker('.$row2['PK_DeviceCategory'].',\"'.htmlspecialchars($row2['Description'],ENT_QUOTES).'\");"))
					auxS'.$row2['PK_DeviceCategory'].'.xID = '.$row2['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row2['PK_DeviceCategory'],$dbADO);
			}

	return $jsTree;
}


function builtTopMenu($website,$dbADO)
{
	$cachedTopMenu=(getcwd()).'/cached/topMenu';
	$cachedIDs=(getcwd()).'/cached/topMenuIDs';

	// get existing devices to compare them with old ones
	$devices=join(',',array_keys(getAssocArray('Device','PK_Device','PK_Device',$dbADO,'WHERE FK_Installation='.$_SESSION['installationID'],'ORDER BY PK_Device ASC')));


	 // if something changed in Device table, or the cached menu does not exist, rebuild menu
	
	if(file_exists($cachedTopMenu)){
		$oldIDs=file($cachedIDs);
		if($devices==trim(@$oldIDs[1])){
			//echo 'cached '.$cachedTopMenu;
			return join('',file($cachedTopMenu));
		}
	}

	$selectMenu = "
		SELECT DISTINCT PageSetup.* FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_PageSetup_Parent IS NULL AND showInTopMenu = 1 AND Website=? AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND FK_Installation=?))
		ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($website,$_SESSION['installationID']));

	$menuPages='';
	$pos=0;	
	$pagesArray=array();
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='menuObject.item'.$pos.' = "'.$rowSelectMenu['Description'].'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$pos.' = "'.str_replace('\'','\\\\\'',$rowSelectMenu['pageURL']).'"
		';
		$menuPages.=getSubmenu($website,$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
	}
	// write top menu file and devices/page setup IDs
	
	writeFile($cachedTopMenu,$menuPages,'w');
	$IDs="#Devices IDs\n";
	$IDs.=$devices;
	writeFile($cachedIDs,$IDs,'w');

	if(file_exists($cachedTopMenu)){
		writeFile($GLOBALS['ErrorLog'],date('d-m-Y H:i:s').' ERROR: The cached menu cannot be written to '.$cachedTopMenu."\n\n",'a+');
		return join('',file($cachedTopMenu));
	}else{
		return $menuPages;
	}
}

function getSubmenu($website,$level,$parentID,$dbADO)
{
	$selectMenu = "
		SELECT PageSetup.* 
		FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_PageSetup_Parent =? AND showInTopMenu = 1 AND Website=? AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND FK_Installation=?))
		ORDER BY OrderNum";
	$resSelectMenu = $dbADO->Execute($selectMenu,array($parentID,$website,$_SESSION['installationID']));
	$menuPages='';
	$pos=0;	
	if($resSelectMenu->RecordCount()>0){
		$suffix=(substr($level,-1)=='_')?substr($level,0,-1):$level;
		$menuPages.='menuObject.menu_items_text_color_roll'.$suffix.' = "#FFFFFF"
		';
	}
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$menuPages.='
			menuObject.item'.$level.$pos.' = "'.$rowSelectMenu['Description'].'"
		';
		if($rowSelectMenu['pageURL']!='')
			$menuPages.='menuObject.url'.$level.$pos.' = "'.str_replace('\'','\\\\\'',$rowSelectMenu['pageURL']).'"
		';
		$menuPages.=getSubmenu($website,$level.$pos.'_',$rowSelectMenu['PK_PageSetup'],$dbADO);
		$pos++;
		
	}
	return $menuPages;
}

function generatePullDown($name,$tableName,$valueField,$labelField,$selectedValue,$dbADO,$filterTable='',$jsEvents='')
{
	$pullDown='
		<select name="'.$name.'" '.$jsEvents.'>	
			<option value="0">- Please select -</option>';
	$query="SELECT $tableName.$valueField,$tableName.$labelField FROM $tableName $filterTable ORDER BY $labelField ASC";
	$res=$dbADO->Execute($query);
	while($row=$res->FetchRow()){
		$pullDown.='<option value="'.$row[$valueField].'" '.(($row[$valueField]==$selectedValue)?'selected':'').' title="'.$row[$labelField].'">'.$row[$labelField].'</option>';
	}
	$pullDown.='</select>';
	return $pullDown;
}

function getDeviceTemplatesFromCategory($categoryID,$dbADO,$withNames=0)
{
	/*
	$GLOBALS['childsDeviceCategoryArray']=array();
	getDeviceCategoryChildsArray($categoryID,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;
	$categoriesArray=$GLOBALS['childsDeviceCategoryArray'];
	*/
	$categoriesArray=getDescendantsForCategory($categoryID,$dbADO);
	
	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$categoriesArray).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$DTArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		if($withNames==0){
			$DTArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
		}else{
			$DTArray[$rowDeviceCategory['PK_DeviceTemplate']]=$rowDeviceCategory['Description'];
		}
	}
	return $DTArray;
}

// convert a Y-m-d date in a custom format
function formatMySQLDate($mySQLformat,$customFormat)
{
	$dataParts=explode('-',$mySQLformat);
	return date($customFormat,mktime(0,0,0,$dataParts[1],$dataParts[2],$dataParts[0]));
}

function displayCriteria($FK_CriteriaParmNesting,$eventHandlerID,$installationID,$dbADO)
{
	$out='';
	$cplArray=array();
	$resCPL=$dbADO->Execute('SELECT * FROM CriteriaParmList ORDER BY Description ASC');
	while($rowCPL=$resCPL->FetchRow()){
		$cplArray[$rowCPL['PK_CriteriaParmList']]=$rowCPL['Description'];
	}
	
	$resCPN=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?',array($FK_CriteriaParmNesting));
	$rowCPN=$resCPN->FetchRow();
	$out.='
		<table border="0" width="100%">
			<tr bgcolor="#E7E7E7">
				<td><B>(</B></td>
				<td colspan="2"><input type="checkbox" name="nestingNot_'.$FK_CriteriaParmNesting.'" value="1" '.(($rowCPN['IsNot']==1)?'checked':'').' onClick="self.location=\'index.php?section=editCriteria&action=update&ehID='.$eventHandlerID.'&chCPN='.$FK_CriteriaParmNesting.'&setNot='.(($rowCPN['IsNot']==1)?'0':'1').'\'"> '.translate('TEXT_NOT_CONST').' <input type="checkbox" name="nestingAnd_'.$FK_CriteriaParmNesting.'" value="1" '.(($rowCPN['IsAnd']==1)?'checked':'').' onClick="self.location=\'index.php?section=editCriteria&action=update&ehID='.$eventHandlerID.'&chCPN='.$FK_CriteriaParmNesting.'&setAnd='.(($rowCPN['IsAnd']==1)?'0':'1').'\'"> '.translate('TEXT_AND_CONST').' </td>
				<td colspan="3" align="right"><a href="index.php?section=editCriteria&ehID='.$eventHandlerID.'&fkCPN='.$FK_CriteriaParmNesting.'&action=addSegment">'.translate('TEXT_ADD_SEGMENT_CONST').'</a> &nbsp;&nbsp;&nbsp;<a href="index.php?section=editCriteria&ehID='.$eventHandlerID.'&parentCPN='.$FK_CriteriaParmNesting.'&action=addNesting">'.translate('TEXT_ADD_NESTING_CONST').'</a> &nbsp;&nbsp;<a href="#" onClick="if(confirm(\''.translate('TEXT_DELETE_NESTING_CONFIRMATION_CONST').'\'))self.location=\'index.php?section=editCriteria&ehID='.$eventHandlerID.'&delCPN='.$FK_CriteriaParmNesting.'&action=delNesting\'">'.translate('TEXT_DELETE_CONST').'</a></td>
			</tr>';
	$queryCP='
			SELECT CriteriaParm.*, CriteriaParmList.Description AS CPL_Description
			FROM CriteriaParm
			LEFT JOIN CriteriaParmList ON FK_CriteriaParmList=PK_CriteriaParmList
			WHERE FK_CriteriaParmNesting=?';
	$resCP=$dbADO->Execute($queryCP,array($FK_CriteriaParmNesting));
	while($rowCP=$resCP->FetchRow()){
		$GLOBALS['displayedCriteriaParms'][]=$rowCP['PK_CriteriaParm'];
		$out.='
			<tr>
				<td>&nbsp;&nbsp;';
			$out.='
				<select name="CriteriaParmID_'.$rowCP['PK_CriteriaParm'].'" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
					';
				foreach ($cplArray AS $cplID=>$cplDescription){
					$out.='<option value="'.$cplID.'" '.(($rowCP['FK_CriteriaParmList']==$cplID)?'selected':'').'>'.$cplDescription.'</option>';
				}
				$out.='</select></td>
				<td colspan="3">';
			// exception: FK_CriteriaParmList=24 EventParameter
			if($rowCP['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$resEventParameter=$dbADO->Execute('SELECT * FROM EventParameter ORDER BY Description ASC');
					$out.='Event parameter: <select name="CriteriaParmParm_'.$rowCP['PK_CriteriaParm'].'" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
						<option value="0" ></option>';
					while($rowEP=$resEventParameter->FetchRow()){
						if(isset($_POST['CriteriaParmParm_'.$rowCP['PK_CriteriaParm']])){
							$isSelectedParm=($rowEP['PK_EventParameter']==$_POST['CriteriaParmParm_'.$rowCriteria['PK_CriteriaParm']])?'selected':'';
						}
						else{	
							$isSelectedParm=($rowCP['Parm']==$rowEP['PK_EventParameter'])?'selected':'';	// check old value
						}
						$out.='<option value="'.$rowEP['PK_EventParameter'].'" '.$isSelectedParm.'>'.$rowEP['Description'].'</option>';						
						if($isSelectedParm=='selected')				
							$selectedEventParameter=$rowEP['Description'];
					}
					$out.='</select>';
				}
				if ($rowCP['FK_CriteriaParmList']==$GLOBALS['CPL_State']) {
					$queryTable="SELECT * FROM Device WHERE FK_Installation='".$installationID."' ORDER BY Description ASC";
					$resTable2=$dbADO->Execute($queryTable);
					$out.='Device: <select name="CriteriaParmParm_'.$rowCP['PK_CriteriaParm'].'" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
										<option value=""></option>';
					$parmDevice = $rowCP['Parm'];
					$parmData = "";
					if (strpos($rowCP['Parm'], ".") > 0) {
					         $parmDevice = substr($rowCP['Parm'], 0, strpos($rowCP['Parm'],"."));
					   	 $parmData =  substr($rowCP['Parm'], strpos($rowCP['Parm'],".")+1);
					}
					while($rowTable2=$resTable2->FetchRow()) {
						if(isset($parmDevice)) {
							$isSelectedParm=($rowTable2['PK_Device']==$parmDevice)?'selected':'';
						}
						$out.='<option value="'.$rowTable2['PK_Device'].'" '.$isSelectedParm.'>'.$rowTable2['Description'].'</option>';
					}
						
					$out.='			
					</select>';
					if (isset($parmDevice)) {
					   	$queryTable="SELECT d.PK_Device,dt.FK_DeviceCategory FROM Device d INNER JOIN DeviceTemplate dt ON d.FK_DeviceTemplate=dt.PK_DeviceTemplate WHERE d.FK_Installation='".$installationID."' AND d.PK_Device='".$parmDevice."'";
						$resCat=$dbADO->Query($queryTable);
						if($resCat->RecordCount()>0){
							$row=$resCat->FetchRow();
							$DeviceCategory=$row['FK_DeviceCategory'];
						}
						if ($DeviceCategory == $GLOBALS['rootClimate']) {
					             // Display options for mode,fan,on-off,setpoint,temp
						     $stateArray=array(1=>'On/Off',2=>'Mode',3=>'Fan',4=>'Set Point',5=>'Current Temp');
						     $out.='<select name="CriteriaParmParm_'.$rowCP['PK_CriteriaParm'].'_2" onChange="document.editCriteria.changedCP.value='.$rowCP['PK_CriteriaParm'].';document.editCriteria.submit();">
								<option value=""></option>';
						     foreach($stateArray AS $key => $value){
						  	  $out.='<option value="'.$key.'" '.(($parmData==$key)?'selected':'').'>'.$value.'</option>';
						     }
						     $out.='			
						     </select>';
						}
					}
				}
				$out.=' <select name="CriteriaParmOperator_'.$rowCP['PK_CriteriaParm'].'">
						<option value="1" '.(($rowCP['Operator']==1)?'selected':'').'>=</option>
						<option value="2" '.(($rowCP['Operator']==2)?'selected':'').'>&lt;&gt;</option>
						<option value="3" '.(($rowCP['Operator']==3)?'selected':'').'>&gt;</option>
						<option value="4" '.(($rowCP['Operator']==4)?'selected':'').'>&lt;</option>
						<option value="5" '.(($rowCP['Operator']==5)?'selected':'').'>contain</option>
				</select> ';
				if($rowCP['FK_CriteriaParmList']==$GLOBALS['PK_EventParameterParmList']){
					$out.='Value: ';
					switch(@$selectedEventParameter){
						case 'PK_Device':
						case 'PK_Room':
						$tableName=substr($selectedEventParameter,3);
						$out.=generatePullDown('CriteriaParmValue_'.$rowCP['PK_CriteriaParm'],$tableName,'PK_'.$tableName,'Description',$rowCP['Value'],$dbADO,"WHERE FK_Installation='$installationID'");
						break;
						default:
						$out.='<input type="textbox" name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'" value="'.$rowCP['Value'].'">';
						break;
					}
				}else{
					if(substr($rowCP['CPL_Description'],0,3)=='PK_' && $rowCP['CPL_Description'] != 'PK_EventList'){
						$out.='<select name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'">
										<option value="0" ></option>';
						$tableName=substr($rowCP['CPL_Description'],3);
						$filterTable=($tableName=='Device' || $tableName=='Room')?" WHERE FK_Installation='".$installationID."' ":'';
						$queryTable="SELECT * FROM $tableName $filterTable ORDER BY Description ASC";
						$resTable=$dbADO->Execute($queryTable);
						while($rowTable=$resTable->FetchRow()){
							$out.='<option value="'.$rowTable[$rowCP['CPL_Description']].'" '.(($rowCP['Value']==$rowTable[$rowCP['CPL_Description']])?'selected':'').'>'.$rowTable['Description'].'</option>';
						}
						$out.='</select>';
					}
					else{
						$out.='<input type="text" name="CriteriaParmValue_'.$rowCP['PK_CriteriaParm'].'" value="'.$rowCP['Value'].'">';
					}
				}
				$out.='
				</td>
				<td align="center">'.(($rowCPN['IsAnd']==1)?'And':'Or').'</td>
				<td align="center"><a href="#" onClick="if(confirm(\''.translate('TEXT_DELETE_CRITERIA_CONFIRMATION_CONST').'\'))self.location=\'index.php?section=editCriteria&action=delete&ehID='.$eventHandlerID.'&dcpID='.$rowCP['PK_CriteriaParm'].'\'">'.translate('TEXT_DELETE_CONST').'</a></td>			
			</tr>';
	}
	$resNestingChilds=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE FK_CriteriaParmNesting_Parent=?',array($FK_CriteriaParmNesting));
	if($resNestingChilds->RecordCount()!=0){
		$out.='<tr>
				<td colspan="6"><table width="100%" border="0">
					<tr>
						<td width="20">&nbsp;</td>
						<td>';
		while($rowNestingChilds=$resNestingChilds->FetchRow()){
			$out.=displayCriteria($rowNestingChilds['PK_CriteriaParmNesting'],$eventHandlerID,$installationID,$dbADO);
		}
				$out.='</td>
					</tr>
				</table>
				</td>
			</tr>';
	}
	$out.='
			<tr bgcolor="#E7E7E7">
				<td colspan="6"><B>)</B></td>
			</tr>
	</table>';
	$out.='<input type="hidden" name="displayedCriteriaParm" value="'.join(',',$GLOBALS['displayedCriteriaParms']).'">';
	return $out;
}

function deleteCriteriaParmNesting($cpnID,$dbADO)
{
	$dbADO->Execute('DELETE FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?',array($cpnID));
	$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=?',array($cpnID));
	
	$resChilds=$dbADO->Execute('SELECT * FROM CriteriaParmNesting WHERE FK_CriteriaParmNesting_Parent=?',array($cpnID));
	while($rowChilds=$resChilds->FetchRow()){
		deleteCriteriaParmNesting($rowChilds['PK_CriteriaParmNesting'],$dbADO);
	}
}

function setOrbitersNeedConfigure($installationID,$dbADO)
{
	$orbitersArray=getValidOrbitersArray($installationID,$dbADO);
	if(count($orbitersArray)!=0){
		$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device IN ('.join(',',$orbitersArray).') AND FK_Installation=?',array($installationID));
		$dbADO->Execute('UPDATE Orbiter SET Regen=1,Modification_LastGen=0 WHERE PK_Orbiter IN ('.join(',',$orbitersArray).')'); 
	}
}

function setDCERouterNeedConfigure($installationID,$dbADO)
{
	$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['rootDCERouter'],$installationID));
}

function addScenariosToRoom($roomID, $installationID, $dbADO)
{
	$insertTelecomScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) SELECT '.$GLOBALS['ArrayIDCommunicationScenarios'].','.$installationID.',\'Phones Scenario\','.$GLOBALS['TelecomScenariosTemplate'].',Description FROM Room WHERE PK_Room=?';
	$dbADO->Execute($insertTelecomScenario,array($roomID));
	$cgID=$dbADO->Insert_ID();

	$insertCG_R='INSERT INTO CommandGroup_Room (FK_Room, FK_CommandGroup,Sort) VALUES (?,?,?)';
	$dbADO->Execute($insertCG_R,array($roomID,$cgID,$cgID));

	$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) VALUES(?,?,?)";
	$dbADO->Execute($queryInsertCommandGroup_Command,array($cgID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));
	$CG_C_insertID=$dbADO->Insert_ID();

	$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
	
	$values=array();
	$values[$GLOBALS['commandParamPK_DesignObj']]=$GLOBALS['TelecomMenu'];
			
	addScenarioCommandParameters($CG_C_insertID,$GLOBALS['commandGotoScreen'],$dbADO,$values);						
	
	$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup, FK_Room,Sort) VALUES (?,?,?)';
	$insertCG_C='
		INSERT INTO CommandGroup_Command 
			(FK_CommandGroup,FK_Command,TurnOff,OrderNum,FK_Device)
		VALUES
			(?,?,?,?,?)';
	$insertCG_C_CP='
		INSERT INTO CommandGroup_Command_CommandParameter 
			(FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter)
		VALUES
			(?,?,?)';
	
	$selectArmDisarm='SELECT * FROM CommandGroup WHERE FK_Template=? AND FK_Installation=?';
	$resArmDisarm=$dbADO->Execute($selectArmDisarm,array($GLOBALS['SecurityArmDisarmTemplate'],$installationID));
	if($resArmDisarm->RecordCount()){
		$rowArmDisarm=$resArmDisarm->FetchRow();
		$armDisarmCG=$rowArmDisarm['PK_CommandGroup'];
	}else{
		$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'Arm Disarm',0,0,0,$GLOBALS['SecurityArmDisarmTemplate']));
		$armDisarmCG=$dbADO->Insert_ID();

		$dbADO->Execute($insertCG_C,array($armDisarmCG,$GLOBALS['commandGotoScreen'],0,0,$GLOBALS['localOrbiter']));
		$cg_cID=$dbADO->Insert_ID();
	
		$values=array();
		$values[$GLOBALS['commandParameterObjectScreen']]=$GLOBALS['mnuSecurityCamerasDesignObj'];
			
		addScenarioCommandParameters($cg_cID,$GLOBALS['commandGotoScreen'],$dbADO,$values);						
		
	}
	$dbADO->Execute($insertCG_Room,array($armDisarmCG,$roomID,$armDisarmCG));
	
	$selectSOS='SELECT * FROM CommandGroup WHERE FK_Template=? AND FK_Installation=?';
	$resSOS=$dbADO->Execute($selectSOS,array($GLOBALS['SecuritySOSTemplate'], $installationID));
	if($resSOS->RecordCount()){
		$rowSOS=$resSOS->FetchRow();
		$sosCG=$rowSOS['PK_CommandGroup'];
	}else{
		$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'*SOS*',0,0,0,$GLOBALS['SecuritySOSTemplate']));
		$sosCG=$dbADO->Insert_ID();

		$dbADO->Execute($insertCG_C,array($sosCG,$GLOBALS['commandGotoScreen'],0,0,$GLOBALS['localOrbiter']));
		$cg_cID=$dbADO->Insert_ID();
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
	}
	$dbADO->Execute($insertCG_Room,array($sosCG,$roomID,$sosCG));
}

function getMediaPluginID($installationID,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['rootMediaPlugin'],$installationID));
	$row=$res->FetchRow();
	return @$row['PK_Device'];
}

function pulldownFromArray($valuesArray,$name,$selectedValue,$extra='',$valueKey='key',$zeroValueDescription='',$highlightValue=-1)
{
	$out='<select name="'.$name.'" '.$extra.'>';
	if($zeroValueDescription=='') $zeroValueDescription = '- '.translate('TEXT_PLEASE_SELECT_CONST').' -';
	if($zeroValueDescription!='')
		$out.='<option value="0">'.$zeroValueDescription.'</option>';
	foreach ($valuesArray AS $key=>$value){
		$optionValue=($valueKey=='key')?$key:$value;
		$out.='<option value="'.$optionValue.'" '.(($optionValue==$selectedValue)?'selected':'').' '.((in_array($optionValue,explode(',',$highlightValue)))?'style="background:lightgreen;"':'').' title="'.$value.'">'.$value.'</option>
		';
	}
	$out.='</select>';
	
	includeLangFile('editMediaFile.lang.php');
	
		//print_r($selectedValue);
	if ($name == 'subtype' || 'fileFormat')
			{
				if ($name =='subtype')
				{
					$out.='</select> '.translate('TEXT_SET_MEDIA_TYPE_CONST');
					
				}
				else if ($name=='fileFormat')
				{
					$out.='</select> '.translate('TEXT_SET_FILE_FORMAT_CONST');
					
				}			
			}	
			return $out;
}

//$GLOBALS['wizardChilds']=array();
function getChildsOfWizard($page,$dbADO)
{
	
	$childs=array();
	$res=$dbADO->Execute('
		SELECT  
		IF(PK_PageSetup=?,1,0) AS pos,
		PageSetup.*
		FROM PageSetup
		ORDER BY pos desc,PK_PageSetup ASC',array($page));
	while($row=$res->FetchRow()){
		if($row['PK_PageSetup']==$page){
			$childs[$row['PK_PageSetup']]=$row['pageURL'];
		}
		if(in_array($row['FK_PageSetup_Parent'],array_keys($childs))){
			$childs[$row['PK_PageSetup']]=$row['pageURL'];
		}
	}

	return $childs;
	
	
	$res=$dbADO->Execute('SELECT * FROM PageSetup WHERE FK_PageSetup_Parent=? ORDER BY OrderNum ASC',array($page));
	while($row=$res->FetchRow()){
		$GLOBALS['wizardChilds'][$row['PK_PageSetup']]=$row['pageURL'];
		getChildsOfWizard($row['PK_PageSetup'],$dbADO);
	}
	
	return $GLOBALS['wizardChilds'];
}

function deviceForScenariosSelector_old($name,$selectedValue,$dbADO,$allowNoValue=1,$extra='')
{
	$out='
		<select name="'.$name.'" '.$extra.'>';
	if($allowNoValue==1){
		$out.='<option value="0">-'.translate('TEXT_PLEASE_SELECT_CONST').'-</option>';
	}
	$out.='
			<option value="-300" '.(($selectedValue=='-300')?'selected':'').'>[Local Orbiter]</option>';
							
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia IS NULL
		ORDER BY FK_Room ASC,Description ASC';
	$resTopDevices=$dbADO->Execute($query,array($_SESSION['installationID']));
							
	if ($resTopDevices) {
		while ($rowTopDevice = $resTopDevices->FetchRow()) {
			$roomName=($rowTopDevice['RoomName']!='')?stripslashes($rowTopDevice['RoomName']):'No Room';
			$out.='<option '.($rowTopDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowTopDevice['PK_Device'].'" title="Device template #'.$rowTopDevice['FK_DeviceTemplate'].': '.$rowTopDevice['Template'].'">'.$rowTopDevice['Description'].' ['.$roomName.']</option>';
			$out.=pulldownChildsDevice($rowTopDevice['PK_Device'],$selectedValue,$dbADO,1);
		}
	}
						
	$out.='
		</select>';
	return $out;
}

function getDeviceNameForScenarios($deviceID,$dbADO)
{
	if((int)$deviceID==0){
		return '';		
	}
	
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE PK_Device = ? ';
	$resDevice=$dbADO->Execute($query,array($deviceID));
							
	if ($resDevice->RecordCount()>0) {
		$row = $resDevice->FetchRow();
		return '<span title="Device template #'.$row['FK_DeviceTemplate'].' : '.$row['Template'].'"><B>'.$row['Description'].'</B> ['.(($row['RoomName']!='')?$row['RoomName']:'None').']</span>';
	}else
		return '';
	
}

function pulldownChildsDevice($parentDevice,$selectedValue,$dbADO,$depth,$filter='')
{
	$out='';
	$query = '
		SELECT Device.*, Room.Description AS RoomName,DeviceTemplate.Description AS Template
		FROM Device 
		LEFT JOIN Room ON FK_Room=PK_Room
		INNER JOIN DeviceTemplate ON FK_DEviceTemplate=PK_DeviceTemplate
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia =? '.$filter.'
		ORDER BY FK_Room ASC,Description ASC';
	$resDevices=$dbADO->Execute($query,array($_SESSION['installationID'],$parentDevice));
	while($rowDevice=$resDevices->FetchRow()){
		$roomName=($rowDevice['RoomName']!='')?stripslashes($rowDevice['RoomName']):'No Room';
		$prefix='&nbsp;&nbsp;&nbsp;|';
		for($i=0;$i<$depth;$i++)
			$prefix.='---';
		$out.='<option '.($rowDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowDevice['PK_Device'].'" title="Device template #'.$rowDevice['FK_DeviceTemplate'].': '.$rowDevice['Template'].'">'.$prefix.' '.$rowDevice['Description'].' ['.$roomName.']</option>';
		$out.=pulldownChildsDevice($rowDevice['PK_Device'],$selectedValue,$dbADO,($depth+1));
	}
	return $out;
}

function getInstallWizardDeviceTemplates($step,$dbADO,$device='',$distro=0,$operatingSystem=0)
{
	if($distro!=0){
		$queryDistro='SELECT * FROM Distro WHERE PK_Distro=?';
		$resDistro=$dbADO->Execute($queryDistro,array($distro));
		if($resDistro->RecordCount()>0){
			$rowDistro=$resDistro->FetchRow();
			$distroName=$rowDistro['Description'];
			$operatingSystem=$rowDistro['FK_OperatingSystem'];
		}else
			$distroName='';
	}
	
	if($operatingSystem!=0){
		$queryOperatingSystem='SELECT * FROM OperatingSystem WHERE PK_OperatingSystem=?';
		$resOperatingSystem=$dbADO->Execute($queryOperatingSystem,array($operatingSystem));
		if($resOperatingSystem->RecordCount()>0){
			$rowOperatingSystem=$resOperatingSystem->FetchRow();
			$operatingSystemName=$rowOperatingSystem['Description'];
		}else
			$operatingSystemName='';
	}
			
	$out='<table>';
	$queryInstallWizard='
		SELECT InstallWizard.*,
			DeviceCategory.Description AS Category,
			DeviceTemplate.Description AS Template
		FROM InstallWizard
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE step=?';
	$resInstallWizard=$dbADO->Execute($queryInstallWizard,array($step));
	if($resInstallWizard->RecordCount()==0){
		$out.='
			<tr>
				<td>&nbsp;</td>
			</tr>';
	}else{
		$out.='
			<tr class="normaltext">
				<td colspan="5">'.translate('TEXT_OPTIONAL_DEVICES_INFO_CONST').'</td>
			</tr>';
	}
	$oldCategory='';
	$displayedTemplatesRequired=array();
	while($row=$resInstallWizard->FetchRow()){
		if($row['Category']!=$oldCategory){
			$oldCategory=$row['Category'];
			$displayCategory=$row['Category'];
		}else 
			$displayCategory='-';
		// check if the device actually exist to display actual entries
		if($device!=''){
			if($device!=@$_SESSION['CoreDCERouter']){
				$orbiterMDChild=getMediaDirectorOrbiterChild($device,$dbADO);
				
				$queryDevice='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia IN (?,?)';
				$resDevice=$dbADO->Execute($queryDevice,array($row['FK_DeviceTemplate'],$device,$orbiterMDChild));
			}else{
				// if device is DCE Router, check also childs of Core
				$queryDevice='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND (FK_Device_ControlledVia=? OR FK_Device_ControlledVia=?)';
				$resDevice=$dbADO->Execute($queryDevice,array($row['FK_DeviceTemplate'],$_SESSION['CoreDCERouter'],$_SESSION['CoreID']));
			}
			$deviceTemplateChecked=($resDevice->RecordCount()==0)?'':'checked';
			$rowDevice=$resDevice->FetchRow();
			$oldDevice=$rowDevice['PK_Device'];
		}

		$out.='
			<tr class="alternate_back">
				<td align="center"><b>'.$displayCategory.'</b></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;'.$row['Template'].'</td>';
		$query='
			SELECT 
				InstallWizard_Distro.*,
				InstallWizard.Default
			FROM InstallWizard_Distro
				INNER JOIN InstallWizard ON FK_InstallWizard=PK_InstallWizard
			WHERE (FK_Distro IS NULL OR FK_Distro=?) AND (FK_OperatingSystem=? OR FK_OperatingSystem IS NULL) AND FK_InstallWizard=?';
		$res=$dbADO->Execute($query,array($distro,$operatingSystem,$row['PK_InstallWizard']));
		$rowIWD=$res->FetchRow();
		$templateIsChecked=((isset($deviceTemplateChecked) && @$isCoreFirstTime!=1)?$deviceTemplateChecked:(($rowIWD['Default']==1)?'checked':''));
		$out.='<td><input type="checkbox" '.(($res->RecordCount()==0)?'disabled':'').' '.$templateIsChecked.' name="device_'.$device.'_requiredTemplate_'.$row['FK_DeviceTemplate'].'" value="1"> 
			<input type="hidden" name="templateName_'.$row['FK_DeviceTemplate'].'" value="'.$row['Template'].'">
			<input type="hidden" name="oldDevice_'.$device.'_requiredTemplate_'.$row['FK_DeviceTemplate'].'" value="'.@$oldDevice.'">
		';
		if($res->RecordCount()==0)
			$out.='Not available for '.(($distro!=0)?$distroName:'').(($operatingSystem!=0)?' / '.$operatingSystemName:'');
		else{
			$displayedTemplatesRequired[]=$row['FK_DeviceTemplate'];
			$out.=$rowIWD['Comments'];
		}
		$out.='
				</td>
			</tr>
		';
	}

	$out.='	<input type="hidden" name="displayedTemplatesRequired_'.$device.'" value="'.(join(',',$displayedTemplatesRequired)).'">
	</table>';

	return $out;
}

function getMediaDirectorOrbiterChild($MD_PK_Device,$dbADO)
{
	$DTArray=getDeviceTemplatesFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
	if(count($DTArray)==0)
		return null;
	$getOrbiterChild='SELECT * FROM Device WHERE FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Device_ControlledVia=?';
	$resOrbiterChild=$dbADO->Execute($getOrbiterChild,array($MD_PK_Device));
	if($resOrbiterChild->RecordCount()!=0){
		$rowOrbiterChild=$resOrbiterChild->FetchRow();
		return $rowOrbiterChild['PK_Device'];
	}
	return null;
}

function getDevicesFromCategories($categoriesArray,$dbADO)
{
	unset($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray']=array();

	$devicesFullArray=array();
	foreach ($categoriesArray AS $categoryID){
		$devicesFullArray=$devicesFullArray+getDevicesArrayFromCategory($categoryID,$dbADO);
	}

	return $devicesFullArray;
}

function commandPulldownForDevice($deviceID,$dbADO)
{
	$out='';
	if ($deviceID!=0) {
		if(cleanInteger(@$_REQUEST['newDevice']!='-300')){
			$query = "
				SELECT PK_Command,Command.Description
				FROM Device 
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
				INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				WHERE PK_Device=?";
			$resNewCommand = $dbADO->Execute($query,array($deviceID));
		}else{
			$query = "
				SELECT PK_Command,Command.Description
				FROM Command 
				INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				WHERE FK_DeviceTemplate=?
				ORDER BY Command.Description ASC";
			$resNewCommand = $dbADO->Execute($query,array($GLOBALS['deviceTemplateOrbiter']));
		}
		if ($resNewCommand) {
			$out.='<select name="addNewDeviceCommand">
					<option value="0">- '.translate('TEXT_PLEASE_SELECT_CONST').' -</option>';
			while ($rowNewCommand = $resNewCommand->FetchRow()) {
				$out.='<option value="'.$rowNewCommand['PK_Command'].'" title="'.$rowNewCommand['Description'].'">'.$rowNewCommand['Description'].'</option>';
			}
			if ($resNewCommand->RecordCount()==0) {
				$out.='<option value="0">-'.translate('TEXT_NO_COMMAND_CONST').'-</option>';
			}
			$resNewCommand->Close();
			$out.='</select>';
		}
	}else{
		return '';
	}
	return $out;
}

function lightingDevicesTable($cgID,$dbADO)
{
	$out='
		<table cellpadding="2">
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_DEVICE_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_TYPE_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_UNCHANGED_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_ON_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_OFF_CONST').'</B></td>
				<td align="center" colspan="2"><B>'.translate('TEXT_SET_LEVEL_CONST').'</B></td>
			</tr>';

	$lightingDevicesArray=getValidLightingObjectsArray($_SESSION['installationID'],$dbADO);
	if(count($lightingDevicesArray)==0)
	$lightingDevicesArray[]=0;
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE PK_Device IN ('.join(',',$lightingDevicesArray).')
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice);
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="6">'.translate('TEXT_NO_LIGHTING_DEVICES_CONST').'</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$lightingCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['genericSetLevelCommand'];
		$queryCGCommands='
				SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
				FROM Command 
				INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
				INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
				LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
				WHERE FK_Device =? AND PK_Command IN ('.$lightingCommands.') AND FK_CommandGroup=?
				ORDER BY Description ASC';

		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$dimValue='';
		if($resCGCommands->RecordCount()==0)
		$selectedCommand=1;
		else{
			$rowCGCommands=$resCGCommands->FetchRow();
			switch($rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
				$selectedCommand=2;
				break;
				case $GLOBALS['genericOFFCommand']:
				$selectedCommand=3;
				break;
				case $GLOBALS['genericSetLevelCommand']:
				$selectedCommand=4;
				$dimValue=($rowCGCommands['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCommands['IK_CommandParameter']:'';
				break;
			}
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.(($rowGetRoomsDevice['RoomName']=='')?'None':$rowGetRoomsDevice['RoomName']).'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'></td>
				<td align="center"><input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.scenarioWizard.command_'.$rowGetRoomsDevice['PK_Device'].'[3].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="updateDevices" value="'.translate('TEXT_UPDATE_CONST').'"  > <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
			</tr>
		';
	}
	$out.='</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
		<script>
		function validateForm()
		{
			devicesNo='.count($displayedDevices).';
			devicesArray=new Array(devicesNo);';
			$posStart=0;
			foreach($displayedDevices as $Device){
				$out.='devicesArray['.$posStart.']='.$Device.';';
				$posStart++;
			}
		$out.='
			for(i=0;i<devicesNo;i++){
				eval("isDim=document.scenarioWizard.command_"+devicesArray[i]+"[3].checked");
				if(isDim==true){
					eval("dimVal=parseInt(document.scenarioWizard.dimValue_"+devicesArray[i]+".value)");
					if(dimVal<-100 || dimVal>100 || isNaN(dimVal)){
						alert("'.translate('TEXT_NUMBER_RANGE_REQUIRED_CONST').'");
						eval("document.scenarioWizard.dimValue_"+devicesArray[i]+".focus()");
						return false;
					}
				}
			}
			return true;
		}
		</script>';
	
	return $out;
}

function climateDevicesTable($cgID,$dbADO)
{
	// include language files
	includeLangFile('scenarioWizard.lang.php');
	
	$out='
		<fieldset>
			<legend>'.translate('TEXT_CLIMATE_SECTION_THERMOSTAT_DEVICES_CONST').'</legend>
			<table>
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_DEVICE_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_TYPE_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_UNCHANGED_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_AUTO_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_HEAT_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_COOL_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_OFF_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_SET_TEMPERATURE_CONST').'</B></td>
			</tr>';
			
	$climateDevicesArray=getValidClimateObjectsArray($_SESSION['installationID'],$dbADO);
	if(count($climateDevicesArray)==0)
	$climateDevicesArray[]=0;

	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			LEFT JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE PK_Device IN ('.join(',',$climateDevicesArray).') AND FK_DeviceCommandGroup = ?
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice,array($GLOBALS['ThermostatCommands']));
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="9">'.translate('TEXT_CLIMATE_NO_DEVICES_AVAILABLE_CONST').'</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$climateCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['setCoolHeat'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['setTemperatureCommand'];
		$queryCGCommands='
			SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
			FROM Command 
			INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
			INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
			LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
			WHERE FK_Device =? AND PK_Command IN ('.$climateCommands.') AND FK_CommandGroup=?
			ORDER BY Description ASC';
		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$temperature='';
		if($resCGCommands->RecordCount()==0)
			$selectedCommand=1;
		else{
			while($rowCGC=$resCGCommands->FetchRow()){
				if($rowCGC['FK_Command']!=$GLOBALS['setTemperatureCommand'])
					$rowCGCommands=$rowCGC;
				else
					$rowCGCSetLevel=$rowCGC;
			}
			switch(@$rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
					$selectedCommand=2;
				break;
				case $GLOBALS['setCoolHeat']:
					if($rowCGCommands['IK_CommandParameter']=='H')
						$selectedCommand=3;
					else
						$selectedCommand=4;
				break;
				case $GLOBALS['genericOFFCommand']:
					$selectedCommand=5;
				break;
				default:
					$selectedCommand=0;
				break;
			}
			$temperature=(@$rowCGCSetLevel['FK_CommandParameter']==$GLOBALS['commandParameterValueToAsign'])?$rowCGCSetLevel['IK_CommandParameter']:'';
			
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.(($rowGetRoomsDevice['RoomName']=='')?'None':$rowGetRoomsDevice['RoomName']).'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="5" '.(($selectedCommand==5)?'checked':'').'></td>
				<td align="center"><input type="text" name="temperature_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$temperature.'" size="3"></td>
			</tr>';
	}

	$out.='</table></fieldset><br />
	<fieldset>
		<legend>'.translate('TEXT_CLIMATE_SECTION_ONOFF_DEVICES_CONST').'</legend>
		<table>
			<tr><td colspan="8">&nbsp;</td></tr>
			<tr class="tablehead">
				<td align="center"><B>'.translate('TEXT_DEVICE_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_TYPE_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_UNCHANGED_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_ON_CONST').'</B></td>
				<td align="center"><B>'.translate('TEXT_OFF_CONST').'</B></td>
				<td align="center" colspan="3">&nbsp;</td>	
			</tr>';
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			LEFT JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=Device.FK_DeviceTemplate
			WHERE PK_Device IN ('.join(',',$climateDevicesArray).') AND FK_DeviceCommandGroup = ?
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice,array(83));
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="9">'.translate('TEXT_CLIMATE_NO_DEVICES_AVAILABLE_CONST').'</td>
			</tr>';
	}
	//$displayedDevices=array(); // Removed this because if there are thermostat devices without "other" devices the update / cancel buttons are not shown
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$climateCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['genericOFFCommand'];
		$queryCGCommands='
			SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
			FROM Command 
			INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
			INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
			LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
			WHERE FK_Device =? AND PK_Command IN ('.$climateCommands.') AND FK_CommandGroup=?
			ORDER BY Description ASC';
		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$temperature='';
		if($resCGCommands->RecordCount()==0) {
			$selectedCommand=1;
		}else{
			$rowCGCommands=$resCGCommands->FetchRow();

			switch(@$rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
					$selectedCommand=2;
				break;
				case $GLOBALS['genericOFFCommand']:
					$selectedCommand=5;
				break;
				default:
					$selectedCommand=0;
				break;
			}
			$temperature=(@$rowCGCSetLevel['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCSetLevel['IK_CommandParameter']:'';
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.(($rowGetRoomsDevice['RoomName']=='')?'None':$rowGetRoomsDevice['RoomName']).'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="5" '.(($selectedCommand==5)?'checked':'').'></td>
				<td align="center" colspan="3">&nbsp;</td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldTemperature_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$temperature.'">		
		';
	}

	if(count($displayedDevices)>0){
		$out.='</table></fieldset>
				<tr>
					<td colspan="9" align="center"><input type="submit" class="button" name="updateDevices" value="'.translate('TEXT_UPDATE_CONST').'"  > <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
				</tr>';
	}
	$out.='
			<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
	return $out;
}

function advancedCommandGroupCommandsTable($cgID,$section,$dbADO)
{
	$selectCommandsAssigned = "
		SELECT CommandGroup_Command.*
		FROM CommandGroup_Command				
		LEFT JOIN Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
		WHERE (Device.FK_Installation = ? OR Device.FK_Installation IS NULL) AND CommandGroup_Command.FK_CommandGroup = ?
		ORDER BY PK_CommandGroup_Command ASC";
	$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($_SESSION['installationID'],$cgID));
	$out='
	<script>
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}	
	</script>
		<input type="hidden" name="commandToTest" value="">
		<input type="hidden" name="cgc" value="">
		<table border="0" cellpadding="2" cellspacing="0">			
			<tr>
				<td colspan="2">'.editCommandGroupCommands($cgID,$dbADO).'</td>
			</tr>
			<tr>
				<td colspan="2">'.addCommandToCommandGroup($cgID,$dbADO).'</td>
			</tr>
		</table>';
	return $out;
}

function processLightingScenario($cgID,$dbADO)
{
	$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
	foreach($displayedDevicesArray AS $elem){
		$deviceCommand=(int)@$_POST['command_'.$elem];
		$dimValue=@$_POST['dimValue_'.$elem];
		$oldCommand=@$_POST['oldCommand_'.$elem];
		$oldDimValue=@$_POST['oldDimValue_'.$elem];
		$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
		$deleteParameters='
			DELETE CommandGroup_Command_CommandParameter
			FROM CommandGroup_Command_CommandParameter
			JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
			WHERE FK_CommandGroup=? AND FK_Device=?';

		if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldDimValue!=$dimValue))	{
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
			$dbADO->Execute($deleteParameters,array($cgID,$elem));
			switch($deviceCommand){
				case 1:
					// do nothing, delete is done above
				break;
				case 2:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
				break;
				case 3:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				break;
				case 4:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
					$cgcInsertID=$dbADO->Insert_ID();

					
					$values=array();
					$values[$GLOBALS['commandParamAbsoluteLevel']]=$dimValue;
						
					addScenarioCommandParameters($cgcInsertID,$GLOBALS['genericSetLevelCommand'],$dbADO,$values);						
					
				break;
			}
		}
	}
}

function processClimateScenario($cgID,$dbADO)
{
	$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
	foreach($displayedDevicesArray AS $elem){
		$deviceCommand=(int)@$_POST['command_'.$elem];
		$temperature=@$_POST['temperature_'.$elem];
		$oldCommand=@$_POST['oldCommand_'.$elem];
		$oldTemperature=@$_POST['oldTemperature_'.$elem];
		$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
		$deleteParameters='
			DELETE CommandGroup_Command_CommandParameter
			FROM CommandGroup_Command_CommandParameter
			JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
			WHERE FK_CommandGroup=? AND FK_Device=?';
		if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldTemperature!=$temperature))	{
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericHeatCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericCoolCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['setCoolHeat']));
			$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['setTemperatureCommand']));
			$dbADO->Execute($deleteParameters,array($cgID,$elem));
			switch($deviceCommand){
				case 1:
					// do nothing, delete is done above
				break;
				case 2:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
				break;
				case 3:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['setCoolHeat']));
					$cgcInsertID=$dbADO->Insert_ID();
	
					$values=array();
					$values[$GLOBALS['commandParameterOnOff']]='H';
						
					addScenarioCommandParameters($cgcInsertID,$GLOBALS['setCoolHeat'],$dbADO,$values);						

				break;
				case 4:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['setCoolHeat']));
					$cgcInsertID=$dbADO->Insert_ID();
	
					$values=array();
					$values[$GLOBALS['commandParameterOnOff']]='C';
						
					addScenarioCommandParameters($cgcInsertID,$GLOBALS['setCoolHeat'],$dbADO,$values);						
					
				break;
				case 5:
					$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				break;
			}

			if($temperature!=''){
				$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
				$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['setTemperatureCommand']));
				$cgcInsertID=$dbADO->Insert_ID();
				
				$values=array();
				$values[$GLOBALS['commandParameterValueToAsign']]=$temperature;
						
				addScenarioCommandParameters($cgcInsertID,$GLOBALS['setTemperatureCommand'],$dbADO,$values);						
				
			}

		}
	}
}

function processAdvancedScenarios($cgID,$section,$dbADO)
{
	$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:0;
	$from=@$_REQUEST['from'];
	$roomID=(int)@$_REQUEST['roomID'];
	$ehID=@$_REQUEST['ehID'];
	$CommandIsTested=(isset($_REQUEST['commandToTest']) && (int)$_REQUEST['commandToTest']>0)?1:0;
	$returnHook=null;
	
	if(isset($_REQUEST['cgc']) && (int)$_REQUEST['cgc']>0){
		$toDel=(int)$_REQUEST['cgc'];
		$commandsDeleted='';
		$parametersDeleted='';
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$query = $dbADO->Execute($deleteObjFromDevice,array($toDel));
		//delete saved values
		if ($dbADO->Affected_Rows()>0) {
			$commandsDeleted = translate('TEXT_COMMAND_REMOVED_FROM_COMMAND_GROUP_CONST');
			$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
			$query = $dbADO->Execute($deleteParamValues,array($toDel));
			if ($dbADO->Affected_Rows()>0) {
				$parametersDeleted = translate('TEXT_PARAMETER_VALUES_ALSO_DELETED_CONST');
			} else {
				$parametersDeleted = translate('TEXT_NO_PARAMETER_VALUES_DELETED_CONST');
			}

		} else {
			$commandsDeleted = translate('TEXT_ERROR_COMMAND_NOT_REMOVED_FROM_COMMAND_GROUP_CONST');
		}
		if($section=='scenarioWizard'){
			header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=$commandsDeleted $parametersDeleted");
		}
		else{
			header("Location: index.php?section=$section&ehID=$ehID&wizard=$wizard&msg=$commandsDeleted $parametersDeleted");
		}
		exit();
	}

	list ($isModified,$parametersUpdatedAlert,$returnHook)=processEditScenarioCommands($cgID,$dbADO);
	
	if($CommandIsTested==1){

		$commandToTest=$_REQUEST['commandToTest'];
		$queryCommands='
			SELECT CommandGroup_Command_CommandParameter.*, FK_Command,FK_Device 
			FROM CommandGroup_Command
			LEFT JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command 
			WHERE PK_CommandGroup_Command=?';

		$resCommands=$dbADO->Execute($queryCommands,array($commandToTest));
		$commandParmsArray=array();
		while($rowCommands=$resCommands->FetchRow()){
			$commandParmsArray[]=$rowCommands['FK_CommandParameter'].' "'.$rowCommands['IK_CommandParameter'].'"';
			$FK_Command=$rowCommands['FK_Command'];
			$FK_Device=$rowCommands['FK_Device'];
		}
		$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$FK_Device.' 1 '.$FK_Command.' '.join(' ',$commandParmsArray);
		exec($commandToSend);
		if($section=='scenarioWizard'){
			header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=The command with parameters was sent: $commandToSend#hook_$commandToTest");
		}else{
			header("Location: index.php?section=$section&ehID=$ehID&wizard=$wizard&msg=The command with parameters was sent");
		}
		exit();
	}
	
	return array($isModified,$parametersUpdatedAlert,$returnHook);
}

function getDeviceNames($dbADO,$filter='')
{
	$devices=array();
	$resDevices=$dbADO->Execute('SELECT * FROM Device WHERE FK_Installation=? '.$filter,$_SESSION['installationID']);
	while($row=$resDevices->fetchRow()){
		$devices[$row['PK_Device']]=$row['Description'];
	}
	return $devices;
}

function setLeftMenu($dbADO)
{
	$jsRedirect='';
	//return $jsRedirect;
	$wizardPagesArray=getChildsOfWizard(1,$dbADO);
	$toCompare='index.php?'.$_SERVER['QUERY_STRING'];
	if(in_array($toCompare,$wizardPagesArray)){
		$pageID=array_search($toCompare,$wizardPagesArray);
		$isWizard=1;
		$redirectLink='index.php?section=wizard&pageSetup='.$pageID.'&senderID=0';
		$jsRedirect='<script>
			function getQueryVariable(query,variable) {
			  var vars = query.split("&");
			  for (var i=0;i<vars.length;i++) {
			    var pair = vars[i].split("=");
			    if (pair[0] == variable) {
			      return pair[1];
			    }
			  } 
			}

			try{
				var queryStr=top.treeframe.location.search.substring(9,top.treeframe.location.search.length);
				if(getQueryVariable(queryStr,"pageSetup")!='.$pageID.'){
					top.treeframe.location=\''.$redirectLink.'\';
				}
			}catch(e){
				// no frame
			}
		</script>';
	}
	return $jsRedirect;
}

function CheckValidCode($code,$dbADO)
{
	list($device, $pin) = explode("-", $code);
	$sql = "SELECT ActivationCode
			FROM Device
	 			INNER JOIN Installation ON Device.FK_Installation = Installation.PK_Installation
			WHERE Device.PK_Device = ? LIMIT 1";
	$res = $dbADO->Execute($sql, array($device));
	if ($res->RecordCount() == 0)
		return false;
	$row = $res->FetchRow();
	if ($row['ActivationCode'] !== $pin)
		return false;
	return true;
}

// return an array of packages
function GetActivationSh($code,$param='install',$otherParam='')
{
	global $dbPlutoMainUser;
	global $dbPlutoMainPass;
	global $dbPlutoMainServer;


	list($device, $pin) = explode("-", $code);
#	echo("/usr/pluto/bin/ConfirmDependencies -r -h ".$dbPlutoMainServer." -u ".$dbPlutoMainUser." -p ".$dbPlutoMainPass." -d $device install");
	exec("/usr/pluto/bin/ConfirmDependencies -r -D pluto_main -h $dbPlutoMainServer -u $dbPlutoMainUser -p '$dbPlutoMainPass' -d $device $otherParam $param", $result, $retcode);
	// || die("ERROR. Can't generate answer: $device:$pin");

	if ($retcode == 0)
		$return = "OK\n" . implode("\n", (array)$result) . "\n#EOF";
	else
		$return = "ERROR. Something went terribly wrong while generating the activation script. The installation can not continue. Please contact Plutohome to solve this problem. Thank you.";
	return $return;
}

function htmlPulldown($contentArray,$name,$selectedValue='None',$unselectedLabel)
{
	if(!is_array($contentArray))
		return '';	// error
	$out='<select name="'.$name.'">
		<option value="0">'.$unselectedLabel.'</option>';
	foreach ($contentArray AS $key=>$label){
		$out.='<option value="'.$key.'" '.(($key==$selectedValue)?'selected':'').' title="'.$label.'">'.$label.'</option>';
	}
	$out.='</select>';
	return $out;
}

function getAssocArray($table,$keyField,$labelField,$dbADO,$whereClause='',$orderClause='')
{
	$retArray=array();
	$res=$dbADO->Execute("SELECT $keyField,$labelField FROM $table $whereClause $orderClause");
	while($row=$res->FetchRow()){
		$cleanLabel=cleanField($labelField);
		$keyField=cleanField($keyField);
		$retArray[$row[$keyField]]=$row[$cleanLabel];
	}
	return $retArray;
}

function createDevice($FK_DeviceTemplate,$FK_Installation,$controlledBy,$roomID,$dbADO,$childOfMD=0,$ipAddress='',$macAddress='')
{
	global $dbPlutoMainDatabase;
	
	$orbiterID=getMediaDirectorOrbiterChild($controlledBy,$dbADO);
	$parentID=($childOfMD==0)?$orbiterID:$controlledBy;
	
	
	// old create device by calling directly the command
	
	if($GLOBALS['inDebug']==1){
		$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$FK_DeviceTemplate.' -i '.$FK_Installation.(((int)$controlledBy!=0)?' -C '.$controlledBy:'');
		$insertID=exec_batch_command($cmd);
		$dbADO->Execute('UPDATE Device SET FK_Room=? WHERE PK_Device=?',array($roomID,$insertID));
	}else{
	
	
		$msgSendCommand='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 27 1 718 44 '.$FK_DeviceTemplate;
		$msgSendCommand.=((int)$roomID!='')?' 57 '.$roomID:'';
		$msgSendCommand.=((int)$controlledBy!=0)?' 156 '.$controlledBy:'';
		$msgSendCommand.=($ipAddress!='')?' 58 \''.$ipAddress.'\'':'';
		$msgSendCommand.=($macAddress!='')?' 47 \''.$macAddress.'\'':'';
		
		$out=exec_batch_command($msgSendCommand,1);
		write_weblog($out);
		
		if(strpos($out,'0:OK')===false){
			error_redirect('ERROR: Error creating device.  Be sure LinuxMCE is running.','');
		}
		
		$insertID=(int)trim(substr($out,7));
	
	
		if($insertID==0){
			error_redirect('ERROR: device not created, check if DCE router is running.','');
		}
	}
	
	
	return $insertID;
}

function controlledViaPullDown($pulldownName,$deviceID,$dtID,$deviceCategory,$controlledVia,$dbADO,$zeroOption='0,- Please Select -',$jsEvent='')
{
	//$optionsArray=getControlledViaDeviceTemplates($deviceID,$dtID,$deviceCategory,$dbADO);
	$optionsArray=getParentsForControlledVia($deviceID,$dbADO);

	$selectOptions='';
	foreach ($optionsArray AS $key=>$value){
		$selectOptions.='<option value="'.$key.'" '.(($key==$controlledVia)?'selected':'').' title="'.$value.'">'.$value.'</option>';
	}
	
	$zeroValues=explode(',',$zeroOption);
	$out='
	<select name="'.$pulldownName.'" '.$jsEvent.'>
		<option value="'.@$zeroValues[0].'" title="'.$zeroValues[1].'">'.$zeroValues[1].'</option>
		'.$selectOptions.'
	</select>';

	return $out;
}

function getControlledViaDeviceTemplates($deviceID,$dtID,$deviceCategory,$dbADO)
{
	
	$installationID=(int)$_SESSION['installationID'];
	
	$whereClause='';
	if ($deviceID!=0) {
		$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
		$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$installationID));
		if ($resSelectMasterDeviceForParent) {
			$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
			$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];

			$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
			$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($DeviceTemplateForParent));

			$GLOBALS['childsDeviceCategoryArray'] = array();
			if ($resSelectControlledViaCategory) {
				while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
					$GLOBALS['childsDeviceCategoryArray'][]=$rowSelectControlledVia['FK_DeviceCategory'];
					getDeviceCategoryChildsArray($rowSelectControlledVia['FK_DeviceCategory'],$dbADO);
				}
			}
			$controlledViaCategoryArray = cleanArray($GLOBALS['childsDeviceCategoryArray']);

			$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
			$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

			$childsDeviceTemplateArray = array();
			if ($resSelectControlledViaDeviceTemplate) {
				while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
					$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];
				}
			}



			if (count($controlledViaCategoryArray)>0) {
				$whereClause.=' and (FK_DeviceCategory in ('.join(",",$controlledViaCategoryArray).')';
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
				$whereClause.=')';
			} else {
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
			}

		}
	}
	$whereClause.=" AND PK_Device!='$deviceID'";
	$queryDeviceTemplate = "
		SELECT DISTINCT Device.Description,Device.PK_Device,DeviceTemplate.Description AS Template,Room.Description AS Room
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation=? $whereClause order by Device.Description asc";
	$resDeviceTemplate = $dbADO->Execute($queryDeviceTemplate,$installationID);
	$optionsArray=array();
	$optionsArrayLowerCase=array();
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$label=(@$GLOBALS['DT_&_Room']==1)?$row['Description'].' ('.$row['Template'].') - ('.$row['Room'].')':$row['Description'];
			$optionsArray[$row['PK_Device']]=$label;
			$optionsArrayLowerCase[$row['PK_Device']]=strtolower($label);
		}
	}
	
	// if device is AV with UsesIR=1, override default controlled_via based on category and device template
	$controlledByIfIR=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
	$controlledByIfNotIR=getDevicesFromCategories(array($GLOBALS['rootComputerID']),$dbADO);
	
	// get selected category Device Templates
	$avArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
	if(in_array($dtID,$avArray)){
		$queryDevice='
			SELECT 
				Device.*,
				DeviceTemplate.Description AS Template,Room.Description AS Room,
				FK_CommMethod
			FROM Device 
				INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
				LEFT JOIN Room ON FK_Room=PK_Room
				LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup
			WHERE PK_Device=?';	
		$resDevice=$dbADO->Execute($queryDevice,array($deviceID));
		$rowD=$resDevice->FetchRow();

		$tmpArray=array();
		$tmpArray[$deviceID]=$rowD['Description'].' ('.$rowD['Template'].') - ('.$rowD['Room'].')';
		$devicesAllowedToControll=(@$rowD['FK_CommMethod']==1)?array_diff($controlledByIfIR,$tmpArray):array_diff($controlledByIfNotIR,$tmpArray);

		foreach($devicesAllowedToControll as $key => $value){
			$optionsArray[$key]=$value;
			$optionsArrayLowerCase[$key]=strtolower($value);
		}
	}
	asort ($optionsArrayLowerCase, SORT_STRING);
	$optionsArrayOriginal=array();
	foreach ($optionsArrayLowerCase As $id=>$label){
		$optionsArrayOriginal[$id]=$optionsArray[$id];
	}

	return $optionsArrayOriginal;
}


function PortForHumans($device,$deviceNames)
{
	$devname = substr(strrchr($device, '/'), 1);

	if (strstr($devname, "ttyS") === $devname)
	{
		$devid = substr($devname, 4);
		if ($devid{0} === "_")
		{
			# it's a virtual serial port
			list($PK_Device, $number) = split("_", substr($devid, 1));
			$number++;
			# TODO: translate $PK_Device to actual device name
			return "Port $number on 'device ".@$deviceNames[$PK_Device]."'";
		}
		else
		{
			# it's a hardware serial port on the Core
			$number = $devid + 1;
			return "$devname | COM$number";
		}
	}
	
	return "Unknown: $devname";
}
function SerialPortInfo($parent,$name,$dbADO)
{
	//This function will return a more readable form of the serial device stored in the database.
	//This will make the SerialPort dropdowns easier to read in the web admin, while retaining the ability
	//to use the correct serial port even if the /dev/ttyUSBx number changes.


	//This function has no purpose for REAL serial devices, so lets omit them
	if(substr($name,0,4) == "/dev") {
		return $name;
	}

	//Is this serial port on a Media Director?
	$cmd = "";
	if($parent != 1) {
		//Serial port is located on an MD
		$ip=getFieldsAsArray('Device','IPaddress',$dbADO,'WHERE PK_Device='.$parent);
		$ipAddress=$ip['IPaddress'][0];

		$cmd = "sudo -u root /usr/pluto/bin/TranslateRemoteSerialPort.sh ".$ipAddress." ".$name;
	} else {
		$cmd = "/usr/pluto/bin/TranslateSerialPort.sh ".$name;
	}

	$res=exec($cmd);
	
	//and return the ACTUAL serial port device
	return $res." (".$name.")";


	
}
function serialPortsPulldown($name,$selectedPort,$allowedToModify,$topParent,$dbADO,$deviceID,$cssStyle="")
{
	$installationID=(int)$_SESSION['installationID'];

	if($topParent==0){
		return translate('TEXT_ERROR_TOP_PARENT_DEVICE_NOT_FOUND_CONST');
	}
	
	$portDeviceData=getDeviceData($topParent,$GLOBALS['AvailableSerialPorts'],$dbADO);
	if($portDeviceData==''){
		return translate('TEXT_ERROR_NO_SERIAL_PORTS_FOUND_CONST');
	}

	$serial_ports=explode(',',$portDeviceData);

	$usedPorts=getFieldsAsArray('Device_DeviceData','Device.Description,Device.IPaddress,FK_Device,IK_DeviceData',$dbADO,'INNER JOIN Device ON FK_Device=PK_Device WHERE FK_Installation='.$installationID.' AND FK_DeviceData='.$GLOBALS['Port'].' AND PK_Device!='.$deviceID);
	$serialPortAsoc=array();
	foreach($serial_ports AS $key=>$value){
		$usedBy=array();
		for($i=0;$i<count(@$usedPorts['IK_DeviceData']);$i++){
			if($value==$usedPorts['IK_DeviceData'][$i]){
				if(getTopLevelParent($usedPorts['FK_Device'][$i],$dbADO)==$topParent){
					$usedBy[]=$usedPorts['Description'][$i];
				}
			}
		}
		if(count($usedBy)>0){
			$serialPortAsoc[$value]=SerialPortInfo($topParent,$value,$dbADO)." Used by '".join(', ',$usedBy)."'";
		}else{
			$serialPortAsoc[$value]=SerialPortInfo($topParent,$value,$dbADO);
		}
	}
	
	$extra=((isset($allowedToModify) && $allowedToModify==0)?'disabled':'');
	$extra.='style="width:300px;" class="'.$cssStyle.'"';
	$out=pulldownFromArray($serialPortAsoc,$name,$selectedPort,$extra,'key');
	
	return $out;
}

function soundCardPulldown($name,$selectedCard,$allowedToModify,$topParent,$dbADO,$deviceID,$cssStyle="")
{
	$installationID=(int)$_SESSION['installationID'];

	if($topParent==0){
		return translate('TEXT_ERROR_TOP_PARENT_DEVICE_NOT_FOUND_CONST');
	}
	
	$cardDeviceData=getDeviceData($topParent,$GLOBALS['AvailableSoundCards'],$dbADO);
	if($cardDeviceData==''){
		return translate('TEXT_ERROR_NO_SOUND_CARDS_FOUND_CONST');
	}

	$sound_cards=explode(',',$cardDeviceData);

	$usedCards=getFieldsAsArray('Device_DeviceData','Device.Description,FK_Device,IK_DeviceData',$dbADO,'INNER JOIN Device ON FK_Device=PK_Device WHERE FK_Installation='.$installationID.' AND FK_DeviceData='.$GLOBALS['SoundCard'].' AND PK_Device!='.$deviceID);
	$soundCardAsoc=array();
	foreach($sound_cards AS $key=>$value){
		$usedBy=array();
		for($i=0;$i<count(@$usedCards['IK_DeviceData']);$i++){
			if($value==$usedCards['IK_DeviceData'][$i]){
				if(getTopLevelParent($usedCards['FK_Device'][$i],$dbADO)==$topParent){
					$usedBy[]=$usedCards['Description'][$i];
				}
			}
		}
		if(count($usedBy)>0){
			$soundCardAsoc[$value]=$value.' Used by '.join(', ',$usedBy);
		}else{
			$soundCardAsoc[$value]=$value;
		}
	}
	
	$extra=((isset($allowedToModify) && $allowedToModify==0)?'disabled':'');
	$extra.=' class="'.$cssStyle.'"';
	$out=pulldownFromArray($soundCardAsoc,$name,$selectedCard,$extra,'key');
	
	return $out;
}
// return the deviceID of the Infrared plugin from current installation
function getInfraredPlugin($installationID,$dbADO)
{
	$res=$dbADO->Execute('SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['InfraredPlugIn'],$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->Fetchrow();
		return $row['PK_Device'];
	}
}

// return the deviceID of the telecom plugin from current installation
function getTelecomPlugin($installationID,$dbADO)
{
	$res=$dbADO->Execute('SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($GLOBALS['TelecomPlugin'],$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->Fetchrow();
		return $row['PK_Device'];
	}
}

function isInfrared($deviceTemplate,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT DeviceTemplate.FK_CommMethod 
		FROM DeviceTemplate
		WHERE PK_DeviceTemplate=? AND FK_CommMethod=1',array($deviceTemplate));
	if($res->RecordCount()>0){
		return 1;
	}else{
		return 0;
	}
}


function getDeviceInformation($deviceID,$dbADO)
{
	$sql='
		SELECT 
			Device.*,
			DeviceTemplate.Description AS DeviceTemplate,
			DeviceCategory.Description AS Category,
			Manufacturer.Description AS Manufacturer
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		WHERE PK_Device=?
		';
	$res=$dbADO->Execute($sql,array($deviceID));
	$row=array();
	if($res){
		$row=$res->FetchRow();
	}
	return $row;
}

function getTopLevelParentIP($deviceID,$dbADO)
{
	$topParent=0;
	if($deviceID!=0){
		$res=$dbADO->Execute('
			SELECT FK_DeviceCategory,FK_DeviceTemplate,FK_Device_ControlledVia,IPaddress 
			FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_Device=?',array($deviceID));
		$row=$res->FetchRow();
		if((int)$row['FK_Device_ControlledVia']!=0){
			$topParent=getTopLevelParentIP($row['FK_Device_ControlledVia'],$dbADO);
		}else{
//			if($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'] || $row['FK_DeviceTemplate']==$GLOBALS['rootMediaDirectorsID']){
			if($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'] || $row['FK_DeviceCategory']==$GLOBALS['rootMediaDirectors'] 
			    || $row['FK_DeviceCategory']==$GLOBALS['rootMobileInternetDevice']){
				$topParent=($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'] && $row['IPaddress']=='')?'127.0.0.1':$row['IPaddress'];
			}else{
				$topParent=0;
			}
		}
	}
	
	return $topParent;
}

function getTopLevelParent($deviceID,$dbADO)
{
	$topParent=0;
	if($deviceID!=0){
		$res=$dbADO->Execute('
			SELECT FK_DeviceTemplate,FK_Device_ControlledVia,IPaddress,FK_DeviceCategory 
			FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_Device=?',array($deviceID));
		$row=$res->FetchRow();
		if((int)$row['FK_Device_ControlledVia']!=0){
			$topParent=getTopLevelParent($row['FK_Device_ControlledVia'],$dbADO);
		}else{
//			if($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'] || $row['FK_DeviceTemplate']==$GLOBALS['rootMediaDirectorsID']){
			if($row['FK_DeviceCategory']==$GLOBALS['CategoryCore'] || $row['FK_DeviceCategory']==$GLOBALS['rootMediaDirectors'] || $row['FK_DeviceTemplate']==$GLOBALS['rootJogglersID']
			    || $row['FK_DeviceCategory']==$GLOBALS['rootMobileInternetDevice']){
				$topParent=$deviceID;
			}else{
				$topParent=0;
			}
		}
	}
	
	return $topParent;
}

function getDeviceFromDT($installationID,$DeviceTemplate,$dbADO,$extraCondition='')
{
	$res=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=? '.$extraCondition,array($DeviceTemplate,$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}
}

function getDeviceData($deviceID,$deviceData,$dbADO)
{
	$req=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($deviceID,$deviceData));
	if($req->RecordCount()==0){
		return null;
	}else{
		$row=$req->FetchRow();
		return $row['IK_DeviceData'];
	}
}

function devicesTree($name,$selectedValue,$dbADO,$filter,$extra='')
{
	$out='
		<select name="'.$name.'" '.$extra.'>';
	$out.='<option value="0">-Please select-</option>';
							
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation = ? AND FK_Device_ControlledVia IS NULL '.$filter.'
		ORDER BY FK_Room ASC,Description ASC';
	$resTopDevices=$dbADO->Execute($query,array($_SESSION['installationID']));
							
	if ($resTopDevices) {
		while ($rowTopDevice = $resTopDevices->FetchRow()) {
			$roomName=($rowTopDevice['RoomName']!='')?stripslashes($rowTopDevice['RoomName']):'No Room';
			$out.='<option '.($rowTopDevice['PK_Device']==$selectedValue?' selected="selected" ':'').' value="'.$rowTopDevice['PK_Device'].'" title="Device template #'.$rowTopDevice['FK_DeviceTemplate'].': '.$rowTopDevice['Template'].'">'.$rowTopDevice['Description'].' ['.$roomName.']</option>';
			$out.=pulldownChildsDevice($rowTopDevice['PK_Device'],$selectedValue,$dbADO,1,$filter);
		}
	}
						
	$out.='
		</select>';
	return $out;
}

function getDeviceGroups($installationID, $dbADO)
{
	$req=$dbADO->Execute('SELECT PK_DeviceGroup,Description FROM DeviceGroup WHERE FK_Installation=? ORDER BY Description ASC',array($installationID));
	$dg=array();
	while($row=$req->Fetchrow()){
		$dg[$row['PK_DeviceGroup']]=$row['Description'];
	}
	return $dg;
}

function getDeviceTemplatesControlledBy($parentID,$dbADO)
{
	$whereClause='';
	$selectMasterDeviceForParent = '
		SELECT Device.FK_DeviceTemplate,FK_DeviceCategory,FK_DeviceCategory_Parent 
		FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE PK_Device = ?';
	$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($parentID));
	if ($resSelectMasterDeviceForParent) {
		$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
		$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];
		$DeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory'];
		$ParentDeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory_Parent'];

		$queryDTControlledByCategory ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE FK_DeviceCategory = ? OR FK_DeviceCategory = ?';
		$resDTControlledByCategory = $dbADO->Execute($queryDTControlledByCategory,array($DeviceCategoryForParent,$ParentDeviceCategoryForParent));

		$childsDeviceTemplateArray = array();
		if ($resDTControlledByCategory) {
			while ($rowDTControlledByCategory= $resDTControlledByCategory->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowDTControlledByCategory['FK_DeviceTemplate'];
			}
		}

		$querySelectControlledViaDeviceTemplate ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate_ControlledVia = ?';
		$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

		
		if ($resSelectControlledViaDeviceTemplate) {
			while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate'];
			}
		}

		if (count($childsDeviceTemplateArray)) {
			$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
		}
	}



	$queryDeviceTemplate = "select PK_DeviceTemplate ,Description from DeviceTemplate where 1=1 $whereClause order by Description asc";
	$resDeviceTemplate = $dbADO->_Execute($queryDeviceTemplate);
	$DeviceTemplates = array();
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$DeviceTemplates[].=$row['PK_DeviceTemplate'];
		}
	}
	
	return $DeviceTemplates;
}

function getProperties($primaryKey,$table,$properties,$field,$dbADO)
{
	$propArray=explode(',',$properties);
	$res=$dbADO->Execute("SELECT $properties FROM $table WHERE $field=?", array($primaryKey));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row;
	}else{
		return null;
	}
}

function displayRemotes($mdID,$dbADO,$section)
{
	$out='';
	$remotes=array();
	
	$DTarray=getDeviceTemplatesFromCategory($GLOBALS['RemoteControlls'],$dbADO);
	if(count($DTarray==0)){
		$DTarray[]=0;
	}
	
	$res=$dbADO->Execute('SELECT PK_Device,Description FROM Device WHERE FK_Device_ControlledVia=? AND FK_DeviceTemplate IN ('.join(',',$DTarray).')',array($mdID));
	while($row=$res->fetchRow()){
		$remotes[$row['PK_Device']]=$row['Description'];
	}
	$delLinks=(count($remotes)>0)?'<table width="100%">':'';
	foreach ($remotes AS $rid=>$description){
		$delLinks.='
		<tr>
			<td><a href="index.php?section=editDeviceParams&deviceID='.$rid.'">'.$description.'</a></td>
			<td><a href="javascript:if(confirm(\''.translate('TEXT_DELETE_REMOTE_CONFIRMATION_CONST').'\'))self.location=\'index.php?section='.$section.'&type=media_directors&action=del&delRemote='.$rid.'\';">'.translate('TEXT_DELETE_CONST').'</a></td>
		</tr>';
	}
	$delLinks.=(count($remotes)>0)?'</table>':'';
	$out.=$delLinks.'<input type="button" class="button_fixed" name="button" value="'.translate('TEXT_ADD_REMOTE_CONST').'" onClick="document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=mediaDirectors&categoryID='.$GLOBALS['RemoteControlls'].'&parmToKeep='.urlencode('mdID='.$mdID).'\',\'width=800,height=600,toolbar=1,scrollbars=1,resizable=1\');"> ';
	
	return $out;
}

function processRemotes($dbADO)
{
	global $dbPlutoMainDatabase;
	$installationID=(int)$_SESSION['installationID'];
	$mdID=(int)@$_REQUEST['mdID'];

	unset($_SESSION['from']);
	$deviceTemplate=(int)@$_REQUEST['deviceTemplate'];
	if($deviceTemplate!=0 && $mdID!=0){
		$insertID=createDevice($deviceTemplate,$installationID,$mdID,NULL,$dbADO,1);

		setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
		$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
		exec($commandToSend);
	}
	
	if(isset($_REQUEST['delRemote']) && (int)$_REQUEST['delRemote']!=0){
		deleteDevice((int)$_REQUEST['delRemote'],$dbADO);
	}
}

function getArrayFromTable($tableName,$primary,$secundary,$dbADO,$filter='',$orderBy='')
{
	$res=$dbADO->execute("SELECT $primary,$secundary FROM $tableName $filter $orderBy");
	$result=array();
	while($row=$res->Fetchrow()){
		$result[$row[$primary]]=$row[$secundary];
	}
	
	return $result;
}

function getFieldsAsArray($tableName,$fields,$dbADO,$filter='',$orderBy='')
{
	$res=$dbADO->execute("SELECT $fields FROM $tableName $filter $orderBy");
	$fields=str_replace('DISTINCT ','',$fields);
	$fieldsArray=explode(',',$fields);

	$result=array();
	while($row=$res->Fetchrow()){
		foreach ($fieldsArray AS $field){
			$cleanField=(strpos($field,'.')!==false)?substr($field,strpos($field,'.')+1):$field;
			$cleanField=(strpos($cleanField,' AS ')!==false)?substr($cleanField,strpos($cleanField,' AS ')+4):$cleanField;
			$cleanField=trim(str_replace('`','',$cleanField));

			$result[$cleanField][]=$row[$cleanField];
		}
	}
	return $result;	
}

function getFields($tableName,$fields,$dbADO,$filter='',$orderBy='')
{
	$res=$dbADO->execute("SELECT $fields FROM $tableName $filter $orderBy");
	$fields=str_replace('DISTINCT ','',$fields);
	$fieldsArray=explode(',',$fields);

	$result=array();
	$pos=0;
	while($row=$res->Fetchrow()){
		foreach ($fieldsArray AS $field){
			$cleanField=(strpos($field,'.')!==false)?substr($field,strpos($field,'.')+1):$field;
			$cleanField=(strpos($cleanField,' AS ')!==false)?substr($cleanField,strpos($cleanField,' AS ')+4):$cleanField;
			$cleanField=trim(str_replace('`','',$cleanField));

			$result[$pos][$cleanField]=$row[$cleanField];
		}
		$pos++;
	}
	return $result;	
}


function displayReceivers($mdID,$categArray,$dbADO)
{
	$out='';

	$templateArray=getArrayFromTable('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_DeviceCategory IN ('.join(',',$categArray).')','ORDER BY Description ASC');
	
	if(count($templateArray)==0)
		$templateArray[]=0;
	$receiverArray=getFieldsAsArray('Device','PK_Device,FK_DeviceTemplate',$dbADO,'WHERE FK_Device_ControlledVia='.$mdID.' AND FK_DeviceTemplate IN ('.join(',',array_keys($templateArray)).')');
	if(count($receiverArray)==0)
		$selectedReceiver=0;
	else
		$selectedReceiver=$receiverArray['PK_Device'][0];
	$out.='
		<input type="hidden" name="oldReceiverDT_'.$mdID.'" value="'.@$receiverArray['FK_DeviceTemplate'][0].'">
		<input type="hidden" name="oldReceiver_'.$mdID.'" value="'.$selectedReceiver.'">';
		
	$out.=pulldownFromArray($templateArray,'receiver_'.$mdID,@$receiverArray['FK_DeviceTemplate'][0],'style="width:300px;"');
	
	return $out;
}

function processReceiver($mdID,$dbADO)
{
	$oldReceiver=(int)@$_POST['oldReceiver_'.$mdID];
	$oldReceiverDT=(int)@$_POST['oldReceiverDT_'.$mdID];
	$newReceiver=(int)@$_POST['receiver_'.$mdID];
	if($oldReceiver==0){
		if($newReceiver!=0){
			createDevice($newReceiver,$_SESSION['installationID'],$mdID,NULL,$dbADO,1);
		}
	}else{
		if($newReceiver==0){
			deleteDevice($oldReceiver,$dbADO);
		}elseif($oldReceiverDT!==$newReceiver){
			deleteDevice($oldReceiver,$dbADO);
			createDevice($newReceiver,$_SESSION['installationID'],$mdID,NULL,$dbADO,1);
		}
	}
}

function getLogName($dbADO,$deviceID,$dtID,$dtDescription,$parentID=0,$isOrbiter=0,$isGSD=0,$commandLine='')
{
	if($commandLine!='' && $isGSD==0){
		$logName=$deviceID.'_'.$commandLine.'.log';
	}else{
		if($isOrbiter==0){
			if($isGSD==0){
				$logName=$deviceID.'_'.str_replace(array(' ','\''),array('_','_'),$dtDescription);
			
				$patern=array("/[ :+=()<]/","/[->*?\$\.%\/]/","/#/","/__/","/^_*/","/_*$/");
				$replacement =array("_","","","","","");
				$logName=preg_replace($patern,$replacement,$logName);
			
				// override log namimg rules for DCE router and orbiters
				$logName=($dtID==$GLOBALS['rootDCERouter'])?'DCERouter.log':$logName.'.log';
				
				// override log name for childs of DCE router - plugins
				$logName=($parentID!=0)?'DCERouter.log':$logName;
			}else{
				$logName=$deviceID.'_Generic_Serial_Device.log';
			}
		}else{
			$logName=$deviceID.'_LaunchOrbiter.sh.log';
		}
	}

	// Find PK_Device of MD if this device is running on a MD
	$MDID = 0;
	$foundDevice = false;
	while (!$foundDevice) {
	      $selectDevice='
			SELECT 
			d2.FK_Device_ControlledVia,
			d2.PK_Device as ParentPKDevice,
			dt.FK_DeviceCategory as ParentDeviceCategory
			FROM Device d1
			INNER JOIN Device d2 ON d1.FK_Device_ControlledVia=d2.PK_Device
			INNER JOIN DeviceTemplate dt ON d2.FK_DeviceTemplate=dt.PK_DeviceTemplate
			WHERE d1.PK_Device=?';
		$resDevice=$dbADO->Execute($selectDevice,array($deviceID));
		$rowDevice=$resDevice->FetchRow();
		if ($resDevice->RecordCount()>0) {
			$parentDeviceCategory=$rowDevice['ParentDeviceCategory'];
			$parentDevicePK = $rowDevice['ParentPKDevice'];
			if ($parentDeviceCategory == 8) {
			       // Device is on MD
			       $MDID = $parentDevicePK;
			       $foundDevice = true;
			} else if ($parentDeviceCategory == 7) {
		               // device is on Core
		               $found = true;
			} else if ($parentDevicePK == '') {
		       	       // no more devices up the tree
		               $foundDevice = true;
		        }
		        $deviceID = $parentDevicePK;
		} else {
		        // Unable to find computer device
			$foundDevice = true;
		}
	}
	
	

	$logName='/var/log/pluto/'.$logName;
	if ($MDID > 0) {
		//omit the Hybrid device (which is the only MD type that does not have a null entry in FK_Device_ControlledVia)
		$hybridQuery='SELECT PK_Device FROM Device WHERE PK_Device=? AND FK_Device_ControlledVia IS NULL';
		$resHybridQuery=$dbADO->Execute($hybridQuery,array($MDID));

		if ($resHybridQuery->RecordCount()>0) {
	        	$logName='/usr/pluto/diskless/'.$MDID.$logName;
		}
	}
	
	return $logName;
}

function processAudioSettings($deviceID,$dbADO)
{
	$audioSettings=($_POST['audioSettings_'.$deviceID]=='0')?'':$_POST['audioSettings_'.$deviceID];
	$AC3setting=@$_POST['ac3_'.$deviceID];
	$newAS=$audioSettings.@$AC3setting;
	$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newAS,$deviceID,$GLOBALS['AudioSettings']));

	# FIXME: Make this more generic - i.e. ANY audio device on the system, perhaps referencing the InstallWizard tables for 'default' devices.
	$xinePlayerData=getFieldsAsArray('Device Xine','Xine.PK_Device AS PK_Device',$dbADO,'
		INNER JOIN Device Orbiter ON Xine.FK_Device_ControlledVia=Orbiter.PK_Device
		INNER JOIN Device MD ON Orbiter.FK_Device_ControlledVia=MD.PK_Device
		WHERE MD.PK_Device='.$deviceID.' AND Xine.FK_DeviceTemplate='.$GLOBALS['XinePlayer']);

	$omxPlayerData=getFieldsAsArray('Device OMX','OMX.PK_Device AS PK_Device',$dbADO,'
		INNER JOIN Device Orbiter ON OMX.FK_Device_ControlledVia=Orbiter.PK_Device
		INNER JOIN Device MD ON Orbiter.FK_Device_ControlledVia=MD.PK_Device
		WHERE MD.PK_Device='.$deviceID.' AND OMX.FK_DeviceTemplate='.$GLOBALS['OMXPlayer']);

	$ac3DD=(isset($_POST['ac3_'.$deviceID]))?'Pass Through':'Stereo 2.0';
	if(isset($xinePlayerData['PK_Device'][0]) && $xinePlayerData['PK_Device'][0]!=0){
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($ac3DD,$xinePlayerData['PK_Device'][0],$GLOBALS['OutputSpeakerArrangement']));
	}elseif(isset($omxPlayerData['PK_Device'][0]) && $omxPlayerData['PK_Device'][0]!=0){
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($ac3DD,$omxPlayerData['PK_Device'][0],$GLOBALS['OutputSpeakerArrangement']));
	}else{
		$error=" ERROR: cannot find Xine or OMX player device for device #$deviceID";
		writeFile($GLOBALS['ErrorLog'],date('d-m-Y H:i:s')."$error\n\n",'a+');
	}
	return @$error;	
}

function processVideoSettings($deviceID,$dbADO)
{
	if(isset($_POST['videoSettings_'.$deviceID]) && $_POST['videoSettings_'.$deviceID]!='0'){
		$newVS=$_POST['videoSettings_'.$deviceID].'/'.$_POST['refresh_'.$deviceID];
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newVS,$deviceID,$GLOBALS['VideoSettings']));
	}
}

function isCritical($deviceID)
{
	if(in_array($deviceID,$GLOBALS['CriticalDeviceTemplates'])){
		return true;
	}
	else{
		return false;
	}
}

function formatDeviceData($deviceID,$DeviceDataArray,$dbADO,$isIPBased=0,$specificFloorplanType=0,$boolJsValidation=0,$default='input',$cssStyle='')
{
	$jsValidation='';
	$deviceDataBox='<script type="text/javascript" src="javascript/wz_tooltip/wz_tooltip.js"></script>';
	$deviceDataBox.='<table cellpadding="2" cellspacing="0" border="0" width="100%">';

	foreach ($DeviceDataArray AS $rowDDforDevice){
		if(!in_array($rowDDforDevice['FK_DeviceData'],$GLOBALS['DeviceDataToDisplay']))
			$GLOBALS['DeviceDataToDisplay'][]=$rowDDforDevice['FK_DeviceData'];

		$ddValue=$rowDDforDevice['IK_DeviceData'];

		if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='') && $rowDDforDevice['FK_DeviceData']!=''){
			$formElementName='deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'];
			$deviceDataBox.='
				<tr>
					<td align="left" valign="middle" title="'.@$rowDDforDevice['Tooltip'].'"><b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$rowDDforDevice['dd_Description']).'</b></td>
					<td align="right">';
			$itemDisabled=((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'');
			$defaultFormElement=($default=='input')?'<input type="text" name="'.$formElementName.'" value="'.@$ddValue.'" '.$itemDisabled.' class="'.$cssStyle.'" style="width:200px;">':'<textarea name="'.$formElementName.'" '.$itemDisabled.' rows="2" "class="'.$cssStyle.'" style="width:200px;">'.@$ddValue.'</textarea>';
			
			switch($rowDDforDevice['typeParam']){
				case 'int':
				if(in_array($rowDDforDevice['dd_Description'],$GLOBALS['DeviceDataLinkedToTables'])){
					$tableName=str_replace('PK_','',$rowDDforDevice['dd_Description']);
					$filterQuery='';
					$tableFields='*';
					switch($tableName){
						case 'Device':
							$filterQuery=" WHERE FK_Installation='".$installationID."'";
							$orderQuery=' ORDER BY Description ASC';
						break;
						case 'FloorplanObjectType':
							$filterQuery=($specificFloorplanType!=0)?" WHERE FK_FloorplanType='".@$specificFloorplanType."'":'';
							$orderQuery=' ORDER BY Description ASC';
						break;
						case 'Users':
							$tableFields='*, Users.UserName AS Description';
							$filterQuery='';
							$orderQuery=' ORDER BY Username ASC';
						break;
						default:
							$filterQuery='';
							$orderQuery=' ORDER BY Description ASC';
						break;
					}


					$queryTable="SELECT $tableFields FROM $tableName $filterQuery $orderQuery";
					$resTable=$dbADO->Execute($queryTable);
					$deviceDataBox.='<select name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" '.$itemDisabled.' class="'.$cssStyle.'" style="width:200px;">
												<option value="0">'.(in_array($rowDDforDevice['FK_DeviceCategory'],array($GLOBALS['StorageDevices'],$GLOBALS['RaidCategory'],$GLOBALS['NetworkStorage']))?'Public':'').'</option>';
					$deviceDataBox.=(in_array($rowDDforDevice['FK_DeviceCategory'],array($GLOBALS['StorageDevices'],$GLOBALS['RaidCategory'],$GLOBALS['NetworkStorage'])))?'<option value="-1" '.(($ddValue==-1)?'selected':'').'>Use LMCE\'s directory structure</option>':'';							
					
					while($rowTable=$resTable->FetchRow()){
						$itemStyle=($tableName=='FloorplanObjectType' && is_null(@$rowTable['FK_DesignObj_Control']))?' style="background-color:red;"':'';
						$deviceDataBox.='<option value="'.$rowTable[$rowDDforDevice['dd_Description']].'" '.(($rowTable[$rowDDforDevice['dd_Description']]==@$ddValue)?'selected':'').' '.$itemStyle.' title="'.$rowTable['Description'].'">'.$rowTable['Description'].'</option>';
					}
					$deviceDataBox.='</select>';
				}
				else{
					$deviceDataBox.=$defaultFormElement;
				}
				$jsValidation.=($boolJsValidation==1 && @$tableName!='Users')?"frmvalidator.addValidation(\"$formElementName\",\"numeric\",\"".translate('TEXT_WARNING_NUMERICAL_ONLY_CONST')."\");\n":'';
				break;
				case 'bool':
					$deviceDataBox.='<input type="checkbox" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="1" '.((@$ddValue!=0)?'checked':'').' '.$itemDisabled.'>';
				break;
				default:
					if ($rowDDforDevice['FK_DeviceData']==$GLOBALS['Port'])
					{
						$deviceDataBox.=serialPortsPulldown('deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'],$ddValue,$rowDDforDevice['AllowedToModify'],getTopLevelParent($deviceID,$dbADO),$dbADO,$deviceID,$cssStyle);
					}
					elseif ($rowDDforDevice['FK_DeviceData']==$GLOBALS['SoundCard'])
					{
						$deviceDataBox.=soundCardPulldown('deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'],$ddValue,$rowDDforDevice['AllowedToModify'],getTopLevelParent($deviceID,$dbADO),$dbADO,$deviceID,$cssStyle);
					}
					else
					{
						// if port channel, check for controlled via options
						if($rowDDforDevice['FK_DeviceData']==$GLOBALS['PortChannel']){
							$choicesArray=parentHasChoices($deviceID,$dbADO);
							if(count($choicesArray)>0){
								$formElement=pulldownFromArray($choicesArray,'deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'],$ddValue,'class="'.$cssStyle.'" style="width:200px;"');
								$deviceDataBox.=$formElement;
							}else{
								$deviceDataBox.=$defaultFormElement;
							}
						}else{
							$deviceDataBox.=$defaultFormElement;
						}
					}
			}
			$GLOBALS['mdDistro']=($rowDDforDevice['FK_DeviceData']==$GLOBALS['rootPK_Distro'])?@$ddValue:0;

			$deviceDataBox.=($itemDisabled=='disabled')?'<input type="hidden" name="isDisabled_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="1">':'';
			$deviceDataBox.='
							<input type="hidden" name="oldDeviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.$ddValue.'">';					
			unset($ddValue);	
			if($rowDDforDevice['Tooltip']=="") {
				$tooltip = translate('TEXT_NO_TOOLTIP_FOR_DEVICE_DATA_CONST');
			} else {
				$tooltip = $rowDDforDevice['Tooltip'];
			}
			$deviceDataBox.='
				</td>
					<td valign="top"><img src="img/question.gif" onmouseover="Tip(\''.trim(addslashes($tooltip)).'\',ABOVE,true,WIDTH,350,FADEIN,250,FADEOUT,250,BGCOLOR,\'#C0FFC0\')" onmouseout="UnTip()"></td>
				</tr>';
		}

	}
	if($isIPBased==1){
		$deviceDataBox.='
			<input type="hidden" name="oldIP_'.$deviceID.'" value="'.@$rowDDforDevice['IPaddress'].'">
			<input type="hidden" name="oldMAC_'.$deviceID.'" value="'.@$rowDDforDevice['MACaddress'].'">
			<tr>
				<td align="left"><B>IP</B></td>
				<td align="right"><input type="text" name="ip_'.$deviceID.'" value="'.@$rowDDforDevice['IPaddress'].'" style="width:200px;"></td>
			</tr>
			<tr>
				<td align="left"><B>MAC</B></td>
				<td align="right"><input type="text" name="mac_'.$deviceID.'" value="'.@$rowDDforDevice['MACaddress'].'" style="width:200px;"></td>
			</tr>';
		if($boolJsValidation==1){
			$jsValidation.=' frmvalidator.addValidation("mac_'.$deviceID.'","mac","'.translate('TEXT_INVALID_MAC_ADDRESS_CONST').'");';
			$jsValidation.=' frmvalidator.addValidation("ip_'.$deviceID.'","ip","'.translate('TEXT_INVALID_IP_ADDRESS_CONST').'");';
		}
	}
	$deviceDataBox.='</table>';
	
	return $deviceDataBox.'<script>'.$jsValidation.'</script>';
}

function formatNonEditDeviceData($deviceID,$DeviceDataArray,$dbADO,$isIPBased=0)
{
	$deviceDataBox='<table width="240" cellpadding="2" cellspacing="0">';

	foreach ($DeviceDataArray AS $rowDDforDevice){
		
		if(!in_array($rowDDforDevice['FK_DeviceData'],$GLOBALS['DeviceDataToDisplay']))
			$GLOBALS['DeviceDataToDisplay'][]=$rowDDforDevice['FK_DeviceData'];

		$ddValue=$rowDDforDevice['IK_DeviceData'];

		if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='')){
			$deviceDataBox.='
				<tr>
					<td width="170" align="right"><b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$rowDDforDevice['dd_Description']).'</b>&nbsp;'.((@$rowDDforDevice['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowDDforDevice['Tooltip'].'" border="0" style="vertical-align: middle;">':'').'</td>
					<td>';
			
			switch($rowDDforDevice['typeParam']){
				case 'int':
				if(in_array($rowDDforDevice['dd_Description'],$GLOBALS['DeviceDataLinkedToTables'])){
					$tableName=str_replace('PK_','',$rowDDforDevice['dd_Description']);
					$filterQuery='';
					switch($tableName){
						case 'Device':
						$filterQuery=" WHERE FK_Installation='".$installationID."'";
						break;
						case 'FloorplanObjectType':
						$filterQuery=" WHERE FK_FloorplanType='".@$specificFloorplanType."'";
					}

					$queryTable="SELECT * FROM $tableName $filterQuery WHERE ".$rowDDforDevice['dd_Description']."=?";
					$resTable=$dbADO->Execute($queryTable, array($ddValue));
					if($resTable->RecordCount()>0){
						$row=$resTable->FetchRow();
						$ddLabel=$row['Description'];
					}else{
						$ddLabel='&nbsp;';
					}
					
					$deviceDataBox.=$ddLabel;
				}
				else
					$deviceDataBox.=@$ddValue;
				break;
				case 'bool':
					$deviceDataBox.=((@$ddValue!=0)?'Y':'N');
				break;
				default:
					$deviceDataBox.=@$ddValue;
			}

			unset($ddValue);
			$deviceDataBox.='</td>
				</tr>';
		}

	}
	if($isIPBased==1){
		$deviceDataBox.='
			<tr>
				<td><B>IP</B></td>
				<td>'.$rowD['IPaddress'].'</td>
			</tr>
			<tr>
				<td><B>MAC</B></td>
				<td>'.$rowD['MACaddress'].'</td>
			</tr>';
	}
	$deviceDataBox.='</table>';
	
	return $deviceDataBox;
}

function getPipes($deviceID,$dbADO){
	$pipes=array();
	$res=$dbADO->Execute('
		SELECT 
			Device_Device_Pipe.*,
			Device.Description,
			Cin.Description AS input,
			Cout.Description AS output
		FROM Device_Device_Pipe
		INNER JOIN Device ON FK_Device_To=PK_Device 
		LEFT JOIN Command Cin ON Device_Device_Pipe.FK_Command_Input=Cin.PK_Command
		LEFT JOIN Command Cout ON Device_Device_Pipe.FK_Command_Output=Cout.PK_Command
		WHERE FK_Device_From=?',array($deviceID));
	while($row=$res->FetchRow()){
		$pipes[$row['FK_Pipe']]['to']=$row['Description'];
		$pipes[$row['FK_Pipe']]['input']=($row['input']!='')?$row['input']:'none';
		$pipes[$row['FK_Pipe']]['output']=($row['output']!='')?$row['output']:'none';
	}
	
	return $pipes;
}

function getmicrotime() 
{ 
    list($usec, $sec) = explode(" ", microtime()); 
    return ((float)$usec + (float)$sec); 
}

function getScenarios($arrayID,$installationID,$dbADO)
{
	$scenariosArray=array();
	$selectCG='
		SELECT * 
		FROM CommandGroup
		WHERE FK_Array=? AND FK_Installation=?
		ORDER BY Description ASC';
	$resCG=$dbADO->Execute($selectCG,array($arrayID,$installationID));
	while($row=$resCG->FetchRow()){
		$scenariosArray[]=$row;
	}
	
	return $scenariosArray;
}

function getUnixStamp($date)
{
	$dateArray=explode(' ',$date);
	$dateParts=explode('-',$dateArray[0]);
	$timeParts=explode(':',$dateArray[1]);

	return mktime($timeParts[0],$timeParts[1],$timeParts[2],$dateParts[1],$dateParts[2],$dateParts[0]);
}

function cleanMediaType($var)
{
	return str_replace('np_','',$var);
}

function getOSDFromMD($mdID,$dbADO)
{

	$getOrbiterChild='SELECT * FROM Device WHERE FK_DeviceTemplate = ?  AND FK_Device_ControlledVia=?';
	$resOrbiterChild=$dbADO->Execute($getOrbiterChild,array($GLOBALS['ONScreenOrbiter'],$mdID));
	if($resOrbiterChild->RecordCount()!=0){
		$rowOrbiterChild=$resOrbiterChild->FetchRow();
		return $rowOrbiterChild;
	}
	return null;
}

// return an input box if device template is not orbiter or 2 checkboxes if it is
function getStateFormElement($deviceID,$name,$State,$dbADO)
{
	$out='';
	if(isOrbiter($deviceID,$dbADO)){
		$out='
		<input type="checkbox" name="'.$name.'_app" value="1" '.((strpos($State,'APP')!==false)?'checked':'').'> Resend application to phone<br>
		<input type="checkbox" name="'.$name.'_VMC" value="1" '.((strpos($State,'VMC')!==false)?'checked':'').'> Resend VMC to phone';
	}else{
		$out.='<input type="text" name="'.$name.'" value="'.$State.'" class="input_big">';
	}
	
	return $out;
}

function getStateValue($name)
{
	$stateArray=array();
	if(isset($_POST[$name.'_app'])){
		$stateArray[]='NEED APP';
	}
	
	if(isset($_POST[$name.'_VMC'])){
		$stateArray[]='NEED VMC';
	}

	return join('|',$stateArray);
}

function getCheckedDeviceCommandGroup($dtID,$deviceCategory,$dbADO)
{
	$querySelCheckedCommandGroups = '
		SELECT 
		PK_DeviceCommandGroup,
		DeviceCommandGroup.Description,
		FK_DeviceTemplate,
		FK_Command,
		Command.Description AS Command
		FROM DeviceCommandGroup
		LEFT JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup=PK_DeviceCommandGroup AND FK_DeviceTemplate=?
		LEFT JOIN DeviceCategory ON (FK_DeviceCategory=PK_DeviceCategory OR FK_DeviceCategory=FK_DeviceCategory_Parent)
		INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_DeviceCommandGroup=PK_DeviceCommandGroup
		INNER JOIN Command ON FK_Command=PK_Command
		WHERE PK_DeviceCategory=? OR FK_DeviceCategory IS NULL
		ORDER BY Description,Command ASC';
	$resSelCheckedCommandGroups = $dbADO->Execute($querySelCheckedCommandGroups,array($dtID,$deviceCategory));
	$dcgArray=array();
	while ($row= $resSelCheckedCommandGroups->FetchRow()) {
		$dcgArray[$row['PK_DeviceCommandGroup']]['Description']=$row['Description'];
		$dcgArray[$row['PK_DeviceCommandGroup']]['Checked']=(is_null($row['FK_DeviceTemplate']))?0:1;
		$dcgArray[$row['PK_DeviceCommandGroup']]['Commands'][]=$row['Command'];
		$dcgArray[$row['PK_DeviceCommandGroup']]['CommandIDs'][]=$row['FK_Command'];
	}
	
	return $dcgArray;
}

function DeviceCommandGroupTable($dtID,$deviceCategory,$dbADO,$disabled=''){
	$dcgArray=getCheckedDeviceCommandGroup($dtID,$deviceCategory,$dbADO);
	$out='<table>';
	$oldCheckedDCG=array();
	foreach ($dcgArray AS $dcg=>$value){
		if($value['Checked']==1){
			$oldCheckedDCG[]=$dcg;
		}
		$out.='
		<tr>
			<td><input type="checkbox" name="dcg_'.$dcg.'" value="1" '.(($value['Checked']==1)?'checked':'').' '.$disabled.'></td>
			<td title="'.join(', ',$value['Commands']).'">'.$value['Description'].'</td>
		</tr>
		<input type="hidden" name="commands_'.$dcg.'" value="'.join(',',$value['CommandIDs']).'">
		';
	}
	$out.='</table>
		<input type="hidden" name="deviceCG" value="'.join(',',array_keys($dcgArray)).'" '.$disabled.'>
		<input type="hidden" name="oldCheckedDCG" value="'.join(',',$oldCheckedDCG).'">
	';
	
	return $out;
}

function reorderMultiPulldownJs()
{
	$jsFunctions='
	<script>
	function moveUp(srcList)
	{
		try{
			eval("selectedPos="+srcList+".selectedIndex");
			eval("selectedElement="+srcList+"["+srcList+".selectedIndex].value");
			eval("srcLen="+srcList+".length");
		}catch(e){
			alert("Please select the command to move.");
		}
	
	
		len=0;
		for( var i = 0; i < srcLen; i++ ) { 
			if(selectedPos==len){
				if(len>0){
					tmpText=eval(srcList+"["+(len-1)+"].text");
					tmpValue=eval(srcList+"["+(len-1)+"].value");
					
					eval(srcList+"["+(len-1)+"].text="+srcList+"["+len+"].text");
					eval(srcList+"["+(len-1)+"].value="+srcList+"["+len+"].value");

					eval(srcList+"["+len+"].text=\'"+tmpText+"\'");
					eval(srcList+"["+len+"].value=\""+tmpValue+"\"");
					
					eval(srcList+"["+len+"].selected=false");
					eval(srcList+"["+(len-1)+"].selected=true");
				}
			}
			len++;
		}	
	}

	function moveDown(srcList)
	{
		try{
			eval("selectedPos="+srcList+".selectedIndex");
			eval("selectedElement="+srcList+"["+srcList+".selectedIndex].value");
			eval("srcLen="+srcList+".length");
		}catch(e){
			alert("Please select the command to move.");
		}
	
	
		len=0;
		for( var i = 0; i < srcLen; i++ ) { 
			if(selectedPos==len){
				if(len!=(srcLen-1)){
					tmpText=eval(srcList+"["+(len+1)+"].text");
					tmpValue=eval(srcList+"["+(len+1)+"].value");
					
					eval(srcList+"["+(len+1)+"].text="+srcList+"["+len+"].text");
					eval(srcList+"["+(len+1)+"].value="+srcList+"["+len+"].value");

					eval(srcList+"["+len+"].text=\'"+tmpText+"\'");
					eval(srcList+"["+len+"].value=\""+tmpValue+"\"");
					
					eval(srcList+"["+len+"].selected=false");
					eval(srcList+"["+(len+1)+"].selected=true");
				}
			}
			len++;
		}	
	}
	
	function addToPulldown(srcList,newVal,newText){
		try{
			eval("srcLen="+srcList+".length");
		}catch(e){
			//alert("Invalid pulldown.");
		}
	
	

		alreadyInPulldown=0;
		for( var i = 0; i < srcLen; i++ ) { 
			tmpValue=eval(srcList+"["+i+"].value");
			if(tmpValue==newVal){
				alreadyInPulldown=1;
			}
		}	
		if(alreadyInPulldown==0){
			eval(srcList+".options["+srcLen+"] = new Option( \""+newText+"\", \""+newVal+"\")");
		}
	}
	
	function removeFromPulldown(srcList,delVal){
		try{
			eval("srcLen="+srcList+".length");
		}catch(e){
			//alert("Invalid pulldown.");
		}
	
	

		for( var i = 0; i < srcLen; i++ ) { 
			tmpValue=eval(srcList+"["+i+"].value");
			if(tmpValue==delVal){
				eval(srcList+".options["+i+"] = null");
			}
		}	
	}	
	</script>
	';
	
	return $jsFunctions;
}

// retrieve a matrix table with all commands and infrared groups
function getIrGroup_CommandsMatrix($dtID,$InfraredGroupsArray,$userID,$comMethod,$publicADO,$deviceID)
{
	$restrictedCommandsArray=array(194=>'Toggle power',192=>'On',193=>'Off',205=>'1',89=>'Vol Up',90=>'Vol down',63=>'Skip Fwd',64=>'Skip Back');
	$out='';
	if(count($InfraredGroupsArray)==0){
		return '';
	}

	$comMethodFilter=(!is_null($comMethod))?' AND FK_CommMethod='.$comMethod:'';	
	$codesData=getFieldsAsArray('InfraredGroup_Command','PK_InfraredGroup_Command,IRData,FK_InfraredGroup,FK_Command,InfraredGroup.Description AS IRG_Name',$publicADO,'INNER JOIN Command ON FK_Command=PK_Command INNER JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup WHERE (FK_DeviceTemplate IS NULL OR FK_DeviceTemplate='.$dtID.') AND FK_InfraredGroup IN ('.join(',',$InfraredGroupsArray).') AND FK_Command IN ('.join(',',array_keys($restrictedCommandsArray)).')'.$comMethodFilter,'ORDER BY FK_InfraredGroup ASC,FK_Command ASC');

	$irDataArray=array();
	$commandGrouped=array();
	$irgNames=array();
	for($i=0;$i<count(@$codesData['FK_InfraredGroup']);$i++){
		$irDataArray[$codesData['PK_InfraredGroup_Command'][$i]]=$codesData['IRData'][$i];
		$commandGrouped[$codesData['FK_InfraredGroup'][$i]][$codesData['FK_Command'][$i]]=$codesData['PK_InfraredGroup_Command'][$i];
		$irgNames[$codesData['FK_InfraredGroup'][$i]]=$codesData['IRG_Name'][$i];
	}
	if(session_name()=='Web-admin'){
		if($deviceID>0){
			$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia,FK_DeviceCategory',$publicADO,'INNER JOIN DeviceTemplate ON Fk_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device='.$deviceID);
		}
		$GLOBALS['DT_&_Room']=1;
		$controlledByRows='
		<tr>
			<td colspan="10" align="right">'.translate('TEXT_SEND_CODES_TO_DEVICE_CONST').': '.controlledViaPullDown('controlledVia',$deviceID,$dtID,@$deviceInfo['FK_DeviceCategory'][0],@$deviceInfo['FK_Device_ControlledVia'][0],$publicADO,'0,- Please select -','onChange="document.addModel.action.value=\'changeParent\';document.addModel.submit();"').'</td>
		</tr>';
		$GLOBALS['DT_&_Room']=0;
	}

	// display table header
	$out='
	<table cellpadding="3" cellspacing="0" border="0">
		'.@$controlledByRows.'
		<tr class="normaltext" class="tablehead">
			<td align="center"><B>Infrared Group</B></td>';
	foreach ($restrictedCommandsArray AS $cmdID=>$cmdName){
		$out.='
			<td align="center"><B>'.$cmdName.'</B></td>';
	}
	$out.='
			<td><B>'.translate('TEXT_ACTION_CONST').'</B></td>
		</tr>';

	// display codes/commands
	$keysArray=array_keys($commandGrouped);
	for($i=0;$i<count($commandGrouped);$i++){
		$color=($i%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr class="normaltext" bgcolor="'.$color.'">
			<td><B><a href="index.php?section=irCodes&dtID='.$dtID.'&irGroup='.$keysArray[$i].'&deviceID='.@$_REQUEST['deviceID'].'&action=pick_code&resync=true">'.$irgNames[$keysArray[$i]].'</a></B></td>';
		foreach ($restrictedCommandsArray AS $cmdID=>$cmdName){
			$pk_irgc=@$commandGrouped[$keysArray[$i]][$cmdID];
			$testCodeBtn=(session_name()=='Web-admin' && isset($pk_irgc))?' <input type="button" class="button" name="testCode" value="T" 
			onClick="frames[\'codeTester\'].location=\'index.php?section=testCode&irData=\'+escape(\''.urlencode($irDataArray[$pk_irgc]).'\')+\'&deviceID='.@$_REQUEST['deviceID'].'&sender='.urlencode('addModel&step=7').'\';">':'';
			$out.='<td align="center">'.((isset($commandGrouped[$keysArray[$i]][$cmdID]))?'<input type="button" class="button" name="copyCB" value="V" onClick="window.open(\'index.php?section=displayCode&irgcID='.$pk_irgc.'\',\'_blank\',\'\');">'.$testCodeBtn:'N/A').'</td>';
		}
		$out.='
			<td><input type="button" class="button" name="btn" onClick="self.location=\'index.php?section=irCodes&action=update&dtID='.$dtID.'&irGroup='.$keysArray[$i].'&deviceID='.@$_REQUEST['deviceID'].'&resync=true\';" value="'.translate('TEXT_THIS_WORKS_CONST').'"></td>
		</tr>';
	}
	
	$out.='
	</table>';
	
	
	return $out;
}


function pickDeviceTemplate_old($categoryID, $boolManufacturer,$boolCategory,$boolDeviceTemplate,$returnValue,$defaultAll,$section,$firstColText,$dbADO,$useframes=0,$genericSerialDevicesOnly=0)
{
	$redirectUrl='index.php?section='.$section;
	$from=@$_REQUEST['from'];
	
	global $dbPlutoMainDatabase;
	$out='
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>';
	
	$selectedManufacturer = (int)@$_REQUEST['manufacturers'];
	$selectedDeviceCateg= (int)@$_REQUEST['deviceCategSelected'];
	$selectedDevice= (int)@$_REQUEST['deviceSelected'];
	$selectedModel= (int)@$_REQUEST['model'];
	$justAddedNode = (int)@$_GET['justAddedNode'];

	$actionX = (isset($_REQUEST['actionX']) && cleanString($_REQUEST['actionX'])!='null')?cleanString($_REQUEST['actionX']):'form';
	
	$manufacturersArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$dbADO,'','ORDER BY Description ASC');
	$categsArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$dbADO,'WHERE PK_DeviceCategory='.$selectedDevice,'ORDER BY Description ASC');
	$selectedDeviceCategName=@$categsArray[$selectedDevice];
	$formName=str_replace('operations/','',$section);	
	
	if ($actionX=='form') {
	
		$AVDeviceCategories=getDeviceCategoryChildsArray($GLOBALS['rootAVEquipment'],$dbADO);
		$jsTree = '';
		if(is_null($categoryID))
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent IS NULL ORDER BY Description";
		else 
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent ='$categoryID' ORDER BY Description";
		$rs = $dbADO->_Execute($selectDeviceCategories);
			while ($row = $rs->FetchRow()) {		
				$jsTree.='
				auxS'.$row['PK_DeviceCategory'].' = insFld(foldersTree, gFld("'.$row['Description'].' #'.$row['PK_DeviceCategory'].'", "javascript:setPicker('.$row['PK_DeviceCategory'].',\"'.htmlspecialchars($row['Description'],ENT_QUOTES ).'\");"));
				auxS'.$row['PK_DeviceCategory'].'.xID = '.$row['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row['PK_DeviceCategory'],$dbADO);
			}
		$rs->Close();
		
		
		$selectModels = '';
		$modelsJsArray = '
				modelsArray = new Array();
		';

		$where = " AND 1=1 ";
		$manufOrDevice = 0;
		if ($selectedManufacturer!=0) {	
			$where.=" AND FK_Manufacturer = '$selectedManufacturer'";
			$manufOrDevice = 1;
		}

		if ($selectedDevice!=0) {
			if ($selectedDeviceCateg==0) {
				$GLOBALS['childsDeviceCategoryArray'] = array();
				
				getDeviceCategoryChildsArray($selectedDevice,$dbADO);
				$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
				$GLOBALS['childsDeviceCategoryArray'][]=$selectedDevice;
				
				$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
				
				$where.=" and FK_DeviceCategory in ($childArray)";
			} else {		
				$where .= " and FK_DeviceCategory = '$selectedDevice'";	
			}
			$manufOrDevice = 1;
		}elseif(!is_null($categoryID)){
			$GLOBALS['childsDeviceCategoryArray'] = array();
				
			getDeviceCategoryChildsArray($categoryID,$dbADO);
			$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
			$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;
			
			$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
			$where.=" and FK_DeviceCategory in ($childArray)";
		}		
		
		$where.=(($genericSerialDevicesOnly==1)?' AND CommandLine=\''.$GLOBALS['GenericSerialDeviceCommandLine'].'\'':'');

		if ($manufOrDevice || $defaultAll==1)	{
		
			$queryModels = "SELECT DeviceTemplate.*
							FROM DeviceTemplate 
							WHERE 1=1 $where ORDER BY Description";	
			$rs = $dbADO->_Execute($queryModels);
			
			if(ereg('parentID',@$_SESSION['from']) && @$_SESSION['parentID']!=0){
				$allowedDevices=getDeviceTemplatesControlledBy($_SESSION['parentID'],$dbADO);
			}
			$arJsPos=0;
				$avDT=getAssocArray('DeviceTemplate','PK_DeviceTemplate','InfraredGroup.FK_CommMethod AS FK_CommMethod',$dbADO,'LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup');
				while ($row = $rs->FetchRow()) {
					$irHighlight=(in_array($row['PK_DeviceTemplate'],array_keys($avDT)) && $avDT[$row['PK_DeviceTemplate']]==1)?'style="background-color:#FFDFDF;"':'';
					if(count(@$allowedDevices)==0 || (count(@$allowedDevices)>0 && in_array($row['PK_DeviceTemplate'],@$allowedDevices))){
						$selectModels.="<option ".($selectedModel==$row['PK_DeviceTemplate']?" selected ":"")." value='{$row['PK_DeviceTemplate']}' ".(($row['IsPlugAndPlay']==1)?'style="background-color:lightgreen;"':'')." $irHighlight>{$row['Description']}</option>";
					}
					
					if ($row['PK_DeviceTemplate']>0) {
					$modelsJsArray.='
					
						model'.$row['PK_DeviceTemplate'].' = new Array();
					    model'.$row['PK_DeviceTemplate'].'.userID = \''.@$_SESSION['userID'].'\';
						modelsArray['.$row['PK_DeviceTemplate'].'] = model'.$row['PK_DeviceTemplate'].';			
						';
					}
					$arJsPos++;
				}
			$rs->Close();
		
		
		}
			
		
		$scriptInHead = "
		</style>
		
		<!-- As in a client-side built tree, all the tree infrastructure is put in place
		     within the HEAD block, but the actual tree rendering is trigered within the
		     BODY -->
		
		<!-- Code for browser detection -->
		<script src=\"scripts/treeview/treeview/ua.js\"></script>
		
		<!-- Infrastructure code for the tree -->
		<script src=\"scripts/treeview/ftiens4.js\"></script>
		
		<!-- Execution of the code that actually builds the specific tree.
		     The variable foldersTree creates its structure with calls to
			 gFld, insFld, and insDoc -->
		<script>
		cssClass='normaltext'
		USETEXTLINKS = 1
		STARTALLOPEN = 0
		USEFRAMES = $useframes
		USEICONS = 0
		WRAPTEXT = 1
		PRESERVESTATE = 1
		ICONPATH = 'scripts/treeview/' 
		HIGHLIGHT = 1
		
		";
		if(is_null($categoryID)){
			$scriptInHead.="
				foldersTree = gFld('<b>Device Category</b>', 'javascript:setPicker(0,\"\");');";
		}else{
			$queryCategoryName='SELECT Description FROM DeviceCategory WHERE PK_DeviceCategory=?';
			$resCategoryName=$dbADO->Execute($queryCategoryName,$categoryID);
			$rowCategoryName=$resCategoryName->FetchRow();
			$scriptInHead.="
				foldersTree = gFld('<b>".$rowCategoryName['Description']."</b>', 'javascript:setPicker($categoryID,\"".htmlspecialchars($rowCategoryName['Description'],ENT_QUOTES)."\");');";
		}
		$scriptInHead.="
		foldersTree.xID = 1001635872;
		$jsTree
		
		foldersTree.treeID = 't2'
		
		function getObj(obj) {
				if (document.layers) {
					if (typeof obj == 'string') {
						return document.layers[obj];
					} else 	{
						return obj;
					}
				}
				if (document.all) {
					if (typeof obj == 'string') {
						return document.all(obj);
					} else {
						return obj;
					}
				}

				return null;	
			} 

			function showHideObject(obj) {
				obj = getObj(obj);					
				if(document.all || getObj) {  			
						if (obj.style.visibility == \"visible\") {
							obj.style.visibility = \"hidden\";
							obj.style.display = \"none\";
						} else {
						    obj.style.visibility = \"visible\";
							obj.style.display = \"block\";
						}
				} else if (document.layers) {
						if (obj.style.visibility == \"visible\") {
							obj.visibility = \"hide\";	
						} else {
						obj.visibility = \"show\";	
						}
				}
			} 
		
		function setPicker(val,descr){
			if(document.$formName.manufacturers.selectedIndex!=0 && document.$formName.manufacturers.selectedIndex!=-1){
				document.getElementById('manufInfo').innerHTML=document.$formName.manufacturers.options[document.$formName.manufacturers.selectedIndex].text;
			}

			if(val!='manufacturers'){
				document.getElementById('categInfo').innerHTML=descr;
				document.$formName.deviceSelected.value=val;
			}

			document.$formName.actionX.value='form';
			document.$formName.submit();
		}

		function addModel(){
			if(document.$formName.deviceSelected.value!=0 && document.$formName.manufacturers.selectedIndex!=0 && document.$formName.manufacturers.selectedIndex!=-1){
				self.location='index.php?section=addModel&mID='+document.$formName.manufacturers[document.$formName.manufacturers.selectedIndex].value+'&dcID='+document.$formName.deviceSelected.value+'&step=1&from=$from';
			}
		}
		
		
		$modelsJsArray
		
		</script>
		";
		if ($selectedManufacturer!=0 && $selectedDevice!=0) {
			$disabledAdd=' ';
		} else {
			$disabledAdd=' disabled="disabled" ';
		}

		$out.=$scriptInHead;
		$out.='
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>	
		<div class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>	
		
		<form action="index.php" method="POST" name="'.$formName.'">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="actionX" value="add">
		<input type="hidden" name="deviceCategSelected" value="'.$selectedDeviceCateg.'">
		<input type="hidden" name="deviceSelected" value="'.$selectedDevice.'">
		<input type="hidden" name="modelSelected" value="'.$selectedModel.'">
		<input type="hidden" name="allowAdd" value="'.(int)@$_REQUEST['allowAdd'].'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="parmToKeep" value="'.@$_REQUEST['parmToKeep'].'">
		
		<table cellpadding="5" cellspacing="0" border="0" align="center">
				<input type="hidden" name="selectedField" value="" />';
				if($returnValue!=0){
					$out.='
					<tr>
						<td colspan="3">'.translate('TEXT_ADD_DEVICE_INFO_CONST').'</td>
					</tr>';
				}
				if($boolDeviceTemplate==1 && $disabledAdd!=''){
					$out.='
					<tr>
						<td colspan="3" class="normaltext">'.translate('TEXT_ADD_DEVICE_TEMPLATE_INFO_CONST').' <input type="button" class="button" name="addDeviceTemplate" value="Add"'. $disabledAdd .'  onClick="javascript:addModel();"/></td>
					</tr>';
				}
				$out.='		
					<tr>
						<th width="25%">'.translate('TEXT_MANUFACTURERS_CONST').'</th>
						<th width="25%">'.translate('TEXT_DEVICE_CATEGORY_CONST').'</th>
						<th width="25%">
						'.translate('TEXT_MODELS_CONST').'
						
						</th>				
					</tr>
					<tr valign="top">
						<td width="25%" align="center"  valign="top">
							<span id="manufInfo" class="confirm">&nbsp;'.@$manufacturersArray[$selectedManufacturer].'</span><br>
							'.pulldownFromArray($manufacturersArray,'manufacturers',$selectedManufacturer,'multiple onchange="setPicker(\'manufacturers\',\'\');" size="10" style="z-index:1;"').'<br /><br />';
					if($boolManufacturer==1 && isset($_SESSION['userID'])){
						$out.='
							<input type="text" name="Manufacturer_Description" size="15" />
							<input type="submit" class="button" name="addManufacturer" value="'.translate('TEXT_ADD_CONST').'"  />';
					}
					$out.=$firstColText.'
						</td>
						<td width="25%" align="left" valign="top">
							<span id="categInfo" class="confirm">&nbsp;'.@$selectedDeviceCategName.'</span><br>
							<span style="display:none;">
								<table border=0>
									<tr>
										<td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td>
									</tr>
								</table>
							</span>
							<span>
							<script>
								initializeDocument();						
							</script>
							<noscript>
							A tree for site navigation will open here if you enable JavaScript in your browser.
							</noscript>
							</span>
							<br /><br />';
						if($boolCategory==1){
							$out.='
							<input type="text" name="DeviceCategory_Description" size="15" />
							<input type="submit" class="button" name="addDeviceCategory"   value="Add '.($selectedDevice==0?' Top Level Child':' Child').'" />
							'.($selectedDevice!=0?'<input type="button" class="button" name="editDeviceCategory" value="Edit" onClick="javascript: windowOpen(\'index.php?section=editDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=600,height=250,toolbar=1\');" />':'<input   type="submit" class="button" name="editDeviceCategory" value="Edit" disabled="disabled" />').'';
							getDeviceCategoryChildsNo($selectedDevice,$dbADO);					
							$childsToDelete = $GLOBALS['childsDeviceCategoryNo'];
							$out.='
							&nbsp;&nbsp;'.($selectedDevice!=0?'<input type="button" class="button" name="deleteDeviceCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) self.location=\'index.php?section='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'&actionX=del\';" />':'<input type="submit" class="button" name="deleteDeviceCategory"   value="Delete" disabled="disabled" />');
						}
						$out.='
						</td>
						<td width="25%" align="center"  valign="top" class="normaltext">
							<script>
								function checkEdit(targetSection) {
									if(document.'.$formName.'.model.selectedIndex!=0){							
										//self.location=\'index.php?section=editMasterDevice&model=\'+document.'.$formName.'.model[document.'.$formName.'.model.selectedIndex].value;
										windowOpen(\'index.php?section=editMasterDevice&model=\'+document.'.$formName.'.model[document.'.$formName.'.model.selectedIndex].value,\'width=1024,height=768,scrollbars=1,resizable=1,fullscreen=1\');
									}
								}
		
								function updateModelDetail(byItem) {
									objF=document.'.$formName.';
									eval("tmpFlag=objF."+byItem+".selectedIndex");
									eval("targetObj=objF."+byItem);
									eval("targetArray="+byItem+"sArray");
									if (tmpFlag!= "undefined") {
										document.getElementById(\'modelManuf\').innerHTML = \'<br />Manufacturer: \'+targetArray[targetObj[tmpFlag].value].manufacturerName;
										document.getElementById(\'modelDeviceDescription\').innerHTML = \'<br />Category: \'+targetArray[targetObj[tmpFlag].value].deviceDescription;
									} else {
										document.getElementById(\'modelManuf\').innerHTML = \'\';
										document.getElementById(\'modelDeviceDescription\').innerHTML = \'\';
									} 					
								}

							</script>
							<select name="model" id="model" size="10">
									<option value="" selected="selected">'.(($section=='deviceTemplatePicker')?'- Please select -':'All').'</option>
									'.$selectModels.'	
							</select>';
							if($returnValue==0){
								$out.='<br><input type="button" class="button" name="edit_DeviceTemplate" value="Show Model" onClick="javascript:checkEdit(\'model\');" />';
							}else{
								$out.='<br><input type="button" class="button" name="pickDT" value="Add device" onClick="opener.location=\'index.php?section='.$_SESSION['from'].'&deviceTemplate=\'+document.'.$section.'.model[document.'.$section.'.model.selectedIndex].value+\'&action=add&add=1&'.@$_REQUEST['parmToKeep'].'\';self.close();" />';
							}
							$out.='
							<hr />
							<em>* '.translate('TEXT_MODELS_COLOR_INFO_CONST').'</em><br>
						<b><span id="modelManuf"></span><span id="modelDeviceDescription"></span></b><br /><input type="button" class="button" name="close" value="'.translate('TEXT_CLOSE_CONST').'" onClick="self.close();">
							
							';
							
					
					
							$disabled_str = "disabled";
				
				
				$out.='
						</td>				
					</tr>';

				$out.='
				</table>
							
		</form>
		';
	}elseif($actionX=='del'){
		deleteDC($selectedDevice,$dbADO);
		$out.="
			<script>
				self.location='$redirectUrl&manufacturers=$selectedManufacturer&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&msg=Device category deleted.';
			</script>";
	} else {
		
		 $addDeviceCategory = @cleanString($_POST['addDeviceCategory']);
		 $deviceCategoryDesc = cleanString(@$_POST['DeviceCategory_Description']);
		 
		 $addDeviceTemplate = @cleanString($_POST['addDeviceTemplate']);
		 $DeviceTemplate_Desc = @cleanString($_POST['DeviceTemplate_Description']);
	
		 $addManufacturer = @cleanString($_POST['addManufacturer']);
		 $Manufacturer_Description = @cleanString($_POST['Manufacturer_Description']);	 
		
		 $deviceCategSelected = (int)$_POST['deviceCategSelected'];
		 $deviceSelected = (int)$_POST['deviceSelected'];
		 $manufacturerSelected = (int)@$_POST['manufacturers'];
	
		 // process form only if user is logged
		 if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']===true) {
	
			 // add new category
			 $justAddedNode = 0;
			 if (strstr($addDeviceCategory,'Add')) {
			 	if ($deviceCategoryDesc!='') {
			 		$queryInsertCategoryDesc = "insert into DeviceCategory(FK_DeviceCategory_Parent,Description,IsAV)
			 				values(?,?,0)";
			 		$deviceCategParent=($deviceSelected==0)?$categoryID:$deviceSelected;
			 		$dbADO->Execute($queryInsertCategoryDesc,array($deviceCategParent,$deviceCategoryDesc));	 			
			 		$justAddedNode = $deviceSelected;
			 		header("Location: $redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");	 			 		
				}
			 }	
			 	 
			 //add new master device list
			 if (strstr($addDeviceTemplate,'Add')) {
			 	
			 	if ($DeviceTemplate_Desc!='') {
			 		if ($deviceSelected!=0 && $manufacturerSelected!=0) {	 			
			 			$queryInsertMasterDevice = 'INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,Define) values(?,?,?,"")';
			 			$res = $dbADO->Execute($queryInsertMasterDevice,array($DeviceTemplate_Desc,$deviceSelected,$manufacturerSelected));	 			
			 			$dtID=$dbADO->Insert_ID();

			 			if(in_array($deviceSelected,$GLOBALS['TVdevicesArray'])){
							$openTunerConfig='windowOpen(\'index.php?section=tunerConfig&categoryID='.$deviceSelected.'&dtID='.$dtID.'\',\'width=600,height=400,toolbar=1,scrollbars=1,resizable=1\')';
						}else
							$openTunerConfig='';

		 				$out.='
		 				
							<script>
								'.$openTunerConfig.'
								self.location="index.php?section='.$section.'&manufacturers='.$manufacturerSelected.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'&allowAdd='.$boolDeviceTemplate.'&justAddedNode='.$justAddedNode.'";
							</script>';
		 			
			 		}	 			 		
			 	}
			 }
			 
			 // add new manufacturer
			 if (strstr($addManufacturer,'Add')){
			 	if ($Manufacturer_Description!='') {
		 			$queryInsertManufacturer = 'INSERT INTO Manufacturer (Description) values(?)';
		 			$res = $dbADO->Execute($queryInsertManufacturer,array($Manufacturer_Description));	 			
		 			header("Location: $redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");
		 		}	 			 		
			 }
			 
			$out.="
				<script>
					self.location='$redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode';
				</script>";
		}else{
			$out.="
				<script>
					self.location='$redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&error=Please login if you want to change device templates.';
				</script>";
		}
	}
	return $out;
}

function getMediaTypeCheckboxes($dtID,$publicADO,$mediaADO,$deviceID)
{
	$mediaTypeCheckboxes='';
	$resMT=$publicADO->Execute('
		SELECT MediaType.Description, PK_MediaType, FK_DeviceTemplate 
		FROM MediaType 
		LEFT JOIN DeviceTemplate_MediaType ON FK_MediaType=PK_MediaType AND FK_DeviceTemplate=?
		WHERE DCEaware=0',array($dtID));
	$displayedMT=array();
	$checkedMT=array();
	while($rowMT=$resMT->FetchRow()){
		$checked='';
		$displayedMT[]=$rowMT['PK_MediaType'];
		$puldown='&nbsp;';
		if($rowMT['FK_DeviceTemplate']!=''){
			$checked='checked';
			$checkedMT[]=$rowMT['PK_MediaType'];

			$mediaProvidersArray=getAssocArray('MediaProvider','PK_MediaProvider','Description',$mediaADO,'WHERE EK_MediaType='.$rowMT['PK_MediaType']);
			if($deviceID!=0){
				$ddMediaProvider=getFieldsAsArray('Device_DeviceData','IK_DeviceData',$publicADO,'WHERE FK_Device='.$deviceID.' AND FK_DeviceData='.$GLOBALS['MediaProvider']);
				$ddParts=explode(',',$ddMediaProvider['IK_DeviceData'][0]);
				$currentMediaProvider=array();
				foreach ($ddParts AS $mpForMediaType){
					$mpParts=explode(':',$mpForMediaType);
					@$currentMediaProvider[$mpParts[0]]=@$mpParts[1];
				}
				if(count($mediaProvidersArray)==0){
					$puldown='&nbsp;';
				}else{
					$puldown=pulldownFromArray($mediaProvidersArray,'mediaProvider_'.$rowMT['PK_MediaType'],@$currentMediaProvider[$rowMT['PK_MediaType']],'style="width:200px;"');
				}
			}
		}
			
		$mediaTypeCheckboxes.='<input type="checkbox" name="mediaType_'.$rowMT['PK_MediaType'].'" '.@$checked.'>'.str_replace('np_','',$rowMT['Description']).' '.@$puldown;
		$mediaTypeCheckboxes.=((count($displayedMT))%5==0)?'<br>':'';
	}
	$mediaTypeCheckboxes.='
	<input type="hidden" name="displayedMT" value="'.join(',',$displayedMT).'">
	<input type="hidden" name="checkedMT" value="'.join(',',$checkedMT).'">';
	
	return $mediaTypeCheckboxes;
}

// extract codes and put them in a multi dimensional array
function extractCodesTree($infraredGroupID,$dtID,$dbADO,$restriction=''){
	global $inputCommandsArray, $dspmodeCommandsArray,$powerCommands;
	$installationID=(int)$_SESSION['installationID'];

	$userID=(int)@$_SESSION['userID'];

	if($infraredGroupID==0){
		return array();
	}
	$codesQuery='
			SELECT 
				PK_InfraredGroup_Command,
				IRData,
				Command.Description,
				OriginalKey,
				IF(PK_Command=192 OR PK_Command=193 OR PK_Command=194,1, IF(FK_CommandCategory=22,2, IF(FK_CommandCategory=27,3, IF(FK_CommandCategory=21,4,5) ) ) ) AS GroupOrder,
				CommandCategory.Description AS CommandCategory,
				FK_Command 
			FROM InfraredGroup_Command 
			JOIN Command ON FK_Command=PK_Command JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory 
			WHERE FK_InfraredGroup=? '.$restriction.'
			ORDER BY GroupOrder,CommandCategory.Description,Description';

	$res=$dbADO->Execute($codesQuery,array($infraredGroupID));
	$codesArray=array();
	while($row=$res->FetchRow()){
		$categoryLabel=($row['CommandCategory']=='Generic')?'Power':$row['CommandCategory'];
		// move input select commands to inputs category
		$categoryLabel=($row['FK_Command']==$GLOBALS['inputSelectCommand'])?'Inputs':$categoryLabel;
		// move DSP mode commands to DSP mode
		$categoryLabel=($row['FK_Command']==$GLOBALS['DSPModeCommand'])?'DSP Modes':$categoryLabel;
		
		$codesArray[$categoryLabel]['PK_IGC'][$row['PK_InfraredGroup_Command']]=$row['PK_InfraredGroup_Command'];
		$codesArray[$categoryLabel]['Description'][$row['PK_InfraredGroup_Command']]=$row['Description'];
		$codesArray[$categoryLabel]['OriginalKey'][$row['PK_InfraredGroup_Command']]=$row['OriginalKey'];
		$codesArray[$categoryLabel]['CommandCategory'][$row['PK_InfraredGroup_Command']]=$categoryLabel;
		$codesArray[$categoryLabel]['FK_Command'][$row['PK_InfraredGroup_Command']]=$row['FK_Command'];
		$codesArray[$categoryLabel]['IRData'][$row['PK_InfraredGroup_Command']]=$row['IRData'];
		$GLOBALS['displayedIRGC'][]=$row['PK_InfraredGroup_Command'];
		$GLOBALS['displayedCommands'][]=$row['FK_Command'];

		// remove input commands from input array if they are implemented
		if(in_array($row['FK_Command'],array_keys($inputCommandsArray))){
			unset ($inputCommandsArray[$row['FK_Command']]);
		}

		// remove DSP mode commands from input array if they are implemented
		if(in_array($row['FK_Command'],array_keys($dspmodeCommandsArray))){
			unset ($dspmodeCommandsArray[$row['FK_Command']]);
		}
		
		// remove power commands from power commands array if they are implemented
		if(in_array($row['FK_Command'],array_keys($powerCommands))){
			unset ($powerCommands[$row['FK_Command']]);
		}
	}

	return $codesArray;
}

// parse the multidimensiona array with codes and return html code for table rows
function getCodesTableRows($section,$infraredGroupID,$dtID,$deviceID,$codesArray,$togglePower,$toggleInput,$toggleDSP){
	global $inputCommandsArray, $dspmodeCommandsArray,$powerCommands;
	//print_array($codesArray);
	$categNames=array_keys($codesArray);
	if(!in_array('Power',$categNames)){
		$categNames[]='Power';
	}	
	if(!in_array('Inputs',$categNames)){
		$categNames[]='Inputs';
	}
	if(!in_array('DSP Modes',$categNames)){
		$categNames[]='DSP Modes';
	}

	$newCodeSection=($section=='rubyCodes')?'newRubyCode':'newIRCode';
	
	$out='';
	for($i=0;$i<count($categNames);$i++){
		$out.='
			<tr>
				<td colspan="4" align="center">
					<fieldset style="width:98%">
						<legend><B>'.$categNames[$i].'</B></legend>';
		if($categNames[$i]=='Power'){
			if($togglePower==1){
				if(!@in_array(194,$codesArray['Power']['FK_Command'])){
					$out.='Toggle power not implemented: ';
					$out.='<b><a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID=194&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');">Click to add</a>';
				}
			}elseif(count($powerCommands)>0){
				$out.='Power commands not implemented: ';
				foreach ($powerCommands AS $inputId=>$inputName){
					$powerCommands[$inputId]='<a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$inputId.'&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');">'.$inputName.'</a>';
				}
				$out.='<b>'.join(', ',$powerCommands).'</b> <em>(Click to add)</em>';
			}
		}
				
		// display input commands not implemented
		if($categNames[$i]=='Inputs'){
			if($toggleInput==1){
				if(@!in_array($GLOBALS['inputSelectCommand'],$codesArray['Inputs']['FK_Command'])){
					$out.='Input select not implemented: ';
					$out.='<b><a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$GLOBALS['inputSelectCommand'].'&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');">Click to add</a>';
				}
			}elseif(count($inputCommandsArray)>0){
				$out.='Input commands not implemented: ';
				foreach ($inputCommandsArray AS $inputId=>$inputName){
					$inputCommandsArray[$inputId]='<a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$inputId.'&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');">'.$inputName.'</a>';
				}
				$out.='<b>'.join(', ',$inputCommandsArray).'</b> <em>(Click to add)</em>';
			}
		}

		// display DSP modes commands not implemented
		if($categNames[$i]=='DSP Modes'){
			if($toggleDSP==1){
				if(@!in_array($GLOBALS['DSPModeCommand'],@$codesArray['DSP Modes']['FK_Command'])){
					$out.='DSP Mode not implemented: ';
					$out.='<b><a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$GLOBALS['DSPModeCommand'].'&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');">Click to add</a>';
				}
			}elseif(count($dspmodeCommandsArray)>0){
				$out.='DSP Mode commands not implemented: ';
				foreach ($dspmodeCommandsArray AS $dspmId=>$dspmName){
					$dspmodeCommandsArray[$dspmId]='<a href="javascript:windowOpen(\'index.php?section='.$newCodeSection.'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$dspmId.'&action=sendCommand\',\'width=750,heighght=310,toolbar=1,scrollbars=1,resizable=1\');">'.$dspmName.'</a>';
				}
				$out.='<b>'.join(', ',$dspmodeCommandsArray).'</b> <em>(Click to add)</em>';
			}
		}

		if(isset($codesArray[$categNames[$i]]['PK_IGC'])){
			for($pos=0;$pos<count($codesArray[$categNames[$i]]['PK_IGC']);$pos++){
				$codeCommandsKeys=array_keys($codesArray[$categNames[$i]]['PK_IGC']);
				$irg_c=$codeCommandsKeys[$pos];
				$out.=formatCode($section,$codesArray[$categNames[$i]],$irg_c,$infraredGroupID,$dtID,$deviceID);
			}
		}

		$out.='
					</fieldset>
				</td>
			</tr>	
			';
	}
	
	return $out;
}


function formatCode($section,$dataArray,$pos,$infraredGroupID,$dtID,$deviceID){

	$deleteButton='<input type="button" class="button_fixed" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.'.$section.'.action.value=\'delete\';document.'.$section.'.irgroup_command.value='.$pos.';document.'.$section.'.submit();}">';
	$clearButton ='<input type="button" class="button_fixed" name="clearCustomCode" value="Clear code" onClick="if(confirm(\'Are you sure you want to clear this code?\')){document.'.$section.'.action.value=\'clear\';document.'.$section.'.irgroup_command.value='.$pos.';document.'.$section.'.submit();}">';
	
	$viewParamsButton=($section=='rubyCodes')?'<input type="button" class="button_fixed" name="viewParams" value="View params" onClick="windowOpen(\'index.php?section=editCommand&from=rubyCodes&commandID='.$dataArray['FK_Command'][$pos].'\',\'width=600,height=400,toolbar=1,resizable=1,scrollbars=1\');"><br>':'';
	$extendedEditorBtn=($section=='rubyCodes')?'<input type="button" class="button_fixed" name="extendedEditor" value="Extended Editor" onClick="windowOpen(\'index.php?section=editCode&from=rubyCodes&irgcID='.$pos.'\',\'width=800,height=600,toolbar=1,resizable=1,scrollbars=1\');"><br>':'';
	$testButton=((int)$deviceID!=0 && $section !='rubyCodes')?'<br> <input type="button" class="button_fixed" name="testCode" value="Test code" onClick="frames[\'codeTester\'].location=\'index.php?section=testCode&irData=\'+escape(document.'.$section.'.irData_'.$pos.'.value)+\'&deviceID='.$deviceID.'&sender='.$section.'\';"> <a name="test_'.$pos.'">':'';
	
	$out='
		<table width="100%">
			<tr class="alternate_back">
				<td align="center" width="100"><a name="code'.$pos.'"></a><B>#'.$dataArray['FK_Command'][$pos].' '.$dataArray['Description'][$pos].(($dataArray['OriginalKey'][$pos]!='')?' ('.$dataArray['OriginalKey'][$pos].')':'').'</B> <br><input type="button" class="button" name="learnCode" value="New code" onClick="windowOpen(\'index.php?section='.(($section=='rubyCodes')?'newRubyCode':'newIRCode').'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$dataArray['FK_Command'][$pos].'&action=sendCommand\',\'width=750,height=310,toolbar=1,scrollbars=1,resizable=1\');" '.((!isset($_SESSION['userID']))?'disabled':'').'></td>
				<td><textarea name="irData_'.$pos.'" rows="2" style="width:100%">'.$dataArray['IRData'][$pos].'</textarea></td>
				<td align="center" width="120">'.$viewParamsButton.$extendedEditorBtn.@$deleteButton.$clearButton.$testButton.'</td>
			</tr>
		</table>';

	return $out;

}

// create an embedded device template for selected device template 
function createEmbeddedDeviceTemplate($name,$manufacturer,$deviceCategory,$userID,$parentID,$commandID,$mediaType,$publicADO){
	$publicADO->Execute('
		INSERT INTO DeviceTemplate 
			(Description,FK_Manufacturer,FK_DeviceCategory,Define) 
		VALUES 
			(?,?,?,"")',
	array($name,$manufacturer,$deviceCategory));
	$embeddedID=$publicADO->Insert_ID();

	$publicADO->Execute('
		INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
			(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
		VALUES
			(?,?,1,1)',
	array($embeddedID,$parentID));
	$insertID=$publicADO->Insert_ID();

	$publicADO->Execute('
		INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
			(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
		VALUES 
			(?,?,?)',
	array($insertID,1,$commandID));
	
	$publicADO->Execute('
		INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
			(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
		VALUES 
			(?,?,?)',
	array($insertID,2,$commandID));
	
	$publicADO->Execute('
		INSERT INTO DeviceTemplate_Input 
			(FK_DeviceTemplate,FK_Command) 
		VALUES 
			(?,?)',
	array($embeddedID,$commandID));
	
	if(!is_null($mediaType)){
		$publicADO->Execute('
			INSERT INTO DeviceTemplate_MediaType 
				(FK_DeviceTemplate,FK_MediaType) 
			VALUES 
				(?,?)',
		array($embeddedID,$mediaType));
	}

}

function testScenario($scenarioID){
	
	$commandToSend='/usr/pluto/bin/MessageSend localhost 0 0 10 '.$scenarioID;
	exec($commandToSend);
}

function deviceDataElement($name,$rowDDforDevice,$dbADO)
{

	$ddValue=$rowDDforDevice['IK_DeviceData'];

	if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='')){
		switch($rowDDforDevice['typeParam']){
			case 'int':
			if(in_array($rowDDforDevice['dd_Description'],$GLOBALS['DeviceDataLinkedToTables'])){
				$tableName=str_replace('PK_','',$rowDDforDevice['dd_Description']);
				$filterQuery='';
				switch($tableName){
					case 'Device':
					$filterQuery=" WHERE FK_Installation='".$installationID."'";
					break;
					case 'FloorplanObjectType':
					$filterQuery=" WHERE FK_FloorplanType='".@$specificFloorplanType."'";
					break;
				}
				$queryTable="SELECT * FROM $tableName $filterQuery ORDER BY Description ASC";
				$resTable=$dbADO->Execute($queryTable);
				$deviceDataElement='<select name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
											<option value="0"></option>';
				while($rowTable=$resTable->FetchRow()){
					$itemStyle=($tableName=='FloorplanObjectType' && is_null(@$rowTable['FK_DesignObj_Control']))?' style="background-color:red;"':'';
					$deviceDataElement.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').' '.$itemStyle.' title="'.$rowTable['dd_Description'].'">'.$rowTable['dd_Description'].'</option>';
				}
				$deviceDataElement.='</select>';
			}
			else
				$deviceDataElement='<input type="text" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
			break;
			case 'bool':
				$deviceDataElement='<input type="checkbox" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
			break;
			default:
			if($rowDDforDevice['FK_DeviceData']==$GLOBALS['Port']){
				$deviceDataElement=@$ddValue;
				$deviceDataElement.='<input type="hidden" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.@$ddValue.'">&nbsp;<a href="javascript:windowOpen(\'index.php?section=editDeviceDeviceData&deviceID='.$deviceID.'&dd='.$rowDDforDevice['FK_DeviceData'].'&from=avWizard\',\'status=0,resizable=1,width=600,height=250,toolbar=1\');">Edit</a>';
			}else{
				$deviceDataElement='<input type="text" name="deviceData_'.$deviceID.'_'.$rowDDforDevice['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
			}
		}
	}

	return $deviceDataElement;
}


function getParentsForControlledVia($deviceID,$dbADO)
{
	global $avArray,$controlledByIfIR,$controlledByIfNotIR;
	$installationID=(int)$_SESSION['installationID'];

	
	$whereClause='';
	if ($deviceID!=0) {
		$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
		$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$installationID));
		if ($resSelectMasterDeviceForParent) {
			$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
			$dtID = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];

			$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
			$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($dtID));

			$categoriesChilds = array();
			if ($resSelectControlledViaCategory) {
				while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
					$categoriesChilds=array_concat($categoriesChilds,getDescendantsForCategory($rowSelectControlledVia['FK_DeviceCategory'],$dbADO));
				}
			}
			$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
			$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($dtID));

			$childsDeviceTemplateArray = array();
			if ($resSelectControlledViaDeviceTemplate) {
				while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
					$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];
				}
			}


			if (count($categoriesChilds)>0) {
				$whereClause.=' and (FK_DeviceCategory in ('.join(",",$categoriesChilds).')';
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
				$whereClause.=')';
			} else {
				if (count($childsDeviceTemplateArray)) {
					$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
				}
			}

		}
	}
	$whereClause.=" AND PK_Device!='$deviceID'";
	$queryDeviceTemplate = "
		SELECT DISTINCT Device.Description,Device.PK_Device,DeviceTemplate.Description AS Template,Room.Description AS Room
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation=? $whereClause order by Device.Description asc";

	$resDeviceTemplate = $dbADO->Execute($queryDeviceTemplate,array($installationID));

	$optionsArray=array();
	$optionsArrayLowerCase=array();
	if($resDeviceTemplate) {
		while ($row=$resDeviceTemplate->FetchRow()) {
			$label=(@$GLOBALS['DT_&_Room']==1)?$row['Description'].' ('.$row['Template'].') - ('.$row['Room'].')':$row['Description'];
			$optionsArray[$row['PK_Device']]=$label;
			$optionsArrayLowerCase[$row['PK_Device']]=strtolower($label);
		}
	}

	// get AV Device Templates if global variable is not set already
	if(!isset($avArray)){
		$avArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
	}
	if(in_array(@$dtID,$avArray)){
		
		// if device is AV with UsesIR=1, override default controlled_via based on category and device template
		if(!isset($controlledByIfIR)){
			$controlledByIfIR=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
		}
		if(!isset($controlledByIfNotIR)){
			$controlledByIfNotIR=getDevicesFromCategories(array($GLOBALS['rootComputerID']),$dbADO);
		}
		
		
		$queryDevice='
			SELECT 
				Device.*,
				DeviceTemplate.Description AS Template,Room.Description AS Room,
				DeviceTemplate.FK_CommMethod
			FROM Device 
				INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
				LEFT JOIN Room ON FK_Room=PK_Room
				LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup
			WHERE PK_Device=?';	
		$resDevice=$dbADO->Execute($queryDevice,array($deviceID));
		$rowD=$resDevice->FetchRow();

		$tmpArray=array();
		$tmpArray[$deviceID]=$rowD['Description'].' ('.$rowD['Template'].') - ('.$rowD['Room'].')';
		$devicesAllowedToControll=(@$rowD['FK_CommMethod']==1)?array_diff($controlledByIfIR,$tmpArray):array_diff($controlledByIfNotIR,$tmpArray);

		foreach($devicesAllowedToControll as $key => $value){
			$optionsArray[$key]=$value;
			$optionsArrayLowerCase[$key]=strtolower($value);
		}
	}

	asort ($optionsArrayLowerCase, SORT_STRING);
	$optionsArrayOriginal=array();
	foreach ($optionsArrayLowerCase As $id=>$label){
		$optionsArrayOriginal[$id]=$optionsArray[$id];
	}

	return $optionsArrayOriginal;
}

// get all ancestors for the category 
function getAncestorsForCategory($categoryID,$dbADO)
{
	$parentsDC=array();
	$res=$dbADO->Execute('
		SELECT  
		IF(PK_DeviceCategory=?,1,0) AS pos,
		DeviceCategory.*
		FROM DeviceCategory
		ORDER BY pos desc,FK_DeviceCategory_Parent DESC',array($categoryID));
	while($row=$res->FetchRow()){
		if($row['PK_DeviceCategory']==$categoryID){
			$parentsDC[]=$row['PK_DeviceCategory'];
		}
		if(in_array($row['PK_DeviceCategory'],$parentsDC) && !is_null($row['FK_DeviceCategory_Parent'])){
			$parentsDC[]=$row['FK_DeviceCategory_Parent'];
		}
	}
	
	
	return $parentsDC;
}

// get all childs for the category 
function getDescendantsForCategory($categoryID,$dbADO)
{
	$childsDC=array();
	$res=$dbADO->Execute('
		SELECT  
		IF(PK_DeviceCategory=?,1,0) AS pos,
		DeviceCategory.*
		FROM DeviceCategory
		ORDER BY pos desc,PK_DeviceCategory ASC',array($categoryID));
	while($row=$res->FetchRow()){
		if($row['PK_DeviceCategory']==$categoryID){
			$childsDC[]=$row['PK_DeviceCategory'];
		}
		if(in_array($row['FK_DeviceCategory_Parent'],$childsDC)){
			$childsDC[]=$row['PK_DeviceCategory'];
		}
	}

	return $childsDC;
}

function autogeneratedScenarios($array,$entArea,$dbADO,$wizard,$backPage,$room=0){

	if($room==0){
		$query='
			SELECT CommandGroup.* 
			FROM CommandGroup
			LEFT JOIN CommandGroup_EntertainArea ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Array=? AND FK_EntertainArea=?  AND AutoGeneratedDate>0';
		$res=$dbADO->Execute($query,array($array,$entArea));
	}else{
		$query='
			SELECT CommandGroup.* 
			FROM CommandGroup
			LEFT JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Array=? AND FK_Room=? AND AutoGeneratedDate>0';
		$res=$dbADO->Execute($query,array($array,$room));
	}

	$autoArray=array();
	while($row=$res->FetchRow()){
		$autoArray[]='<input type="checkbox" name="checkbox" '.(($row['Disabled']==1)?'':'checked').' onClick="self.location=\'process.php?action=setAutogeneratedScenario&cgID='.$row['PK_CommandGroup'].'&lastValue='.$row['Disabled'].'&backsection='.$backPage.'\';"><a title="'.$row['Hint'].'" href="index.php?section=scenarioWizard&cgID='.$row['PK_CommandGroup'].'&wizard='.$wizard.'&from='.$backPage.'">'.$row['Description'].'</a>';
	}
	
	return 'Autogenerated scenarios: '.join(' ',$autoArray).((count($autoArray)==0)?'N/A':'');
}

// convert xxx.yyy or aaa.bbb as ccc to yyy or ccc
function cleanField($field){
	$cleanField=(strpos($field,' AS ')!==false)?substr($field,strpos($field,' AS ')+4):$field;
	$cleanField=(strpos($cleanField,'.')!==false)?substr($cleanField,strpos($cleanField,'.')+1):$cleanField;
	
	return $cleanField;
} 

//put into an array the tree from table param
function getNodesArray($table,$pk_field,$fk_parent_field,$dbADO){
	$nodesArray=array();
	$res=$dbADO->Execute("SELECT * FROM $table ORDER BY $fk_parent_field ASC");
	while($row=$res->FetchRow()){
		$parent=(is_null($row[$fk_parent_field]))?'root_node':$row[$fk_parent_field];
		$nodesArray[$parent][]=$row[$pk_field];
		
		$nodesArray['description'][$row[$pk_field]]=$row['Description'];
		$nodesArray['parent'][$row[$pk_field]]=$row[$fk_parent_field];
	}
	
	return $nodesArray;
}

function writeFile($filename,$content,$mode='w+')
{
   if (!$handle = @fopen($filename, $mode)) {
         return 1; // Cannot open file ($filename)
    }

    // Write $content to our opened file.
    if (fwrite($handle, $content) === FALSE) {
        return 2;	// Cannot write to file ($filename)
    }	
    
    fclose($handle);
    
    return 0;
}

// get all ancestors for the device
function getAncestorsForDevice($deviceID,$dbADO)
{
	$parents=array();
	$res=$dbADO->Execute('
		SELECT  
		IF(PK_Device=?,1,0) AS pos,
		Device.*
		FROM Device
		ORDER BY pos desc,FK_Device_ControlledVia DESC',array($deviceID));
	while($row=$res->FetchRow()){
		if($row['PK_Device']==$deviceID){
			$parents[]=$row['PK_Device'];
		}
		if(in_array($row['PK_Device'],$parents) && !is_null($row['FK_Device_ControlledVia'])){
			$parents[]=$row['FK_Device_ControlledVia'];
		}
	}
	
	
	return $parents;
}

function isPhone($deviceTemplate,$dbADO){
	$fields=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory',$dbADO,'WHERE PK_DeviceTemplate='.$deviceTemplate);
	if(count($fields)==0){
		return false;
	}
	$phonesDC=getDescendantsForCategory($GLOBALS['rootPhones'],$dbADO);
	if(in_array($fields['FK_DeviceCategory'][0],$phonesDC)){
		return true;
	}else{
		return false;
	}
	
}

function createEventHandler($description,$eventID,$installationID,$dbADO,$cannedEvent=0){

	$insertCriteriaParmNesting='INSERT INTO CriteriaParmNesting (IsAnd,IsNot) VALUES (1,0)';
	$dbADO->Execute($insertCriteriaParmNesting);
	$insertID=$dbADO->Insert_ID();
		
	$insertCriteria='INSERT INTO Criteria(FK_CriteriaParmNesting,FK_CriteriaList,Description,FK_Installation) VALUES (?,?,?,?)';
	$dbADO->Execute($insertCriteria,array($insertID,$GLOBALS['EventCriteriaList'],'event',$installationID));
	$criteriaID=$dbADO->Insert_ID();
		
	$insertCommandGroup='
		INSERT INTO CommandGroup 
			(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
		VALUES
			(?,?,?,?,?,?,?)';
	$dbADO->Execute($insertCommandGroup,array($GLOBALS['EventsHandlerArray'],$installationID,'Event ActionGroup',0,0,0,$GLOBALS['EventsHandlerTemplate']));
	$cgID=$dbADO->Insert_ID();

	if($cannedEvent!=0){
		$insertEventHandler="
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents,FK_Event)
			SELECT '$description',$criteriaID,$installationID,$cgID,$cannedEvent,FK_Event FROM CannedEvents WHERE PK_CannedEvents=?";
		$dbADO->Execute($insertEventHandler,array($cannedEvent));
	}else{
		$insertEventHandler="
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents,FK_Event)
			VALUES
				(?,?,?,?,?,?)";
		$dbADO->Execute($insertEventHandler,array($description,$criteriaID,$installationID,$cgID,NULL,$eventID));
	}
	
	$ehID=$dbADO->Insert_ID();
	
	return $ehID;

}

function deleteDC($deviceCategory,$dbADO){
	getDeviceCategoryChildsArray($deviceCategory,$dbADO);

	$GLOBALS['childsDeviceCategoryArray']=array();
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;

	if ((int)$deviceCategory!=0) {
		$deleteObjFromDevice = 'DELETE FROM DeviceCategory WHERE PK_DeviceCategory IN ('.join(",",$GLOBALS['childsDeviceCategoryArray']).')';
		$query = $dbADO->_Execute($deleteObjFromDevice);
		
		return true;
	}
	
	return false;
}

function error_redirect($message,$url){
	$_SESSION['error_message']=$message;
	$url=($url=='')?'javascript:history.back();':$url;
	$_SESSION['retry_url']=$url;
	header('Location: index.php?section=error_message');
	exit();
}

function getPowerFile($dbADO,$installationID){
	
	includeLangFile('powerFile.lang.php');
	if($installationID==0){
		error_redirect(translate('TEXT_ERROR_INVALID_INSTALLATION_NUMBER_CONST'),'index.php?section=powerFile');		
	}
	
	$deviceArr=getAssocArray('Device','0','PK_Device',$dbADO,'WHERE FK_Installation='.$installationID.' AND FK_DeviceTemplate='.$GLOBALS['PowerfileC200']);
	if(count($deviceArr)==0){
		error_redirect(translate('TEXT_ERROR_POWERFILE_NOT_FOUND_CONST'),'index.php?section=powerFile');
	}

	return $deviceArr[0];
}

function get_slots_info($powerFileID,$mediadbADO){
	// hard-coded to extract performer and album
	$slotInfo=array();
	$res=$mediadbADO->Execute('
		SELECT * FROM Disc
		LEFT JOIN Disc_Attribute ON Disc_Attribute.FK_Disc=PK_Disc
		LEFT JOIN Attribute ON FK_Attribute=PK_Attribute
		LEFT JOIN Picture_Disc ON Picture_Disc.FK_Disc=PK_Disc
		WHERE ((Track=0 AND FK_AttributeType IN (2,3,13)) OR FK_Attribute IS NULL) AND EK_Device=?
		ORDER BY Slot ASC
		',$powerFileID);
	
	while($row=$res->FetchRow()){
		if($row['FK_AttributeType']==2){
			$slotInfo[$row['Slot']]['Performer']=$row['Name'];
		}
		if($row['FK_AttributeType']==3){
			$slotInfo[$row['Slot']]['Album']=$row['Name'];
		}
		if($row['FK_AttributeType']==13){
			$slotInfo[$row['Slot']]['Title']=$row['Name'];
		}		
		$slotInfo[$row['Slot']]['PK_Disc']=$row['PK_Disc'];
		$slotInfo[$row['Slot']]['ID']=$row['ID'];
		$slotInfo[$row['Slot']]['Picture']=$row['FK_Picture'];
		$slotInfo[$row['Slot']]['EK_MediaType']=$row['EK_MediaType'];
	}

	return $slotInfo;
}

function get_rip_format($installationID,$dbADO){
	$ripFormats=array('mp3'=>'mp3','ogg'=>'ogg', 'flac'=>'flac', 'wav'=>'wav');
	/*
	$mediaPluginID=getDeviceFromDT($installationID,$GLOBALS['rootMediaPlugin'],$dbADO);
	if($mediaPluginID!==null){
		$mediaPluginDD=getDD($mediaPluginID,$GLOBALS['RipFormat'],$dbADO);
		$selectedRipFormat=$mediaPluginDD[$GLOBALS['RipFormat']];
	}else{
		$selectedRipFormat='flac';
	}
	*/
	$selectedRipFormat='flac';
	
	return array('formats'=>$ripFormats,'selected'=>$selectedRipFormat);
}

function getDD($deviceID,$deviceDataValues,$dbADO)
{
	$ddArray=array();
	$fields=explode(',',$deviceDataValues);
	foreach ($fields AS $fieldID){
		$ddArray[$fieldID]=null;
	}
	
	$res=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData IN ('.$deviceDataValues.')',array($deviceID));
	if($res->RecordCount()==0){
		return $ddArray;
	}else{
		while($row=$res->FetchRow()){
			$ddArray[$row['FK_DeviceData']]=$row['IK_DeviceData'];
		}
	}


	return $ddArray;
}

function editCommandsByWizard($wizard,$section,$commandGroupID,$dbADO){
	
	$out='
		<table align="center">
			<tr bgcolor="#D1D9EA">
				<td><B>Add commands using Wizard:</B> <select name="wizard" onChange="document.editTimedEvent.submit();">
					<option value="0" '.(($wizard==0)?'selected':'').'>'.translate('TEXT_LIGHTING_WIZARD_CONST').'</option>
					<option value="1" '.(($wizard==1)?'selected':'').'>'.translate('TEXT_CLIMATE_WIZARD_CONST').'</option>
					<option value="2" '.(($wizard==2)?'selected':'').'>'.translate('TEXT_IRRIGATION_WIZARD_CONST').'</option>	
					<option value="3" '.(($wizard==3)?'selected':'').'>'.translate('TEXT_ADVANCED_WIZARD_CONST').'</option>
				</select>
				</td>
			</tr>
		</table>
		<div align="center">';
	switch ($wizard){
		case 0:
			$out.=lightingDevicesTable($commandGroupID,$dbADO);
		break;
		case 1:
			$out.=climateDevicesTable($commandGroupID,$dbADO);
		break;
		case 2:
			$out.=irrigationCommandGroupCommandsTable($commandGroupID,$section,$dbADO);
		break;
		case 3:
			$out.=advancedCommandGroupCommandsTable($commandGroupID,$section,$dbADO);
		break;
	}
		
	$out.='</div>';
		
	return $out;
}

function irrigationCommandGroupCommandsTable($cgID,$section,$dbADO){
	$devicesArray=array();

	$pos=0;	
	$irigationDT=getDeviceTemplatesFromCategory($GLOBALS['IrrigationDevices'],$dbADO);
	if(count($irigationDT)==0){
		return translate('TEXT_ERROR_NO_DEVICE_TEMPLATE_FOR_IRIGATION_DEVICES_CONST');
	}
	$res=$dbADO->Execute('
		SELECT Device.Description,PK_Device,DeviceTemplate.Description AS Template,Room.Description AS Room
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE FK_DeviceTemplate IN ('.join(',',$irigationDT).')
		ORDER BY Device.Description ASC
		');
	while($row=$res->FetchRow()){
		$pos++;
		$devicesArray[$row['PK_Device']]['Description']=$row['Description'];
		$devicesArray[$row['PK_Device']]['Template']=$row['Template'];
		$devicesArray[$row['PK_Device']]['Room']=$row['Room'];
		$devicesArray[$row['PK_Device']]['Delay']=0;
		$devicesArray[$row['PK_Device']]['Commands']=array();
		$devicesArray[$row['PK_Device']]['Pos']=$pos;
		$devicesArray[$row['PK_Device']]['PK_CommandGroup_Command']=array();
	}	
	
	$res=$dbADO->Execute('
		SELECT PK_CommandGroup_Command, PK_Device,FK_Command,IK_CommandParameter,Device.Description,DeviceTemplate.Description AS Template,Room.Description AS Room
		FROM CommandGroup_Command
		LEFT JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command 
		INNER JOIN Device ON FK_Device=PK_Device 
		INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN Room ON FK_Room=PK_Room 
		INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup 
		WHERE PK_CommandGroup=?
		ORDER BY FK_Device,OrderNum ASC',array($cgID));	
	$pos=0;	
	if($res->RecordCount()!=0){
		while($row=$res->FetchRow()){
			$pos++;			
			$devicesArray[$row['PK_Device']]['Description']=$row['Description'];
			$devicesArray[$row['PK_Device']]['Template']=$row['Template'];
			$devicesArray[$row['PK_Device']]['Room']=$row['Room'];
			if($row['FK_Command']==$GLOBALS['DelayCommand']){
				$devicesArray[$row['PK_Device']]['Delay']=$row['IK_CommandParameter'];
			}
			$devicesArray[$row['PK_Device']]['Commands'][]=$row['FK_Command'];
			$devicesArray[$row['PK_Device']]['Pos']=$pos;
			$devicesArray[$row['PK_Device']]['PK_CommandGroup_Command'][]=$row['PK_CommandGroup_Command'];
		}
	}
	$out=irrigationHTML($devicesArray);

	return $out;
}

function irrigationHTML($devicesArray){
	if(count($devicesArray)==0){
		return translate('TEXT_NO_IRRIGATION_DEVICES_CONST');
	}
	$out='
	<table cellpadding="3" cellspacing="0">
		<tr class="tablehead">
			<td align="center"><B>'.translate('TEXT_DEVICE_CONST').' / '.translate('TEXT_ROOM_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_TYPE_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_UNCHANGED_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_ON_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_OFF_CONST').'</B></td>
			<td align="center" colspan="2"><B>'.translate('TEXT_DELAY_CONST').'</B></td>
			<td align="center"><B>'.translate('TEXT_ACTION_CONST').'</B></td>
		</tr>';
	$pos=0;

	foreach ($devicesArray AS $device=>$data){
		$commands_no=count($data['Commands']);
		$cgcID=(count($data['PK_CommandGroup_Command'])==1)?$data['PK_CommandGroup_Command'][0]:@$data['PK_CommandGroup_Command'][1];
		$color=($pos%2!=0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td align="center">'.$data['Description'].' / '.$data['Room'].'</td>
			<td align="center">'.$data['Template'].'</td>
			<td align="center"><input type="radio" name="cmd_'.$device.'" value="0" '.(($commands_no==0)?'checked':'').'></td>
			<td align="center"><input type="radio" name="cmd_'.$device.'" value="'.$GLOBALS['genericONCommand'].'" '.(($commands_no==1 && $data['Commands'][0]==$GLOBALS['genericONCommand'])?'checked':'').'></td>
			<td align="center"><input type="radio" name="cmd_'.$device.'" value="'.$GLOBALS['genericOFFCommand'].'" '.(($commands_no==1 && $data['Commands'][0]==$GLOBALS['genericOFFCommand'])?'checked':'').'></td>
			<td align="center"><input type="radio" name="cmd_'.$device.'" value="'.$GLOBALS['DelayCommand'].'" '.(($commands_no==3)?'checked':'').'></td>
			<td><input type="text" name="delay_'.$device.'" value="'.@$data['Delay'].'" style="width:30px;" onClick="eval(\'document.scenarioWizard.cmd_'.$device.'[3].checked=true\');"></td>
			<td align="center"><input type="submit" class="button" name="up_'.$device.'" value="U"> <input type="submit" class="button" name="down_'.$device.'" value="D"></td>
		</tr>
		<input type="hidden" name="cgc_'.$device.'" value="'.$cgcID.'">
		<input type="hidden" name="pos_'.$device.'" value="'.$pos.'">
		<input type="hidden" name="old_commands_'.$device.'" value="'.join(',',$data['Commands']).'">';
		$pos++;
	}
	$out.='
		<tr bgcolor="#E7E7E7">
			<td align="center" colspan="8"><input type="submit" class="button" name="update_irrigation" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>	
	</table>
	<input type="hidden" name="irrigationDevices" value="'.join(',',array_keys($devicesArray)).'">';
	
	return $out;
}

function processIrrigationScenario($cgID,$section,$dbADO){
	$irrigationDevices=explode(',',$_POST['irrigationDevices']);
	$addIrrigationCommands=(int)@$_POST['addIrrigationCommands'];

	$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command,OrderNum) VALUES (?,?,?,?)';		
	$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
//$dbADO->debug=true;
	foreach ($irrigationDevices AS $device){
		$delay=(int)$_POST['delay_'.$device];
		$PK_CommandGroup_Command=(int)$_POST['cgc_'.$device];
		$pos_group=(int)$_POST['pos_'.$device];
		$pos=3*$pos_group;
		$cmd=(int)@$_POST['cmd_'.$device];
		$old_commands_array=explode(',',$_POST['old_commands_'.$device]);

		if($PK_CommandGroup_Command==0){
			// add commands to command group
			// add commands for selected device
			
			// on
			if($cmd==$GLOBALS['genericONCommand'] || $cmd==$GLOBALS['DelayCommand']){
				$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericONCommand'],$pos+1));
			}
			if($cmd==$GLOBALS['DelayCommand']){
				$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['DelayCommand'],$pos+2));
				$cgcInsertID=$dbADO->Insert_ID();
			
				// add command parameter the delay for sprinkler
				$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['delayTime'],$delay));
			}
			
			// off
			if($cmd==$GLOBALS['DelayCommand'] || $cmd==$GLOBALS['genericOFFCommand']){
				$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericOFFCommand'],$pos+3));
			}
		}else{
			$nc=count($old_commands_array);
			switch ($cmd){
				case $GLOBALS['genericONCommand']:
					if($nc!=1 || ($nc==1 && $old_commands_array[0]!=$GLOBALS['genericONCommand'])){
						// old command changed, delete command with parameters and add new one
						delete_command_from_cg($cgID,$device,$old_commands_array,$dbADO);
						$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericONCommand'],$pos+1));
					}
				break;
				case $GLOBALS['DelayCommand']:
					if($nc!=3){
						// old command changed, delete command with parameters and add new one
						delete_command_from_cg($cgID,$device,$old_commands_array,$dbADO);
						$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericONCommand'],$pos+1));
						$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['DelayCommand'],$pos+2));
						$cgcInsertID=$dbADO->Insert_ID();
						$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['delayTime'],$delay));
						$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericOFFCommand'],$pos+3));
					}else{
						$dbADO->Execute('UPDATE CommandGroup_Command_CommandParameter SET IK_CommandParameter=? WHERE FK_CommandGroup_Command=? AND FK_CommandParameter=?',array($delay,$PK_CommandGroup_Command,$GLOBALS['delayTime']));					
					}
				break;
				case $GLOBALS['genericOFFCommand']:
					if($nc!=1 || ($nc==1 && $old_commands_array[0]!=$GLOBALS['genericOFFCommand'])){
						// old command changed, delete command with parameters and add new one
						delete_command_from_cg($cgID,$device,$old_commands_array,$dbADO);
						$dbADO->Execute($insertCG_C,array($cgID,$device,$GLOBALS['genericOFFCommand'],$pos+3));
					}
				break;
				case 0:
					if(count($old_commands_array)>0)
						delete_command_from_cg($cgID,$device,$old_commands_array,$dbADO);
				break;
			}
		}
		
		if(isset($_POST['up_'.$device])){
			if($pos_group!=0){
				$lowerPos=array(3*($pos_group-1)+1,3*($pos_group-1)+2,3*($pos_group-1)+3);
				$thisPos=array(3*$pos_group+1,3*$pos_group+2,3*$pos_group+3);
				$tmpPos=array(-1*(3*($pos_group-1)+1),-1*(3*($pos_group-1)+2),-1*(3*($pos_group-1)+3));
				
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=-OrderNum WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$lowerPos).')',array($cgID));
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=OrderNum-3 WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$thisPos).')',array($cgID));
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=(-1)*OrderNum+3 WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$tmpPos).')',array($cgID));
			}
		}
		
		if(isset($_POST['down_'.$device])){
			if($pos_group!=(count($irrigationDevices)-1)){

				$higherPos=array(3*($pos_group+1)+1,3*($pos_group+1)+2,3*($pos_group+1)+3);
				$thisPos=array(3*$pos_group+1,3*$pos_group+2,3*$pos_group+3);
				$tmpPos=array(-1*(3*($pos_group+1)+1),-1*(3*($pos_group+1)+2),-1*(3*($pos_group+1)+3));
				
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=-OrderNum WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$higherPos).')',array($cgID));
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=OrderNum+3 WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$thisPos).')',array($cgID));
				$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=(-1)*OrderNum-3 WHERE FK_CommandGroup=? AND OrderNum IN ('.join(',',$tmpPos).')',array($cgID));
			}
		}

	}
}

function delete_command_from_cg($cgID,$device,$commands,$dbADO){
   foreach ($commands as &$command) {
	  $command = adodb_addslashes($command);
   }
	$deleteParameters='
		DELETE CommandGroup_Command_CommandParameter 
		FROM CommandGroup_Command_CommandParameter 
		JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
		WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command IN ('.join(',',$commands).')';
	$dbADO->Execute($deleteParameters,array($cgID,$device));
	
	$deleteCommandGroup_Command='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command IN ('.join(',',$commands).')';
	$dbADO->Execute($deleteCommandGroup_Command,array($cgID,$device));
}

function validate_installation($sessionID,$dbADO){
	if($sessionID==1){
		$installationsArray=array_keys(getAssocArray('Installation','PK_Installation','Description',$dbADO,'INNER JOIN Installation_Users ON FK_Installation=PK_Installation WHERE FK_Users='.$_SESSION['userID'].' ORDER BY FK_Installation ASC LIMIT 0,1'));
		if(count($installationsArray)>0 && $installationsArray[0]!=1){
			$_SESSION['installationID']=$installationsArray[0];
		}
	}
}

function parentHasChoices($deviceID,$dbADO){
	$choicesArray=array();
	
	$ddArray=array_values(getAssocArray('Device','FK_DeviceData','IK_DeviceData',$dbADO,'INNER JOIN Device_DeviceData ON FK_Device=FK_Device_ControlledVia WHERE PK_Device='.$deviceID.' AND FK_DeviceData='.$GLOBALS['PortChannelChoices']));
	if(count($ddArray)>0){
		$ddParts=explode("\n",$ddArray[0]);

		foreach ($ddParts AS $part){
			$items=explode("\t",trim($part));
			if(count($items)!=2){
				return array();
			}
			$choicesArray[$items[0]]=$items[1];
		}
	}
	
	return $choicesArray;
}


function deviceForScenariosSelector($name,$selectedValue,$dbADO,$allowNoValue=1,$extra='')
{
	$out='
		<select name="'.$name.'" '.$extra.'>';
	if($allowNoValue==1){
		$out.='<option value="0">-'.translate('TEXT_PLEASE_SELECT_CONST').'-</option>';
	}
	$out.='
			<option value="-300" '.(($selectedValue=='-300')?'selected':'').'>[Local Orbiter]</option>';
							
	$query = '
		SELECT Device.*, Room.Description AS RoomName, DeviceTemplate.Description AS Template,FK_Device_RouteTo
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE Device.FK_Installation = ? 
		ORDER BY FK_Device_ControlledVia ASC,FK_Room ASC,Description ASC';
	$res=$dbADO->Execute($query,array($_SESSION['installationID']));
	
	$devicesArray=array();						
	while ($row= $res->FetchRow()) {
		$roomName=($row['RoomName']!='')?stripslashes($row['RoomName']):'No Room';
		
		$devicesArray[$row['PK_Device']]['Parent']=$row['FK_Device_ControlledVia'];
		$devicesArray[$row['PK_Device']]['Description']=$row['Description'].' ['.$roomName.']';
		$devicesArray[$row['PK_Device']]['FK_DeviceTemplate']=$row['FK_DeviceTemplate'];
		$devicesArray[$row['PK_Device']]['Template']=$row['Template'];
		if(!is_null($row['FK_Device_ControlledVia'])){
			if($row['FK_Device_ControlledVia']==$row['FK_Device_RouteTo']){
				if(!isset($devicesArray[$row['FK_Device_ControlledVia']]['Children'])){
					$devicesArray[$row['FK_Device_ControlledVia']]['Children'][]=$row['PK_Device'];
				}else{
					array_unshift($devicesArray[$row['FK_Device_ControlledVia']]['Children'],$row['PK_Device']);
				}
				$devicesArray[$row['PK_Device']]['Embedded']=1;
			}else{
				$devicesArray[$row['FK_Device_ControlledVia']]['Children'][]=$row['PK_Device'];
				$devicesArray[$row['PK_Device']]['Embedded']=0;
			}
		}
	}

						
	foreach ($devicesArray AS $deviceID=>$deviceInfo){
		if(is_null(@$deviceInfo['Parent']) && isset($deviceInfo['Description'])){
			$out.='<option '.($deviceID==$selectedValue?'selected':'').' value="'.$deviceID.'" title="Device template #'.$deviceInfo['FK_DeviceTemplate'].': '.$deviceInfo['Template'].'">'.$deviceInfo['Description'].'</option>';
			$out.=getOptions($devicesArray,@$deviceInfo['Children'],$selectedValue,'&nbsp;&nbsp;|---');
		}
	}
	$out.='
		</select>';
	return $out;
}

function getOptions($devicesArray,$childrenArray,$selectedValue,$prefix){
	if(count($childrenArray)==0){
		return '';
	}
	
	$out='';
	foreach ($childrenArray AS $childID){
		$deviceInfo=$devicesArray[$childID];
		$out.='<option '.($childID==$selectedValue?'selected':'').' value="'.$childID.'" title="Device template #'.$deviceInfo['FK_DeviceTemplate'].': '.$deviceInfo['Template'].'" '.(($deviceInfo['Embedded']==1)?'style="background:yellow;"':'').'>'.$prefix.' '.$deviceInfo['Description'].'</option>';		
		$out.=getOptions($devicesArray,@$deviceInfo['Children'],$selectedValue,$prefix.'--- ');
	}
	
	return $out;
}

function getSpecificFloorplanType($dcID,$dbADO){
	$ancestors=getAncestorsForCategory($dcID,$dbADO);
	
	// child of Environment > Lights 
	if(in_array($GLOBALS['rootLights'],$ancestors)){
		return $GLOBALS['LightingFoorplanType'];
	}

	// child of Climate 
	if(in_array($GLOBALS['rootClimate'],$ancestors)){
		return $GLOBALS['ClimateFoorplanType'];
	}

	// child of Security
	if(in_array($GLOBALS['rootSecurity'],$ancestors)){
		return $GLOBALS['SecurityFoorplanType'];
	}

	// child of cameras
	if(in_array($GLOBALS['rootCameras'],$ancestors)){
		return $GLOBALS['SecurityFoorplanType'];
	}

	// child of phones
	if(in_array($GLOBALS['rootPhones'],$ancestors)){
		return $GLOBALS['PhonesFoorplanType'];
	}

	// child of cameras
	if(in_array($GLOBALS['IrrigationDevices'],$ancestors)){
		return $GLOBALS['ClimateFoorplanType'];
	}
	
	return 0;
}

function pickDeviceTemplate($categoryID, $manufacturerID,$returnValue,$defaultAll,$section,$dbADO,$genericSerialDevicesOnly=0){
	// include language files
	includeLangFile('deviceTemplatePicker.lang.php');

	$from=@$_REQUEST['from'];
	parse_str($from);

	if(isset($type) && $type!=''){
		$restrictToCategory=$categoryID;
	}
	

	$manufacturersArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$dbADO,'','ORDER BY Description ASC');
	$deviceCategoryPicker=(isset($_REQUEST['deviceCategoryPicker']))?$_REQUEST['deviceCategoryPicker']:0;
	$dtID=(isset($_REQUEST['dtID']))?$_REQUEST['dtID']:0;

	switch (@$deviceCategoryPicker){
		case 1:
			$deviceCategoriesArray=getHierachicalCategories($dbADO,@$restrictToCategory);
			$deviceCategoryFormElement=pulldownFromArray($deviceCategoriesArray,'categoryID',$categoryID,'style="width:400px;" class="input_big" onChange="setDeviceCategory(-1);"');
		break;
		case 2:
			$deviceCategoryFormElement=getdTree($categoryID,$dbADO,@$restrictToCategory);
		break;
		default:
			$deviceCategoriesArray=getAlphaCategories($dbADO,@$restrictToCategory);
			$deviceCategoryFormElement=pulldownFromArray($deviceCategoriesArray,'categoryID',$categoryID,'style="width:400px;" class="input_big" onChange="setDeviceCategory(-1);"');
	}

	if(isset($_REQUEST['do_filter']) || (int)@$_REQUEST['autofilter']==1 || (int)@$_SESSION['parentID']>0 || (int)@$_REQUEST['categoryID']>0){
		//echo 'do_filter categ: '.$categoryID.' manuf: '.$manufacturerID;
		
		$deviceTemplatesArray=getAllowedDT((int)@$_SESSION['parentID'],$categoryID,$manufacturerID,$dbADO,$from);
		$deviceTemplatesPulldown=pulldownFromArray($deviceTemplatesArray,'template',$dtID,'style="width:400px;" class="input_big" onchange="setDeviceTemplate();"');
		
	}else{
		$filter=($from=='genericSerialDevices')?' WHERE CommandLine=\''.$GLOBALS['GenericSerialDeviceCommandLine'].'\'':'';
		$deviceTemplatesArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,$filter,'ORDER BY Description ASC');
		$deviceTemplatesPulldown=pulldownFromArray($deviceTemplatesArray,'template',$dtID,'style="width:400px;" class="input_big" onchange="setDeviceTemplate();"');
	}
	$parentDeviceText=translate('TEXT_NONE_CONST');
	if((int)@$_SESSION['parentID']>0){
		$parentDeviceText='# '.(int)@$_SESSION['parentID'];
		$dtstar=' ***';
		$dtNote='<br>'.$dtstar.' '.translate('TEXT_DEVICE_TEMPLATE_CONTROLLED_VIA_NOTE_CONST');
	}
	
	$out=jsManufCategories(array_keys($deviceTemplatesArray),$dbADO);
	$out.=dtPickerJS($returnValue);
	$out.='

	<form action="index.php" method="post" name="deviceTemplatePicker">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="action" value="choose">
		<input type="hidden" name="from" value="'.@$_REQUEST['from'].'">
		<input type="hidden" name="parmToKeep" value="'.@$_REQUEST['parmToKeep'].'">
	
	<table align="center" border="0" cellpadding="2" cellspacing="0" width="600">';
	if($returnValue==1){
		$out.='
		<tr>
			<td><B>'.translate('TEXT_PARENT_DEVICE_CONST').'</B></td>
			<td colspan="2">'.$parentDeviceText.'</td>
			<td>&nbsp;</td>
		</tr>';
	}
	$out.='
		<tr class="alternate_back">
			<td><B>'.translate('TEXT_MANUFACTURER_CONST').'</B></td>
			<td>'.pulldownFromArray($manufacturersArray,'manufacturerID',$manufacturerID,'style="width:400px;" class="input_big" onchange="setManufacturer();"').'</td>
			<td width="35"><b><span id="manufacturerSelected" class="confirm"></span></b></td>
			<td><input type="button" class="button_fixed" value="'.translate('TEXT_ADD_MANUFACTURER_CONST').'" onclick="add_manufacturer_popup();"></td>
		</tr>
		<tr class="alternate_back">
			<td valign="top"><B>'.translate('TEXT_DEVICE_CATEGORY_CONST').'</B></td>
			<td valign="top">'.$deviceCategoryFormElement.'</td>
			<td><b><span id="categorySelected" class="confirm"></span></b></td>
			<td valign="top"><input type="button" class="button_fixed" value="'.translate('TEXT_ADD_DEVICE_CATEGORY_CONST').'" onclick="add_category_popup();"></td>
		</tr>	
		<tr class="alternate_back">
			<td>&nbsp;</td>
			<td colspan="2">
				<input type="radio" name="deviceCategoryPicker" value="0" '.(($deviceCategoryPicker==0)?'checked':'').' onclick="changeCategoryPicker();"> '.translate('TEXT_DEVICE_CATEGORY_PULLDOWN_ALPHA_CONST').'<br>
				<input type="radio" name="deviceCategoryPicker" value="1" '.(($deviceCategoryPicker==1)?'checked':'').' onclick="changeCategoryPicker();"> '.translate('TEXT_DEVICE_CATEGORY_PULLDOWN_HIERARCHICAL_CONST').'<br>
				<input type="radio" name="deviceCategoryPicker" value="2" '.(($deviceCategoryPicker==2)?'checked':'').' onclick="changeCategoryPicker();"> '.translate('TEXT_DEVICE_CATEGORY_TREEVIEW_CONST').'<br>
			</td>
			<td>&nbsp;</td>
		</tr>	
		<tr class="alternate_back">
			<td colspan="4" align="center">
				<input type="checkbox" name="autofilter" value="1" '.(((int)@$_REQUEST['autofilter']==1)?'checked':'').' onClick="document.deviceTemplatePicker.submit();">'.translate('TEXT_AUTOFILTER_CONST').' *
			</td>
		</tr>	
		<tr class="alternate_back">
			<td colspan="4" align="center">
				<input type="submit" name="do_filter" class="button_fixed" value="'.translate('TEXT_APPLY_FILTER_CONST').'">
			</td>
		</tr>	
		<tr>
			<td colspan="4" align="center">&nbsp;</td>
		</tr>		
		<tr class="alternate_back">
			<td><B>'.translate('TEXT_DEVICE_TEMPLATE_CONST').@$dtstar.'</B></td>
			<td>'.$deviceTemplatesPulldown.'</td>
			<td><b><span id="templateSelected" class="confirm"></span></b></td>
			<td><input type="button" class="button_fixed" value="'.translate('TEXT_PICK_MODEL_CONST').'" onclick="pickDeviceTemplate();"></td>
		</tr>
		<tr>
			<td colspan="4" align="center">&nbsp;</td>
		</tr>		
		<tr class="alternate_back">
			<td colspan="4" align="center">
				'.translate('TEXT_NEW_DEVICE_TEMPLATE_NOTICE_CONST').' **<br>
				<input type="button" class="button_fixed" value="'.translate('TEXT_ADD_DEVICE_TEMPLATE_CONST').'" onclick="add_device_template_popup();">
			</td>
		</tr>		
		<tr>
			<td colspan="4" align="center">
				<span class="err" id="error_box"></span>&nbsp;
			</td>
		</tr>		
		<tr>
			<td colspan="4">
			<table width="100%">
				<tr class="alternate_back">
					<td align="center"><B>'.translate('TEXT_TEMPLATE_SEARCH_INFO_CONST').'</B></td>
				</tr>
				<tr class="alternate_back">
					<td align="center"><input type="text" name="dtID" value="'.$dtID.'" onChange="setDeviceTemplatePulldown();" onKeyPress="return processEnter(this,event);"> <input type="button" class="button" name="searchBtn" value="'.translate('TEXT_SEARCH_TEMPLATES_CONST').'"></td>
				</tr>
			</table>
			</td>
		</tr>	
		<tr>
			<td colspan="4" align="left">
				<em>* '.translate('TEXT_AUTOFILTER_INFO_CONST').'<br>
					** '.translate('TEXT_DC_MANUFACTURER_NEEDED_CONST').'
					'.@$dtNote.'
				</em>
			</td>
		</tr>	
		<tr>
			<td colspan="4">&nbsp;</td>
		</tr>	
		<tr>
			<td colspan="4"><B>'.translate('TEXT_NOTES_CONST').'</B><br>'.translate('TEXT_PICKER_INFO_CONST').'</td>
		</tr>	
		<tr>
			<td colspan="4" align="center">
				<input type="reset" class="button_fixed" name="closeBtn" value="'.translate('TEXT_CLOSE_CONST').'" onClick="self.close();">
			</td>
		</tr>		
	</table>
	</form>
	';
	
	return $out;
}

// get an associative array with device categories, and in description there are also displayed the parents
function getAlphaCategories($dbADO,$restrictToCategory=0){
	
	$filter='';
	if($restrictToCategory!=0){
		$restrictedCategories=getDescendantsForCategory($restrictToCategory,$dbADO);
		$filter='WHERE PK_DeviceCategory IN ('.join(',',$restrictedCategories).')';
	}

	$categoriesArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$dbADO,'','ORDER BY Description ASC');
	$categoriesWithParents=($restrictToCategory!=0)?array():$categoriesArray;
	$categoriesParents=getAssocArray('DeviceCategory','PK_DeviceCategory','FK_DeviceCategory_Parent',$dbADO,$filter,'ORDER BY Description ASC');

	foreach ($categoriesArray AS $cID=>$description){
		$parents=array();
		$parents=getDCParents($parents,$cID,$categoriesParents,$categoriesArray);
		$parentSuffix=(count($parents)>0)?' < '.join(' < ',$parents):'';
		if($restrictToCategory!=0){
			if(in_array($cID,$restrictedCategories)){
				$categoriesWithParents[$cID]=$description.$parentSuffix;
			}
		}else{
			$categoriesWithParents[$cID]=$description.$parentSuffix;
		}
	}

	return $categoriesWithParents;
}

function getDCParents($parents,$cID,$categoriesParents,$categoriesArray){

	if(isset($categoriesParents[$cID]) && (int)$categoriesParents[$cID]!=0){
		$parents[]=$categoriesArray[$categoriesParents[$cID]];
		$parents+=getDCParents($parents,$categoriesParents[$cID],$categoriesParents,$categoriesArray);
	}
	
	return $parents;
}

function getHierachicalCategories($dbADO,$restrictToCategory=0){
	$restrictedCategories=array();
	if($restrictToCategory!=0){
		$restrictedCategories=getDescendantsForCategory($restrictToCategory,$dbADO);
	}	
	
	$categoriesHierarchical=array();

	$categoriesArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$dbADO,'','ORDER BY FK_DeviceCategory_Parent ASC,Description ASC');
	$categoriesParents=getAssocArray('DeviceCategory','PK_DeviceCategory','FK_DeviceCategory_Parent',$dbADO,'','ORDER BY Description ASC');

	foreach ($categoriesParents AS $cID=>$parent){
		if($parent==''){
			if($restrictToCategory!=0){
				if(in_array($cID,$restrictedCategories)){
					$categoriesHierarchical[$cID]=$categoriesArray[$cID];
				}
			}else{
				$categoriesHierarchical[$cID]=$categoriesArray[$cID];
			}
			$categoriesHierarchical=getHierachicalChilds($categoriesHierarchical,$cID,$categoriesArray,$categoriesParents,$restrictedCategories,1);			
		}
	}

	return $categoriesHierarchical;
}

function getHierachicalChilds($categoriesHierarchical,$pid,$categoriesArray,$categoriesParents,$restrictedCategories=array(),$level=0){
	foreach ($categoriesParents AS $cID=>$parent){
		if($parent==$pid){
			$prefix='';
			for($i=0;$i<$level;$i++){
				$prefix.='&nbsp;&nbsp;&nbsp;&nbsp;';
			}
			
			if(count($restrictedCategories)==0 || in_array($cID,$restrictedCategories)){
				$categoriesHierarchical[$cID]=$prefix.$categoriesArray[$cID];
			}
			$categoriesHierarchical=getHierachicalChilds($categoriesHierarchical,$cID,$categoriesArray,$categoriesParents,$restrictedCategories,$level+1);
		}
	}
	
	return $categoriesHierarchical;
}

// build a treeview with categories
function getdTree($categoryID,$dbADO,$restrictToCategory=0){
	$restrictedCategories=array();
	if($restrictToCategory!=0){
		$restrictedCategories=getDescendantsForCategory($restrictToCategory,$dbADO);
	}

	$categoriesArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$dbADO,'','ORDER BY FK_DeviceCategory_Parent ASC,Description ASC');
	$categoriesParents=getAssocArray('DeviceCategory','PK_DeviceCategory','FK_DeviceCategory_Parent',$dbADO,'','ORDER BY Description ASC');

	$jsNodes='';
	foreach ($categoriesParents AS $cID=>$parent){
		if($parent==''){
			$url='';
			if(count($restrictedCategories)==0 || in_array($cID,$restrictedCategories)){
				$url='javascript:setDeviceCategory('.$cID.',1);';
			}
			$jsNodes.='d.add('.$cID.',0,\''.$categoriesArray[$cID].'\',\''.$url.'\');';
			$jsNodes=getsTreeChilds($jsNodes,$cID,$categoriesArray,$categoriesParents,$restrictedCategories);
		}
	}

//die($jsNodes);
	$out='
	<link rel="StyleSheet" href="scripts/dtree/dtree.css" type="text/css" />
	<script type="text/javascript" src="scripts/dtree/dtree.js"></script>
	
<div class="dtree">
	<script type="text/javascript">
		<!--

		d = new dTree(\'d\');

		d.add(0,-1,\''.translate('TEXT_DEVICE_CATEGORY_CONST').'\');
		'.$jsNodes.'		
	
		d.config.useLines=true;
		d.config.useIcons=false;
	
		document.write(d);

		//-->
	</script>
	<input type="hidden" name="categoryID" value="">
</div>	
	';

	return $out;
}

function getsTreeChilds($jsNodes,$pid,$categoriesArray,$categoriesParents,$restrictedCategories=array()){

	foreach ($categoriesParents AS $cID=>$parent){
		if($parent==$pid){
			if(count($restrictedCategories)==0 || in_array($cID,$restrictedCategories)){
				$jsNodes.='d.add('.$cID.','.$parent.',\''.addslashes($categoriesArray[$cID]).'\',\'javascript:setDeviceCategory('.$cID.',1);\');';
			}
			$jsNodes=getsTreeChilds($jsNodes,$cID,$categoriesArray,$categoriesParents,$restrictedCategories);
		}
	}
	
	return $jsNodes;
}

function dtPickerJS($returnValue){
	// include language files
	includeLangFile('deviceTemplatePicker.lang.php');

	$out='
	<script>
	function setDeviceCategory(val,setHidden){
		setErrorMessage("");
		if(val==-1){
			for(i=0;i<document.deviceTemplatePicker.categoryID.length;i++){
				if(document.deviceTemplatePicker.categoryID[i].selected)
					val=document.deviceTemplatePicker.categoryID[i].value;
			}
		}
		if(setHidden==1){
			document.deviceTemplatePicker.categoryID.value=val;	
		}
		document.getElementById("categorySelected").innerHTML=val;
		
		if(document.deviceTemplatePicker.autofilter.checked){
			document.deviceTemplatePicker.submit();
		}
	}

	function setManufacturer(){
		setErrorMessage("");
		for(i=0;i<document.deviceTemplatePicker.manufacturerID.length;i++){
			if(document.deviceTemplatePicker.manufacturerID[i].selected)
				val=document.deviceTemplatePicker.manufacturerID[i].value;
		}

		//document.deviceTemplatePicker.manufSelected.value=val;
		document.getElementById("manufacturerSelected").innerHTML=val;
		
		if(document.deviceTemplatePicker.autofilter.checked){
			document.deviceTemplatePicker.submit();
		}
	}

	function setDeviceTemplate(){
		setErrorMessage("");
		for(i=0;i<document.deviceTemplatePicker.template.length;i++){
			if(document.deviceTemplatePicker.template[i].selected)
				val=document.deviceTemplatePicker.template[i].value;
		}

		//document.deviceTemplatePicker.dtID.value=val;
		document.getElementById("templateSelected").innerHTML=val;
	}	
	
	function changeCategoryPicker(){
		document.deviceTemplatePicker.submit();
	}

	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	
	function pickDeviceTemplate(){
		dt=document.deviceTemplatePicker.template.value;
		if(dt>0){
			if('.$returnValue.'==1){
				opener.location="index.php?section='.$_SESSION['from'].'&deviceTemplate="+dt+"&action=add&add=1&'.@$_REQUEST['parmToKeep'].'";
				self.close();
			}else{
				windowOpen("index.php?section=editMasterDevice&model="+dt,"width=1024,height=768,toolbar=1,scrollbars=1,resizable=1");
			}
		}else{
			// error case, display error message
			setErrorMessage("'.translate('TEXT_ERROR_DEVICE_TEMPLATE_NOT_SELECTED_CONST').'");
		}
	}
	
	function setErrorMessage(value){
		try{
			document.getElementById("error_box").innerHTML=value;
		}catch(e){
			//
		}
		
	}
	
	function setManufacturerPulldown(val){
		
		if(val!=0 && val!=document.deviceTemplatePicker.manufacturerID.value){		
			pos=0;
			for(i=0;i<document.deviceTemplatePicker.manufacturerID.length;i++){
				if(document.deviceTemplatePicker.manufacturerID[i].value==val)
					pos=i;
			}
		
			if(pos!=0){
				document.deviceTemplatePicker.manufacturerID[pos].selected=true;
				document.getElementById("manufacturerSelected").innerHTML=val;
			}else{
				document.getElementById("manufacturerSelected").innerHTML="";
				document.deviceTemplatePicker.manufacturerID[0].selected=true;
			}
		}else{
			document.deviceTemplatePicker.manufacturerID[0].selected=true;
			document.getElementById("manufacturerSelected").innerHTML="";
		}
	}
	
	function setCategoryPulldown(val){
	
		val=(isNaN(val))?-1:val;
	
		// if it\'s treeview, use its function to set menu item
		if(document.deviceTemplatePicker.deviceCategoryPicker[2].checked){
			d.openTo(val,true);
		}else{
			if(val!=0 && val!=document.deviceTemplatePicker.categoryID.value){	
				pos=0;
				for(i=0;i<document.deviceTemplatePicker.categoryID.length;i++){
					if(document.deviceTemplatePicker.categoryID[i].value==val)
						pos=i;
				}
			
				if(pos!=0){
					document.deviceTemplatePicker.categoryID[pos].selected=true;
					document.getElementById("categorySelected").innerHTML=val;
				}else{
					document.deviceTemplatePicker.categoryID[0].selected=true;
					document.getElementById("categorySelected").innerHTML="";
				}
			}else{
					document.deviceTemplatePicker.categoryID[0].selected=true;
					document.getElementById("categorySelected").innerHTML="";
			}
		}
	}	
	
	function setDeviceTemplatePulldown(){
		val=parseInt(document.deviceTemplatePicker.dtID.value);
		if(val!=0 && !isNaN(val) && val!=document.deviceTemplatePicker.template.value){
			pos=0;
			for(i=0;i<document.deviceTemplatePicker.template.length;i++){
				if(document.deviceTemplatePicker.template[i].value==val)
					pos=i;
			}

			if(pos!=0){
				setErrorMessage("");
				document.deviceTemplatePicker.template[pos].selected=true;
				setManufacturerPulldown(manufacturersArray[val]);
				setCategoryPulldown(deviceCategoriesArray[val]);
				document.getElementById("templateSelected").innerHTML=val;
			}else{
				document.deviceTemplatePicker.template[0].selected=true;
				document.getElementById("templateSelected").innerHTML="";
				setErrorMessage("'.translate('TEXT_DEVICE_TEMPLATE_DOES_NOT_EXIST_OR_INVALID_CONTROLED_VIA_CONST').'");
				setManufacturerPulldown(0);
				setCategoryPulldown(0);
			}
		}else{
			document.deviceTemplatePicker.template[0].selected=true;
			document.getElementById("templateSelected").innerHTML="";
		}
	}	

	function add_device_template_popup(){
		setErrorMessage("");
		manuf=parseInt(document.deviceTemplatePicker.manufacturerID.value);
		categ=parseInt(document.deviceTemplatePicker.categoryID.value);
		if(!isNaN(manuf) && !isNaN(categ) && manuf>0 && categ>0){
			url="index.php?section=addModel&mID="+manuf+"&dcID="+categ+"&step=1&from='.urlencode(@$_SESSION['from']).'";
			try{
				if(typeof opener=="undefined" || opener ===null){
					window.open(url,"popup","status=0,resizable=1,width=800,height=600,scrollbars=1");
				}else{
					self.location=url;
				}
				
			}catch(e){
				self.location=url;
			}
		}else{
			setErrorMessage("'.translate('TEXT_ERROR_PICK_MANUF_AND_DC_CONST').'");
		}
	}
	
	function add_manufacturer_popup(){
		windowOpen("index.php?section=addManufacturer","width=640,height=480,toolbar=1,scrollbars=1,resizable=1");
	}
	
	function add_category_popup(){
		categ=parseInt(document.deviceTemplatePicker.categoryID.value);
		windowOpen("index.php?section=addCategory&parentID="+categ,"width=640,height=480,toolbar=1,scrollbars=1,resizable=1");
	}
	
	function processEnter(myfield,e)
	{
		var keycode;
		if (window.event) keycode = window.event.keyCode;
		else if (e) keycode = e.which;
		else return true;

		if (keycode == 13)
		{
			setDeviceTemplatePulldown();
			return false;
		}
		else
		return true;
	}	
	</script>
	';
	
	return $out;
}

function jsManufCategories($deviceTemplatesArray,$dbADO){
	if(count($deviceTemplatesArray)==0){
		return '';
	}
	
	$res=$dbADO->Execute('SELECT PK_DeviceTemplate,FK_Manufacturer,FK_DeviceCategory FROM DeviceTemplate WHERE PK_DeviceTemplate IN ('.join(',',$deviceTemplatesArray).')');
	$manufJsArray=array();
	$categJsArray=array();
	while($row=$res->FetchRow()){
		$manufJsArray[]='manufacturersArray['.$row['PK_DeviceTemplate'].']='.$row['FK_Manufacturer'].';';
		$categJsArray[]='deviceCategoriesArray['.$row['PK_DeviceTemplate'].']='.$row['FK_DeviceCategory'].';';
	}
	
	$out='
	<script>
		manufacturersArray=new Array();
		'.join("\n",$manufJsArray).'
		deviceCategoriesArray=new Array();
		'.join("\n",$categJsArray).'
	</script>
	';
	
	return $out;
}

// return an associative array with the device templates allowed as childs for a specific device
// if parent is not specified, return all device templates from category and its childs & manufacturer
function getAllowedDT($parentID,$categoryID,$manufacturerID,$dbADO,$from){
	if($parentID!=0){
		$allowedDevices=getDeviceTemplatesControlledBy($parentID,$dbADO);
	}
	$filter='WHERE 1=1 ';
	$filter.=(count(@$allowedDevices)>0)?'AND PK_DeviceTemplate IN ('.join(',',$allowedDevices).') ':'';
	$filter.=($from=='genericSerialDevices')?' AND CommandLine=\''.$GLOBALS['GenericSerialDeviceCommandLine'].'\'':'';
	
	if($categoryID>0){
		$subcategories=getDescendantsForCategory($categoryID,$dbADO);
		$filter.='AND FK_DeviceCategory IN ('.join(',',$subcategories).')';
	}
	
	if($manufacturerID>0){
		$filter.='AND FK_Manufacturer ='.$manufacturerID;
	}
	
	$dtArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,$filter,'ORDER BY Description ASC');	
	
	return $dtArray;
}

function queryExternalServer($url){
	$ch = curl_init();
	
	// set URL and other appropriate options
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_HEADER, false);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
	
	// grab URL and pass it to the browser
	$result=curl_exec($ch);
	
	// close CURL resource, and free up system resources
	if (curl_errno($ch)) {
   		return curl_error($ch);
	} else {
   		curl_close($ch);
	}
	
	return $result;
}

function import_remote_sql($remoteUrl,$dbADO,$table=''){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;

	// call for WebDB_Get.sh
	// all the rest will be removed
	$cmd='/usr/pluto/bin/WebDB_Get.sh \''.$remoteUrl.'\' \''.$table.'\'';
	$notImported=exec($cmd,$retArray);
	//echo $cmd;
//	print_array($retArray);
	return $notImported;
	
	// get data from plutohome server
	$remoteData=queryExternalServer($remoteUrl);

	if(!ereg('Database import',$remoteData)){
		@error_log($remoteData,$GLOBALS['ErrorLog']);
		return 1;
	}
		
	if(ereg('ERROR',$remoteData)){
		error_log($remoteData,3,$GLOBALS['ErrorLog']);
		return 1;
	}
	
	$queryDataArray=array();
	$remoteDataArray=explode("\n",$remoteData);

	// second line is the list of psc_id to remove
	$idsToRemove=explode(',',$remoteDataArray[1]);

	if($remoteDataArray[1]!='' && count($idsToRemove)>0 && $table!=''){
		$dbADO->Execute('DELETE FROM '.$table.' WHERE psc_id IN ('.join(',',$idsToRemove).')');
	}
	
	// remove first 2 lines, the comment confirming the remote page was loaded and the comma separated list of psc_ids
	for($i=2;$i<count($remoteDataArray);$i++){
		$queryDataArray[]=$remoteDataArray[$i];
	}	

	// waring: str_replace is needed because in bash single quote inside single quotes is not escaped
	if($remoteData!='No records'){
		$importCmd='echo \''.str_replace(array("'","\n"),array("'\"'\"'"," "),join("\n",$queryDataArray)).'\' | mysql '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.(($dbPlutoMainPass!='')?' -p'.$dbPlutoMainPass:'').'; echo $?';

		$status=exec($importCmd,$retArray);
		if($status!=0){
			@error_log(join("\n",$retArray),3,$GLOBALS['ErrorLog']);
		}
		
		return $status;
	}
	
	return 1;
}

// PHP version of the .sh file who will do the same thing
function GetIRCodesForDevice($deviceID,$dbADO,$dtID=0,$restricted=0){
	// TODO: comment all and call the sh instead

	$cmd='/usr/pluto/bin/WebDB_GetIR.sh '.(int)$deviceID.' '.$dtID.' '.$restricted;
	
	exec($cmd,$out,$retvar);
	
	return $retvar;

	// get device template ID and infrared group ID for the device
	// if $dtID!=0 ignore device and only get FK_InfraredGroup
	if($dtID==0){
		$data=getFieldsAsArray('Device','FK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device='.$deviceID);
	}else{
		$data=getFieldsAsArray('DeviceTemplate','PK_DeviceTemplate,FK_InfraredGroup,FK_DeviceCategory,FK_Manufacturer',$dbADO,'WHERE PK_DeviceTemplate='.$dtID);
	}
	
	if(count($data)>0){
		// check if table exists; if not, try to create it geting schema from remote location
		$res=$dbADO->Execute('SHOW TABLES LIKE "InfraredGroup_Command"');
		if($res->RecordCount()!=0){
			// get the codes for the device template or infrared group
			$notImported=import_remote_sql($LinuxMCEHost.'/GetIRCodes.php?Server='.urlencode($GLOBALS['sqlCVS_Server']).'&PK_InfraredGroup='.$data['FK_InfraredGroup'][0].'&PK_DeviceTemplate='.$data['FK_DeviceTemplate'][0],$dbADO,'InfraredGroup_Command');	
			
			// get the codes for the same manufacturer and device category
			$notImported=import_remote_sql($LinuxMCEHost.'/GetIRCodes.php?Server='.urlencode($GLOBALS['sqlCVS_Server']).'&PK_Manufacturer='.$data['FK_Manufacturer'][0].'&PK_DeviceCategory='.$data['FK_DeviceCategory'][0],$dbADO,'InfraredGroup_Command');	
			
			return $notImported;
		}else{
			// create schema
			$notCreated=import_remote_sql($LinuxMCEHost.'/GetIRCodes.php?Server='.urlencode($GLOBALS['sqlCVS_Server']).'&Create=1',$dbADO);
			
			if($notCreated==0){
				// get the codes for the device template or infrared group
				$notImported=import_remote_sql($LinuxMCEHost.'/GetIRCodes.php?Server='.urlencode($GLOBALS['sqlCVS_Server']).'&PK_InfraredGroup='.$data['FK_InfraredGroup'][0].'&PK_DeviceTemplate='.$data['FK_DeviceTemplate'][0],$dbADO,'InfraredGroup_Command');	
				
				return $notImported;
			}else{
				return 1;
			}
		}
	}
	
	// no data either for device or device template, return error code
	return 1;
}

function exec_batch_command($cmd,$all_output=0){
	writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\t".$cmd."\n",'a+');
	$last_line=exec($cmd,$retArray,$retval);
	if($retval===0){
		if($all_output==1){
			return join("\n",$retArray);
		}else{
			return $last_line;
		}
	}else{
		return false;
	}
}

function wikiLink($str){
	$wikiLink=str_replace(' ','_',$str);
	$wikiLink=urlencode(str_replace('__','_',$wikiLink)); 
	
	return $wikiLink;
}

// return the deviceID of the plugin from current installation and selected device template
function getPlugin($installationID,$dtID,$dbADO)
{
	$res=$dbADO->Execute('SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?',array($dtID,$installationID));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->Fetchrow();
		return $row['PK_Device'];
	}
}

// parameters: 
//	onscreen orbiter ID who is parent of xine player
//	database connection
function getAcceleration($orbiterMDChild,$dbADO){
	$res=$dbADO->Execute('
		SELECT IK_DeviceData
		FROM Device_DeviceData
		INNER JOIN Device ON FK_Device=PK_Device
		WHERE FK_Device_ControlledVia=? AND FK_DeviceData=?
	',array($orbiterMDChild,$GLOBALS['hardware_acceleration_dd']));
	
	if($res->RecordCount()==0){
		return false;
	}
	$row=$res->FetchRow();

	return $row['IK_DeviceData'];
}

function setAcceleration($orbiterMDChild,$value,$dbADO){
	$res=$dbADO->Execute('
		UPDATE Device_DeviceData 
		INNER JOIN Device ON FK_Device=PK_Device
		SET IK_DeviceData=?
		WHERE FK_Device_ControlledVia=? AND FK_DeviceData=?
	',array($value,$orbiterMDChild,$GLOBALS['hardware_acceleration_dd']));
}

/**
 * @return true/false
 * @param string $file
 * @desc check if the file is directory, to avoid 4G limitation
 * execute command line stat --format="%F" <director/fisier>
*/
function is_dir64($file){
	$type=exec_batch_command("stat --format=\"%F\" -L \"".str_replace(array('\\','"','$'),array('\\\\','\"','\$'),$file)."\"",1);
	if($type=='directory'){
		return true;
	}
	return false;
}

/**
 * @return void
 * @param int $cgcID
 * @param int $commandID
 * @param ADOConnection $dbADO
 * @param array $values
 * @desc create the parameters for the command specified as parameter, from Command_CommandParameter
*/
function addScenarioCommandParameters($cgcID,$commandID,$dbADO,$values=array(),$screen=0){
	$cmdParams=array_keys(getAssocArray('Command_CommandParameter','FK_CommandParameter','Description',$dbADO,'WHERE FK_Command='.$commandID));
	if($screen!=0){
		$extraParams=array_keys(getAssocArray('Screen_CommandParameter','FK_CommandParameter','Description',$dbADO,'WHERE FK_Screen='.$screen));
		$cmdParams=array_merge($cmdParams,$extraParams);
	}
	$insertCG_C_CP='
		INSERT IGNORE INTO CommandGroup_Command_CommandParameter 
			(FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter)
		VALUES
			(?,?,?)';
	
	foreach ($cmdParams AS $param){
		$dbADO->Execute($insertCG_C_CP,array($cgcID,$param,@$values[$param]));
	}
}

function get_child_device_data($orbiterID,$deviceDataID,$dbADO){
	$res=$dbADO->Execute('
		SELECT IK_DeviceData
		FROM Device_DeviceData
		INNER JOIN Device ON FK_Device=PK_Device
		WHERE FK_Device_ControlledVia=? AND FK_DeviceData=?
	',array($orbiterID,$deviceDataID));
	
	if($res->RecordCount()==0){
		return false;
	}
	$row=$res->FetchRow();

	return $row['IK_DeviceData'];
}

function set_child_device_data($orbiterID,$deviceDataID,$value,$dbADO){
	$res=$dbADO->Execute('
		UPDATE Device_DeviceData 
		INNER JOIN Device ON FK_Device=PK_Device
		SET IK_DeviceData=?
		WHERE FK_Device_ControlledVia=? AND FK_DeviceData=?
	',array($value,$orbiterID,$deviceDataID));
}

function set_device_data($deviceID,$deviceData,$value,$dbADO){
	$dbADO->Execute("UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?",array($value,$deviceID,$deviceData));
}

function displayScreenSavers($orbiterID,$dbADO)
{
	$out='';

	$categArray=getDescendantsForCategory($GLOBALS['ScreenSavers'],$dbADO);
	
	$templateArray=getArrayFromTable('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_DeviceCategory IN ('.join(',',$categArray).')','ORDER BY Description ASC');
	
	if(count($templateArray)==0)
		$templateArray[]=0;
	$ssArray=getFieldsAsArray('Device','PK_Device,FK_DeviceTemplate',$dbADO,'WHERE FK_Device_ControlledVia='.$orbiterID.' AND FK_DeviceTemplate IN ('.join(',',array_keys($templateArray)).')');
	if(count($ssArray)==0)
		$selectedss=0;
	else
		$selectedss=$ssArray['PK_Device'][0];
	$out.='
		<input type="hidden" name="oldssDT_'.$orbiterID.'" value="'.@$ssArray['FK_DeviceTemplate'][0].'">
		<input type="hidden" name="oldss_'.$orbiterID.'" value="'.$selectedss.'">';
		
	$out.=pulldownFromArray($templateArray,'ss_'.$orbiterID,@$ssArray['FK_DeviceTemplate'][0],'style="width:200px;"');
	
	return $out;
}

function processScreenSavers($orbiterID,$dbADO)
{
	$oldss=(int)@$_POST['oldss_'.$orbiterID];
	$oldssDT=(int)@$_POST['oldssDT_'.$orbiterID];
	$newss=(int)@$_POST['ss_'.$orbiterID];
	if($oldss==0){
		if($newss!=0){
			createDevice($newss,$_SESSION['installationID'],$orbiterID,NULL,$dbADO,0);
		}
	}else{
		if($newss==0){
			deleteDevice($oldss,$dbADO);
		}elseif($oldssDT!==$newss){
			deleteDevice($oldss,$dbADO);
			createDevice($newss,$_SESSION['installationID'],$orbiterID,NULL,$dbADO,0);
		}
	}
}

function restartX($orbiterID,$IPAddress){
	// todo for Razvan: put correct script here
	exec_batch_command('/usr/pluto/bin/RestartX.sh '.$orbiterID.' '.$IPAddress);
}

function savePic($url,$path){
	// remove \n from url
	$url=str_replace("\n",'',$url);
	
	// copy the pic localy if the param is specified
	// path need to be a working path with write permission
	if($path!=''){
		if(file_exists($path)){
			$cmd='sudo -u root rm -f "'.$path.'"';
			exec_batch_command($cmd);
		}
		$getPicCmd='wget --header=\'User-Agent: Lynx/2.8.5rel.1 libwww-FM/2.14 SSL-MM/1.4.1 GNUTLS/1.0.16\' -O - \''.$url.'\' > '.$path;
		$coverArtPage=exec_batch_command($getPicCmd,1);
	}
}

function deleteScans($id,$mediadbADO){
	$picsToDel=array_keys(getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','PK_CoverArtScanEntry',$mediadbADO,'WHERE FK_CoverArtScan='.$id));
	foreach ($picsToDel AS $pic){
		@unlink('/home/coverartscan/'.$pic.'.jpg');
		@unlink('/home/coverartscan/'.$pic.'_tn.jpg');
	}
	$mediadbADO->Execute('DELETE FROM CoverArtScanEntry WHERE FK_CoverArtScan=?',array($id));
	$mediadbADO->Execute('DELETE FROM CoverArtScan WHERE PK_CoverArtScan=?',array($id));
}

function syncAttributes($table,$itemValue,$id,$mediadbADO){
	$attributeTypes=getAssocArray('AttributeType','Description','PK_AttributeType',$mediadbADO);
	$existingAttributes=getAssocArray($table.'_Attribute','Attribute.Name','FK_Attribute',$mediadbADO,'INNER JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_'.$table.'='.$itemValue);
	$joinArray=(count($existingAttributes)>0)?array_values($existingAttributes):array(0);
	$combined=getAssocArray($table.'_Attribute','Attribute.Name','FK_Attribute',$mediadbADO,'INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute INNER JOIN '.$table.' ON FK_'.$table.'=PK_'.$table.' INNER JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType WHERE FK_Attribute IN ('.join(',',$joinArray).') AND CombineAsOne=1');

	
	$allAttributes=getAssocArray('Attribute','Attribute.Name','PK_Attribute',$mediadbADO);
	$allAttributeTypes=getAssocArray('Attribute','PK_Attribute','FK_AttributeType',$mediadbADO);
	$amazonAttributes=getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','Attributes',$mediadbADO,'WHERE PK_CoverArtScanEntry='.$id);
	if(count($amazonAttributes)==0){
		return '';		
	}
	$amazonAttributesArray=explode("\n",$amazonAttributes[$id]);

	foreach ($amazonAttributesArray AS $line){
		$parts=explode("\t",trim($line));
		// todo: attribute mapping
		if(count($parts)==2){
			$atype=str_replace(array('Average Customer Review', 'Actor','Directors','Artist'),array('Rating', 'Performer','Director','Performer'),$parts[0]);
			$aname=$parts[1];
			
			if($atype=='Synopsis'){
				$aname=str_replace(array('<p>','</p>','<br>'),array("\n",'',"\n",),$aname);
				$aname=strip_tags($aname);
				$mediadbADO->Execute('INSERT INTO LongAttribute (FK_'.$table.',FK_AttributeType,Text)  VALUES (?,?,?)',array($itemValue,37,$aname));
			}else{
				if(isset($attributeTypes[$atype])){
					// if the combination of attribute/attribute type doesn't exist, add it
					if(!(isset($allAttributes[$aname]) && ($allAttributeTypes[$allAttributes[$aname]] == $attributeTypes[$atype]))){
						$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($attributeTypes[$atype],$aname));
						$aid=$mediadbADO->Insert_Id();
					}else{
						// if it's CombineAsOne=1 add it
						$noAdd=(isset($combined[$aname]))?1:0;
						$aid=$allAttributes[$aname];
					}
		
					if(!isset($existingAttributes[$aname]) && @$noAdd==0){
		
						$mediadbADO->Execute('INSERT INTO '.$table.'_Attribute (FK_'.$table.',FK_Attribute) VALUES (?,?)',array($itemValue,$aid));
					}
				}
			}
		}
	}
}

function generateTopMenu($website,$installationID,$dbADO)
{

	$selectMenu = "
		SELECT DISTINCT PageSetup.* FROM PageSetup 
		LEFT JOIN DeviceTemplate ON PageSetup.FK_Package=DeviceTemplate.FK_Package
		LEFT JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		WHERE showInTopMenu = 1 AND Website=? AND (PageSetup.FK_Package IS NULL OR (PK_Device IS NOT NULL AND FK_Installation=?))
		ORDER BY OrderNum ASC";
	$res = $dbADO->Execute($selectMenu,array($website,$installationID));

	$menuItems=array();
	$GLOBALS['labelArray']=array();
	$GLOBALS['urlArray']=array();
	while ($row= $res->FetchRow()) {
		$menuItems[(int)$row['FK_PageSetup_Parent']][]=$row['PK_PageSetup'];
		$GLOBALS['labelArray'][$row['PK_PageSetup']]=$row['Description'];
		$GLOBALS['urlArray'][$row['PK_PageSetup']]=$row['pageURL'];
	}
	
	$menu='
	<div id="menu">
	';
	foreach ($menuItems[0] AS $rootNode){
		$menu.='
		<ul id="nav">
		  <li><h2><a class="topitem" href="'.((isset($GLOBALS['urlArray'][$rootNode]) && $GLOBALS['urlArray'][$rootNode]!='')?@$GLOBALS['urlArray'][$rootNode]:'javascript:void(0);').'" class="topurl">'.str_replace(' ','&nbsp;',$GLOBALS['labelArray'][$rootNode]).'</a></h2>
		  '.menuChildItems($rootNode,$menuItems).'
		  </li>
		  </ul>
		  ';
	}
	$menu.='
	
	</div>';
	
	return $menu;
}

function menuChildItems($parentNode,$menuItems){

	$menu='';

	if(count(@$menuItems[$parentNode])>0){
		$menu='
		<ul>';
		foreach ($menuItems[$parentNode] AS $node){
		if ($GLOBALS['labelArray'][$node]!='Phones setup') {
			$menu.='
			<li><a href="'.((isset($GLOBALS['urlArray'][$node]) && $GLOBALS['urlArray'][$node]!='')?@$GLOBALS['urlArray'][$node]:'javascript:void(0);').'">'.$GLOBALS['labelArray'][$node].'</a>'.menuChildItems($node,$menuItems).'</li>';
			} else {
			$menu.='
			<li><a href="index.php?section=devices&type=phones">'.$GLOBALS['labelArray'][$node].'</a>'.menuChildItems($node,$menuItems).'</li>';
		}
		}
		$menu.='
		</ul>';
	}
		
	return $menu;
}

function updateDeviceControlledBy($deviceID,$controlledBy,$dbADO){
	if(!is_null($controlledBy)){
		$topParent=getTopLevelParent($controlledBy,$dbADO);
		
		// if the top level parent is changed, set need configure and call config_device_changes
		if($topParent==$controlledBy){
			$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',array($controlledBy));
			
			$installationID = (int)$_SESSION['installationID'];
			$appServerID=getDeviceFromDT($installationID,$GLOBALS['AppServer'],$dbADO);
			
			appServerExec($appServerID,'config_device_changes');
		}
	}
	
	$dbADO->Execute('UPDATE Device SET FK_Device_ControlledVia=? WHERE PK_Device=?',array($controlledBy,$deviceID));
}

function appServerExec($appServerID,$command,$params=''){
	// /usr/pluto/bin/MessageSend dcerouter -targetType device 0 $APPSERVERID 1 67 13 "$COMMAND"
	$cmd='/usr/pluto/bin/MessageSend dcerouter -targetType device 0 '.$appServerID.' 1 67 13 "'.$command.'"'.(($params!='')?' 51 '.$params:'');
	return exec_batch_command($cmd,1);
}

function getOrbitersInTheSameRoom($commandGroupID,$dbADO){
	if($commandGroupID==0){
		return array();
	}
	$eaArray=array_keys(getAssocArray('CommandGroup_EntertainArea','FK_EntertainArea','FK_EntertainArea',$dbADO,'WHERE FK_CommandGroup='.$commandGroupID));
	if(count($eaArray)>0){
		return array_keys(getAssocArray('Device','PK_Device','Description',$dbADO,'INNER JOIN Device_EntertainArea ON FK_Device=PK_Device WHERE FK_EntertainArea IN ('.join(',',$eaArray).') AND FK_DeviceTemplate='.$GLOBALS['ONScreenOrbiter'].' GROUP BY PK_Device'));
	}
}

function editCommandGroupCommands($commandGroupID,$dbADO){
	$selectCommandsAssigned = "
		SELECT CommandGroup_Command.*
		FROM CommandGroup_Command		
		LEFT JOIN Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
		WHERE CommandGroup_Command.FK_CommandGroup = ?
		ORDER BY OrderNum ASC
		";
	$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($commandGroupID));
	
	$out='
	<script type="text/javascript" src="javascript/wz_tooltip/wz_tooltip.js"></script>
	<input type="hidden" name="cgcID" id="cgcID" value="">';
	if ($resCommandAssigned->RecordCount()>0) {
		$out.='	<fieldset>
					<legend>'.translate('TEXT_COMMANDS_CONST').'</legend>
						<table cellpadding="0" cellspacing="0">
									';
		$pos=0;
		// TODO: get device ids for -300 = local orbiter and multiple orbiters handling
		$installationID = (int)@$_SESSION['installationID'];

		while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {
			$devicesToSend=array();
			$commandsToSend=array();
			if($rowCommandAssigned['FK_Device']==-300){
				$devicesToSend=getOrbitersInTheSameRoom($commandGroupID,$dbADO);
			}else{
				$devicesToSend=array($rowCommandAssigned['FK_Device']);	
			}
			if(count($devicesToSend)>0){
				foreach ($devicesToSend AS $deviceID){
					$commandsToSend[$deviceID] = "'/usr/pluto/bin/MessageSend dcerouter -targetType device 0 $deviceID'+' 1 ".$rowCommandAssigned['FK_Command']."'";
				}
			}
			$class_color=(($pos%2==0)?'alternate_back':'');
			if($pos > 0) $out.='<tr><td colspan="3"><hr/></td></tr>';
			$out.='
						<tr class="'.$class_color.'">
							<td valign="top">
								<a name="hook_'.$rowCommandAssigned['PK_CommandGroup_Command'].'"></a>
								<input type="hidden" name="device_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" value="'.$rowCommandAssigned['FK_Device'].'">
								'.getDeviceNameForScenarios($rowCommandAssigned['FK_Device'],$dbADO).'</td>';

			$out.='
					 		</td>
						<td>
						';
			if($rowCommandAssigned['FK_Device']!='-300'){
				$query = "
					SELECT PK_Command,Command.Description,Device.FK_DeviceTemplate,DeviceTemplate.FK_InfraredGroup AS FK_InfraredGroup
					FROM Device 
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
					INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
					INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
					INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
					WHERE PK_Device=?";
				$resNewCommand = $dbADO->Execute($query,array($rowCommandAssigned['FK_Device']));
			}else{
				$query = "
					SELECT PK_Command,Command.Description
					FROM Command 
					INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_Command = Command.PK_Command
					INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
					WHERE FK_DeviceTemplate=?
					ORDER BY Command.Description ASC";
				$resNewCommand = $dbADO->Execute($query,array(cleanInteger($GLOBALS['deviceTemplateOrbiter'])));
			}
			if ($resNewCommand) {
				$out.='<select name="deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">';
				
				$availableCommands=array();
				$irGroup=0;
				while ($rowNewCommand = $resNewCommand->FetchRow()) {
					$irGroup=(int)@$rowNewCommand['FK_InfraredGroup'];
					$availableCommands[$rowNewCommand['PK_Command']]=$rowNewCommand['Description'];
				}
				
				if($irGroup!=0){
					$availableCommands+=getAssocArray('InfraredGroup_Command','FK_Command','Description',$dbADO,'INNER JOIN Command ON FK_Command=PK_Command WHERE FK_InfraredGroup='.$irGroup);
				}	
				asort($availableCommands);		
				foreach ($availableCommands AS $command=>$description){
					$out.='<option value="'.$command.'" '.($rowCommandAssigned['FK_Command']==$command?'selected="selected"':'').'>'.$description.'</option>';
				}				
						
				if ($resNewCommand->RecordCount()==0) {
					$out.='<option value="0">-'.translate('TEXT_SCENARIO_NO_COMMAND_CONST').'-</option>';
				}
				$out.='</select>';
			}


			$query = "
				SELECT 
					Command_CommandParameter.FK_CommandParameter,
					Command_CommandParameter.Description as C_CP_Description,
					CommandParameter.Description as CP_Description,
					CommandGroup_Command_CommandParameter.IK_CommandParameter,
					ParameterType.Description as PT_Description,
					FK_CommandGroup_Command
				FROM Command_CommandParameter 
				INNER JOIN Command on FK_Command = PK_Command
				INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
				INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
				LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
				WHERE FK_Command = ? AND FK_CommandGroup_Command =?
				ORDER BY CommandParameter.Description asc
				";
			$resSelectParameters = $dbADO->Execute($query,array($rowCommandAssigned['FK_Command'],$rowCommandAssigned['PK_CommandGroup_Command']));


			if ($resSelectParameters) {
				$out.='<table>';
				while ($rowSelectParameters=$resSelectParameters->FetchRow()) {
					if(count($devicesToSend)>0){
						foreach ($devicesToSend AS $deviceID){
							$commandsToSend[$deviceID] .= "+escape(((document.getElementById('CommandParameterValue_".$rowCommandAssigned['PK_CommandGroup_Command']."_".$rowSelectParameters['FK_CommandParameter']."').value!='')?' ".$rowSelectParameters['FK_CommandParameter']." \''+document.getElementById('CommandParameterValue_".$rowCommandAssigned['PK_CommandGroup_Command']."_".$rowSelectParameters['FK_CommandParameter']."').value+'\'':''))";
						}					
					}
if($rowSelectParameters['C_CP_Description']=="") {
	$tooltip = "Sorry, no tip available for this command parameter."; 
} else { 
	$tooltip = $rowSelectParameters['C_CP_Description']; 
} 
										
					$out.="
						<tr ".(strlen(trim($rowSelectParameters['CP_Description']))==0?" bgColor='lightgreen' ":"").">
							<td>#{$rowSelectParameters['FK_CommandParameter']} <span title=\"{$rowSelectParameters['C_CP_Description']}\">{$rowSelectParameters['CP_Description']}</span> ({$rowSelectParameters['PT_Description']})</td>
							<td><input type='text' name=\"CommandParameterValue_{$rowCommandAssigned['PK_CommandGroup_Command']}_{$rowSelectParameters['FK_CommandParameter']}\" id=\"CommandParameterValue_{$rowCommandAssigned['PK_CommandGroup_Command']}_{$rowSelectParameters['FK_CommandParameter']}\" value=\"{$rowSelectParameters['IK_CommandParameter']}\" >";
							
							// if command == 13, display file picker link
							if($rowSelectParameters['FK_CommandParameter']==13){
								$out.=' <a href="javascript:windowOpen(\'index.php?section=filePicker&from=editCommandGroup&cgcID='.$rowCommandAssigned['PK_CommandGroup_Command'].'&cp='.$rowSelectParameters['FK_CommandParameter'].'\',\'width=640,height=480,toolbar=1,resizable=1,scrollbars=1\');"><img src="include/images/pick.gif" border="0" align="bottom"></a>';
							} else {
								$out.=' <img src="img/question.gif" onmouseover="Tip(\''.trim(addslashes($tooltip)).'\',ABOVE,true,WIDTH,350,FADEIN,250,FADEOUT,250,BGCOLOR,\'#C0FFC0\')" onmouseout="UnTip()">';
							}
							$out.='					
							<td>
						</tr>';

					// for FK_Command==741, GoTo screen, display the parameters for that screen
					if($rowCommandAssigned['FK_Command']==$GLOBALS['GoToScreen'] && (int)$rowSelectParameters['IK_CommandParameter']!=0 && $rowSelectParameters['FK_CommandParameter']==159){
						$resExtra=$dbADO->Execute('
							SELECT CommandParameter.Description,PK_CommandParameter,FK_CommandGroup_Command,IK_CommandParameter,Screen_CommandParameter.Description AS TitleLong,ParameterType.Description as PT_Description
							FROM Screen_CommandParameter 
							INNER JOIN CommandParameter ON Screen_CommandParameter.FK_CommandParameter=PK_CommandParameter
							INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
							LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter=PK_CommandParameter AND (FK_CommandGroup_Command=? OR FK_CommandGroup_Command IS NULL)
							LEFT JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
							WHERE FK_Screen=? 
							',array($rowSelectParameters['FK_CommandGroup_Command'],$rowSelectParameters['IK_CommandParameter']));
						while($row=$resExtra->FetchRow()){
							$out.="
							<tr>
								<td>#{$row['PK_CommandParameter']} <span title=\"{$row['TitleLong']}\">{$row['Description']}</span> ({$row['PT_Description']})</td>
								<td><input type='text' name=\"CommandParameterValue_{$rowCommandAssigned['PK_CommandGroup_Command']}_{$row['PK_CommandParameter']}\" value=\"{$row['IK_CommandParameter']}\" >
								</td>
							</tr>";
						}
					}
				}
				$out.='</table>';
			}
			
			$cmd_to_test="'no=".count($commandsToSend)."'";		
			$pos=0;
			if(count($devicesToSend)>0){
				foreach ($devicesToSend AS $deviceID){
					$pos++;
					$cmd_to_test.="+'&cmd$pos='+".$commandsToSend[$deviceID];
				}
			}		
			
			$out.='
					
							</td>
						<td valign="top">
						<input type="submit" class="button_fixed" name="up_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" value="'.translate('TEXT_UP_CONST').'"><br>
						<input type="submit" class="button_fixed" name="down_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" value="'.translate('TEXT_DOWN_CONST').'"><br>
						<input type="button" class="button_fixed" name="editA" value="'.translate('TEXT_REMOVE_CONST').'" onClick="if(confirm(\''.translate('TEXT_CONFIRM_DELETE_COMMAND_FROM_CG_CONST').'\'))windowOpen(\'index.php?section=deleteCommandFromCommandGroup_Command&from=editCommandGroup&cgID='.$rowCommandAssigned['PK_CommandGroup_Command'].'\',\'width=100,height=100,toolbar=1,resizable=1,scrollbars=1\');"">
						</td>						
					</tr>
					<tr class="'.$class_color.'">
						<td align="center" colspan="3">
							<input type="button" class="button" name="testCommand" value="'.translate('TEXT_TEST_COMMAND_CONST').'" onClick="frames[\'commandTester\'].location=\'testCommand.php?\'+'.$cmd_to_test.';">
						</td>
					</tr>
					';
			$pos++;
		}

		$out.='
									<tr>
										<td colspan="3" align="center">
											<a name="hook_0"></a>
											<input type="submit" class="button" name="addNewDeviceButton" value="'.translate('TEXT_SAVE_CONST').'"  >
										</td>
									</tr>
								</table>
								<iframe name="commandTester" id="commandTester" src="testCommand.php" width="500" height="200" style="display:none;"></iframe>
				</fieldset>
				';				
	}
	
	return $out;
	
}

function addCommandToCommandGroup($cgID,$dbADO){
	$out='<a name="addDevice"></a>
					 Device:'.
	deviceForScenariosSelector('newDevice',cleanInteger(@$_REQUEST['newDevice']),$dbADO,1,'onChange="this.form.action.value=\'form\';this.form.submit();"');

	if (isset($_REQUEST['newDevice']) && cleanInteger($_REQUEST['newDevice'])!=0) {
		if(cleanInteger(@$_REQUEST['newDevice']!='-300')){
			$query = "
				SELECT PK_Command,Command.Description,Device.FK_DeviceTemplate,DeviceTemplate.FK_InfraredGroup AS FK_InfraredGroup
				FROM Device
				INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
				INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				WHERE PK_Device=?";
			$resNewCommand = $dbADO->Execute($query,array(cleanInteger($_REQUEST['newDevice'])));
		}else{
			$query = "
				SELECT PK_Command,Command.Description
				FROM Command 
				INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_Command = Command.PK_Command
				INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
				WHERE FK_DeviceTemplate=?
				ORDER BY Command.Description ASC";
			$resNewCommand = $dbADO->Execute($query,array(cleanInteger($GLOBALS['deviceTemplateOrbiter'])));
		}

		$out.=' Command: <select name="addNewDeviceCommand">
							<option value="0">-please select-</option>';
		$availableCommands=array();
		$irGroup=0;
		while ($rowNewCommand = $resNewCommand->FetchRow()) {
			$irGroup=(int)@$rowNewCommand['FK_InfraredGroup'];
			$availableCommands[$rowNewCommand['PK_Command']]=$rowNewCommand['Description'];
		}
		
		if($irGroup!=0){
			$availableCommands+=getAssocArray('InfraredGroup_Command','FK_Command','Description',$dbADO,'INNER JOIN Command ON FK_Command=PK_Command WHERE FK_InfraredGroup='.$irGroup);
		}	
		asort($availableCommands);
			
		foreach ($availableCommands AS $command=>$description){
			$out.='<option value="'.$command.'">'.$description.'</option>';
		}
		if ($resNewCommand->RecordCount()==0) {
			$out.='<option value="0">-no command-</option>';
		}
		$resNewCommand->Close();
		$out.='</select>';
	}
	$out.='
			<input type="submit" class="button" name="addNewDeviceButton" value="'.translate('TEXT_ADD_CONST').'"  >
			';
	return $out;
	
}

function processEditScenarioCommands($commandGroupID,$dbADO){
	
	includeLangFile('editCommandGroup.lang.php');

	$isModified=0;
	$parametersUpdatedAlert='';
	$returnHook='';
	
	$installationID = (int)@$_SESSION['installationID'];
	$x=cleanInteger(@$_POST['newDevice']);
	$y=cleanInteger(@$_POST['addNewDeviceCommand']);

	
	if ($y!=0 && $x!=0) {
		$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command
				(FK_CommandGroup,FK_Command,FK_Device) values(?,?,?)";			
		$insertRs = $dbADO->Execute($queryInsertCommandGroup_Command,array($commandGroupID,$y,$x));
		
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=PK_CommandGroup_Command WHERE PK_CommandGroup_Command=?',array($dbADO->Insert_Id()));
		$hook='hook_'.$dbADO->Insert_ID();
	}

	$isModified = 0;
	if ($dbADO->Affected_Rows()>0) {
		$isModified=1;
	}
	$selectCommandsAssigned = "
				SELECT CommandGroup_Command.* FROM 
					CommandGroup_Command				
					LEFT JOIN 
						Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
					WHERE 
						(Device.FK_Installation = ? OR Device.FK_Installation IS NULL)
				AND CommandGroup_Command.FK_CommandGroup = ?
				";								

	$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($installationID,$commandGroupID));

	$parametersUpdatedAlert = translate('TEXT_PARAMETERS_NOT_UPDATED_CONST');

	if ($resCommandAssigned) {
		while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {
			if(isset($_POST['up_'.$rowCommandAssigned['PK_CommandGroup_Command']])){
				commandMoveUp($rowCommandAssigned['PK_CommandGroup_Command'],$rowCommandAssigned['OrderNum'],$commandGroupID,$dbADO);
			}
			
			if(isset($_POST['down_'.$rowCommandAssigned['PK_CommandGroup_Command']])){
				commandMoveDown($rowCommandAssigned['PK_CommandGroup_Command'],$rowCommandAssigned['OrderNum'],$commandGroupID,$dbADO);
			}
			

			$deviceSelected = isset($_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$x;
			$commandSelected = isset($_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$y;

			$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Device = ? WHERE PK_CommandGroup_Command = ? ';
			$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($deviceSelected,$rowCommandAssigned['PK_CommandGroup_Command']));

			if ($dbADO->Affected_Rows()==1) {//enter here only if the Device is changed
				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = NULL WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($rowCommandAssigned['PK_CommandGroup_Command']));
				//delete old parameters
				$deleteParamValues = 'delete from CommandGroup_Command_CommandParameter where
									FK_CommandGroup_Command = ? 
						';
				$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));

			} elseif($commandSelected!=0) {
				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = ? WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($commandSelected,$rowCommandAssigned['PK_CommandGroup_Command']));
				if ($dbADO->Affected_Rows()==1) {//if we have changed the command, delete old values
					$returnHook=$rowCommandAssigned['PK_CommandGroup_Command'];
					$deleteParamValues = 'delete from CommandGroup_Command_CommandParameter where
									FK_CommandGroup_Command = ? 
							';
					$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));
				}
				//echo "here2";

			}
			//die();
			//command parameters

			$query = "SELECT Command_CommandParameter.FK_CommandParameter,Command_CommandParameter.Description as C_CP_Description,CommandParameter.Description as CP_Description,
										CommandGroup_Command_CommandParameter.IK_CommandParameter,
										ParameterType.Description as PT_Description
											FROM Command_CommandParameter 
												INNER JOIN Command on FK_Command = PK_Command
												INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
												INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
												LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
									  WHERE FK_Command = ?
								";

			$resSelectParameters = $dbADO->Execute($query,array($commandSelected));

			if ($resSelectParameters) {
				while ($rowSelectParameters=$resSelectParameters->fetchRow()) {
					$value=cleanString(@$_POST['CommandParameterValue_'.$rowCommandAssigned['PK_CommandGroup_Command'].'_'.$rowSelectParameters['FK_CommandParameter']]);
					//see if we need to update or to insert
					$checkExists = "SELECT * FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter = ?";
					$resExists=$dbADO->Execute($checkExists,array($rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					if ($resExists->RecordCount()==1) {
						$update = "UPDATE CommandGroup_Command_CommandParameter  SET  IK_CommandParameter  = ? WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter =? ";
						$dbADO->Execute($update,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					} else {
						$insert = "INSERT INTO CommandGroup_Command_CommandParameter (IK_CommandParameter,FK_CommandGroup_Command,FK_CommandParameter) values(?,?,?)";
						$dbADO->Execute($insert,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
					}
					if ($dbADO->Affected_Rows()==1) {
						$parametersUpdatedAlert=translate('TEXT_PARAMETERS_UPDATED_CONST');
					}

					if($rowCommandAssigned['FK_Command']==$GLOBALS['GoToScreen'] && (int)$rowSelectParameters['IK_CommandParameter']!=0 && $rowSelectParameters['FK_CommandParameter']==159){
						$resExtra=$dbADO->Execute('
									SELECT CommandParameter.Description,PK_CommandParameter,FK_CommandGroup_Command,IK_CommandParameter,Screen_CommandParameter.Description AS TitleLong,ParameterType.Description as PT_Description
									FROM Screen_CommandParameter 
									INNER JOIN CommandParameter ON Screen_CommandParameter.FK_CommandParameter=PK_CommandParameter
									INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
									LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter=PK_CommandParameter AND (FK_CommandGroup_Command=? OR FK_CommandGroup_Command IS NULL)
									LEFT JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
									WHERE FK_Screen=? 
									',array($rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['IK_CommandParameter']));
						while($row=$resExtra->FetchRow()){
							$value=cleanString(@$_POST['CommandParameterValue_'.$rowCommandAssigned['PK_CommandGroup_Command'].'_'.$row['PK_CommandParameter']]);

							//see if we need to update or to insert
							$checkExists = "SELECT * FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter = ?";
							$resExists=$dbADO->Execute($checkExists,array($rowCommandAssigned['PK_CommandGroup_Command'],$row['PK_CommandParameter']));
							if ($resExists->RecordCount()==1) {
								$update = "UPDATE CommandGroup_Command_CommandParameter  SET  IK_CommandParameter  = ? WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter =? ";
								$dbADO->Execute($update,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$row['PK_CommandParameter']));
							} else {
								$insert = "INSERT INTO CommandGroup_Command_CommandParameter (IK_CommandParameter,FK_CommandGroup_Command,FK_CommandParameter) values(?,?,?)";
								$dbADO->Execute($insert,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$row['PK_CommandParameter']));
							}
							if ($dbADO->Affected_Rows()==1) {
								$parametersUpdatedAlert=translate('TEXT_PARAMETERS_UPDATED_CONST');
							}

						}
					}
				}
			}


		}
	}

	return array($isModified,$parametersUpdatedAlert,$returnHook);
}

function commandMoveUp($cgcID,$pos,$cgID,$dbADO){
	if((int)$pos==0){
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($cgcID,$cgcID));
		$pos=$cgcID;
	}

	$upperCmds=getAssocArray('CommandGroup_Command','PK_CommandGroup_Command','OrderNum',$dbADO,'WHERE FK_CommandGroup='.$cgID.' AND OrderNum < '.$pos.' ORDER BY OrderNum DESC LIMIT 0,1');
	if(count($upperCmds)>0){
		$idArray=array_keys($upperCmds);
		$posArray=array_values($upperCmds);
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($pos,$idArray[0]));
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($posArray[0],$cgcID));
	}
}

function commandMoveDown($cgcID,$pos,$cgID,$dbADO){
	if((int)$pos==0){
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($cgcID,$cgcID));
		$pos=$cgcID;
	}

	$upperCmds=getAssocArray('CommandGroup_Command','PK_CommandGroup_Command','OrderNum',$dbADO,'WHERE FK_CommandGroup='.$cgID.' AND OrderNum > '.$pos.' ORDER BY OrderNum ASC LIMIT 0,1');
	if(count($upperCmds)>0){
		$idArray=array_keys($upperCmds);
		$posArray=array_values($upperCmds);
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($pos,$idArray[0]));
		$dbADO->Execute('UPDATE CommandGroup_Command SET OrderNum=? WHERE PK_CommandGroup_Command=?',array($posArray[0],$cgcID));
	}	
}

function updateRoomForEmbeddedDevices($deviceID,$roomID,$dbADO){
	$embedded=getAssocArray('Device','Device.PK_Device AS PK_Device','Device.FK_DeviceTemplate',$dbADO,'
	INNER JOIN Device Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device
	INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate_ControlledVia=Parent.FK_DeviceTemplate AND Device.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate AND RerouteMessagesToParent=1
	WHERE Device.FK_Device_ControlledVia='.$deviceID);
	$embeddedKeys=array_keys($embedded);
	
	if(count($embeddedKeys)>0){
		$dbADO->Execute('UPDATE Device SET FK_Room=? WHERE PK_Device IN ('.join(',',$embeddedKeys).')',array($roomID));
	}
}

function updateEAForEmbeddedDevices($deviceID,$eaID,$dbADO){
	$embedded=getAssocArray('Device','Device.PK_Device AS PK_Device','Device.FK_DeviceTemplate',$dbADO,'
	INNER JOIN Device Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device
	INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate_ControlledVia=Parent.FK_DeviceTemplate AND Device.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate AND RerouteMessagesToParent=1
	WHERE Device.FK_Device_ControlledVia='.$deviceID);
	
	
	if(count($embedded)>0){
		foreach ($embedded AS $key=>$dt){
			$dbADO->Execute('INSERT IGNORE INTO Device_EntertainArea (FK_EntertainArea,FK_Device) VALUES (?,?)',array($eaID,$key));
		}
	}
}


function set_connector($mediaDirectorID,$value){
	$cmd='sudo -u root /usr/pluto/bin/set_connector.sh';
	
	exec_batch_command($cmd);
}

function set_tv_standard($mediaDirectorID,$value){
	$cmd='sudo -u root /usr/pluto/bin/set_tv_standard.sh';
	
	exec_batch_command($cmd);
}

function getCoreIP($dbADO)
{
	$res=$dbADO->Execute('SELECT IPaddress FROM Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=?',array($GLOBALS['CategoryCore']));
	$row=$res->FetchRow();
	return $row['IPaddress'];
}

function write_weblog($str){
	writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\t".$str."\n",'a+');
}

function get_alert_pic($picname,$PK_Alert_Device=0){
	$extensionsArray=array('png','jpg','gif');
	
	foreach ($extensionsArray AS $extension){
		$picPath=$GLOBALS['SecurityPicsPath'].$picname.'.'.$extension;	
		if(file_exists($picPath)){
			return $picPath;
		}
	}
	if($PK_Alert_Device!=0){
		$diskAlerts=grabAlertFiles($GLOBALS['SecurityPicsPath'],'alert_'.$PK_Alert_Device.'_cam');
		if(count($diskAlerts)!=0){
			return $diskAlerts[0];
		}
	}
	
	return APPROOT.'include/images/alert_no_pic.png';
}

function grabAlertFiles($path,$file_prefix='') {
	$filesArray=array();
	
	$f = 0;
	$filesArray = array();
	if (@$handle = opendir($path)) {
		while (false!== ($file = readdir($handle))) {
			if($file!= "." && $file!= "..") {
				if($file_prefix=='' || ($file_prefix!='' && strpos($file,$file_prefix)!==false)){
					$fName = $file;
					$file = $path.$file;
					$filesArray[$f++] = $file;
				}
			};
		};
		closedir($handle);
	};
	asort( $filesArray ); 
	reset( $filesArray );


	return $filesArray;
} 


function parse_ini_str ( $ini) {
    $lines= explode("\n",$ini);
    $ret=array();
    
    if(count($lines)>0){
	    foreach ($lines AS $line){
	    	$parts=explode('=',$line);
	    	if(count($parts)==2){
	    		$ret[$parts[0]]=$parts[1];
	    	}
	    }
    }
    
    return $ret;
}

function same_network($requestIP,$serverIP,$netMask){
	return ((ip2long($requestIP) & ip2long($netMask)) == (ip2long($serverIP) & ip2long($netMask)))?true:false;
}

function quick_search_box($path=''){
	$out='
	<form name="mediaSearchFrm" method="GET" action="index.php">
	<input type="hidden" name="section" value="searchMedia">
	<input type="hidden" name="path" value="'.htmlentities($path).'">
	<input type="hidden" name="partial" value="1">
	<input type="hidden" name="attributes" value="1">
	<input type="text" name="keyword" value=""> <input type="submit" name="doSearch" class="button" value="Search">
	</form>
	';
	
	return $out;
}

function remove_cover_arts($fileID,$mediadbADO){
	$picsArray=getAssocArray('Picture_File','FK_Picture','FK_Picture',$mediadbADO,'WHERE FK_File='.$fileID);
	if(count($picsArray)==0){
		return false;
	}
	
	foreach ($picsArray as $key=>$value) {
		exec_batch_command('sudo -u root rm '.$GLOBALS['mediaPicsPath'].$key.'_tn.jpg');
		exec_batch_command('sudo -u root rm '.$GLOBALS['mediaPicsPath'].$key.'.jpg');
	}
	
	$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File=? AND FK_Picture IN ('.join(',',array_values($picsArray)).')', array($fileID));
}

function bash_escape($str){
	$str=str_replace(array('\\','$','"','`'),array('\\\\','\$','\"','\`'),$str);
	
	return $str;
}

function getRows($tableName,$fields,$dbADO,$filter='')
{
	$res=$dbADO->execute("SELECT $fields FROM $tableName $filter");
	$fields=str_replace('DISTINCT ','',$fields);
	$fieldsArray=explode(',',$fields);

	$result=array();
	while($row=$res->Fetchrow()){
		foreach ($fieldsArray AS $field){
			$cleanField=(strpos($field,'.')!==false)?substr($field,strpos($field,'.')+1):$field;
			$cleanField=(strpos($cleanField,' AS ')!==false)?substr($cleanField,strpos($cleanField,' AS ')+4):$cleanField;
			$cleanField=trim(str_replace('`','',$cleanField));

			$result[]=$row;
		}
	}
	return $result;	
}

function delete_media_pic($picID,$mediadbADO){
	if($picID!=0){
		$mediadbADO->Execute('DELETE FROM Picture WHERE PK_Picture=?',array($picID));
	
		$mediadbADO->Execute('DELETE FROM Picture_Attribute WHERE FK_Picture=?',array($picID));
	
		$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_Picture=?',array($picID));
	
		@unlink($GLOBALS['mediaPicsPath'].$picID.'.jpg');
		@unlink($GLOBALS['mediaPicsPath'].$picID.'_tn.jpg');
	}
}

function delete_media_pic_if_unused($picID,$mediadbADO) {
	//Is anything else referencing this picture?
	$pictureReference =         'SELECT * FROM Picture_Attribute WHERE FK_Picture = ?';
	$pictureReference .= ' UNION SELECT * FROM Picture_Disc WHERE FK_Picture = ?';
	$pictureReference .= ' UNION SELECT * FROM Picture_Download WHERE FK_Picture = ?';
	$pictureReference .= ' UNION SELECT * FROM Picture_File WHERE FK_Picture = ?';
	$numPictureReferences = $mediadbADO->Execute($pictureReference, array($picID,$picID,$picID,$picID));

	//Delete picture from database and file system only if nothing else is referencing it!
	if($numPictureReferences->RecordCount()==0) {
		$deletePic='DELETE FROM Picture WHERE PK_Picture=?';
		$mediadbADO->Execute($deletePic,array($picID));
		unlink($GLOBALS['mediaPicsPath'].$picID.'.jpg');
		unlink($GLOBALS['mediaPicsPath'].$picID.'_tn.jpg');
	}
}

/**
 * Correctly quotes a string so that all strings are escaped. We prefix and append
 * to the string single-quotes.
 * An example is  escape ( "Don't bother",magic_quotes_runtime () );
 *
 * @param str			the string to quote
 * @param [magic_quotes]	if $s is GET/POST var, set to get_magic_quotes_gpc().
 *
 * @return  quoted string to be sent back to database
 *
 * @author http://www.roscripts.com/snippets/show/157
*/
function escape ( $str, $magic_quotes = false )
{
		switch ( gettype ( $str ) )
		{
			case 'string'	:
				$replaceQuote = "\\'"; 	/// string to use to replace quotes
				if ( ! $magic_quotes ) {

					if ( $replaceQuote [ 0 ] == '\\' ){
						// only since php 4.0.5
						$str = str_replace ( array ( '\\', "\0" ), array ( '\\\\', "\\\0" ), $str );
						//$s = str_replace("\0","\\\0", str_replace('\\','\\\\',$s));
					}
					return  "'" . str_replace ( "'", $replaceQuote, $str ) . "'";
				}

				// undo magic quotes for "
				$str = str_replace ( '\\"','"', $str );

				if ( $replaceQuote == "\\'" ) {// ' already quoted, no need to change anything
					return "'$str'";
				}
				else {// change \' to '' for sybase/mssql
					$str = str_replace ( '\\\\','\\', $str );
					return "'" . str_replace ( "\\'", $treplaceQuote, $str ) . "'";
				}
			break;
			case 'boolean'	:	$str = ($str === FALSE) ? 0 : 1;
						return $str;
				break;
			case 'integer'	:	$str = ($str === NULL) ? 'NULL' : $str;
						return $str;
				break;
			default		:	$str = ($str === NULL) ? 'NULL' : $str;
						return $str;
				break;
		}
}

?>
