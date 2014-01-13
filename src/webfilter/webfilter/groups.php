<?PHP
function webfiltergroups($output,$dbADO) {
	// include language files
 	includeLangFile('common.lang.php');
 	includeLangFile('webfilter.lang.php');
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	if (isset($_GET['group'])) {
	$group= $_GET['group'];
	} else {
	$group=$_POST['group'];
	}
	$res=$dbADO->Execute('SELECT var,val FROM webfilter_proxy WHERE conf="webfilter" AND conf_type="f'.$group.'"');
	$key=array();
	$value=array();
	while ($row=$res->FetchRow()) {
		$key[]=$row['var'];
		$value[]=$row['val'];
	}
	$keyval=array_combine($key, $value);
	
	if ($action == 'form') {
	$out='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="POST" name="webfilter">
	<input type="hidden" name="section" value="webfilter_groups">
	<input type="hidden" name="action" value="save">
	<input type="hidden" name="group" value="'.$_GET['group'].'">
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0" align="center">
		<tr>
		<td colspan="7" align="center">';
		$out.= $keyval["groupname"].'
		<table>
		<tr>
		<td>Groupmode:</td>
		<td><input type="text" name="groupmode" value="'.$keyval["groupmode"].'" /></td>
		</tr>
		<tr>
		<td>Groupname:</td>
		<td><input type="text" name="groupname" value="'.$keyval["groupname"].'" /></td>
		</tr>
		<tr>
		<td>naughtynesslimit:</td>
		<td><input type="text" name="naughtynesslimit" value="'.$keyval["naughtynesslimit"].'" /></td>
		</tr>
		<tr>
		<td>TIME RESTICTION(\'S) IN GROUP UPCOMING TODO!!!!!!</td> 
		</tr>
		<!--<tr>
		<td>optional time restriction</td>
		<td>from: (hour minute)<select name="from_hour"><option value="0">00</option><option value="1">01</option><option value="2">02</option><option value="3">03</option><option value="4">04</option><option value="5">05</option><option value="6">06</option><option value="7">07</option><option value="8">08</option><option value="9">09</option><option value="10">10</option>
		<option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option></select>
		<select name="from_minute"><option value="0">00</option><option value="01">01</option><option value="02">02</option><option value="03">03</option><option value="04">04</option><option value="05">05</option><option value="06">06</option><option value="07">07</option><option value="08">08</option><option value="09">09</option><option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option>
		<option value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option><option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option>
		<option value="37">37</option><option value="38">38</option><option value="39">39</option><option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option><option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option></select>
		To: (hour minute)<select name="to_hour"><option value="0">00</option><option value="1">01</option><option value="2">02</option><option value="3">03</option><option value="4">04</option><option value="5">05</option><option value="6">06</option><option value="7">07</option><option value="8">08</option><option value="9">09</option><option value="10">10</option>
		<option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option></select>
		<select name="to_minute"><option value="0">00</option><option value="01">01</option><option value="02">02</option><option value="03">03</option><option value="04">04</option><option value="05">05</option><option value="06">06</option><option value="07">07</option><option value="08">08</option><option value="09">09</option><option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option>
		<option value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option><option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option>
		<option value="37">37</option><option value="38">38</option><option value="39">39</option><option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option><option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option></select><br />
		Day\'s <input type="checkbox" name="0" value="0" /> Monday <input type="checkbox" name="1" value="0" /> Tuesday <input type="checkbox" name="2" value="0" /> Wednesday <input type="checkbox" name="3" value="0" /> Thursday <input type="checkbox" name="4" value="0" /> Friday <input type="checkbox" name="5" value="0" /> Saterday <input type="checkbox" name="6" value="0" /> Sunday <input type="checkbox" name="all" value="**" /> No internet acces.
		</td>
		</tr>-->
		<!--<tr>
		<td>Block porn:</td>
		<td><select ><option value=""></option><option value="">enabled</option><option value="">disabled</option></select></td>
		</tr>
		<tr>
		<td>safe search:</td>
		<td><select ><select></td>
		</tr>
		<tr>
		<td>Block advertising:</td>
		<td><select ><select></td>
		</tr>
		<tr>
		<td>Block proxy\'s:</td>
		<td><select ><select></td>
		</tr>-->
		</table>
		</td>
		</tr>
		<tr>
		<td><input type="submit" name="save" value="save" /></form></td>
		<tr>
		</table>';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=webfilter&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit(0);
		}
			if($_POST['action']=='save') {
				foreach($_POST as $key => $val) {
					if ($key == 'section' || $key=='group' || $key=='save' || $key=='action') {
					} else {
						$dbADO->Execute('UPDATE webfilter_proxy SET val="'.$val.'" WHERE conf="webfilter" AND conf_type="f'.$group.'" AND var="'.$key.'" AND val="'.$keyval[$key].'"');
					}
				}
				header("Location: index.php?section=webfilter_groups&group=".$_POST['group']."&msg=".translate('TEXT_WEBFILTER_UPDATED_CONST'));
			}
	}
	$output->setMenuTitle(translate('TEXT_ADVANCED_CONST').' |');
	$output->setPageTitle(translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array(translate('TEXT_WEBFILTER_GLOBAL_CONST')=>'index.php?section=webfilter',translate('TEXT_WEBFILTER_GROUPS_CONST')=>'index.php?section=webfilter_groups&group='.$group));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_WEBFILTER_RULES_CONST'));
	$output->output();
}

/*
			if ($oldgroups==1) {
			writeConf('/etc/dansguardian/lists/filtergroupslist','daniel=filter2', ,);
			}else{
			writeConf('/etc/dansguardian/lists/filtergroupslist','', ,);
			}

*/
?>