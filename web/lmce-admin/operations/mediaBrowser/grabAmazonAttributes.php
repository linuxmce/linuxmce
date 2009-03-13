<?
function grabAmazonAttributes($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	$url=$_REQUEST['amazonURL'];
	$fileID=(int)@$_REQUEST['fileID'];
	
	if(isset($_REQUEST['grab'])){
		if(isset($_POST['grabBtn'])){
			$out=processGrabAttributes($mediadbADO);
		}else{
			$cmd='wget \''.$url.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
			$html=exec_batch_command($cmd,1);	
			
			$out=grabAttributesForm($html,$mediadbADO);
		}
	
		$output->setBody($out);
		$output->output();	
	}else{
		$cmd='wget \''.$url.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
		$html=exec_batch_command($cmd,1);	
		
		
		$linksBar='<div align="right"><input type="button" name="back" value="Back" onClick="history.back();"> <input type="button" name="back" value="This is my file" onClick="self.location=\'index.php?section=grabAmazonAttributes&fileID='.$fileID.'&amazonURL='.$url.'&grab=1\'"></div><br>';
		$output->setReloadLeftFrame(false);
		die($linksBar.$html);
	}
}

// return false if cover art is not found or the html path if found
function getCoverArt($out){
	$coverUrl=preg_match_all('/return amz_js_PopWin\(\'(.*?)\',\'AmazonHelp\'/s',$out,$matches);
	if(count($coverUrl)==0){
		return false;
	}
	foreach ($matches[1] as $url){
		if(ereg('gp/product/image',$url)){
			$cmd='wget --header=\'Accept-encoding: gzip\' --header=\'User-Agent: Lynx/2.8.5rel.1 libwww-FM/2.14 SSL-MM/1.4.1 GNUTLS/1.0.16\' -O - \''.$url.'\' | gunzip -c';
			$coverArtPage=exec_batch_command($cmd,1);

			$isImg=preg_match('/<img src="http:\/\/(.*?)" id="prodImage" \/>/',$coverArtPage,$cmatches);
			if(count($cmatches)>0){
				// picture found
				return $cmatches[1];
			}
		}
	}
	return false;
}

function grabAttributes($string){
	$attributes=array();

	// extract page title as attribute
	$hasTitle=preg_match('/<b class=\"sans\">(.*?)<\/b>/s',$string,$tmatches);
	if($hasTitle!=0 && @count($tmatches)>0){
		$attributes['Title']=$tmatches[1];
	}
	
	
	$atribs=preg_match('/<b class=h1>Product Details<\/b>(.*?)<\/ul>/s',$string,$matches);

	if($atribs==0){
		$natribs=preg_match('/<b class="h1">Product Details<\/b>(.*?)<\/ul>/s',$string,$matches);
		if($natribs==0){
			return false;
		}
	}
	$toRemove=array();
	$toRemove[]='<b class=h1>Product Details</b><br>
</a>';
	$toRemove[]='<b class="h1">Product Details</b><br \/>';
	
	$toRemove[]="\n";
	
	// remove header and carriage returns
	$dataChunk=str_replace($toRemove,'',$matches[0]);
	
	$dataParts=explode("<li",$dataChunk);

	for($i=0;$i<count($dataParts);$i++){
		$isAttribute=preg_match('/<b\>(.*?)<\/b\>/s',$dataParts[$i],$amatches);
		if($isAttribute>0){
			$attributeType=str_replace(':','',$amatches[1]);
			$allowed=null;
			// hardcoded for Average Customer Review
			if($attributeType=='Average Customer Review'){
				$allowed='<img>';
			}			
			$attributeValue=strip_tags($dataParts[$i],$allowed);
			$attributeValue=str_replace(array(' class="small">','>'),'',$attributeValue);
			$attributeValue=str_replace($attributeType.':','',$attributeValue);
			$attributeValue=str_replace($attributeType,'',$attributeValue);
			
			// if the attribute is 'Actors' or 'Starring' I'll break it after comas, and remove 'See more'
			if($attributeType=='Actors' || $attributeType=='Starring'){
				$actors=explode(',',strip_tags($attributeValue));
				$cleanActors=array();
				foreach ($actors AS $actor){
					if($actor!='See more'){
						$cleanActors[]=trim($actor);
					}
				}
				$attributes[$attributeType]=$cleanActors;
			}else{
				$attributes[$attributeType]=trim($attributeValue);
			}
		}
	}

	return $attributes;
}

