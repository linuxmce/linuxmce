<?php
function transmissionValidatelinks($transmissionADO, $VLpage, $transmission_files_per_page) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/transmissiondaemon.lang.php');
	include(APPROOT.'/operations/others/transmissionEditLinks.php');

	$noPagesArray=array(
	10=>'10',
	20=>'20',
	50=>'50',
	100=>'100',
	500=>'500',
	1000=>'1000',
	2000=>'2000',
	10000=>'10000',
	1000000=>'All *'
	);

	$operation = (isset($_REQUEST['operation']) && $_REQUEST['operation']!='')?cleanString($_REQUEST['operation']):'';
	$ID = (isset($_REQUEST['ID']) && $_REQUEST['ID']!='')?cleanString($_REQUEST['ID']):'';

	transmissionEdit($transmissionADO, $operation, $ID);

	$operation="";
	$ID="";

	$out='
		<script>
	       	function ChangeLink(ID, operation)
	       	{
			document.getElementById("operation").value=operation;
			document.getElementById("ID").value=ID;
			document.getElementById("validate_links").click();
		}
		</script>

		<input type="hidden" name="operation" id="operation" value="'.$operation.'">
		<input type="hidden" name="ID" id="ID" value="'.$ID.'">
		<table>';

	$queryLinks='SELECT * FROM Torrent_Links;';
	$Linkresults=$transmissionADO->Execute($queryLinks);

	if ($Linkresults->RecordCount()>0){
		$recordCount=$Linkresults->RecordCount();
		$fileFrom=(($VLpage-1)*$transmission_files_per_page) + 1;

		if (($VLpage*$transmission_files_per_page) > $recordCount){
			$toFile=$recordCount;
		}else{
			$toFile=$VLpage*$transmission_files_per_page;
		};

		$noPages=ceil($recordCount/$transmission_files_per_page);
		$navBarArray=array();

		for($i=1;$i<$noPages+1;$i++){
			$navBarArray[]=($i==$VLpage)?$i:'<a href="javascript:{}" onclick="document.getElementById(\'VLpage\').value='.$i.';document.transmission.submit();" class="white_link">'.$i.'</a>';
		}

		$navBar=join(' | ',$navBarArray);
		$itemCounter=$fileFrom ." to ".$toFile;

		$queryLinks='SELECT * FROM Torrent_Links LIMIT '.($fileFrom - 1).','.$transmission_files_per_page.';';
		$Linkresults=$transmissionADO->Execute($queryLinks);

		$out.='<tr><td class="tablehead">'.$TEXT_TRANSMISSION_LINK_STATUS.'</td><td class="tablehead" colspan=2>'.$TEXT_TRANSMISSION_LINK_LOCATION.'</td></tr>';
		$out.=DisplayLinks($Linkresults);
		$out.='
			<tr class="tablehead">
				<td>&nbsp;</td>
				<td><B>Files per page:</B> '.pulldownFromArray($noPagesArray,'transmission_files_per_page',$transmission_files_per_page,'onChange="document.transmission.VLpage.value=1;document.transmission.files_per_page.value=this.value;document.transmission.submit();"','key','').', Displaying '.$itemCounter.' Items</td>
				<td align="right">'.$navBar.'</td>
			</tr>';
	}else{
		$out.='<tr><td class="tablehead">'.$TEXT_TRANSMISSION_NO_LINKS.'</td></tr>';
	};

	$out.='</table>';

	return $out;
}

function DisplayLinks($Linkresults){
	$result='';

	foreach ($Linkresults as $link){
		$delete=false;

		if (file_exists($link["Torrent_Location"])){
			$result.='<tr><td><img src=include/images/db.gif align=middle border=0></td><td colspan=2>'.$link["Torrent_Location"].'</td></tr>';
		}else{
			if ( $link["Link_Status"]=="extracting"){
				$result.='<tr><td>'.$TEXT_TRANSMISSION_EXTRACTING.'<td><td colspan=2>'.$link["Torrent_Location"].'</td></tr>';
			}else{
				$delete=true;
				$result.='<tr><td><input type="button" value="Delete Link" name="Delete" onclick="ChangeLink('.$link["Link_ID"].', \'delete\')"></td><td colspan=2>'.$link["Torrent_Location"].'</td></tr>';
			};
		};

		if (file_exists($link["Link_Location"].DIRECTORY_SEPARATOR.$link["Link_Name"]) || $delete){
			$result.='<tr><td><img src=include/images/sync.gif align=middle border=0></td><td colspan=2>'.$link["Link_Location"].DIRECTORY_SEPARATOR.$link["Link_Name"].'</td></tr>';
		}else{
			$result.='<tr><td><input type="button" value="Repair Link" name="Repair" onclick="ChangeLink('.$link["Link_ID"].', \'repair\')"></td><td colspan=2>'.$link["Link_Location"].DIRECTORY_SEPARATOR.$link["Link_Name"].'</td></tr>';
		};
	}

	return $result;
}
?>
