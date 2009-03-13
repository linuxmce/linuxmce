<?
function searchAmazon($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	$keyword=str_replace('.mp3','',@$_REQUEST['sstring']);
	$fileID=(isset($_REQUEST['fileID']))?(int)$_REQUEST['fileID']:$_SESSION['lastFileID'];
	
	$searchUrl='http://www.amazon.com/gp/search/ref=nb_ss_gw/102-7168857-3489732?url=search-alias%3Daps&field-keywords='.urlencode($keyword).'&Go.x=0&Go.y=0&Go=Go';
	$cmd='wget \''.$searchUrl.'\' -O -';
	
	$out=exec_batch_command($cmd,1);	
	
	// replace <a href="/something with absolute urls
	$out=str_replace('a href="/','a href="http://www.amazon.com/',$out);
	
	// product url pattern: http://www.amazon.com/exec/obidos/tg/detail/-/B00005L8ED?v=glance
	// replace them with internal urls
	
	$pattern='/http:\/\/www.amazon.com\/exec\/obidos\/tg\/detail\/-\/(.*)\?v=glance/';
	$replacement='index.php?section=grabAmazonAttributes&fileID='.$fileID.'&amazonURL=http://www.amazon.com/exec/obidos/tg/detail/-/\\1?v=glance';

	$out=preg_replace($pattern,$replacement,$out);
	
	$pattern='/http:\/\/www.amazon.com\/gp\/product\/(.*)/';
	$replacement='index.php?section=grabAmazonAttributes&fileID='.$fileID.'&amazonURL=http://www.amazon.com/gp/product/\\1';

	$out=preg_replace($pattern,$replacement,$out);

	
	die($out);	
}
?>