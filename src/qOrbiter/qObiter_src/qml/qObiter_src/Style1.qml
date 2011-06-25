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
    //main stage styles
    property int orbiterH: 480
    property int orbiterW: 720
    property color stageBG: "aliceblue"

    //textStyles

    property int title_size: 16
    property color tile_color: "lightgrey"

    property int t_but_size: 12
    property color t_but_color: "darkgrey"

    //title and other misc windo accents

    //button styling
    property int buttonH:60
    property int buttonW: 60
    property color button_system_color: "lightgrey"
    property color button_system_color_hover: "lightblue"

    property color button_action_color: "lightgrey"
    property color button_action_color_hover: "lightblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10
    //navigations
    property color arrow_color_nav: "aliceblue"
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color rowbgColor: "transparent"

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: "lightgrey"
    property string not_pos: "anchors: parent.top"

    //special
    property color alert_bg: "blue"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: "lightblue"
    property color advanced_bg_text: "black"

    //stage
    property color stage_bg: "steelblue"

}
