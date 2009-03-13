<?php
// tools.inc.php -- contains two helpful functions, XmlParser() and GetData()

// Parser originally by Torsten Köster (torsten at jserver dot de) with some mods by me

// List elements as of the 2005-02-23 Amazon WSDL
$xml_list_elements = array(
"Accessory", "Actor","Argument","Artist","ASIN","AudioFormat","Author","BrowseNode","BrowseNodes",
"CameraManualFeatures","CartItem","ChildTransactionItems","Creator","Customer", "CustomerReviews",
"Customers","Director","Disc","EditorialReview","Element","Error","Feature","Feedback",
"Format","Header", "Information","Item", "Items","Language", "List","ListmaniaList","ListItem","Lists",
"Offer","OfferListing","Operation", "OperationInformation","Package","Parameter","PhotoFlashType",
"PictureFormat","Platform","ResponseGroup","ResponseGroupInformation","Review","SavedForLaterItem",
"SearchIndex","Seller","SellerListing","SellerListings","Sellers","Shipment","SimilarProduct",
"SpecialFeatures", "SupportedImageType","Track", "Transaction","TransactionItem","TransactionItemId",
"Transactions"
);

// Global error strings from XmlParser and GetData
$parser_error='';
$getdata_error='';

// returns associative array or false on error. If there's an error, the global $parser_error will
// contain the error details
function XmlParser($string)
{
    global $parser_error;

    $parser_error='';
    $values=array();

    // Create parser
    $p = xml_parser_create("UTF-8");
    xml_parser_set_option($p,XML_OPTION_CASE_FOLDING,false);
    xml_parser_set_option($p,XML_OPTION_SKIP_WHITE,true);

    // Parse into array structure
    $rc = xml_parse_into_struct($p,$string,$values);

    /* Check for Parsing Error */
    if (!$rc)
    {
        $errorcode = xml_get_error_code($p);
        $errstring = xml_error_string($errorcode);
        $byte= xml_get_current_byte_index($p);
        $parser_error = "XML PARSER ERROR: Error Code= $errorcode, Explanation= $errstring, Byte Number= $byte";
        xml_parser_free($p);
        return false;
    }

    xml_parser_free($p);

    // We store our path here
    $hash_stack = array();

    // This is our target
    $ret = array();

    foreach ($values as $val) {

        switch ($val['type']) {
            case 'open': // Start array structure
            array_push($hash_stack, $val['tag']);
            $valarg= (isset($val['attributes'])) ? $val['attributes'] : null;
            $ret = composeArray($ret, $hash_stack, $valarg);
            break;

            case 'close': // All done with this element
            array_pop($hash_stack);
            break;

            case 'complete':
            array_push($hash_stack, $val['tag']);
            $valarg=(isset($val['value']))? $val['value'] : null;
            // handle all attributes except those in 'open' container tags
            if (isset($val['attributes'])) {
                $temparr=array($val['tag'] => $valarg);
                $valarg=array_merge($val['attributes'], $temparr);
            };
            $ret = composeArray($ret, $hash_stack, $valarg);
            array_pop($hash_stack);
            break;

            default:
            // Ignoring CDATA type
        }
    }

    return $ret;
}

function &composeArray($array, $elements, $value)
{
    global $xml_list_elements;

    // Get current element
    $element = array_shift($elements);

    // Does the current element refer to a list?
    if (in_array($element,$xml_list_elements))
    {
        // Are there more elements?
        if(sizeof($elements) > 0)
        {
            $array[$element][sizeof($array[$element])-1] = &composeArray($array[$element][sizeof($array[$element])-1], $elements, $value);
        }
        else // It's an array
        {
            $size = (isset($array[$element]))?  sizeof($array[$element]) : 0;
            $array[$element][$size] = $value;
        }
    }
    else
    {
        // Are there more elements?
        if(sizeof($elements) > 0)
        {
            $array[$element] = &composeArray($array[$element], $elements, $value);
        }
        else
        {
            $array[$element] = $value;
        }
    }

    return $array;
}



// Returns the response as a string or false on error
function GetData($url, $timeout) {
    global $getdata_error;

    // Parse the URL into parameters for fsockopen
    $UrlArr = parse_url($url);
    $host = $UrlArr['host'];
    $port = (isset($UrlArr['port'])) ? $UrlArr['port'] : 80;
    $path = $UrlArr['path'] . '?' . $UrlArr['query'];

    // Zero out the error response
    $errno = null;
    $errstr = '';
    $getdata_error = '';

    // Open the connection to Amazon
    $fp = @fsockopen($host, $port, $errno, $errstr, $timeout);

    // Failed to open the URL
    if (!is_resource($fp)) {
        $getdata_error = "Fsockopen error number = $errno Details = $errstr";
        return false;
    }

    // Send an HTTP GET header and Host header
    if (!(fwrite($fp, 'GET '. $path .' HTTP/1.0' . "\r\n". 'Host: ' . $host . "\r\n\r\n"))) {
        fclose($fp);
        $getdata_error = "Fwrite error. Could not write GET and Host headers";
        return false;
    }

    // Block on the socket port, waiting for response from Amazon
    if (function_exists('socket_set_timeout')) {
        @socket_set_timeout($fp, $timeout);
        socket_set_blocking($fp, true);
    }

    // Get the HTTP response code from Amazon
    $line = fgets($fp , 1024);

    if ($line == false){
        fclose($fp);
        $getdata_error = "Fgets error. Did not receive any data back from Amazon";
        return false;
    }

    // HTTP return code of 200 means success
    if (!(strstr($line, '200'))) {
        fclose($fp);
        $getdata_error = "HTTP error. Did not receive 200 return code from Amazon. Instead, received this: $line";
        return false;
    }
    // Find blank line between header and data
    do {
        $line = fgets($fp , 1024);
        if ($line == false) {
            fclose($fp);
            $getdata_error = "Fgets: did not receive enough data back from Amazon";
            return false;
        }
        if (strlen($line) < 3) {
            break;
        }
    } while (true);

    $xml='';
    // Fetch the data from Amazon
    while ($line = fread($fp, 8192))
    {
        if ($line == false) {
            fclose($fp);
            $getdata_error = "Fread: error reading data from Amazon";
            return false;
        }
        $xml .= $line;
    }

    fclose($fp);
    return $xml;
}
?>
