<?

/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com
	GPLv2 Licensed
	TVDB Data Grabber
	TvDB.com data grabber for television shows from the web admin, both for single and multiple files 
	
	checktvdb.php		- 	initialization
	tvdbUtils.php 		-	common include
	tvdbseries.php		-	Episode confirmation page
	tvdbAttributes.php	- 	Displays inserted attributes and screenshot
	
	
 */


include('TMDb.php');
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
$fileID=$_GET['fileID'];
error_reporting(1);
	// tvdb api key 
	
function initialSetup($fileID,$mediadbADO,$dbADO, $output) {

searchSeries($fileID,$mediadbADO,$dbADO, $output) ;
	
}

function searchSeries($fileID,$mediadbADO,$dbADO, $output){
		include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');

 $externalAttributesBtn3='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'">';
 $scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>

 				<script>
 				function clear()
 				{this.value="";}

function box()
{document.getElementById("specials").innerHTML = "<div id=\"searchBox\" align=\"right\" stlye=\"display: none\"><input type=\"text\" id=\"seriesName\" value=\"Enter Special Name\" onclick=\"clear()\"><br><input type=\"hidden\" value=\"0\" name=\"specSeas\"><br><input type=\"submit\" onclick=\"getSpecials()\"/></div>";
 				}
 				</script>';
$out.="TMDB";
$output->setScriptInHead($scriptInHead);
$output->setReloadLeftFrame(false);
$output->setMenuTitle('Search TVDB |');
$output->setScriptInBody('bgColor="#F0F3F8" onLoad="$(\'appear_test\').appear(); return false;"');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();
}





?>