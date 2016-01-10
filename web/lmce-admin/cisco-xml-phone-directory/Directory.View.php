<?php
/**
 * Directory_View - Base Abstract class which must be overriden.
 * At the very least, render() should be overriden.
 *
 * @package	Directory
 * @version	1.0
 * @author	Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 *********************************************************************/

class Directory_View {

	/**
	 * render() - This should be overriden to return the needed
	 * XML for the view, in the end, the string returned should
	 * return the entire document. 
	 *
	 * @return	string	The XML Document.
	 *************************************************************/
	function render() {
	
		$output = "<CiscoIPPhoneText>";
		$output.= " <Title>Brand New View</Title>";
		$output.= " <Prompt>View Not Defined Yet</Prompt>";
		$output.= " <Text>Override render() and do something!</Text>";
		$output.= "</CiscoIPPhoneText>";		
	
		return $output;

	}


	/**
	 * Directory_View - Constructor. Doesn't do much.
	 *
	 *************************************************************/
	function Directory_View() {
	
		// nothin.	
	
	}

	/**
	 * Generate a URL to the server, given the link.
	 *
	 * @param	string	$url	The Mode to select
	 *
	 * @param	string	$page	The Page to select
	 *
	 * @return	string	The Completed URL.
	 */
	function linkForRequest($request,$page = "0") {
	
		return "http://" . $_SERVER["SERVER_ADDR"] . ":"
		       		 . $_SERVER["SERVER_PORT"] . ""
				 . $_SERVER["SCRIPT_NAME"] . "?mode="
				 . urlencode($request);
	
	}

}
?>
