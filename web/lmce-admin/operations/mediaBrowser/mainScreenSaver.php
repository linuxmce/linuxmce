<?
function mainScreenSaver($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$records_per_page=50;
	$screenSaverAttribute=getScreenSaverAttribute($mediadbADO);
	$screenSaverDisabledAttribute=getScreenSaverAttribute($mediadbADO,46);
	
	if($action=='form'){
		$flickerEnabled=flickrStatus();
		$flicker_enable_disable_text=($flickerEnabled==1)?$TEXT_DISABLE_FLICKR_CONST:$TEXT_ENABLE_FLICKR_CONST;
		$flicker_status=($flickerEnabled==1)?$TEXT_ENABLED_CONST:$TEXT_DISABLED_CONST;
		
		if($path!=''){
			$physicalFiles=grabFiles($path,'');
			
			$out.='
			<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
			<script>
			function windowOpen(locationA,attributes) 
			{
				window.open(locationA,\'\',attributes);
			}
			

			function syncPath(path)
			{
				top.treeframe.location=\'index.php?section=leftScreenSaver&startPath=\'+escape(path);
				self.location=\'index.php?section=mainScreenSaver&path=\'+escape(path);
			}
		function set_checkboxes()
		{
		   val=document.mainScreenSaver.sel_all.checked;
		   for (i = 0; i < mainScreenSaver.elements.length; i++)
		   {
			tmpName=mainScreenSaver.elements[i].name;
		     if (mainScreenSaver.elements[i].type == "checkbox")
		     {
		         mainScreenSaver.elements[i].checked = val;
		     }
		   }
		}			

		function updateScreenSaver(event) {
		         var element = this;
			 element.disabled = true;
			        new Ajax.Request("index.php?section=mainScreenSaver", {
				  method:"post",
				  parameters:{section: "mainScreenSaver", action:"ajax", setScreenSaver: (this.checked?1:0), pkFile: this.name.substr(5) },
				  onSuccess: function(transport) {
				        element.disabled = false;  
				  	var response = transport.responseText || "";
					var o = eval("("+response+")");
				  }
				});
		}
		function addKeywordEvent(e) {
		       var el = Event.element(e);
		       var fileID = el.name.substr(6);
		       var lookupEl = $("newkw_lookup");
		       if (e.keyCode == Event.KEY_RETURN) {
		           var pkAttr = 0;
		           if (!e.shiftKey && lookupEl.selectedIndex >= 0) {
			      pkAttr = lookupEl.options[lookupEl.selectedIndex].value;
			   }
		           if (pkAttr > 0 || el.getValue() != "") {
			        new Ajax.Request("index.php?section=mainScreenSaver", {
				  method:"post",
				  parameters:{section: "mainScreenSaver", action:"ajax", addAttribute:1, pkFile: fileID, pkAttribute: pkAttr, Name:el.getValue() },
				  onSuccess: function(transport) {
				        el.value = ""; 
				  	var response = transport.responseText || "";
					var o = eval("("+response+")");
					if (o.result) {
					    clearLookup();
					    loadKeywords(fileID);
					}
				  }
				});
			   }
			   e.preventDefault();
		       } else if (e.keyCode == Event.KEY_DOWN) {
		       	      if (lookupEl.selectedIndex < lookupEl.options.length-1) {
		               	      lookupEl.selectedIndex++;
			      }
		       } else if (e.keyCode == Event.KEY_UP) {
		              if(lookupEl.selectedIndex > 0) {
		              	      lookupEl.selectedIndex--;
			      }
		       } else {
			       loadLookup(el);
		       }
		}
		function loadLookup(el) {
		   	var lookupEl = $("newkw_lookup");
		        lookupEl.show();
			var fileID = el.name.substr(6);
		        new Ajax.Request("index.php?section=mainScreenSaver", {
			  method:"get",
			  parameters:{section: "mainScreenSaver", action:"ajax", loadAttributes: 1, Name:el.value },
			  onSuccess: function(transport) {
			  	var response = transport.responseText || "";
				var o = eval("("+response+")");
				if (o.result) {
				   var elPos = el.cumulativeOffset();
				   lookupEl.setStyle({left: elPos[0]+"px", top: (elPos[1]+20)+"px"});
				   lookupEl.options.length = 0;
				   for (var i = 0; i < o.data.length; i++) {
				       for (var key in o.data[i]) {
				              var optEl = new Element("option", {value:key});
					      optEl.text = o.data[i][key];
					      lookupEl.options.add(optEl);
				       }
				   }
				   if (lookupEl.options.length > 0) {
				            lookupEl.options[0].selected = true;
				   }
				}
			  }
			});		
		}
		function clearLookup(el) {
			 $("newkw_lookup").hide();
			 $("newkw_lookup").blur();
		}
		function loadKeywords(file) {
			 var fileID = file;
			 new Ajax.Request("index.php", {
				  method:"get",
				  parameters:{section: "mainScreenSaver", action:"ajax", loadKeywords: fileID },
				  onSuccess: function(transport) {
				  	var response = transport.responseText || "";
				  	$(\'keywords_\'+fileID).update(response);
				  }
		  	 });
		}
		function removeAttributeSpan(e, el) {
			 if (!e) e = window.event;
			 if (e.shiftKey) {
	 			 var id = el.id.substring(8);
	 			 var fileID = id.substring(0, id.indexOf("_"));
				 var attrID = id.substring(id.indexOf("_")+1);
			         new Ajax.Request("index.php?section=mainScreenSaver", {
				     method:"post",
				      parameters:{section: "mainScreenSaver", action:"ajax", removeAttribute: attrID, pkFile: fileID },
				      onSuccess: function(transport) {
				  	  var response = transport.responseText || "";
					  var o = eval("("+response+")");
					  if (o.result) {
					      $("keyword_"+fileID+"_"+attrID).remove();
					  }
				  }
				});
			}
		}

		document.observe("dom:loaded", function() {
		        $$(\'input[type="checkbox"]\').invoke("observe", "click", updateScreenSaver);
		        $$(\'input[class="newkw"]\').invoke("observe", "keyup", addKeywordEvent);
		        $$(\'input[class="newkw"]\').invoke("observe", "keydown", function(event) { if (event.keyCode == Event.KEY_RETURN) event.preventDefault();});
			$$(\'td[class="keywords"]\').each(function(e) { loadKeywords(e.id.substring(9)); });
		});
			</script>
			<style type="text/css">
td.imageData {
vertical-align: top;
}
table select {
width: 260px;
}
td.imageData table td {
padding: 0px;
}
.kwSpan {
   background-color: gray;
   padding:1px;
   margin-right: 4px;
}<
.kwHover {
   background-color: light-red;
   padding:2px;
   margin-right: 4px;
}
#newkw_lookup {
  position: absolute;
  width: 240px;
}

</style>			
			<a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a>
			
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			<form action="index.php" method="POST" name="mainScreenSaver">
			<input type="hidden" name="section" value="mainScreenSaver">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			<input type="hidden" name="page" value="'.$page.'">
			<select id="newkw_lookup" size="8" style="display: none;"><option value="0"></option></select>
';
			
			$out.='
				<input type="hidden" name="flikr" value="'.(($flickerEnabled==0)?1:0).'">
				<fieldset style="width:400px;">
				<legend>Flikr script ('.$flicker_status.')</legend>
				Enable/disable the script who retrieve images from flikr.com<br>
				<input type="submit" class="button" name="flikrBtn" value="'.$flicker_enable_disable_text.'"><hr>
				
				Remove flikr pictures from screensaver and disable the script<br>
				<b>Flikr pictures path:</b> <input type="text" name="flikr_path" value="/home/public/data/pictures/flickr/" style="width:200px;"> <input type="submit" class="button" name="remove_flikr_pics" value="Remove">			
				<hr>
				<input type="submit" name="reload" class="button" value="'.$TEXT_RELOAD_SCREENSAVER_CONST.'">
				</fieldset>
				
				
				<table cellpading="0" cellspacing="0">
					<tr>
						<td>'.getScreensaverFiles($path,$screenSaverAttribute,$screenSaverDisabledAttribute,$mediadbADO,$page,$records_per_page).'</td>
					</tr>';
				$out.='		
				</table>';

		}
	$out.='
			
		</form>
	';

	}else{

		// Ajax
		if (isset($_POST['setScreenSaver'])) {
		       $pkFile = $_POST['pkFile'];
		       $setScreenSaver = $_POST['setScreenSaver'];
		       setScreenSaverState($pkFile, $setScreenSaver == 1, $mediadbADO, $screenSaverAttribute, $screenSaverDisabledAttribute);
		       header('Content-type: application/json');
                       print("{result: 1}\n");
		       exit();
		}
		if (isset($_POST['removeAttribute'])) {
		       $pkFile = $_POST['pkFile'];
		       $pkAttr = $_POST['removeAttribute'];
		       $result = removeAttributeFromFile($pkFile, $pkAttr, $mediadbADO) ? 1 : 0;
		       header('Content-type: application/json');
                       print("{result: ".$result."}\n");
		       exit();
		}
		if (isset($_POST['addAttribute'])) {
		       $pkFile = $_POST['pkFile'];
		       $pkAttr = $_POST['pkAttribute'];
		       $Name = $_POST['Name'];
		       $result = addAttributeToFile($pkFile, $pkAttr, 29, $Name, $mediadbADO) ? 1 : 0;
		       header('Content-type: application/json');
                       print("{result: ".$result."}\n");
		       exit();
		}
		if (isset($_REQUEST['loadKeywords'])) {
		       $pkFile = $_REQUEST['loadKeywords'];
                       print(loadKeywords($pkFile, $mediadbADO));
		       exit();
		}
		if (isset($_REQUEST['loadAttributes'])) {
		       $Name = $_REQUEST['Name'];
                       print(loadAttributes($Name, $mediadbADO));
		       exit();
		}

		if(isset($_POST['reload'])){
			$cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 '.$GLOBALS['PhotoScreenSaver'].' 1 606';
			exec_batch_command($cmd);
			
			header('Location: index.php?section=mainScreenSaver&path='.urlencode($path).'&msg='.cleanString($TEXT_SCREENSAVER_RELOADED_CONST).'&page='.$page);
			exit();			
		}
				
		if(isset($_POST['flikrBtn'])){
			$flickr=(int)@$_REQUEST['flikr'];
			$parm=($flickr==1)?'-enable':'-disable';
			flickrStatus($parm);
		}

		$displayedFiles=cleanString(@$_POST['displayedFiles']);
		if($displayedFiles!=''){
			
			$displayedFilesArray=explode(',',$displayedFiles);
			foreach ($displayedFilesArray AS $fileID){
				if((int)@$_POST['file_'.$fileID]==0){
					if(file_attribute_exist($fileID,$screenSaverAttribute,$mediadbADO)){
						$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverDisabledAttribute,$fileID));
					}
				}else{
					$mediadbADO->Execute('DELETE File_Attribute.* FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=46 AND FK_File=?',array($fileID));					
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverAttribute,$fileID));
				}
			}
		}
		
		if(isset($_POST['remove_flikr_pics'])){
			flickrStatus('-disable');
			
			$flikr_path=cleanString($_POST['flikr_path']);
			$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) SELECT ?, PK_File FROM File INNER JOIN File_Attribute ON PK_File=File_Attribute.FK_File AND FK_Attribute=? WHERE Path LIKE \''.$flikr_path.'%\'',array($screenSaverDisabledAttribute,$screenSaverAttribute));
		}				
		
		$cmd='/usr/pluto/bin/MessageSend localhost -targetType template -o 0 '.$GLOBALS['PhotoScreenSaver'].' 1 606';
		exec_batch_command($cmd);
		
		header('Location: index.php?section=mainScreenSaver&path='.urlencode($path).'&msg='.cleanString($TEXT_SCREENSAVER_UPDATED_CONST).'&page='.$page);
		exit();
		
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_SCREEN_SAVER_CONST);
	$output->setReloadLeftFrame(false);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getUpperLevel($path)
{
	if($path=='/')
		return $path;
	$upLevel=substr($path,0,-1);
	return substr($upLevel,0,strrpos($upLevel,'/'));
}


