import QtQuick 1.0

//sample style file for a qml ui
/*
  color scheme
  Buttons
  Rows
  Grids
  Button Text
  Title Text

  */
Component{
    id: classicStyle


QtObject {
    //main stage styles
    property int orbiterH: 600
    property int orbiterW: 800
    property color stageBG: "olivedrab"

    //textStyles

    property int title_size: 16
    property color tile_color: "blue"

    property int t_but_size: 12
    property color t_but_color: "white"

    //title and other misc windo accents

    //button styling
    property int buttonH:75
    property int buttonW: 75
    property color button_system_color: "blue"
    property color button_system_color_hover: "steelblue"

    property color button_action_color: "blue"
    property color button_action_color_hover: "steelblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10



    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color bgColor: "blue"

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: "lightblue"
    property string not_pos: "anchors: parent.top"

    property color alert_bg: "blue"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: "lightblue"
    property color advanced_bg_text: "White"

    //stage
    property color stage_bg: "burlywood"
}

}
