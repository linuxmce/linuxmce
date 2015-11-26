<?php
function getAttributesByName($mediadbADO, $name) {
	 $attributes = array();
	$query='SELECT DISTINCT PK_Attribute, Name
		FROM File_Attribute INNER JOIN File ON File.PK_File=File_Attribute.FK_Attribute 
		INNER JOIN Attribute ON File_Attribute.FK_Attribute=Attribute.PK_Attribute
		WHERE EK_MediaType = ? AND FK_AttributeType = ? AND Name LIKE ? ORDER BY Name';
	$res=$mediadbADO->Execute($query,array(7, 29, "%".$name."%"));
	$first = true;
	while($row=$res->FetchRow()){
		$attr['PK_Attribute'] = $row['PK_Attribute'];
		$attr['Name'] = $row['Name'];
		$attributes[] = $attr;
	}
	return $attributes;
}

function getScreensaverFiles($path,$attribute,$attribute2,$mediadbADO,$page,$records_per_page){
	$query='
		SELECT PK_File,Filename,Path,fa1.FK_Attribute,fa2.FK_Attribute AS AttributeDisabled
		FROM File 
		LEFT JOIN File_Attribute fa1 ON fa1.FK_File=PK_File AND fa1.FK_Attribute=? 
		LEFT JOIN File_Attribute fa2 ON fa2.FK_File=PK_File AND fa2.FK_Attribute=? 
		WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0';
	$parameters[0] = $attribute;
	$parameters[1] = $attribute2;
	if (!empty($path)) {
	    $query.=' AND Path=?';
	    $parameters[2] = $path;
	}
	$query.=' ORDER BY Filename';
	return getFiles($mediadbADO, $query, $parameters, $page, $records_per_page, false, false);
}
function getPhotoFiles($criteria,$mediadbADO,$page,$records_per_page, $returnFileList){
	$i = 0;
	$query='
		SELECT PK_File,Filename,Path,fa1.FK_Attribute,fa2.FK_Attribute AS AttributeDisabled
		FROM File ';
	$dateCriteria = "";
	for ($j = 0; $j < sizeof($criteria); $j++) {
	    if ($criteria[$j]['type'] == "attr") {
	       $query.='JOIN File_Attribute fac'.$i.' ON fac'.$i.'.FK_File=PK_File AND fac'.$i.'.FK_Attribute=? ';
	       $parameters[$i] = $criteria[$j]['value'];
	       $i++;
	    } else if ($criteria[$j]['type'] == "date") {
	       if ($dateCriteria == "") {
	       	  // Add join for date attribute type
		  $dateCriteria = 'faca'.$i;
		  $query.='JOIN File_Attribute fac'.$i.' ON fac'.$i.'.FK_File=PK_File ';
	       	  $query.=' JOIN Attribute faca'.$i.' ON faca'.$i.'.PK_Attribute=fac'.$i.'.FK_Attribute';
		  $query.=' AND faca'.$i.'.FK_AttributeType=? ';
	      	  $parameters[$i++] = 48; // Year Attribute Type
	       }
	    }
	       		       
	}
	// Always add screen save attributes
	$query.='LEFT JOIN File_Attribute fa1 ON fa1.FK_File=PK_File AND fa1.FK_Attribute=? 
		LEFT JOIN File_Attribute fa2 ON fa2.FK_File=PK_File AND fa2.FK_Attribute=? 
		WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0';
	$parameters[$i++] = getScreenSaverAttribute($mediadbADO);
	$parameters[$i++] = getScreenSaverAttribute($mediadbADO,46);
	// Add date criteria
	for ($j = 0; $j < sizeof($criteria); $j++) {
	    if ($criteria[$j]['type'] == "date") {
	       $query.=' AND STR_TO_DATE('.$dateCriteria.'.Name, \'%Y:%m:%d\') ';
	       $value = $criteria[$j]['value'];
	       if (strpos($value, "=") !== false) {
	           $query.=' = ? ';
		   $value = substr($value, strpos($value, "=")+1);
	       } else if (strpos($value, "<") !== false) {
	           $query.=' < ? ';
		   $value = substr($value, strpos($value, "<")+1);
	       } else if (strpos($value, ">") !== false) {
	       	   $query.=' > ? ';
		   $value = substr($value, strpos($value, ">")+1);
	       } else {
	       	   $query.=' > ? ';
	       }
	       $parameters[$i] = $value;
	       $i++;
	    }
	}
	return getFiles($mediadbADO, $query, $parameters, $page, $records_per_page, true, $returnFileList);
}

