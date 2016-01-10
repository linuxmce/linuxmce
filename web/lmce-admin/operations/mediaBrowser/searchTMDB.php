<?php
/*
	Copyright 2010 - Langston Ball
	golgoj4@gmail.com	
	GPLv2 Licensed
	TMDB.com search and selection.
*/
include('TMDb.php');
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');

function searchTMDB($output,$mediadbADO,$dbADO)
{
	//$externalAttributesBtn3='<input type="button" class="button_fixed" value="Back" onClick="self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'\'">';
 /*   $scriptInHead='<script src="javascript/prototype.js" type="text/javascript" language="JavaScript"></script>
 				<script src="javascript/scriptaculous.js" type="text/javascript" language="JavaScript"> </script>

 				<script>

 				function clear()
 				{
 				this.value="";
 				}

 				function box()
 				{
 				document.getElementById("specials").innerHTML = "<div id=\"searchBox\" align=\"right\" stlye=\"display: none\"><input type=\"text\" id=\"seriesName\" value=\"Enter Special Name\" onclick=\"clear()\"><br><input type=\"hidden\" value=\"0\" name=\"specSeas\"><br><input type=\"submit\" onclick=\"getSpecials()\"/></div>";
 				}
 					 </script>';
 		$output->setScriptInHead($scriptInHead);
	*/
			
				$out.='"TMDB"';		
$output->setReloadLeftFrame(false);
$output->setMenuTitle('Search TMDB ||');
$output->setPageTitle($showName);

$output->setScriptInBody('bgColor="#F0F3F8" ');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

 echo "TMDB";

}


	?>