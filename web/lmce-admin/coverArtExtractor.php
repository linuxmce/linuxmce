<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require_once("include/amazonTools.inc.php");
/**
  * This function will take an existing Amazon request and change it so that it will be usable 
  * with the new authentication.
  *
  * @param string $secret_key - your Amazon AWS secret key
  * @param string $request - your existing request URI
  * @param string $access_key - your Amazon AWS access key
  * @param string $version - (optional) the version of the service you are using
  */

/* @var $mediadbADO ADOConnection */
/* @var $rs ADORecordSet */

$start_time=getmicrotime();

define('SUBID', '1A7XKHR5BYD0WPJVQEG2');   	// Your subscription id
define('ASSOCIATES_ID','ws');              	// Your Associates id
define('AES_VERSION','2005-02-23');        	// The version of AES
define('ENGINE','Amazon');        			

$coverArtPath='/home/coverartscan/';


$searchIndex=$_REQUEST['searchIndex'];
$fileID=isset($_REQUEST['fileID'])?(int)$_REQUEST['fileID']:NULL;
$discID=isset($_REQUEST['discID'])?(int)$_REQUEST['discID']:NULL;
$attributeID=isset($_REQUEST['attributeID'])?(int)$_REQUEST['attributeID']:NULL;

$Keyword1Type=@$_REQUEST['Keyword1Type'];
$Keyword2Type=@$_REQUEST['Keyword2Type'];
$Keyword3Type=@$_REQUEST['Keyword3Type'];

$Keyword1Search=urlencode(cleanString(@$_REQUEST['Keyword1Search']));
$Keyword2Search=urlencode(cleanString(@$_REQUEST['Keyword2Search']));
$Keyword3Search=urlencode(cleanString(@$_REQUEST['Keyword3Search']));

$searchString=(isset($_REQUEST['Keyword1Type']))?$_REQUEST['Keyword1Type'].'='.$Keyword1Search:'';
$searchString.=(isset($_REQUEST['Keyword2Type']))?'&'.$_REQUEST['Keyword2Type'].'='.$Keyword2Search:'';
$searchString.=(isset($_REQUEST['Keyword3Type']))?'&'.$_REQUEST['Keyword3Type'].'='.$Keyword3Search:'';

$isMusic = $searchIndex=='Music';
$orderBy=($isMusic)?'titlerank':'salesrank';
// Make the request for cover arts
$requestCountry = "com"; // can be changed to co.uk, de, jp, fr or ca instead - Thanks to xtra on the wiki.
$request='http://webservices.amazon.' . $requestCountry . '/onca/xml?Service=AWSECommerceService&AssociateTag='.ASSOCIATES_ID.'&Version='.AES_VERSION.'&SubscriptionId='. SUBID.'&Operation=ItemSearch&SearchIndex='.$searchIndex.'&ResponseGroup=Medium&Sort='.$orderBy.'&'.$searchString;
echo $request;
writeFile($GLOBALS['WebExecLogFile'],date('d-m-Y H:i:s')."\t".$request."\n",'a+');

// Get the response from Amazon
$newRequest = getRequest('pO56I5EnilQ+e5FBFk862q+LXPkeyXvmimMGyuDe', $request, 'AKIAJXTAVQJC3NKAQM4A');
$xml = file_get_contents($newRequest);

// Parse the results
$Result = xmlparser($xml);
//print_array($Result);

// add the record in CoverArtScan table
// depending on parameters, FK_File, FK_Disc or FK_Attribute


$mediadbADO->Execute('
	INSERT INTO CoverArtScan 
	(FK_File,FK_Disc,FK_Attribute,Engine,Keyword1Type,Keyword1Search,Keyword2Type,Keyword2Search,Keyword3Type,Keyword3Search) 
	VALUES 
	(?,?,?,?,?,?,?,?,?,?)',array($fileID,$discID,$attributeID,ENGINE.' | '.$searchIndex,$Keyword1Type,$Keyword1Search,$Keyword2Type,$Keyword2Search,$Keyword3Type,$Keyword3Search));
$casID=$mediadbADO->Insert_ID();


$found=0;
foreach ($Result['ItemSearchResponse']['Items'][0]['Item'] as $item) {
	$found++;
	if (isset($item['LargeImage']['URL'])) {
		// grab attributes
		$attributes="ASIN\t".$item["ASIN"][0]."\n";
		//print_array($item['ItemAttributes']);
		foreach ($item['ItemAttributes'] AS $attributeName=>$values){
			if(is_array($values)){
				foreach ($values AS $subAttributeName=>$value){
					if(!is_array($value)){
						$attributes.=(is_int($subAttributeName)?$attributeName:$subAttributeName)."\t".$value."\n";
					}else{
						// TODO: particular cases, like Creator
						// I left them for now
					}
				}
			}else{
				$attributes.=$attributeName."\t".$values."\n";
			}
		}		
		if(isset($item['EditorialReviews']['EditorialReview'][0]['Content'])){
			$attributes.="Synopsis\t".nl2br($item['EditorialReviews']['EditorialReview'][0]['Content'])."\n";
		}

		// If music, the Title should be imported as an Album attribute
		if ($isMusic) {
			$attributes = str_ireplace("\nTitle\t", "\nAlbum\t", $attributes);
		}
		
		// grab the cover art if exist, download it and save the record in database
		$mediadbADO->Execute('INSERT INTO CoverArtScanEntry (FK_CoverArtScan,ID,URL,Attributes) VALUES (?,?,?,?)',array($casID,$item["ASIN"][0],$item["LargeImage"]["URL"],$attributes));
		$entryID=$mediadbADO->Insert_ID();
		savePic($item['LargeImage']['URL'],$coverArtPath.$entryID.'.jpg');
		$resizeFlag=resizeImage($coverArtPath.$entryID.'.jpg', $coverArtPath.$entryID.'_tn.jpg', 256, 256);
		
	}
}

$end_time=getmicrotime();
//print 'Finished in '.round(($end_time-$start_time),3).'s, found: '.$found.' cover arts.';
?>