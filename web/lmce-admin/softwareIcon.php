<?
session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$sID=(int)@$_REQUEST['sID'];
if($sID==0){
	$pic=join('',file('include/images/spacer.png'));
}else{
	$picArray=getAssocArray('Software_Source','FK_Software','Iconstr',$dbADO,'WHERE FK_Software='.$sID.' AND Iconstr IS NOT NULL LIMIT 0,1');
	if(count($picArray)==0){
		$pic=join('',file('include/images/spacer.png'));
	}else{
		$pic=($picArray[$sID]=='')?join('',file('include/images/spacer.png')):$picArray[$sID];
	}
	
}

Header( "Content-type:  image/png");
//send the binary data
echo $pic;
?>