function grabTracks($string){
	$hasTracks=preg_match('/<b class="h1">Track Listings<\/b>(.*?)<hr noshade size=1>/s',$string,$matches);

	if($hasTracks==0){
		return false;
	}

	$toRemove=array();
	$toRemove[]='<b class="h1">Track Listings</b><br>
</a>';
	//$toRemove[]="\n";
	$toRemove[]='<hr noshade size=1>';
	$toRemove[]='<td align=center bgcolor="#dddddd">
<span class=tiny>
&nbsp;
</span></td><td align=center bgcolor="#dddddd">
<span class=tiny>
&nbsp;
</span></td>';
	$toRemove[]='<td align=center bgcolor="#ffffff">
<span class=tiny>
&nbsp;
</span></td><td align=center bgcolor="#ffffff">
<span class=tiny>
&nbsp;
</span></td>';
	
	// remove header and carriage returns
	$dataChunk=str_replace($toRemove,'',$matches[0]);
	$grab=preg_match_all('/<span class=tiny>(.*?)<\/span>/s',$dataChunk,$tmatches);
	
	$tracks=array();
	$pos=0;
	foreach ($tmatches[0] AS $track){
		$pos++;
		$tracks[]=str_replace($pos.'. ','',trim(strip_tags($track)));
	}
	
	return $tracks;
}

function grabAttributesForm($html,$mediadbADO){
	$fileID=(int)@$_REQUEST['fileID'];
	
	$fileData=getFieldsAsArray('File','Path,Filename',$mediadbADO,'WHERE PK_File='.$fileID);
	$out='
		<form action="index.php" method="POST" name="grabAmazonAttributes">
		<input type="hidden" name="section" value="grabAmazonAttributes">
		<input type="hidden" name="fileID" value="'.$fileID.'">	
		<input type="hidden" name="amazonURL" value="'.$_REQUEST['amazonURL'].'">	
		<input type="hidden" name="grab" value="1">	
		
		<table>
			<tr>
				<td class="tablehead" colspan="2">File #'.$fileID.'</td>
			</tr>		
			<tr>
				<td><B>Filename</B></td>
				<td>'.$fileData['Filename'][0].'</td>
			</tr>
			<tr>
				<td><B>Path</B></td>
				<td>'.$fileData['Path'][0].'</td>
			</tr>
			<tr>
				<td valign="top"><B>Attributes</B></td>
				<td>	<table width="100%">';
			$queryAttributes='
				SELECT Attribute.*, AttributeType.Description 
				FROM Attribute
				INNER JOIN File_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_File=?
				ORDER BY Description ASC';
			$resAttributes=$mediadbADO->Execute($queryAttributes,$fileID);
			$pos=0;
			while($rowAttributes=$resAttributes->FetchRow()){
				$pos++;
				$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td><b>'.$rowAttributes['Description'].'</b></td>
							<td>'.$rowAttributes['Name'].'</td>
						</tr>';
			}
			$out.='
				</table>
			</td>
			</tr>
	
			<tr>
				<td colspan="2">&nbsp;</td>
			</tr>	
		';
	$coverArtUrl=getCoverArt($html);
	if($coverArtUrl!==false){
		$out.='
				<tr>
					<td class="tablehead" colspan="2">Cover Art</td>
				</tr>		
				<tr>
					<td colspan="2"><input type="checkbox" name="import_cover_art" value="http://'.$coverArtUrl.'" checked> <a href="http://'.$coverArtUrl.'" target="_blank">[ View cover art ]</a></td>
				</tr>
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>		';
	}
		
	$attributes=grabAttributes($html);
	if($attributes!==false){
		$import=1;
		$out.='
				<input type="hidden" name="attributes_count" value="'.count($attributes).'">
				<tr>
					<td class="tablehead" colspan="2">Attributes</td>
				</tr>';
		$pos=0;
		foreach ($attributes AS $type=>$value){
			$out.='<input type="hidden" name="attribute_value_'.$pos.'" value="'.urlencode(serialize($value)).'">';
			if(!is_array($value)){
				$out.='
				<tr>
					<td><input type="checkbox" name="attribute_type_'.$pos.'" value="'.$type.'" checked> <B>'.$type.'</B></td>
					<td>'.$value.'</td>
				</tr>';		
			}else{
				$subpos=0;
				$out.='<input type="hidden" name="attribute_type_'.$pos.'_array" value="'.$type.'">';
				foreach ($value AS $actor){
					$out.='
				<tr>
					<td><input type="checkbox" name="attribute_type_'.$pos.'_'.$subpos.'" value="'.$type.'" checked> <B>'.$type.'</B></td>
					<td>'.$actor.'</td>
				</tr>';		
					$subpos++;
				}
			}
			$pos++;
		}
	}
		
	$tracks=grabTracks($html);
	if($tracks!==false){
		$import=1;
		$out.='
				<input type="hidden" name="tracks_count" value="'.count($tracks).'">
				<tr>
					<td class="tablehead" colspan="2">Tracks</td>
				</tr>';
		$pos=0;
		foreach ($tracks AS $name=>$value){
		$out.='
				<input type="hidden" name="track_value_'.$pos.'" value="'.$value.'">
				<tr>
					<td><input type="checkbox" name="track_name_'.$pos.'" value="'.$name.'" checked> <B>'.$name.'</B></td>
					<td>'.$value.'</td>
				</tr>';		
			$pos++;
		}
	}
	if(@$import==1){
		$out.='
			<tr>
				<td class="alternate_back" colspan="2" align="center"><input type="submit" class="button" name="grabBtn" value="Grab attributes/tracks"> <input type="button" class="button" value="Cancel" onClick="parent.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'"></td>
			</tr>';
	}
		
	$out.='</table>
	</form>';	
	
	return $out;
}

