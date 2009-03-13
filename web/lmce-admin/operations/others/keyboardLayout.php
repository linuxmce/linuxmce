<?
function keyboardLayout($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/keyboardLayout.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	
	$mediaDirectors=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
	if(count($mediaDirectors)!=0){
		$mediaDirectorsIPs=getAssocArray('Device','PK_Device','IPAddress',$dbADO,'WHERE PK_Device IN ('.join(',',array_keys($mediaDirectors)).')');
	}
		
	if($action=='form'){
		// changed to hard-coded file 
		// $xmlPath=(file_exists('/etc/X11/xkb/rules/xfree86.xml'))?'/etc/X11/xkb/rules/xfree86.xml':'xfree86.xml';
		$xmlPath='xfree86.xml';
		
		$parser=new xml2Array();
		$arr=$parser->parse(join('',file($xmlPath)));
		$keyboardLayoutsArray=getLayoutsArray($arr[0]['children']);
		
		
		$coreKL=exec('/usr/pluto/bin/XKbLayout.sh get 127.0.0.1');

		$out.=setLeftMenu($dbADO).'
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<h3 align="left">'.$TEXT_KEYBOARD_LAYOUT_CONST.'</h3>
			<form action="index.php" method="POST" name="keyboardLayout">
			<input type="hidden" name="section" value="keyboardLayout">
			<input type="hidden" name="action" value="add">
		
		<table cellpadding="2" cellspacing="0">
			<tr class="tablehead">
				<td colspan="2 align="center""><B>'.$TEXT_MEDIA_DIRECTORS_CONST.'</B></td>
			</tr>';
		foreach ($mediaDirectors AS $mdID=>$mdName){
			$mdKL=exec('/usr/pluto/bin/XKbLayout.sh get '.((isset($mediaDirectorsIPs[$mdID]) && $mediaDirectorsIPs[$mdID]!='')?$mediaDirectorsIPs[$mdID]:'127.0.0.1'));
			$out.='
			<tr>
				<td>'.$mdName.'</td>
				<td>'. keyboardLayoutPulldown($keyboardLayoutsArray,$mdKL,'md_'.$mdID).'</td>
			</tr>';
		}
		$out.='
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'"></td>
			</tr>
		</table>
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=keyboardLayout&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		//exec('sudo -u root /usr/pluto/bin/XKbLayout.sh set 127.0.0.1 '.$_POST['core_KL']);

		foreach ($mediaDirectors AS $mdID=>$mdName){
			
			$cmd='sudo -u root /usr/pluto/bin/XKbLayout.sh set '.((isset($mediaDirectorsIPs[$mdID]) && $mediaDirectorsIPs[$mdID]!='')?$mediaDirectorsIPs[$mdID]:'127.0.0.1').' '.$_POST['md_'.$mdID];
			exec($cmd);
		}
		
		header('Location: index.php?section=keyboardLayout&msg='.$TEXT_KEYBOARD_LAYOUTS_UPDATED_CONST);
	}

	$output->setMenuTitle($TEXT_WIZARD_CONST.' |');
	$output->setPageTitle($TEXT_KEYBOARD_LAYOUT_CONST);
	
	$output->setNavigationMenu(array($TEXT_KEYBOARD_LAYOUT_CONST=>'index.php?section=keyboardLayout'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_KEYBOARD_LAYOUT_CONST);
	$output->output();
}



class xml2Array {
  
   var $arrOutput = array();
   var $resParser;
   var $strXmlData;
  
   function parse($strInputXML) {
  
           $this->resParser = xml_parser_create ();
           xml_set_object($this->resParser,$this);
           xml_set_element_handler($this->resParser, "tagOpen", "tagClosed");
          
           xml_set_character_data_handler($this->resParser, "tagData");
      
           $this->strXmlData = xml_parse($this->resParser,$strInputXML );
           if(!$this->strXmlData) {
               die(sprintf("XML error: %s at line %d",
           xml_error_string(xml_get_error_code($this->resParser)),
           xml_get_current_line_number($this->resParser)));
           }
                          
           xml_parser_free($this->resParser);
          
           return $this->arrOutput;
   }
   function tagOpen($parser, $name, $attrs) {
       $tag=array("name"=>$name,"attrs"=>$attrs);
       array_push($this->arrOutput,$tag);
   }
  
   function tagData($parser, $tagData) {
       if(trim($tagData)) {
           if(isset($this->arrOutput[count($this->arrOutput)-1]['tagData'])) {
               $this->arrOutput[count($this->arrOutput)-1]['tagData'] .= $tagData;
           }
           else {
               $this->arrOutput[count($this->arrOutput)-1]['tagData'] = $tagData;
           }
       }
   }
  
   function tagClosed($parser, $name) {
       $this->arrOutput[count($this->arrOutput)-2]['children'][] = $this->arrOutput[count($this->arrOutput)-1];
       array_pop($this->arrOutput);
   }
}


function getLayoutsArray($arr){
	// hard-coded to xml syntax
	$layouts=array();
	foreach ($arr AS $key=>$rest){
		if($rest['name']=='LAYOUTLIST'){
			return extractCleanLayouts($rest['children']);
		}
	}
	
	return $layouts;
}

function extractCleanLayouts($arr){
	$retArr=array();
	foreach ($arr AS $id=>$item){
		if(isset($item['children'][0]['children'][0]['tagData']) && isset($item['children'][0]['children'][1]['tagData'])){
			$key=$item['children'][0]['children'][0]['tagData'];
			$value=$item['children'][0]['children'][1]['tagData'];
		
			$retArr[$key]['description']=$value;
			$retArr[$key]['variants']=getVariants(@$item['children'][1]['children'][0]);
		}
		
	}
	return $retArr;
}

function getVariants($arr){

	$retArr=array();
	if(count($arr)==0){
		return $retArr;
	}
	foreach ($arr AS $id=>$item){
		if(isset($item[0]) && isset($item[0]) && isset($item[0]['children']) && isset($item[0]['children'][0]) && isset($item[0]['children'][0]['tagData']) && isset($item[0]['children'][1]['tagData'])){
			$key=$item[0]['children'][0]['tagData'];
			$value=$item[0]['children'][1]['tagData'];

			$retArr[$key]=$value;
		}
		
	}	
	return $retArr;
}

function keyboardLayoutPulldown($keyboardLayoutsArray,$selected,$name){
	$out='<select name="'.$name.'">';
	foreach ($keyboardLayoutsArray AS $key=>$valueArr){
		$out.='<option value="'.$key.'" '.(($key==$selected || $key==str_replace(' basic','',$selected))?'selected':'').'>'.$valueArr['description'].'</option>';
		if(count($valueArr['variants'])!=0){
			$out.='
			<optgroup label="'.$valueArr['description'].' variants">';
			foreach ($valueArr['variants'] AS $variantCode=>$variantName){
				if($variantCode!='' && $variantCode!='basic')
					$out.='<option value="'.$key.' '.$variantCode.'" '.(($key.' '.$variantCode==$selected)?'selected':'').'>'.$variantName.'</option>';
			}
			$out.='
			</optgroup>';
		}
	}
	$out.='</select>';
	
	return $out;
}
?>
