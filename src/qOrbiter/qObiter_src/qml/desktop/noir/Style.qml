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
        property string skinname: "Noir"
        property string skindir: "noir"
        property string skindescription: "Dark Skin with glossy highlights"
        property string skinversion: "1.0"
        property string skinvariation: "Desktop 800x600"
        property string maincolor: "Black"
        property string accentcolor: "white"
    //main stage styles
    property int orbiterH: 600
    property int orbiterW: 800
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: 90
    property int homesreenbuttonwidth: 90
    property color homescreenscenariobuttoncolor: "darkgrey"
    property color homescreenfloorplanbuttoncolor: "lightgrey"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    //icon sizes
    property int iconHeight: 90
    property int iconWidth: 90
        property int  stdbuttonh: 90
        property int  stdbuttonw: 90

    //textStyles

    property int title_size: 18
    property color tile_color: "whitesmoke"

    property int t_but_size: 16
    property color t_but_color: "darkgrey"

    //title and other misc windo accents

    //button styling
    property int buttonH:95
    property int buttonW: 95
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
