<?php
/**
 * Directory_View_Dial - Basic view class for Dial Pages
 *
 * @package	Directory
 * @version	1.0
 * @author	Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 *********************************************************************/

require_once 'Directory.View.php';

class Directory_View_Dial extends Directory_View {

	/**
	 * The Contacts Passed from the controller.
	 * @var array	$rangeArray;
	 */
	var $matchingContacts;

	function render() {

		$rangeFragment = "";
		foreach ($this->matchingContacts as $contact) {
			$rangeFragment.= "  <DirectoryEntry>";
			$rangeFragment.= "   <Name>".$contact->name." (".$contact->description.")</Name>";
			$rangeFragment.= "   <Telephone>".$contact->dialas."</Telephone>";
			$rangeFragment.= "  </DirectoryEntry>";		
		}

		$output = "<CiscoIPPhoneDirectory>";
		$output.= " <Title>Directory</Title>";
		$output.= " <Prompt>Select Entry to Dial</Prompt>";
		$output.= $rangeFragment;
		$output.= "</CiscoIPPhoneDirectory>";

		return $output;
			
	}

	function Directory_View_Dial($matchingContacts) {

		$this->matchingContacts = $matchingContacts;

	}

}

?>
