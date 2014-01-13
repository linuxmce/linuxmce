<?PHP
$url = explode("::",$_SERVER["QUERY_STRING"]);
# Save the url string, we'll need it later
$url2 = $_SERVER["QUERY_STRING"];
foreach ($url as $temp){
$pieces = explode("==",$temp);
$value[$pieces[0]] = urldecode($pieces[1]);
}
$myurl=$value['DENIEDURL'];
$magic = "a3820964ba71f489db2538b240ff1fc1";
$clientip=$value['IP'];
// Obviously this is completely impossible if you use the
// random passphrase
$unixtimekey = time() + 900;
$time=time();
// If these lines are missing, some web browsers won't work
// properly
if (!eregi("https?://.+/.*", $myurl)){
$myurl = $myurl . "/";
}
// This is the most important part.
// The MD5 Hash MUST be hex encoded and uppercase with no
// padding, salt, etc other than what is shown here
// This can take some experimenting in Perl
// DO NOT just use the time() function at this point
$hash = strtoupper(md5($myurl . $magic . $clientip . $unixtimekey));
$hash .= $unixtimekey;
// If these lines are missing, some URL's won't work properly
if (eregi("\?", $url2)){
$bypass = $value['DENIEDURL'] . "&GBYPASS=" . $hash;
}else{
$bypass = $value['DENIEDURL'] . "?GBYPASS=" . $hash;
}

$dg_entity="LMCE-Webfilter";
$dg_supportemail="DCERouter@LinuxMCE";
$dg_url="-URL-";
$dg_reason="-REASONLOGGED-";
$dg_user="-USER-";
$dg_ip="-IP-";
$dg_host="dcerouter.linuxmce";
$dg_fg="-FILTERGROUP-";
$dg_cat="-CATEGORIES-";


?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>lmce-webfilter</title>
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js" type="text/javascript"></script>

</head>
<style type='text/css'>
#bodywrap {
    margin: 0 auto;
    padding: 0 10px;
    width: 80%;
    min-height: 100%;
    min-width: 600px;
    max-width: 800px;
    text-align: left;

    }
	
#header {
    padding: 5px;
    }

#headerleft {
    /*background-color: #99ff99;*/
    float: left;
    text-align: right;
    }
 
#headerright {
    margin-left: 130px;
    }
	
#info {
    position: relative;
    }
 
#info div {
    margin: 10px 0px 0px 85px;
    padding: 10px;
    background-color: #cfdde5;
    border: 2px solid #ffffff;
    -webkit-border-top-left-radius: 20px;
    -webkit-border-bottom-right-radius: 20px;
    -moz-border-radius-topleft: 20px;
    -moz-border-radius-bottomright: 20px;
    border-top-left-radius: 20px;
    border-bottom-right-radius: 20px;
    }

#footer {
    text-align: right;
    padding-right: 15px;
    }
	
#ad {
    margin: 0;
    padding: 3px;
    height: 100%;
    text-align: center;
    background-color: #eeeeee;
    }
 
#ad table {
    margin: 0 auto;
    vertical-align: middle;
    font-size: 8pt;
    border:1px dashed #999999;
    table-layout: fixed;
    height: 95%;
    width: 100%;
    align: center;
    }