function processGrabAttributes($mediadbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');

	$fileID=(int)$_POST['fileID'];
	$import_cover_art=@$_POST['import_cover_art'];
	$extension=strtolower(str_replace('.','',strrchr($import_cover_art,".")));
	
	if($import_cover_art!=''){
		$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array($extension,$import_cover_art));
		$picID=$mediadbADO->Insert_ID();
		$mediadbADO->Execute('INSERT INTO Picture_File (FK_Picture,FK_File) VALUES (?,?)',array($picID,$fileID));

		// create the file and the thumbnail
		$extension=($extension=='jpeg')?'jpg':$extension;
		$newPicName=$picID.'.'.$extension;
			
			$error='';
			if($extension!='jpg'){
				$error=$TEXT_ERROR_NOT_JPEG_CONST;
			}
			else{
				// create thumbnail
				savePic($import_cover_art,$GLOBALS['mediaPicsPath'].$newPicName);
				if(file_exists($GLOBALS['mediaPicsPath'].$newPicName)){
					// try to delete exisitng thumbnail if user messed the files and database
					if(file_exists($GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension)){
						$cmd='sudo -u root rm -f "'.$GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension.'"';
						exec_batch_command($cmd);
					}
					
					$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$picID.'_tn.'.$extension, 256, 256);
					if(!$resizeFlag){
						$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
					}
				}
			}
		
	}
	
	$existingAttributeTypes=getAssocArray('AttributeType','Description','PK_AttributeType',$mediadbADO);
	$attributes_count=(int)$_POST['attributes_count'];
	for($i=0;$i<$attributes_count;$i++){
		if(isset($_POST['attribute_type_'.$i])){
			$atype=$_POST['attribute_type_'.$i];
			$avalue=unserialize(urldecode($_POST['attribute_value_'.$i]));

			echo $atype.': '.$avalue.'<br>';
			if(in_array($atype,array_keys($existingAttributeTypes))){
				$attributeType=$existingAttributeTypes[$atype];
			}else{
				$mediadbADO->Execute('INSERT INTO AttributeType (Description) VALUES (?)',array($atype));
				$attributeType=$mediadbADO->Insert_ID();
			}
			
		
			$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($attributeType,$avalue));
			$aID=$mediadbADO->Insert_ID();
				
			$mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($aID,$fileID));
		
		}else{
			if(@isset($_POST['attribute_type_'.$i.'_array'])){
				// array
				$avalue=unserialize(urldecode($_POST['attribute_value_'.$i]));
				$atype=$_POST['attribute_type_'.$i.'_array'];

				if(in_array($atype,array_keys($existingAttributeTypes))){
					$attributeType=$existingAttributeTypes[$atype];
				}else{
					$mediadbADO->Execute('INSERT INTO AttributeType (Description) VALUES (?)',array($atype));
					$attributeType=$mediadbADO->Insert_ID();
				}				
				
				for($x=0;$x<count($avalue);$x++){
					if(isset($_POST['attribute_type_'.$i.'_'.$x])){
						
						echo $atype.': '.$avalue[$x].'<br>';
					
						$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($attributeType,$avalue[$x]));
						$aID=$mediadbADO->Insert_ID();
				
						$mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($aID,$fileID));
					
					}
				}
			}
		}
	}
	
	$tracks_count=$_POST['tracks_count'];
	for($i=0;$i<$tracks_count;$i++){
		if(isset($_POST['track_name_'.$i])){
			$tname=$_POST['track_name_'.$i];
			$tvalue=urldecode($_POST['track_value_'.$i]);
		
			$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array(5,$tvalue));
			$aID=$mediadbADO->Insert_ID();
				
			$mediadbADO->Execute('INSERT INTO File_Attribute (FK_Attribute,FK_File,Track) VALUES (?,?,?)',array($aID,$fileID,$i));
		}
	}
	
	
	$fileData=getFieldsAsArray('File','Path,Filename',$mediadbADO,'WHERE PK_File='.$fileID);
	exec_batch_command('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$fileData['Path'][0].'"');
	
	die('<script>parent.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&msg=Attributes extracted from Amazon&err='.@$error.'\';</script>');
}

?>