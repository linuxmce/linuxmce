<?
	$configFile = '/etc/pluto.conf';
	
	$DB_NAME = LoadConfig($configFile,"MySqlDBName");
	$DB_HOST = LoadConfig($configFile,"MySqlHost");
	$DB_PORT = LoadConfig($configFile,"MySqlPort");
	$DB_USER = LoadConfig($configFile,"MySqlUser");
	$DB_PASS = LoadConfig($configFile,"MySqlPassword");
	
  	$LinuxMCEHost="http://linuxmce.org/";
 	$wikiHost='http://wiki.linuxmce.org/';
 
	// pluto_main
	$dbPlutoMainType = 'mysql';
	$dbPlutoMainUser = $DB_USER;
	$dbPlutoMainPass = $DB_PASS;
	$dbPlutoMainDatabase = $DB_NAME;
	$dbPlutoMainServer = $DB_HOST;
	
	
	// pluto media
	$dbPlutoMediaType = "mysql";
	$dbPlutoMediaUser = "root";
	$dbPlutoMediaPass = "";
	$dbPlutoMediaServer = "localhost";
	$dbPlutoMediaDatabase = "pluto_media";	
	
	// pluto security
	$dbPlutoSecurityType = "mysql";
	$dbPlutoSecurityUser = "plutosecurity";
	$dbPlutoSecurityPass = "";
	$dbPlutoSecurityServer = "localhost";
	$dbPlutoSecurityDatabase = "pluto_security";

function LoadConfig($configFile,$configKey) {
	//LoadConfig()
	//Looks for the assigned value of $configKey within $configFile,
	//and returns it. If either $configFile or $configKey is not found, returns NULL.
	//Lines starting with #, // and ; are ignored and treated as comments.

	//Load config file into an array
	$lines = file($configFile);

		//Loop through array
	foreach ($lines as $thisLine) {
		//Is this line a comment?
		if($thisLine[0] <> "#" || $thisLine[0] <> "#" || $thisLine[0] <> "#") {
			//This line is not a comment, lets process it
			
			//find the = sign in this line
			$eqSignPos = strpos($thisLine,"=");
						
			//check for $configKey on the left side of the =
			//by first getting firstchar thru $eqSignPos
			$thisConfigKey =  trim(substr($thisLine,0,$eqSignPos));

			//Is this the configKey we are looking for?
			if($thisConfigKey == $configKey) {
				//Yes! Lets return the configValue on the
				//right side of the =, and get out of here
				return trim(substr($thisLine,$eqSignPos+1,-1));
				break;
			}

		}
	}

	//configKey or configFilewas not found, return NULL
	return NULL;
}
?>