function getFiles($mediadbADO,$query, $parameters, $page,$records_per_page,$isSearching, $returnFileList){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	$res=$mediadbADO->Execute($query, $parameters);
	$records=$res->RecordCount();
	if ($returnFileList) {
	   	$files = "";
		while($row=$res->FetchRow()){
			$files.='"'.$row['Path'].'/'.$row['Filename'].'" ';
		}
	   	return $files;
	}
	$noPages=ceil($records/$records_per_page);
	$start=($page-1)*$records_per_page;
	$end=$page*$records_per_page-1;
	$end=($end>$records)?$records:$end;	

	if ($isSearching) {
	   $out.='Number of hits: '.$records;
	}	
	$pageLinks='Pages: ';
	for($i=1;$i<=$noPages;$i++){
		if ($isSearching) {
		    // We need to POST values to bring all with us to next page
		    $pageLinks.=($i==$page)?' '.$i:' <a href="javascript:" onclick="$(\'page\').value=\''.$i.'\';document.getElementById(\'photoBrowserForm\').submit();">'.$i.'</a>';
		} else {
		    $pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
		}
	}	
	$res=$mediadbADO->Execute($query.' LIMIT '.$start.','.$records_per_page, $parameters);
	if($res->RecordCount()==0){
		return $TEXT_NO_IMAGES_IN_DIRECTORY_CONST;
	}
	$out.='
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
		$thumbPath = $row['Path'].'/'.$row['Filename'].'.tnj';
		$exif = exif_read_data($row['Path'].'/'.$row['Filename'], "IFD0", 0);
		$orientClass = "";
		$height = 0;
		if ($exif['Orientation'] == 6 || $exif['Orientation'] == 8) {
		    $orientClass = "orient_".$exif['Orientation'];
		    if (file_exists($thumbPath)) {
		        $size = getimagesize($thumbPath);
			$height = $size[0]; // rotated 90 degs, width becomes height
		    }
		}
		$dateTime = $exif['DateTime'];
		$picUrl='include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename']));
		$filePic=(!file_exists($thumbPath))?'&nbsp;':'<a href="'.$picUrl.'" target="_blank" tabindex="0"><img src="include/image.php?imagepath='.htmlentities(urlencode($row['Path'].'/'.$row['Filename'])).'.tnj" border="0" class="'.$orientClass.'" id="img_'.$row['PK_File'].'" /></a>';
		$out.='
			<tr class="'.$class.'" id="row_'.$row['PK_File'].'">
				<td id="imgcell_'.$row['PK_File'].'" '.($height > 0 ? 'height="'.$height.'px"' : '').'>'.$filePic.'</td>
				<td class="imageData">
				  <table><tr><td>File: <a href="'.$picUrl.'" target="_blank" tabindex="0">'.$row['Filename'].'</a> Date:'.$dateTime.'</td></tr>
				    <tr><td>Orientation: <input type="text" name="orientation_'.$row['PK_File'].'" value="'.$exif['Orientation'].'" size="1" class="orientation" tabindex="1" /></td></tr>
				    <tr><td>Screensaver: <input type="checkbox" name="file_'.$row['PK_File'].'" value="1" '.((is_null($row['FK_Attribute']) || (!is_null($row['FK_Attribute']) && !is_null($row['AttributeDisabled'])))?'':'checked').' tabindex="1"></td></tr>
				    <tr><td>Keywords:</td></tr>
				    <tr><td id="keywords_'.$row['PK_File'].'" class="keywords"></td></tr>
				    <tr><td>Add keyword: <input class="newkw" name="newkw_'.$row['PK_File'].'" id="kw_'.$row['PK_File'].'" type="text" size="30" value="" title="Enter to add single existing attribute, Shift+Enter to add new attribute" onfocus="scrollToImage(this); loadLookup(this);" onblur="clearLookup(this);" tabindex="1"/></td></tr>
				    </table>
				</td>
			</tr>		
		';
	}
	if (!$isSearching) {
		$out.='
			<tr>
				<td colspan="2"><input type="checkbox" name="sel_all" value="1" onClick="set_checkboxes();"> Select/unselect all<br>
				<input type="submit" class="button" name="save" value="Save">	
			</td>
			</tr>';
	}
	$out.='
			<tr>
				<td colspan="2" align="right">'.$pageLinks.'</td>
			</tr>		
	
	</table>
	<input type="hidden" name="displayedFiles" value="'.join(',',$displayedFiles).'">';
	return $out;
}

function getCSS() {
	 return '<style type="text/css">
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
  z-index:100;
}
.orient_6 {
  transform:rotate(90deg);
  -ms-transform:rotate(90deg);
  -webkit-transform:rotate(90deg);
}
.orient_3 {
  transform:rotate(180deg);
  -ms-transform:rotate(180deg);
  -webkit-transform:rotate(180deg);
}
.orient_8 {
  transform:rotate(-90deg);
  -ms-transform:rotate(-90deg);
  -webkit-transform:rotate(-90deg);
}
</style>			
';
}

