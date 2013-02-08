<?php
class Hosts{
	var $dhcpdHosts;
	var $hosts;
	var $iterator;
	
	function Hosts($filename){
		$this->iterator = -1;
		$this->dhcpdHosts = $filename;
	}
	
	function addHost($name, $mac){
		if(! $fh = fopen($this->dhcpdHosts, a)){
			print("ERROR: ");
			print(" Unable to open $this->dhcpdHosts for writing!\n");
			exit;
		}
		
		$line = "host $name {\n\thardware ethernet $mac;\n}\n";
		fwrite($fh, $line);
		fclose($fh);
	}
	
	function removeHost($mac){
		if(! $fh = fopen($this->dhcpdHosts, r)){
			print("ERROR: ");
			print(" Unable to open '$this->dhcpdHosts' for reading/writing!\n");
			exit;
		}
		
		$found = false;
		
		while(!feof($fh))
			$file[] = fgets($fh, 1024);
		
		for($i = 0; $i < sizeof($file); $i++){
			if(isset($j) && $j < sizeof($file)){
				$file[$i] = $file[$j];
				$j++;
				continue;
			}
			if(isset( $j ) && ( $j >= sizeof($file)) ){
				unset($file[$i]);
				$j++;
				continue;
			}
					
			if(preg_match("/\#? host\s+ (?i)[a-z -]+ \s* \{/x", $file[$i])){
				if(preg_match("/\#? \s+hardware\sethernet\s" . $mac . "\;/x", $file[$i+1])){
					$found = true;
					$j = $i + 3;
					$file[$i] = $file[$j];
					$j++;
				}  // End of Mac match
			} // End of host entry
		} // End of for loop


		if($found){
			fclose($fh);
			$fh = fopen($this->dhcpdHosts, w);
			for($i = 0; $i < sizeof($file); $i++){
				fwrite($fh, $file[$i]);
			}  // End of writing
		}
		
		fclose($fh);
		return $found;
	}
	
	function readHosts(){
		
		if(! $fh = fopen($this->dhcpdHosts, r)){
			print("ERROR: ");
			print(" Unable to open '$this->dhcpdHosts' for reading!\n");
			exit;
		}
		
		while(!feof($fh)){
			$line = fgets($fh, 1024);
			
			if(preg_match("/^(\}|\#\})/", $line))
			{
				$this->hosts[] = $this->parseHost($entry);
				unset($entry);
				continue;
			}
			
			$entry[] = $line;
		}
		
		fclose($fh);
		return;
	}
	
	function parseHost($entry){
		list(,$name) = explode(" ", $entry[0]);
		list(,,$mac) = explode(" ", $entry[1]);
		
		if(preg_match("/^(\#)/", $entry[0]))
			$status = "disabled";
		else 
			$status = "enabled";
		
		return array("hostname" => $name, "mac" => substr($mac, 0, -2), "status" => $status);
	}
	
	function nextHost(){
		if($this->iterator >= sizeof($this->hosts) )
			return false;
		else
			return $this->hosts[++$this->iterator];
	}
	
	function rewindHosts(){
		$this->iterator = -1;
	}
	
	function disableHost($mac){
		if(! $fh = fopen($this->dhcpdHosts, r)){
			print("ERROR: ");
			print(" Unable to open '$this->dhcpdHosts' for reading/writing!\n");
			exit;
		}
		
		$found = false;
		
		while(!feof($fh))
			$file[] = fgets($fh, 1024);
		
		for($i = 0; $i < sizeof($file); $i++){	
			if(preg_match("/^host\s+(?i)[a-z -]+\s*\{/", $file[$i])){
				if(preg_match("/\s+hardware\sethernet\s" . $mac . "\;/", $file[$i+1])){
					$found = true;
					for($j = 0; $j < 3; $j++)
						$file[$i + $j] = "#" . $file[$i + $j];
				}  // End of Mac match
			} // End of host entry line
		} // End of for loop


		if($found){
			fclose($fh);
			$fh = fopen($this->dhcpdHosts, w);
			for($i = 0; $i < sizeof($file); $i++){
				fwrite($fh, $file[$i]);
			}  // End of writing
		}
		
		fclose($fh);
		return $found;
	}
	
	/**
	* @return bool
	* @param string $mac
	* @desc Enables host entry by uncommenting it in the host file if the mac address exists and is commented.
	*/
	function enableHost($mac){
		if(! $fh = fopen($this->dhcpdHosts, r)){
			print("ERROR: ");
			print(" Unable to open '$this->dhcpdHosts' for reading/writing!\n");
			exit;
		}
		
		$found = false;
		
		while(!feof($fh))
			$file[] = fgets($fh, 1024);
		
		for($i = 0; $i < sizeof($file); $i++){	
			if(preg_match("/^\#host\s+(?i)[a-z -]+\s*\{/", $file[$i])){
				if(preg_match("/\#\s+hardware\sethernet\s" . $mac . "\;/", $file[$i+1])){
					$found = true;
					for($j = 0; $j < 3; $j++)
						$file[$i + $j] = substr($file[$i + $j], strpos($file[$i+$j], "#") + 1);
				}  // End of Mac match
			} // End of host entry line
		} // End of for loop


		if($found){
			fclose($fh);
			$fh = fopen($this->dhcpdHosts, w);
			for($i = 0; $i < sizeof($file); $i++){
				fwrite($fh, $file[$i]);
			}  // End of writing
		}
		
		fclose($fh);
		return $found;
	}
	
	function hostExists($hostname, $mac){
		$match = false;

		foreach($this->hosts as $host){
			if(strtoupper($hostname) == strtoupper($host["hostname"])){
				$match = true;
			}
			elseif(strtoupper($mac) == strtoupper($host["mac"])){
				$match = true;
			}
				
			if($match)
				break;
		}
		
		return $match;
	}
		
}

/*
$test = new Hosts("dhcpd.hosts");
$test->readHosts();

$test->enableHost("ff:ff:ff:ff:ff:ff");
$test->disableHost("ff:ff:ff:ff:ff:ff");
$test->removeHost("ff:ff:ff:ff:ff:ff");
$test->addHost("walker", "ff:ff:ff:ff:ff:ff");



$test->addHost("ditch", "0d:ea:f2:00:0e:11");
$test->addHost("spitz", "0d:ea:f2:00:0e:12");
$test->addHost("stallo", "ff:ea:f2:00:0e:11");
$test->addHost("west", "0d:ff:f2:00:ff:11");
$test->addHost("walker", "ff:ff:ff:ff:ff:ff");
$test->addHost("sperry", "00:00:0a:00:00:01");
*/

?>

