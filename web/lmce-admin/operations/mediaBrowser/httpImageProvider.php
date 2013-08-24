<?

$pass=""; $user="root"; $db="pluto_media";
 $con= mysql_connect("localhost", $user, $pass); 	 // connection
	if (!$con) { die('Could not Connect'.mysql_error()); //error messaging
	$connMessage="Fail";
	};
	if ($con) {mysql_select_db("pluto_media") or die(mysql_error()); //connect to media db or error out
	$connMessage="Conn Good";
	};


	$test = $_GET['type'];
    $val = $_GET['val'];

    if($test=="img"){
mediaPicsImage($val);
	}
else if ($test=="attribute"){
	echo attributeImage($val);
}
else if($test=="imdb"){

} else if ($test==="screensaver"){
screensaverImg($val);
}
else{
	
	echo file_get_contents("operations/mediaBrowser/images/addAll.png");
	
}



function mediaPicsImage($fileName){

$img =file_get_contents("mediapics/".$fileName);
echo $img;
}

function attributeImage($fileName){
$test=mysql_query("SELECT * FROM Picture_Attribute WHERE FK_Attribute=\"$attribute\" ") or die (mysql_error("MYsql Error"));	
}

function imdbImage($imdbID){

}

function screensaverImg($ssimg){
$img = file_get_contents($ssimg);
echo $img;
}

?>
