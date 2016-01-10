<?php
header("Content-type: text/javascript");

require_once('include/config/config.inc.php');
require_once('include/utils.inc.php');

function get_keyboard_codes_id($dbADO)
{
	$key_codes_ids = array();

	$res = $dbADO->Execute('SELECT PK_Button, Description,ASCII(Description) AS ascii FROM Button WHERE (((ASCII(Description)>=48 AND ASCII(Description)<58) || (ASCII(Description)>=97 AND ASCII(Description)<123)) AND CHAR_LENGTH(Description)=1) || (PK_Button IN (1,2,3,4,5))');
	while ($row = $res->FetchRow())
	{
		$key_codes_ids[$row['ascii']] = $row['PK_Button'];
	}

	return $key_codes_ids;
}

$keyboard_codes = get_keyboard_codes_id($dbADO);
$js_case = "\n";

$keyboard_codes[38]=1;  // up
$keyboard_codes[40]=2;  // down
$keyboard_codes[37]=3;  // left
$keyboard_codes[39]=4;  // right
$keyboard_codes[13]=5;  // enter

$keyboard_codes[27]=11; // escape


foreach ($keyboard_codes as $code => $command_key)
	$js_case .= "\t\tcase $code: command_to_send = $command_key; break;\n";

$sendKeyJs='
function sendKey(event)
{
	var command_to_send = 0;

	if (event.which)
		key_code = event.which;
	else if (event.keyCode)
		key_code = event.keyCode;

	if (event.ctrlKey || event.altKey || event.metaKey)
		return true;

	// Num Block 1-9
	if (key_code>=97 && key_code<=105)
		key_code=key_code-48;

	// Num Block 0	
	if (key_code==96 ) 
		key_code=48;

	if (key_code > 58)
		key_code += 32;

		
	switch(key_code)
	{'.$js_case.'
		default: command_to_send = 0; break;
	}

	if (command_to_send != 0)
	{
		DoCmd("PLUTO_KEY " + command_to_send);
		event.preventDefault();
		return false;
	}
	return true;
}
';

echo $sendKeyJs;
?>