function getScreensaverFiles($path,$screenSaverAttribute,$screenSaverDisabledAttribute,$mediadbADO,$page,$records_per_page){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	$query='
		SELECT PK_File,Filename,Path,fa1.FK_Attribute,fa2.FK_Attribute AS AttributeDisabled
		FROM File 
		LEFT JOIN File_Attribute fa1 ON fa1.FK_File=PK_File AND fa1.FK_Attribute=? 
		LEFT JOIN File_Attribute fa2 ON fa2.FK_File=PK_File AND fa2.FK_Attribute=? 
		WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0 AND Path=?';
	$res=$mediadbADO->Execute($query,array($screenSaverAttribute,$screenSaverDisabledAttribute,$path));
	$records=$res->RecordCount();
	
	$noPages=ceil($records/$records_per_page);
	$start=($page-1)*$records_per_page;
	$end=$page*$records_per_page-1;
	$end=($end>$records)?$records:$end;	
	
	$pageLinks='Pages: ';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
	}	
	
	$res=$mediadbADO->Execute($query.' LIMIT '.$start.','.$records_per_page,array($screenSaverAttribute,$screenSaverDisabledAttribute,$path));
	if($res->RecordCount()==0){
		return $TEXT_NO_IMAGES_IN_DIRECTORY_CONST;
	}
	$out='
		<table celspacing="0" cellpadding="3" align="center" width="650">
			<tr class="tablehead">
				<td width="20">&nbsp;</td>
				<td><B>'.$TEXT_IMAGE_CONST.'</B></td>
			</tr>';
	$pos=0;
	$displayedFiles=array();
	while($row=$res->FetchRow()){
		$displayedFiles[]=$row['PK_File'];
		$pos++;
		$class=($pos%2==0)?'alternate_back':'';
		$picUrl='include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename']));
		$filePic=(!file_exists($row['Path'].'/'.$row['Filename'].'.tnj'))?'&nbsp;':'<a href="'.$picUrl.'" target="_blank" tabindex="0"><img src="include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename'])).'.tnj" border="0"></a>';
		$out.='
			<tr class="'.$class.'">
				<td>'.$filePic.'</td>
				<td class="imageData">
				  <table><tr><td>File: <a href="'.$picUrl.'" target="_blank" tabindex="0">'.$row['Filename'].'</td></tr>
				    <tr><td>Screensaver: <input type="checkbox" name="file_'.$row['PK_File'].'" value="1" '.((is_null($row['FK_Attribute']) || (!is_null($row['FK_Attribute']) && !is_null($row['AttributeDisabled'])))?'':'checked').' tabindex="1"></td></tr>
				    <tr><td>Keywords:</td></tr>
				    <tr><td id="keywords_'.$row['PK_File'].'" class="keywords"></td></tr>
				    <tr><td>Add keyword: <input class="newkw" name="newkw_'.$row['PK_File'].'" id="kw_'.$row['PK_File'].'" type="text" size="30" value="" title="Enter to add single existing attribute, Shift+Enter to add new attribute" onfocus="loadLookup(this);" onblur="clearLookup(this);" tabindex="1"/></td></tr>
				    </table>
				</td>
			</tr>		
		';
	}
	$out.='
			<tr>
				<td colspan="2"><input type="checkbox" name="sel_all" value="1" onClick="set_checkboxes();"> Select/unselect all<br>
				<input type="submit" class="button" name="save" value="Save">	
			</td>
			</tr>
			<tr>
				<td colspan="2" align="right">'.$pageLinks.'</td>
			</tr>		
	
	</table>
	<input type="hidden" name="displayedFiles" value="'.join(',',$displayedFiles).'">';
	
	return $out;
}