</style>
<script language="JavaScript">
    //Set these for your District
    var dg_entity="EXAMPLE School District"
    var dg_supportemail="support@EXAMPLESCHOOLS.ORG"
    
    //capture information from DansGuardian
    var dg_url="-URL-";
    var dg_reason="-REASONLOGGED-";
    var dg_user="-USER-";
    var dg_ip="-IP-";
    var dg_host="-HOST-";
    var dg_fg="-FILTERGROUP-";
    var dg_cat="-CATEGORIES-";
 
    //****DEBUG****
    //var dg_url="http://www.icanhasinternets.com/2010/04/10-awesome-mascot-fights-caught-on-tape/";
    //var dg_reason="Weighted phrase limit of 50 : 225 ((-article, social)+(game,  sport)+(flash, game)+(flash, game, macromedia)+(shockwave, game)+(shockwave, game, macromedia)+facebook+-security+twitter+-university)";
    //var dg_user="teacher";
    //var dg_ip="10.1.1.10";
    //var dg_host="lab01.hs.exampleschools.org";
    //var dg_fg="Staff";
    //var dg_cat="Weighted Phrases, Games";
 
    //define the report email
    var reportlink='<a href="mailto:'+dg_supportemail+'?subject=Content Filter Request&body=Please click %22Send%22 to submit your request.   %0D%0A   %0D%0A'+dg_url+'   %0D%0A'+dg_reason+'   %0D%0A'+dg_user+', '+dg_ip+', '+dg_fg+'   %0D%0A">Click here to send a report to Tech Support!</a>'
 
    //initialize prettyurl
    var prettyurl=dg_url;
 
    //parse the blocked request
    //req_attr[full url, protocol, domain, remainder]
    var req_attr=/^(\w{3,5})\:\/\/([\w-\.]+)(\/.*)?/i.exec(dg_url);
 
    //parse the reason
    //reason_attr[full text of reason, isolated reason, string that triggered reason]
    var reason_attr=/^(Weighted phrase limit of|Banned site\: Blanket SSL Block|Banned site|Banned Phrase found|Banned combination phrase found|Your username is not allowed to web browse|Banned extension|Banned Regular Expression URL|Banned URL|Virus or bad content detected)\s?[\.\:]?\s?(.*)?/.exec(dg_reason);
 
    //select a human explination based on reported reason
    var nicereason="";
    if(reason_attr)
    {
    switch (reason_attr[1])
    {
        case "Weighted phrase limit of":
        case "Banned combination phrase found":
        case "Banned Phrase found":
            var nicereason="This web page has been filtered because of words or phrases that it contains. The web site <span class='hilite'>"+req_attr[2]+"</span> is not filtered, but this page is. <p> If this web page has content that changes often, such as a news site, then this may be temporary.";
            break;
        case "Banned site: Blanket SSL Block":
            var nicereason="This web page has been blocked because it requires an encrypted connection and has not been approved for use.";
            var prettyurl=prettyurl.replace(req_attr[1], "<strong>"+req_attr[1]+"</strong>");
            break;
        case "Banned site":
            var nicereason="This web page has been filtered because the web site <span class='hilite'>"+reason_attr[2]+"</span> is blocked based on the appropriate use policies of the School District.";
            if (dg_cat != "N/A") {
                nicereason=nicereason+" It has been catagorized as containing "+dg_cat+".";
                }
            break;
        case "Your username is not allowed to web browse":
            var nicereason="The user logged into this workstation or web browser (<span class='hilite'>"+reason_attr[2]+"</span>) is not permitted to access the Internet.";
            break;
        case "Banned extension":
            var nicereason="A <span class='hilite'>"+reason_attr[2].toUpperCase()+"</span> file was requested. The web site "+req_attr[2]+" is not allowed to provide this type of file.";
            break;
        case "Banned Regular Expression URL":
            var nicereason="This web page has been filtered because of words or phrases found in its address.";
            //Isolate the portion of the address that matched.
            var urlregex = new RegExp(reason_attr[2], "i");
            reason_attr[2] = urlregex.exec(dg_url)[0];
            break;
        case "Banned URL":
            var nicereason="This web page has been filtered because this specific area of the web site <span class='hilite'>"+req_attr[2]+"</span> is blocked based on the appropriate use policies of the School District.";
            //Identify the portion of the address that matched.
            var prettyurl=prettyurl.replace(reason_attr[2], "<strong>"+reason_attr[2]+"</strong>");
            break;
        case "Virus or bad content detected":
            var nicereason="This web page or resource has been filtered because it contains a virus or other malicious software!";
            break;
        default:
    }
    }
    else
    {
        var nicereason="You know, I'm really not sure why this page has been blocked. Sorry. Filter says: <pre>"+dg_reason+"</pre>";
    }
    //create pretty url
    if ( req_attr[2] ) {
        var prettyurl=prettyurl.replace(req_attr[2], "<em>"+req_attr[2]+"</em>");
        }
    if ( reason_attr[2] ) {
        var prettyurl=prettyurl.replace(reason_attr[2], "<strong>"+reason_attr[2]+"</strong>");
        }
 
</script>
<body>
<div id='bodywrap'>
<div id="header">
<div id='headerleft'>
<img src="http://192.168.81.1/lmce-admin/include/images/left_frame_pluto_logo.jpg" />
</div>
<div id='headerright'>
<h1 style="background-color: #000;"><span style="color: red; font-weight: bold;">RESTRICTED SITE</span></h1>
<div id='event'><h2>This page has been filtered.</h2></div>
            <div id='urlbox'><?echo $prettyurl;?><script>document.write(prettyurl)</script></div>
