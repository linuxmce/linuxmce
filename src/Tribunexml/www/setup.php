<html>
<head>
<title>Configuring channel Lineups</title>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<script language="JavaScript" src="check.js"></script>
</head>

<body style="padding:0px;">
<div align="center">
<?
//	define('main_path',dirname(__FILE__));
	define('script_name',basename(__FILE__));

	$host	='dcerouter';
	$user	='root';
	$psw	='';
	$dbp	='pluto_myth';
	$dbm	='mythconverg';

	//---------------- Trying to connect to all db ------------------
	$handle_pluto = @mysql_connect($host,$user,$psw,true);
	if ( !$handle_pluto ){
		echo "Cannot connect to host: $host, user: $user<br>";
		exit;
	}
	if ( !@mysql_select_db($dbp,$handle_pluto) ){
		echo "Cannot select database: $dbp<br>";
		exit;
	}
	$handle_myth = @mysql_connect($host,$user,$psw,true);
	if ( !$handle_myth ){
		echo "Cannot connect to host: $host, user: $user<br>";
		exit;
	}
	if ( !@mysql_select_db($dbm,$handle_myth) ){
		echo "Cannot select database: $dbm<br>";
		exit;
	}

	$action = isset($_REQUEST['action'])?$_REQUEST['action']:'OK';
	if(($action=='Cancel')||($action=='OK')){ // default page
		$query="SELECT DISTINCT UserName,name FROM UserLineups";
		$res=sendQuery($query, $handle_pluto);
		echo "<h1><i>Current lineups</i></h1><br/>
	<b>Select linup for modifying/removing or add new lineup.</b>
	<form action='".script_name."' method='post'>";
		if(mysql_num_rows($res)){
			echo "
	<table cellpadding='3' cellspacing='0'>
		<tr>
			<th>&nbsp;</th>
			<th>Lineup name</th>
		</tr>";
			while($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				echo "
		<tr>
			<td><input name='lineup' value='".$row['name']."' type='radio'/></td>
			<td>".$row['UserName']."</td>
		</tr>";
			}
			echo "
	</table>
	<br>
	<input name='action' value='Details' type='submit'/>&nbsp;
	<input name='action' value='Modify' type='submit'/>&nbsp;
	<input name='action' value='Remove' type='submit'/>&nbsp;";
		}
		echo "
	<input name='action' value='Add' type='submit'/></form>";
	}elseif($action=='Details'){
		echo "<h1>Lineup Wizard - Details</h1><br/>";
		$query="SELECT UserName FROM UserLineups WHERE name='$lineup'";
		$res=sendQuery($query, $handle_pluto);
		if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
			echo "
			Lineup: ".$row['UserName']."<br/>";
		}
		list($zipcode,$provider)=explode("|",$lineup);
		$query="SELECT l.row_id, s.station_name name,us.station_name uname FROM Lineups l
		LEFT JOIN Headends h ON l.headend_id=h.headend_id
		LEFT JOIN Stations s ON s.PK_Stations=l.FK_Stations
		LEFT JOIN UserLineups ul ON ul.row_id=l.row_id and ul.FK_Headends=$provider
		LEFT JOIN Stations us ON us.PK_Stations=ul.FK_Stations
		WHERE PK_Headends=$provider and s.station_name<>''";
		$res=sendQuery($query, $handle_pluto);
		if(mysql_num_rows($res)){
			$lineups=array();
			$stations['name']=array();
			while($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				if(!in_array($row['name'],$stations['name']) ){
					$lineups[]=$row['row_id'];
					$stations['name'][]=$row['name'];
					$stations['uname'][]=$row['uname'];
				}
			}
			echo "
	<form action='".script_name."' method='POST'>
		<table cellpadding='3' cellspacing='0' class='normaltext' align='center'>
			<tr>
				<th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th>";
			$rownum=0;
			foreach($lineups as $k=>$l){
				if($rownum++%3==0){
					echo "
			</tr>
			<tr>
				";
				}
				echo "<td><img height='11' width='11' src='".(($stations['name'][$k]!=$stations['uname'][$k])?'i_unchk.gif':'i_chk.gif')."'></td><td>".$stations['name'][$k]."</td>";
			}
			echo "
			</tr>
		</table>
		<input name='action' value='OK' type='submit'>
	</form>";
		}
	}elseif($action=='Modify'){ // Modification lineup
		echo "<h1>Lineup Wizard - Modify</h1><br/>";
		$query="SELECT UserName FROM UserLineups WHERE name='$lineup'";
		$res=sendQuery($query, $handle_pluto);
		if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
			echo "
			Lineup: ".$row['UserName']."<br/>";
		}
		list($zipcode,$provider)=explode("|",$lineup);
		$query="SELECT distinct UserName,sourceid FROM UserLineups WHERE name='$lineup'";
		$res=sendQuery($query, $handle_pluto);
		$sourceid='';
		$lineupName='';
		if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
		 $sourceid=$row['sourceid'];
		 $lineupName=$row['UserName'];
		}
		$query="SELECT l.row_id, s.station_name name,us.station_name uname FROM Lineups l
		LEFT JOIN Headends h ON l.headend_id=h.headend_id
		LEFT JOIN Stations s ON s.PK_Stations=l.FK_Stations
		LEFT JOIN UserLineups ul ON ul.row_id=l.row_id and ul.FK_Headends=$provider
		LEFT JOIN Stations us ON us.PK_Stations=ul.FK_Stations
		WHERE PK_Headends='$provider' and s.station_name<>''";
		$res=sendQuery($query, $handle_pluto);
		if(mysql_num_rows($res)){
			$lineups=array();
			$stations['name']=array();
			while($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				if(!in_array($row['name'],$stations['name']) ){
					$lineups[]=$row['row_id'];
					$stations['name'][]=$row['name'];
					$stations['uname'][]=$row['uname'];
				}
			}
			echo "
	<form action='".script_name."' method='POST'>
		<input type='hidden' name='lineup' value='$lineup'>
		<input type='hidden' name='provider' value='$provider'>
		<input type='hidden' name='sourceid' value='$sourceid'>
		<input type='hidden' name='lineupName' value='$lineupName'>
		<input value='Select all' onclick='checkAll(this.form,true)' type='button'>&nbsp;
		<input value='Unselect all' onclick='checkAll(this.form,false)' type='button'><br>
		<table cellpadding='3' cellspacing='0' class='normaltext' align='center'>
			<tr>
				<th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th>";
			$rownum=0;
			foreach($lineups as $k=>$l){
				if($rownum++%3==0){
					echo "
			</tr>
			<tr>
				";
				}
				echo "<td><input ".(($stations['name'][$k]!=$stations['uname'][$k])?'':'checked')." name='lineupmas[]' value='$l' type='checkbox'></td><td>".$stations['name'][$k]."</td>";
			}
			echo "
			</tr>
		</table>
		<input name='action' value='Cancel' type='submit'>&nbsp;
		<input name='action' value='Remove' type='submit'>&nbsp;
		<input name='action' value='Update' type='submit'>
	</form>";
		}
	}elseif($action=='Remove'){ // Remove lineup
		echo "<h1>Lineup Wizard - Remove</h1><br/>";
		$query="SELECT UserName FROM UserLineups WHERE name='$lineup'";
		$res=sendQuery($query, $handle_pluto);
		if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
			echo "
	Are you realy want to remove lineup: ".$row['UserName']."<br/>
	<form action='".script_name."' method='POST'>
		<input type='hidden' name='lineup' value='$lineup'>
		<input name='action' value='Cancel' type='submit'/>&nbsp;
		<input name='action' value='Yes' type='submit'/>
	</form>";
		}
	}elseif($action=='Yes'){
		$query="DELETE FROM UserLineups WHERE name='$lineup'";
		sendQuery($query, $handle_pluto);
		$query="DELETE FROM videosource WHERE lineupid='$lineup'";
		sendQuery($query, $handle_myth);
		echo "
	Removed OK<br>
	<form action='".script_name."' method='POST'>
		<input name='action' value='OK' type='submit'/>
	</form>";
	}elseif($action=='Update'){ //
		$lineups=join(",",$lineupmas);
		if(!empty($lineups)){
			$query="DELETE FROM UserLineups WHERE name='$lineup'";
			sendQuery($query, $handle_pluto);
			$query="INSERT INTO UserLineups(UserName, name, FK_Headends, sourceid, FK_Stations,row_id, device, tms_chan, service_tier, effective_date, expiration_date)SELECT '$lineupName', '$lineup', $provider, $sourceid, FK_Stations,row_id, device, tms_chan, service_tier, effective_date, expiration_date FROM Lineups WHERE row_id in ($lineups)";
			sendQuery($query, $handle_pluto);
			echo "
	Updated OK<br>
	<form action='".script_name."' method='POST'>
		<input name='action' value='OK' type='submit'/>
	</form>";
		}else{
			echo "
	No channels were selected. Do you want remove this lineup?
	<form action='".script_name."' method='POST'>
		<input name='action' value='Remove' type='submit'/>
		<input name='action' value='Cancel' type='submit'/>
	</form>";
		}
	}elseif($action=='Add'){
		echo "<h1>Lineup Wizard - Step 1</h1><br/>
	<form action='".script_name."' method='POST'>
		Enter Postal Code&nbsp;<input id='zipcode' name='zipcode' value=''/>
		<input name='action' value='Enter' type='submit'/>
	</form>";
	}elseif($action=='Enter'){
		$query="SELECT PK_Headends, community_name, headend_name FROM Headends WHERE zip_code='$zipcode'";
		$res=sendQuery($query, $handle_pluto);
		if(mysql_num_rows($res)){
			echo "<h1>Lineup Wizard - Step 2</h1><br/>
	<form action='".script_name."' method='POST'>
		<input type='hidden' name='zipcode' value='$zipcode'>
		<table cellpadding='3' cellspacing='0' class='normaltext' align='center'>
			<tr>
				<th>&nbsp;</th>
				<th>Provider</th>
			</tr>";
			$providers=array();
			while($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				if(!in_array($row['community_name'].'|'.$row['headend_name'],$providers)){
					$providers[]=$row['community_name'].'|'.$row['headend_name'];
					echo "
			<tr>
				<td><input type='radio' name='provider' value='".$row['PK_Headends']."'/></td>
				<td>".$row['headend_name']." - ".$row['community_name']."</td>
			</tr>";
				}
			}
			echo "
		</table><br/>
		<input name='action' value='Cancel' type='submit'/>&nbsp;
		<input name='action' value='Next' type='submit'/>
	</form>";
		}else{
			echo "<h1>Lineup Wizard - Step 2</h1><br/>
	There are no any providers for this region! <br/>
	<form action='".script_name."' method='POST'>
		<input name='action' value='OK' type='submit'/>
	</form>";
		}
	}elseif($action=='Next'){
		echo "<h1>Lineup Wizard - Step 3</h1><br/>";
		$query="SELECT count(*) from UserLineups WHERE name='$zipcode|$provider'";
		$res=sendQuery($query, $handle_pluto);
		if($row=mysql_fetch_array($res, MYSQL_NUM)){
			if(0!=$row[0]){
				echo "
	This provider exists in you settings. You may change channels for him <br/>
	<form action='".script_name."' method='POST'>
		<input name='action' value='OK' type='submit'>
	</form>";
				exit;
			}
		}
		$query="SELECT l.row_id, s.station_name name FROM Lineups l
		LEFT JOIN Headends h ON h.headend_id=l.headend_id
		LEFT JOIN Stations s ON s.PK_Stations=l.FK_Stations
		WHERE h.PK_Headends=$provider and s.station_name<>''";
		$res=sendQuery($query, $handle_pluto);
		if(mysql_num_rows($res)){
			$stations=array();
			$lineups=array();
			while($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				if(!in_array($row['name'],$stations) ){
					$lineups[]=$row['row_id'];
					$stations[]=$row['name'];
				}
			}
			echo "
	<form action='".script_name."' method='POST'>
		<input type='hidden' name='zipcode' value='$zipcode'>
		<input type='hidden' name='provider' value='$provider'>
		<input value='Select all' onclick='checkAll(this.form,true)' type='button'>&nbsp;
		<input value='Unselect all' onclick='checkAll(this.form,false)' type='button'><br>
		<table cellpadding='3' cellspacing='0' class='normaltext' align='center'>
			<tr>
				<th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th><th>&nbsp;</th><th>Station</th>";
			$rownum=0;
			foreach($lineups as $k=>$l){
				if($rownum++%3==0){
					echo "
			</tr>
			<tr>
				";
				}
				echo "<td><input checked name='lineupmas[]' value='$l' type='checkbox'/></td><td>".$stations[$k]."</td>";
			}
			echo "
			</tr>
		</table>
		<input name='action' value='Cancel' type='submit'>&nbsp;
		<input name='action' value='Finish' type='submit'>
	</form>";
		}
	}elseif($action=='Finish'){
		$lineups=join(",",$lineupmas);
		if(!empty($lineups)){
			$query="SELECT headend_name,community_name FROM Headends WHERE PK_Headends=$provider";
			$res=sendQuery($query, $handle_pluto);
			$providerName="";
			if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				$providerName=$row['headend_name']." - ".$row['community_name']."(for $zipcode)";
			}
			$sourceid='';
			$query="SELECT sourceid FROM videosource WHERE xmltvgrabber='tribunetv' and lineupid='$zipcode|$provider'";
			$res=sendQuery($query, $handle_myth);
			if($row=mysql_fetch_array($res, MYSQL_ASSOC)){
				$sourceid=$row['sourceid'];
			}else{
				sendQuery("INSERT INTO videosource(name,xmltvgrabber,userid,lineupid)
				VALUES ('$zipcode|$provider','tribunetv','','$zipcode|$provider')", $handle_myth);
				$sourceid=mysql_insert_id($handle_myth);
			}
			$query="INSERT INTO UserLineups(UserName, name, FK_Headends, sourceid, FK_Stations,row_id, device, tms_chan, service_tier, effective_date, expiration_date)SELECT '$providerName','$zipcode|$provider', $provider, $sourceid, FK_Stations,row_id, device, tms_chan, service_tier, effective_date, expiration_date FROM Lineups WHERE row_id in ($lineups)";
			sendQuery($query, $handle_pluto);
			echo "
	Updated OK<br>";
		}else{
			echo "
	No channels were selected";
		}
		echo "
	<form action='".script_name."' method='POST'>
		<input name='action' value='OK' type='submit'>
	</form>";
	}

	function sendQuery($query,$handle){
		if(!($res=@mysql_query($query, $handle))){
			echo "Cannot exec<br/>$query<br/>".mysql_error($handle)."<br/>EXIT<br/>=================================<br/>";
			exit;
		};
		return $res;
	}
?>
</div>
</body>
</html>

