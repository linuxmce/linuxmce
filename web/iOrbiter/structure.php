<html>
<head>
<title>LinuxMCE - Orbiter by hand</title>
<script>
function main(nWidth,nHeight) {
  var mainCanvas = document.getElementById("main");
  var mainContext = mainCanvas.getContext("2d");
  var mainBackground = new Image();
  mainBackground.src = "skins/Basic/Media/PVRremote.png";
  mainBackground.onload = function() {
    mainContext.drawImage(mainBackground,0,0,nWidth,nHeight);
  }
}

function drawDO(nWidth,nHeight,nPosX,nPosY,cImage) {
  var mainCanvas = document.getElementById("main");
  var mainContext = mainCanvas.getContext("2d");
  var mainBackground = new Image();
  mainBackground.src = cImage;
  mainBackground.onload = function() {
    mainContext.drawImage(mainBackground,nPosX,nPosY,nWidth,nHeight);
  }

</script>
</head>
<body onload='main(640,480)'>
<?php
/*
  structure reads the pluto_main database and deciphers a
  given skin and UI into a tree containing all relevant
  information to recreate the screen in another descriptive
  language / layout engine
*/

	include_once("lib.inc.php");

	error_reporting(E_ALL);


        function getBackground($FK_DesignObj) {
          global $link;
          $background = "";
          $skin = "Basic";
          // Now we get the background image from the main variation (if such a thing exists)
          $query = "Select Value from DesignObjVariation_DesignObjParameter";
          $query .= " Join DesignObjVariation On FK_DesignObjVariation = PK_DesignObjVariation";
          $query .= " Where Value Is Not Null And Value <> '' And DesignObjVariation.FK_DesignObj = $FK_DesignObj and (FK_UI is null Or FK_UI = 1)";
          $query .= " Order By FK_UI Desc";
          $background = getMyValue($link,$query);          
          if (($background == 1) or ($background == "")) {
            $background = "";
          } else {
            // some path names for images still contain a backslash instead
            // of a forward slash, probably thanks to HADesigner's roots.
            $background = str_replace("\\","/",$background);
            // atm we hardcode the skin Basic
            $background = "skins/$skin/" . $background;
          }
          return $background;
        }
        
        function getDesignObjText($FK_DesignObj) {
          global $link;
          $query = "select PK_DesignObjVariation,Text_LS.FK_Text,Text_LS.FK_Language,Text_LS.Description,";
          $query .= " DesignObjVariation_Text_Skin_Language.X, DesignObjVariation_Text_Skin_Language.Y,";
          $query .= " DesignObjVariation_Text_Skin_Language.Width, DesignObjVariation_Text_Skin_Language.Height,";
          $query .= " DesignObjVariation_Text_Skin_Language.*";
          $query .= " from DesignObjVariation";
          $query .= " Join DesignObjVariation_Text On PK_DesignObjVariation = FK_DesignObjVariation";
          $query .= " Join Text_LS On Text_LS.FK_Text = DesignObjVariation_Text.FK_Text";
          $query .= " Join DesignObjVariation_Text_Skin_Language On DesignObjVariation_Text_Skin_Language.FK_DesignObjVariation_Text = PK_DesignObjVariation_Text";
          $query .= " Where DesignObjVariation.FK_DesignObj = " . $FK_DesignObj;
          $query .= " And Text_LS.FK_Language = 1";
          $query .= " And (DesignObjVariation.FK_UI = 1 or DesignObjVariation.FK_UI IS NULL)";
          $query .= " And DesignObjVariation_Text_Skin_Language.FK_Skin Is Null";
          $query .= " Order By Text_LS.FK_Language";
          
          $arr = getMyArray($link,$query);
          if (is_array($arr)) {
            print "<ul>";
            foreach ($arr as $designobj) {
              print "<li><h3>Description: ". $designobj["Description"] . "</h3></li>\n";
              print "<li>Position: " . $designobj["X"] . " x " . $designobj["Y"] . "</li>\n";
              print "<li>Size: " . $designobj["Width"] . " x " . $designobj["Height"] . "</li>\n";
              print "<li>Return von getDesignObjText</li>";
            }
            print "</ul>";
          }
          
        }
        
        function getDesignObj($FK_DesignObj,$level=0) {
          global $link;
          // Return the position and designobj childs of the FK_DesignObj
          $query = "Select PK_DesignObjVariation_DesignObj, DesignObj.Description, DesignObjVariation_DesignObj.FK_DesignObj_Child,";
          $query .= " DesignObjVariation_DesignObj_Skin_Language.X, DesignObjVariation_DesignObj_Skin_Language.Y,";
          $query .= " DesignObjVariation_DesignObj_Skin_Language.Width, DesignObjVariation_DesignObj_Skin_Language.Height";
          $query .= " From DesignObjVariation_DesignObj_Skin_Language";
          $query .= " Join DesignObjVariation_DesignObj On FK_DesignObjVariation_DesignObj = PK_DesignObjVariation_DesignObj";
          $query .= " Join DesignObjVariation On FK_DesignObjVariation_Parent = PK_DesignObjVariation";
          $query .= " Join DesignObj On PK_DesignObj = FK_DesignObj_Child";
          $query .= " Where DesignObjVariation.FK_DesignObj = $FK_DesignObj";
          $queryUI1 = $query . " And DesignObjVariation.FK_UI = 1";
          $queryStandard = $query . " And DesignObjVariation.FK_UI Is Null";
          $arrUI1 = getMyArray($link,$queryUI1);          
          // Sometimes a DesignObj itself is not available for UI1 one but only for the standard.
//          if (count($arr)==0) {
            $arrStandard = getMyArray($link,$queryStandard);          
//          }
          $arr = array_merge($arrUI1,$arrStandard);
          print "<ul>";
          if (is_array($arr)) {
            print "<ul>";
            foreach ($arr as $designobj) {
              $currentFK_DesignObj = $designobj["FK_DesignObj_Child"];              
              print "<li><h3>Description: ". $designobj["Description"] . "</h3></li>\n";
              print "<li>DesignObj:  ". $designobj["FK_DesignObj_Child"] . "</li>\n";          
              print "<li>Position: " . $designobj["X"] . " x " . $designobj["Y"] . "</li>\n";
              print "<li>Size: " . $designobj["Width"] . " x " . $designobj["Height"] . "</li>\n";
              print "<li>Background: " . getBackground($currentFK_DesignObj) . "</li>\n";              
              print "<li>Text: " . getDesignObjText($currentFK_DesignObj) . "</li>\n";
              getDesignObj($currentFK_DesignObj,$level+1);
              print "<li>Return von getDesignObj</li>";
            }
            print "</ul>";
          }
          print "</ul>";            
          return $arr; 
        }          
	connectDB();
	global $currentUser, $currentScreen, $currentRoom, $currentEntertainArea, $link, $mediaLink;


        // Get all data for the standard variation
        $screen = 48;  // MythTV Remote

        if (isset($_GET["screen"])) {
          $screen = $_GET["screen"];
        }
        
        $query = "SELECT FK_DesignObj From Screen_DesignObj Where FK_Screen = $screen AND FK_UI Is Null AND FK_Skin Is Null";
        
        $FK_DesignObj = getMyValue($link,$query);
        $background = getBackground($FK_DesignObj);
        print "<h2>Screen $screen - Main Background=$background for $FK_DesignObj</h2>";
        // Now that we have the basic background, let's try and find all the objects on this screen.
        $arr = getDesignObj($FK_DesignObj);
        
        mysql_close($mediaLink);
	mysql_close($link);
		
//</script>
//</head>
//<body onload='draw()'>

//<?php
  $origmainWidth = 2128;
  $mainWidth = 640;
  $origmainHeight = 1596;
  $mainHeight = 480;
  print "<canvas id='main' width='$mainWidth' height='$mainHeight'></canvas>";
?>
</body>
</html>
