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
        property string skinname: "Pluto Classic"
        property string skindir: "default"
        property string skindescription: "Pluto Classic Skin"
        property string skinversion: "1.0"
        property string skinvariation: "Desktop 800x600"
        property string maincolor: "midnightblue"
        property string accentcolor: "lightsteelblue"
        property string highlight1: "aliceblue"
        property string highlight2: "slategrey"
    //main stage styles
    property int orbiterH: 720
    property int orbiterW: 1280
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: 9
    property int homesreenbuttonwidth: 9
    property color homescreenscenariobuttoncolor: "aliceblue"
    property color homescreenfloorplanbuttoncolor: "lightsteelblue"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    //icon sizes
    property int iconHeight: 9
    property int iconWidth: 9
        property int  stdbuttonh: 9
        property int  stdbuttonw: 9

    //textStyles
     property string stylebuttonimage: ""
    property int title_size: 18
    property color tile_color: "whitesmoke"

    property int t_but_size: 16
    property color t_but_color: highlight2

    //title and other misc windo accents
 property color titletextcolor: "black"
    //button styling
    property int buttonH:9
    property int buttonW: 9
    property color button_system_color: highlight1
    property color button_system_color_hover: highlight2

    property color button_action_color: "lightsteelblue"
    property color button_action_color_hover: "lightblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: accentcolor
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color rowbgColor: maincolor

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: accentcolor
    property string not_pos: highlight2

    //special
    property color alert_bg: "lightsteelblue"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: highlight1
        property color advanced_bg_text: "black"

    //stage
    property color stage_bg: maincolor
    property color bgcolor: maincolor

}
