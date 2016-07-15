<?php
/***********************************************************************
*   -DHCPd Leases Parser-               Author: Derek Ditch            *
*   Created: 12-Apr-2004               License: GPL                    *
*   This was written because I need to be able to parse a dhcpd.leases *
*   file in PHP.  The only parses I could find were written in PERL. I *
*   like PERL too, but it didn't suit my needs for my current project. *
*   This was written to read the ISC implementation dhcpd.leases file  *
*   for dhcpd v 3.0.1r9. I'm using Debian (Woody) on my server, but it *
*   should work for any distro.                                        *
*   NOTE: This script is based on REGEX in dhcpd-report v 1.53         *
***********************************************************************/

/* Revision History

v0.1 12-Apr-2004
	Initial Version - Kinda sloppy but functional. (No pun intended).
	
v0.2 15-Apr-2004
	Rewrote function to have a cleaner interface.  Also added a class
	definition with an iterator.
	                                                                   */
					   
class Leases{
	var $leases;
	var $leaseIterator;
	
	function Leases(){ 
		$leaseIterator = -1; // To ensure you don't miss the 0 index
	}
	
	
	function parseConfig($filename){
	}
	/*  BEGIN LEASE METHODS  */
	function rewindLease()
	{
		$this->Leases();
	}
	
	function getLease()
	{
	 /* Will return the lease array for the lease of the index that the
	    iterator is currently on.                                      */
		if($this->leaseIterator == -1)
			$this->leaseIterator++;
			
		return $this->leases[$this->leaseIterator];
	
	}
	function nextActive()
	{ /* Will retrieve the next value in the $leases array that contains
	     an active lease. If the last active lease has already been
		 returned by a previous call, the returned value will be false.
		 
		 Pre: Leasefile must have already been processed by readLease().
		 Post: Returns an associative array containing all properties of
		       the active lease.
			   															*/
		while($lease = $this->nextLease())
		{
			if($lease["status"] === "active")
				return $lease;
		}
		
		return false;
	
	}
	
	function nextLease()
	{ /* Will retrieve the next lease in the $leases array. Acts as
		an array iterator.  If the iterator is set to the last index when
		called, false is returned.
		
		Pre: Leasefile must have been processed by readLease().
		Post: Returns an associative array containing the properties of
			  the next lease                                             */
			  															
		if($this->leaseIterator < count($this->leases))
			return $this->leases[++$this->leaseIterator];
		else
			return false;
	}

	function readLease($filename)
	{ /* Opens a lease file, splits it into individual leases,
		 then stores the results of each lease into the $leases array.
		 
		 Pre: Requires a valid dhcpd.leases file as parameter
		 Post: $leases contains an array of all leases in file
	                                                                     */
	
		if(!($leaseFile = @fopen($filename, "r")))
		{
			return false;
		}
		
		while(!feof($leaseFile))
		{
			$line = fgets($leaseFile, 1024);
			
			// Skips lines that don't matter
			if(preg_match("/^(\#)/", $line))
			{
				continue;
			}
			if(preg_match("/^(})/", $line))
			{
				$this->leases[] = $this->parseLease($entry);
				unset($entry);
				continue;
			}
			
			$entry[] = $line;
	
		} // End of while
		
		fclose($leaseFile);
		
		return true;
	}
	
	function parseLease($rgLease)
	{  /* Parses the one lease statement.
	
	   Pre:  Input should be an array containing each line of lease definition
			 starting with lease statement, ending with line before closing '}'
	   Post: Output should be an associative array with each key containing the
			 keyword in each line of the lease statement.
																			   */
		foreach($rgLease as $line){
			
			// Skips lines that don't matter
			if(preg_match("/^(\#|\})/x", $line))
			{
				continue;
			}
			
			if(preg_match("/^lease\s+(?!0)((?:(?:[01]?\d\d?|2[0-4]\d|25[0-5])\.){3}(?:[01]?\d\d?|2[0-4]\d|25[0-5]))\s+\{/x",
				$line, $matches))
			{
				$lease["ip_addr"] = $matches[1];
				continue;
			}

			if(preg_match("/^\s+hardware\s+ethernet\s*\"?(([a-f0-9]{2}:){5}[a-f0-9]{2})\"?\s?\;/x",
				$line, $matches))
			{
				$lease["mac"] = $matches[1];
				continue;
			}
			
			// Split the keywords from the values
			preg_match("/^\s+([a-z -]+[a-z-]+)\s*\"?(.+?)?\"?\;/x", $line, $matches);
			$key = $matches[1];
			$value = $matches[2];
			
			if($key === "ends never")
			{
				$lease["ends"] = "never";
				continue;
			}
			else if ($key === "starts" || $key === "ends")
			{
				// Need to trim the begining to put date into YYYY/MM/DD HH:MM:SS format
				$value = strtotime(substr($value,2));
			
				if($key === "ends" && $value <= time())
					$lease["status"] = "inactive";
				else
					$lease["status"] = "active";
			}
			else if ($key === "client-hostname")
				$key = "hostname";
			
			if(isset($key) && isset($value))
				$lease[$key] = $value;
		} // end of foreach
		
		return $lease;
	} // End of function
	
} // End of class

/* Example code on how to use this class */
/* Will produce a table of all known leases, active or not */
/* Use $test->nextActive() in place of $test->nextLease() for
   only active leases                                      */
/*
$test = new Leases;
$test->readLease("./dhcpd.leases");

print("<table border=1>\n");
print("<tr><td>IP Address</td><td>MAC</td><td>Start Time</td><td>End Time</td><td>Hostname</td><td>Status</td></tr>\n");
while($lease = $test->nextLease()): ?>

<TR>
	<TD><?php= $lease["ip_addr"] ?></TD>
	<TD><?php= $lease["mac"] ?></TD>
	<TD><?php= date("H:i j-M-Y", $lease["starts"]) ?></TD>
	<TD><?php= date("H:i j-M-Y", $lease["ends"]) ?></TD>
	<TD><?php= $lease["hostname"] ?></TD>
	<TD><?php= $lease["status"] ?></TD>
</TR>

<?php
endwhile;
?>
</table>
*/