</div>
<div class='clear'> </div>
</div>
<div id='info'>
        <div id='nicereason'>
            <h3>Why?</h3>
            <p><?echo $value['REASON'];?><script>document.write(nicereason);</script></p>
            <p><a href='#' id='more-why-show' class='showLink' onclick='showHide("more-why");return false;'>Why filter the web?</a></p>
            <div id='more-why' class='more'>
                <p>The <script>document.write(dg_entity);</script> uses the School Web Filter to
                <ul>
                    <li>enforce the <script>document.write(dg_entity);</script> Network Usage Agreement and Acceptable Use Policy,</li>
                    <li>to provide a first line of defense against viruses and other malicious software,</li>
                    <li>and to comply with the requirements of Federal law.</li>
                </ul>
                </p>
                <p>The <a href='http://frwebgate.access.gpo.gov/cgi-bin/getdoc.cgi?dbname=106_cong_public_laws&docid=f:publ554.106'>Children's Internet Protection Act (CIPA)</a> requires <em>"the operation of a technology protection measure with respect to any of its computers with Internet access that protects against access through such computers to visual depictions that are-- (I) obscene; (II) child pornography; or (III) harmful to minors".</em></p>
                <p>The <a href='https://www.ed.gov/policy/gen/guid/fpco/ferpa/index.html'>Family Educational Rights and Privacy Act (FERPA)</a> requires that no student's educational records or personally identifiable information is released without the written consent of that student's parent or guardian.</p>
                <p><a href='#' id='more-why-hide' class='hideLink' onclick='showHide("more-why");return false;'>Okay, I see.</a></p>
            </div>
        </div>
        <div id='help'>
            <h3>Help!</h3>
			<a href="http://localhost/unblock.php?ref=-URL-" id="unblock">OVERIDE (Password Required)</a>
			<div style="border: 1px solid #000; width: 250px; min-height: 75px; position: fixed; left: 5px; margin-left: 15px; top: 50%; margin-top: -37px; display: none;" id="password">
				<form action="http://localhost/unblock.php?ref=<?php echo $_GET["ref"]; ?>" method="post">
					LMCE-User:<br />
					<select name="user" >
					<option value="Ferry">Ferry</option>
					<option value="Frans">Frans</option>
					<option value="Georgia">Georgia</option>
					</select><br />
					Enter pin:<br />
					<input type="password" name="pin" />
					<input type="submit" value="Unblock" />
				</form>
			</div>
            <p>Would you like to see some suggestions on avoiding this page? <a href='#' id='more-help-show' class='showLink' onclick='showHide("more-help");return false;'>Click here!</a></p>
            <div id='more-help' class='more'>
                <p>There are many things that can cause a web page to be filtered.</p>
                <ul>
                    <li>If you typed the name of the web site into the address bar of your browser, make sure that you have spelled it correctly.</li>
                    <li>Certain types of sites, especially news-related sites, often change very quickly. There may be some content that triggers the filter right now but that will be gone later.</li>
                </ul>
                <form action="http://www.google.com/search" method="get">
                    <input name="q" value="" size="40" maxlength="255" type="text">
                    <input name="sa" value="Google Search" type="submit">
                </form>
            </div>
        </div>
        <div id='report'>
            <h3>Report</h3>
            Please contact Tech Support if you think there has been an error.
            <script>document.write(reportlink);</script>
        </div>
        <div id='details'>
            <h3>Details</h3>
            <dl>
                <dt>Web site</dt>
                    <dd><script>document.write(req_attr[2])</script></dd>
                <dt>Full URL</dt>
                    <dd><script>document.write(req_attr[0])</script></dd>
                <dt>Rule</dt>
                    <dd><script>document.write(reason_attr[1])</script></dd>
                    <dd><script>document.write(reason_attr[2])</script></dd>
            </dl>
        </div>
<script type="text/javascript">
$('a#unblock').click(function(e){
e.preventDefault();
$('div#password').fadeIn();});
$('form').submit(function(e)
{
  e.preventDefault();
  $.post("http://localhost/unblock.php" ,{"pass":$('input[name="pin"]').val(),"ref":"<?php echo $_GET["ref"]; ?>"},function(data){
  $("div#password").append("<br />Notice: "+data.msg);
  //setTimeout(function(){window.location = "<?php echo $_GET["ref"]; ?>";}, 20000);
  },"json").fail(function(jqXHR, textStatus, errorThrown){alert("Error: "+errorThrown)});
});
</script>
</div>
    <div id='footer'>
	<? echo $dg_user.'@'.$dg_host.'('.$dg_ip.')';?>
        <script>document.write(dg_user+"@"+dg_host+' ('+dg_ip+')')</script>
    </div>
</div>
<div id='ad'>
<table cellpadding="0" cellspacing="0" border="0">
    <tr>
    <td align="center" valign="middle">
    Ad Removed.
    </td>
    </tr>
</table>
</div>
<script language="JavaScript">
    if ((document.body.clientWidth<=350) || (document.body.clientHeight<=350))
    {
      document.getElementById('bodywrap').style.display = 'none';
      document.getElementById('ad').style.display = 'block';
    }
    else
    {
      document.getElementById('bodywrap').style.display = 'block';
      document.getElementById('ad').style.display = 'none';
    }
    if (dg_fg == 'Staff')
    {
      document.getElementById('report').style.display = 'block';
    }
</script>
</body>
</html>