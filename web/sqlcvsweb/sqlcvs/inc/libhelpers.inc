<?

	// Helper functions




	// ripped from masterusers-site, copyright by plutohome.com


function PRV_plutoid_base35_char($int)
{
        if ($int < 10) return $int;
        if ($int < 24) return chr($int+55); // A-N: 65-78
        return chr($int+56); // P-Z: 80-90
}

function calculatePlutoID($PK_MasterUsers)
{
        $str = PRV_plutoid_base35_char($mod = $PK_MasterUsers%35);
        while ($PK_MasterUsers = ($PK_MasterUsers-$mod)/35)
        {
                $str .= PRV_plutoid_base35_char($mod = $PK_MasterUsers%35);
        }
        return strrev(str_pad($str,6,'0'));
}

function calculatePIN()
{
        $pin='';
        for ($i=0; $i<3; $i++)
        $pin.= PRV_plutoid_base35_char(rand(0,34));
        return $pin;
}

function cleanString($string,$len=1000) {
        return substr(mysql_escape_string(strip_tags(trim($string))),0,$len);
}

function cleanStringWithTags2Show($string,$len=1000) {
        return trim($string);
}

function cleanStringWithTags4DB($string,$len=1000) {
        return mysql_escape_string(trim($string));
}

function cleanInteger($number) {
        return (int)$number;
}

function cleanFloat($number) {
        return (float)$number;
}

function cleanArray($array2Parse) {
        $resultArray = array();
        foreach ($array2Parse as $elem) {
                if (cleanString((trim($elem)))!="") {
                        $resultArray[]=$elem;
                }
        }
        return $resultArray;
}

?>
