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
        property string skinname: "Droid"
        property string skindir: "droid"
        property string skindescription: "Droid skin WIP"
        property string skinversion: "1.0"
        property string skinvariation: "Android 800X480"
        property string maincolor: "grey"
        property string accentcolor: "indianred"
    //main stage styles
    property int orbiterH:800
    property int orbiterW: 480
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: 90
    property int homesreenbuttonwidth: 90
    property color homescreenscenariobuttoncolor: "aliceblue"
    property color homescreenfloorplanbuttoncolor: "lightsteelblue"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    //icon sizes
    property int iconHeight: 90
    property int iconWidth: 90
        property int  stdbuttonh: 90
        property int  stdbuttonw: 90

    //textStyles
     property string stylebuttonimage: ""
    property int title_size: 18
    property color tile_color: "whitesmoke"

    property int t_but_size: 16
    property color t_but_color: "darkgrey"

    //title and other misc windo accents
 property color titletextcolor: "black"
    //button styling
    property int buttonH:95
    property int buttonW: 95
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
