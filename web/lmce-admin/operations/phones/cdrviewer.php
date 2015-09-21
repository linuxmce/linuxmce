<?php
function cdrviewer($output) {
	includeLangFile('common.lang.php');
	includeLangFile('cdrviewer.lang.php');
	$protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";
	
	$out='
	<script language="javascript" type="text/javascript">
		function iFrameHeight() {
			var h = 0;
			if ( !document.all ) {
				h = document.getElementById("blockrandom").contentDocument.height;
				document.getElementById("blockrandom").style.height = h + 60 + "px";
			} else if( document.all ) {
				h = document.frames("blockrandom").document.body.scrollHeight;
				document.all.blockrandom.style.height = h + 20 + "px";
			}
		}
	</script>
	<iframe 
		src="'.$protocol.$_SERVER['HTTP_HOST'].'/lmce-admin/cdrviewer/index.php" 
		onload="iFrameHeight()"	
		id="blockrandom" 
		width="98%" 
		height="1000" 
		frameborder="0" 
		scrolling="auto"">
	</iframe>';

	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_CDR_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_CDR_CONST')=>'index.php?section=cdrviewer'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_CDR_CONST'));
	$output->output();
}
?>
