import QtQuick 1.0

//sample style file for a qml ui
/*
  color scheme
  Button sienna
  Rows   lightbrwn
  Grids  ltgrn
  Button Text white
  Title Text white

  */


QtObject{
    id:classicstyle
    objectName: "orbiter_style"
    //skin description
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "Android-Default"
    property string skindir: "default"
    property string skindescription: "Android phone skin, work in progress"
    property string skinversion: "1.0"
    property string skinvariation: "Android 320X480"
    property string maincolor: "grey"
    property string accentcolor: "indianred"
   //scale function
    function scaleX(x){
        return x/100*appH
    }
    function scaleY(y){
        return y/100*appW
    }
    //end scale function
    //main stage styles
    property int orbiterH:appH
    property int orbiterW: appW
    property int orbiterWp: 480
    property int orbiterHp: 320
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: scaleX(12)
    property int homesreenbuttonwidth: scaleY(12)
    property color homescreenscenariobuttoncolor: "aliceblue"
    property color homescreenfloorplanbuttoncolor: "lightsteelblue"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"



    //icon sizes
    property int iconHeight:scaleY(15)
    property int iconWidth: scaleY(15)
    property int  stdbuttonh: scaleY(12)
    property int  stdbuttonw: scaleY(12)

    //textStyles
    property string stylebuttonimage: ""
    property int title_size: scaleY(10)
    property color tile_color: "whitesmoke"

    property int t_but_size: scaleY(12)
    property color t_but_color: "darkgrey"

    //title and other misc windo accents
    property color titletextcolor: "black"
    //button styling
    property int buttonH:scaleY(12)
    property int buttonW: scaleY(12)
    property int avoptionbuttonh :scaleY(12)
    property int avoptionbuttonw :scaleY(12)
     property int mediabuttonw :scaleY(12)
     property int mediabuttonh :scaleY(12)
    property color button_system_color: "lightsteelblue"
    property color button_system_color_hover: "lightblue"

    property color button_action_color: "lightsteelblue"
    property color button_action_color_hover: "lightblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: "lightsteelblue"
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color rowbgColor: "transparent"

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: "aliceblue"
    property string not_pos: "anchors: parent.top"

    //special
    property color alert_bg: "lightsteelblue"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: "cornflowerblue"
    property color advanced_bg_text: "white"

    //stage
    property color stage_bg: "lightSteelblue"
    property color bgcolor: "lightsteelblue"

}
