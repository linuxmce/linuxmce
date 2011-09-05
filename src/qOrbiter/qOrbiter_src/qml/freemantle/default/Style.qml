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
        property string skinvariation: "N900 800X480"
        property string maincolor: "aliceblue"
        property string accentcolor: "lightsteelblue"
    //main stage styles
    property int orbiterH: 480
    property int orbiterW: 800
    property color stageBG: "slategray"
    property int homescreenrowheight:70
    property int homescreenbuttonheight: 65
    property int homesreenbuttonwidth: 65
    property color homescreenscenariobuttoncolor: "darkgrey"
    property color homescreenfloorplanbuttoncolor: "lightgrey"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    //icon sizes
    property int iconHeight: 65
    property int iconWidth: 65
        property int  stdbuttonh: 65
        property int  stdbuttonw: 65

    //textStyles

    property int title_size: 12
    property color tile_color: "whitesmoke"

    property int t_but_size: 10
    property color t_but_color: "darkgrey"

    //title and other misc windo accents

    //button styling
    property int buttonH:65
    property int buttonW: 65
    property color button_system_color: "chocolate"
    property color button_system_color_hover: "lightblue"

    property color button_action_color: "maroon"
    property color button_action_color_hover: "lightblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: "maroon"
    property color arrow_text_nav: "white"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color rowbgColor: "transparent"

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: "chocolate"
    property string not_pos: "anchors: parent.top"

    //special
    property color alert_bg: "black"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: "black"
    property color advanced_bg_text: "white"

    //stage
    property color stage_bg: "grey"
    property color bgcolor: "black"

}
