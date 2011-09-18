<?php /* $Id: cdr.php 12068 2011-05-06 04:41:41Z p_lindheimer $ */
defined('FREEPBX_IS_AUTH') OR die('No direct script access allowed');


/* -- AMP Begin -- */

$low = $_SESSION["AMP_user"]->_extension_low;
$high = $_SESSION["AMP_user"]->_extension_high;
if ((!empty($low)) && (!empty($high))) {
	$channelfilter="OR (FIELD( SUBSTRING_INDEX( channel, '/', 1 ) , 'SIP', 'IAX2' ) > 0 AND SUBSTRING_INDEX(SUBSTRING(channel,2+LENGTH(SUBSTRING_INDEX( channel, '/', 1 ))),'-',1) BETWEEN $low and $high)";
	$channelfilter.="OR (dstchannel<>'' AND FIELD( SUBSTRING_INDEX( dstchannel, '/', 1 ) , 'SIP', 'IAX2' ) > 0 AND SUBSTRING_INDEX(SUBSTRING(dstchannel,2+LENGTH(SUBSTRING_INDEX( dstchannel, '/', 1 ))),'-',1) BETWEEN $low and $high)";

        $_SESSION["AMP_SQL"] = " AND ((src+0 BETWEEN $low AND $high) OR (dst+0 BETWEEN $low AND $high) OR (dst+0 BETWEEN 8$low AND 8$high) $channelfilter)";
} else {
	$_SESSION["AMP_SQL"] = "";
}

$AMP_CLAUSE = $_SESSION['AMP_SQL'];
if (!isset($AMP_CLAUSE)) {
	$AMP_CLAUSE = " AND src = 'NeverReturnAnything'";
	echo "<font color=red>YOU MUST ACCESS THE CDR THROUGH THE ASTERISK MANAGEMENT PORTAL!</font>";
}

//echo 'AMP_CLAUSE='.$AMP_CLAUSE.'<hr>';
/* -- AMP End -- */

function cdrpage_getpost_ifset($test_vars)
{
	if (!is_array($test_vars)) {
		$test_vars = array($test_vars);
	}
	foreach($test_vars as $test_var) { 
		if (isset($_POST[$test_var])) { 
			global $$test_var;
			$$test_var = addslashes($_POST[$test_var]); 
		} elseif (isset($_GET[$test_var])) {
			global $$test_var; 
			$$test_var = addslashes($_GET[$test_var]);
		}
	}
}

cdrpage_getpost_ifset(array('s', 't'));

$get_vars = array(
'accountcode',
'accountcodetype',
'after',
'atmenu',
'before',
'channel',
'clid',
'current_page',
'clidtype',
'days_compare',
'duration1',
'duration1type',
'duration2',
'duration2type',
'dst',
'dsttype',
'fromday',
'frommonth',
'fromstatsday_sday',
'fromstatsmonth',
'fromstatsmonth_sday',
'letter', 
'list',
'list_total',
'list_total_day',
'min_call',
'months_compare',
'name',
'nb_record',
'order',
'posted',
'resulttype',
's',
'sens',
'sql_limit',
'src',
'srctype',
'stitle',
't',
'today',
'tomonth',
'tostatsday_sday',
'tostatsmonth_sday',
'tostatsmonth',
'totalcall',
'userfield',
'userfieldtype',
'AMP_SQL',
'FG_ACTION_SIZE_COLUMN',
'FG_DELETION',
'FG_TABLE_CLAUSE',
'Period',
'SQLcmd',
);

foreach ($get_vars as $gv) {
	if (!isset($$gv) || !$$gv) {
		$$gv = isset($_REQUEST[$gv]) ? htmlspecialchars($_REQUEST[$gv]) : null;

	}
}

$array = array ("INTRO", "CDR REPORT", "CALLS COMPARE", "MONTHLY TRAFFIC","DAILY LOAD", "CONTACT");
$s = $s ? $s : 1;
$t = (isset($t))?$t:null;
$section="section$s$t";

$racine=$_SERVER['PHP_SELF'];
$update = "03 March 2005";


$paypal="NOK"; //OK || NOK
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>		
		<title>Asterisk CDR</title>
		<meta http-equiv="Content-Type" content="text/html">
		<link rel="stylesheet" type="text/css" media="print" href="cdr/common/print.css">
		<script type="text/javascript" src="cdr/common/encrypt.js"></script>
		<style type="text/css" media="screen">
			@import url("cdr/common/layout.css");
			@import url("cdr/common/content.css");
			@import url("cdr/common/docbook.css");
		</style>
		<meta name="MSSmartTagsPreventParsing" content="TRUE">
	</head>
	<body>

<?php if ($section=="section0"){?>

<?php }elseif ($section=="section1"){?>

	<?php require("call-log.php");?>

<?php }elseif ($section=="section2"){?>

	<?php require("call-comp.php");?>

<?php }elseif ($section=="section3"){?>

	<?php require("call-last-month.php");?>

<?php }elseif ($section=="section4"){?>

	<?php require("call-daily-load.php");?>

<?php }else{?>
	<h1>Coming soon ...</h1>
   
<?php }?>

		
		<br><br><br><br><br><br>
		</div>

			<div class="fedora-corner-br">&nbsp;</div>
			<div class="fedora-corner-bl">&nbsp;</div>
		</div>
		<!-- content END -->
		
		<!-- footer BEGIN -->
		<div id="fedora-footer">

			<br>			
		</div>
		<!-- footer END -->
	</body>
</html>
