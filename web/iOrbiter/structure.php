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
          // Now we get the background image from the main variation (if such a thing exists)
          $query = "Select Value from DesignObjVariation_DesignObjParameter";
          $query .= " Join DesignObjVariation On FK_DesignObjVariation = PK_DesignObjVariation";
          $query .= " Where Value Is Not Null And Value <> '' And DesignObjVariation.FK_DesignObj = $FK_DesignObj and (FK_UI is null Or FK_UI = 1)";
          $query .= " Order By FK_UI Desc";
          $background = getMyValue($link,$query);
          if ($background == 1) {
            $background = "";
          }
          return $background;
        }
        
        print "<ul>";

        
        function getDesignObj($FK_DesignObj,$level=0) {
          global $link;
          // Return the position and designobj childs of the FK_DesignObj
          $query = "Select PK_DesignObjVariation_DesignObj,DesignObjVariation_DesignObj.FK_DesignObj_Child, X,Y,Width,Height";
          $query .= " From DesignObjVariation_DesignObj_Skin_Language";
          $query .= " Join DesignObjVariation_DesignObj On FK_DesignObjVariation_DesignObj = PK_DesignObjVariation_DesignObj";
          $query .= " Join DesignObjVariation On FK_DesignObjVariation_Parent = PK_DesignObjVariation";
          $query .= " Where DesignObjVariation.FK_DesignObj = $FK_DesignObj";
          $queryUI1 = $query . " And DesignObjVariation.FK_UI = 1";
          $queryStandard = $query . " And DesignObjVariation.FK_UI Is Null";
          $arr = getMyArray($link,$queryUI1);          
          // Sometimes a DesignObj itself is not available for UI1 one but only for the standard.
          if (count($arr)==0) {
            $arr = getMyArray($link,$queryStandard);
          }
          print "<ul><li>";
          print "Level: $level - Background for $FK_DesignObj: " . getBackground($FK_DesignObj) . "</li>\n";
          if (is_array($arr)) {

            foreach ($arr as $designobj) {
              $currentFK_DesignObj = $designobj["FK_DesignObj_Child"];              
              print "<li>DesignObj:  ". $designobj["FK_DesignObj_Child"] . "</li>\n";          
              print "<li>Position: " . $designobj["X"] . " x " . $designobj["Y"] . "</li>\n";
              print "<li>Size: " . $designobj["Width"] . " x " . $designobj["Height"] . "</li>\n";
                            
              getDesignObj($currentFK_DesignObj,$level+1);
            }
          }  
          print "</li></ul>";            
          return $arr; 
        }          
        print "</ul>";  
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
        print "Big Background $background<br>\n";
        // Now that we have the basic background, let's try and find all the objects on this screen.
        $arr = getDesignObj($FK_DesignObj);
        
        mysql_close($mediaLink);
	mysql_close($link);
		
?>
