<?php
function myDevices($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/myDevices.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$installationID = (int)@$_SESSION['installationID'];

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'leftTree';

	if ($action == 'showBasicInfo') {
		$out=$TEXT_MYDEVICES_NOTE_CONST.'
	
		<br><br><a href="index.php?section=addMyDevice&parentID=0">'.$TEXT_ADD_TOP_LEVEL_DEVICE_CONST.'</a>
	';	
	} elseif ($action=='leftTree') {
		$myDevicesJsArray = '
			modelsArray = new Array();
	';
		$jsTree='';
		//$query1 = "select * from Device where FK_Device_ControlledVia IS NULL";
		$query1 = "select * from Device where FK_Device_ControlledVia IS NULL and FK_Installation = $installationID";
		$res1 = $dbADO->_Execute($query1);
		if ($res1) {
			while ($row1 = $res1->FetchRow()) {
				$jsTree.='
					auxS'.$row1['PK_Device'].' = insFld(foldersTree, gFld("'.$row1['Description'].'", "index.php?section=editDeviceParams&deviceID='.$row1['PK_Device'].'"));
					auxS'.$row1['PK_Device'].'.xID = -'.$row1['PK_Device'].';
				';
				$jsTree.=getDeviceChilds($row1['PK_Device'],$dbADO);
			}
		}
		$out='';
		$scriptInHead = "

<!-- As in a client-side built tree, all the tree infrastructure is put in place
     within the HEAD block, but the actual tree rendering is trigered within the
     BODY -->

<!-- Code for browser detection -->
<script src=\"scripts/treeview/ua.js\"></script>

<!-- Infrastructure code for the tree -->
<script src=\"scripts/treeview/ftiens4.js\"></script>

<!-- Execution of the code that actually builds the specific tree.
     The variable foldersTree creates its structure with calls to
	 gFld, insFld, and insDoc -->
<script>
USETEXTLINKS = 1
STARTALLOPEN = 1
USEFRAMES = 1
USEICONS = 1
WRAPTEXT = 1
PRESERVESTATE = 1
ICONPATH = 'scripts/treeview/' 
HIGHLIGHT = 1
GLOBALTARGET = 'R'

foldersTree = gFld('<b>$TEXT_MY_DEVICES_CONST</b>', \"index.php?section=myDevices&action=showBasicInfo\");
foldersTree.xID = 1001635872
$jsTree

					auxS = insFld(foldersTree, gFld(\"Add top level device\", \"index.php?section=addMyDevice&parentID=0\"));
					auxS.xID = 0;
					auxS.iconSrc = ICONPATH + \"diffFolder.gif\"
  					auxS.iconSrcClosed = ICONPATH + \"diffFolder.gif\" 
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
		if (getObj) {
			if (typeof obj == 'string') {
				return getObj(obj);
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
$myDevicesJsArray



</script>
";

		$out.='<span style="font-color:#FFFFFF"><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></span>
					<span>
					<script>
						initializeDocument();						
					</script>
					<noscript>
					A tree for site navigation will open here if you enable JavaScript in your browser.
					</noscript>';

		$output->setScriptInHead($scriptInHead);
	}

	//$action=='showBasicInfo'?('Add top level device'=>'index.php?section=editMasterDevice&model='.$deviceID):''

	$output->setNavigationMenu(array($TEXT_MY_DEVICES_CONST=>'index.php?section=myDevices'));
	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_DEVICES_CONST);

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MY_DEVICES_CONST);
	$output->output();
}

function getDeviceChilds($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	$jsTree='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
			$jsTree.= '
					auxS'.$row['PK_Device'].' = insFld(auxS'.$parentID.', gFld("'.$row['Description'].'", "index.php?section=editDeviceParams&deviceID='.$row['PK_Device'].'"))
					auxS'.$row['PK_Device'].'.xID = '.$row['PK_Device'].';
				';
			$jsTree.=getDeviceChilds($row['PK_Device'],$dbADO);
		}
	}
	return $jsTree;
}