/**
 * @param dbADO database connection $mediadbADO
 * @param int FK_AttributeType $type
 * @return int PK_Attribute
 */
function getScreenSaverAttribute($mediadbADO,$type=30){
	$res=$mediadbADO->Execute('SELECT PK_Attribute,Name FROM Attribute WHERE FK_AttributeType='.$type);
	if($res->RecordCount()==0){
		$label=($type==30?'*':'Screen saver disabled');
		$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) SELECT PK_AttributeType,? FROM AttributeType WHERE PK_AttributeType='.$type,array($label));
		return $mediadbADO->Insert_id();
	}
	$attribute=0;
	while($row=$res->FetchRow()){
		if($attribute==0 || $row['Name']=='*'){
			$attribute=$row['PK_Attribute'];
		}
	}
	
	return $attribute;
}

function flickrStatus($param='-status'){
	$cmd='sudo -u root /usr/pluto/bin/flickr_status.sh '.$param;
	
	return trim(exec_batch_command($cmd,1));
}

function file_attribute_exist($fileID,$attributeID,$mediadbADO){
	$res=$mediadbADO->Execute('SELECT * FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$attributeID));
	if($res->RecordCount()>0){
		return true;
	}
	
	return false;
}

function setScreenSaverState($fileID, $enable, $mediadbADO, $screenSaverAttribute, $screenSaverDisabledAttribute) {
	if(!$enable){
		if(file_attribute_exist($fileID,$screenSaverAttribute,$mediadbADO)){
			$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverDisabledAttribute,$fileID));
		}
	}else{
		$mediadbADO->Execute('DELETE File_Attribute.* FROM File_Attribute INNER JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=46 AND FK_File=?',array($fileID));		
		$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverAttribute,$fileID));
	}
}

