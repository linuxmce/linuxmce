<?php

/**
 * Directory.controller.Main - Controller logic for Directory Display.
 * 
 * @package	Directory
 * @version	1.0
 * @author	Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 *********************************************************************/

require_once 'Directory.Model.Contact.php';
require_once 'Directory.View.php';
require_once 'Directory.View.Contacts.php';
require_once 'Directory.View.Dial.php';

class DirectoryController {

	/**
	 * The Request data
	 * @var	array	$request
	 */
	var $request;

	/**
	 * The Contacts Model
	 * @var	object	$contact
	 */
	var $contact;

	/**
	 * The Current Mode
	 * @var string	$mode
	 */
	var $mode;

	/**
	 * The Current Range regexp fragment
	 * @var string	$range
	 */
	var $range;

	/**
	 * Total # of contacts in database.
	 * @var	integer	$totalEntries
	 */
	var $totalEntries;


	/**
	 * DirectoryController - Constructor
	 *
	 * @param	string	$request	The Request Type
	 *
	 * @return	object	DirectoryController Object.
 	 *************************************************************/
	function DirectoryController($request) {

		// Find # of contacts, if less than 31, just show them.
		// and bypass the letter selection page.

		$this->contact = new Contacts();

		if (!$request["mode"]) {

			$this->totalEntries = $this->contact->numEntriesMatching("");

			if ($this->totalEntries > 31) {
				$this->mode = "contacts";
			} else {
				$this->mode = "contacts";
				$this->range = "a-z";
			}

		} else {
	
			if ($this->mode = "dial") {
				$this->range = $request["range"];			
			}		
		
		}

	}

	/**
	 * prepareContacts - Prepare the Letters filter view
	 *
	 * @return	string	the rendered XML for the Letters View
	 */
	function prepareContacts() {

		if (!$this->range) {
			$rangeArray = array();
	
			$rangeLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
			$numPages = ceil($this->totalEntries / 32);
			$letterRanger = ceil($numPages / 26);
	
			for ($i=0;$i<=numPages;$i++) {
	
				$j = $i + $letterRanger - 1;
			
				if ($j > 25) {$j = 25;}			
	
				$rangeLetterBeg = $rangeLetters[$i];
				$rangeLetterEnd = $rangeLetters[$j];			
	
				$rangeArray[$i] = $rangeLetterBeg . "-" . $rangeLetterEnd;
	
			}
		} else {
			$rangeArray = Array('A-Z');		
		}

		$contactsView = new Directory_View_Contacts($rangeArray);
		return $contactsView->render();
	
	}

	/**
	 * prepareDial - Prepare the Dial view
	 *
	 * @return	string	the rendered XML for the Dial View
	 */
	function prepareDial() {

		$matchingContacts = $this->contact->dialMatchingRange($this->range);

		$dialView = new Directory_View_Dial($matchingContacts);
		return $dialView->render();

	}

	/**
	 * render - Output the Cisco XML.
	 *
	 * No Parameters.
	 *
	 * @return	string	$output		The Final Output.
	 *************************************************************/
	function render() {

		switch ($this->mode) {
		
			case "dial":
				$output = $this->prepareDial();
				break;
			case "contacts":
				$output = $this->prepareContacts();
				break;
		}
	return $output;
	}
}
?>
