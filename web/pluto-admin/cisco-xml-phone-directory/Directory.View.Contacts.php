<?php
/**
 * Directory_View_Contacts - Basic view class for Showing Range Pages
 *
 * @package	Directory
 * @version	1.0
 * @author	Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 *********************************************************************/

require_once 'Directory.View.php';

class Directory_View_Contacts extends Directory_View {

	/**
	 * The Ranges Passed from the controller.
	 * @var array	$rangeArray;
	 */
	var $rangeArray;

	function render() {

		$rangeFragment = "";
		foreach ($this->rangeArray as $range) {
			$rangeFragment.= "  <MenuItem>";
			$rangeFragment.= "   <Name>".$range."</Name>";
			$rangeFragment.= "   <URL>".$this->linkForRequest("dial")."&amp;range=".urlencode($range)."</URL>";
			$rangeFragment.= "  </MenuItem>";		
		}

		$output = "<CiscoIPPhoneMenu>";
		$output.= " <Title>Phone Books</Title>";
		$output.= " <Prompt>Select Letter Range</Prompt>";
		$output.= $rangeFragment;
		$output.= "</CiscoIPPhoneMenu>";

		return $output;
			
	}

	function Directory_View_Contacts($rangeArray) {

		$this->rangeArray = $rangeArray;

	}

}

?>