function loadKeywords($pkFile, $mediadbADO) {
	$h = "";
	$query='SELECT PK_Attribute, Name
		FROM File_Attribute INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute 
		WHERE FK_File = ? AND FK_AttributeType = ?';
	$res=$mediadbADO->Execute($query,array($pkFile, 29));
	while($row=$res->FetchRow()){
	        $h.='<span class="kwSpan" id="keyword_'.$pkFile.'_'.$row['PK_Attribute'].'" title="Shift+Click to remove" onclick="removeAttributeSpan(event, this);">'.$row['Name'].'</span>';
	}
	return $h;
}
function loadAttributes($Name, $mediadbADO) {
	$h = "{ result: 1, data: [";
	$query='SELECT DISTINCT PK_Attribute, Name
		FROM File_Attribute INNER JOIN File ON File.PK_File=File_Attribute.FK_Attribute 
		INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute
		WHERE EK_MediaType = ? AND FK_AttributeType = ? AND Name LIKE ? ORDER BY Name';
	$res=$mediadbADO->Execute($query,array(7, 29, $Name."%"));
	$first = true;
	while($row=$res->FetchRow()){
		if (!$first)
		   $h.=",";
	        $h.="{".$row['PK_Attribute'].":'".$row['Name']."'}";
		$first = false;
	}
	$h.="]}";
	return $h;
}
function removeAttributeFromFile($pkFile, $pkAttr, $mediadbADO) {
	return $mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_Attribute=? AND FK_File=?',array($pkAttr, $pkFile));
}

function addAttributeToFile($pkFile, $pkAttr, $pkAttrType, $Name, $mediadbADO) {
	// If new attribute, add it
	$newPKAttr = $pkAttr;
	if (!$newPKAttr) {
		$result = $mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) SELECT PK_AttributeType,? FROM AttributeType WHERE PK_AttributeType=?',array($Name,$pkAttrType));
		if (!result)
		   return false;
		$newPKAttr = $mediadbADO->Insert_id();
	}
	// Connect attribute to file
	return $mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($newPKAttr,$pkFile));
}

?>