function getJS() {
	 return '
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
		function scrollToImage(element) {
		   	 var fileID = element.name.substring(element.name.indexOf("_")+1);
//			 alert($("row_"+fileID).cumulativeOffset().top + " - " + document.viewport.getScrollOffsets().top);
//			 alert(($("row_"+fileID).cumulativeOffset().top+$("row_"+fileID).getHeight()) + " " + (document.viewport.getScrollOffsets().top+window.parent.document.viewport.getHeight()));
			 if ($("row_"+fileID).cumulativeOffset().top+$("row_"+fileID).getHeight() > document.viewport.getScrollOffsets().top+window.parent.document.viewport.getHeight()) {
			     $("row_"+fileID).scrollIntoView(false);
			 } else if ($("row_"+fileID).cumulativeOffset().top < document.viewport.getScrollOffsets().top) {
			     $("row_"+fileID).scrollIntoView();
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
		       	      if (e.shiftKey) {
			      	 	var prevRow = $("row_"+fileID).previous();
					var fromFile = prevRow.id.substring(4);
			      	        new Ajax.Request("index.php?section=mainScreenSaver", {
					method:"post",
				  	parameters:{section: "mainScreenSaver", action:"ajax", copyAttribute:1, pkFile: fileID, fromPkFile: fromFile },
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
			      } else {
			      	     if(lookupEl.selectedIndex > 0) {
		              	     	      lookupEl.selectedIndex--;
			      	     }
			      }
		       } else {
			       loadLookup(el);
		       }
		}
		function loadLookup(el) {
		   	var lookupEl = $("newkw_lookup");
		        lookupEl.show();
			var fileID = el.name.substr(6);
			var elPos = el.cumulativeOffset();
			lookupEl.setStyle({left: elPos[0]+"px", top: (elPos[1]+20)+"px"});
			loadAttributesToSelect("newkw_lookup", el.value);
		}
		function loadAttributesToSelect(elName, value) {
			 var lookupEl = $(elName);
		        new Ajax.Request("index.php?section=mainScreenSaver", {
			  method:"get",
			  parameters:{section: "mainScreenSaver", action:"ajax", loadAttributes: 1, Name:value },
			  onSuccess: function(transport) {
			  	var response = transport.responseText || "";
				var o = eval("("+response+")");
				if (o.result) {
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
				 $("kw_"+fileID).focus();
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
		function changeOrientation(event) {
		         var el = this;
			 var fileID = el.name.substring(12);
			 var imgEl = $("img_"+fileID);
			 var orient = "1";
			 var height = imgEl.height;
			 var width = imgEl.width;
			 if (imgEl.className != "" && imgEl.className.substring(0,7) == "orient_") {
			      orient = imgEl.className.substring(7);
			 }
			 if (event.keyCode == Event.KEY_RIGHT || event.keyCode == Event.KEY_LEFT) {
			     if (event.keyCode == Event.KEY_RIGHT) {
			         if (orient == "1") {
			             orient = "6";
				     height = width;
		 	     	 } else if (orient == "6") {
			             orient = 3;
		 	         } else if (orient == "3") {
			             orient = 8;
				     height = width;
		 	         } else if (orient == "8") {
			             orient = 1;
			         }
			     } else if (event.keyCode == Event.KEY_LEFT) {
			         if (orient == "1") {
			             orient = "8";
				     height = width;
		 	         } else if (orient == "8") {
			             orient = 3;
		 	         } else if (orient == "3") {
			             orient = 6;
				     height = width;
		 	         } else if (orient == "6") {
			             orient = 1;
			         }
		             }
		             new Ajax.Request("index.php?section=mainScreenSaver", {
			     method:"post",
			     parameters:{section: "mainScreenSaver", action:"ajax", changeOrientation: orient, pkFile: fileID },
			     onSuccess: function(transport) {
			  	  var response = transport.responseText || "";
				  var o = eval("("+response+")");
				  if (o.result) {
				  	 el.value = orient;
			 		 imgEl.className = "orient_"+orient;
			 		 $("imgcell_"+fileID).height = height+"px";
				  }
			     }
			 });
		    }
		}
		document.observe("dom:loaded", function() {
		        $$(\'input[type="checkbox"]\').invoke("observe", "click", updateScreenSaver);
		        $$(\'input[class="orientation"]\').invoke("observe", "focus", function() { scrollToImage(this); });
		        $$(\'input[class="newkw"]\').invoke("observe", "keyup", addKeywordEvent);
		        $$(\'input[class="newkw"]\').invoke("observe", "keydown", function(event) { if (event.keyCode == Event.KEY_RETURN) event.preventDefault();});
			$$(\'td[class="keywords"]\').each(function(e) { loadKeywords(e.id.substring(9)); });
		        $$(\'input[class="orientation"]\').invoke("observe", "keydown", changeOrientation);
		});
			</script>';

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


?>