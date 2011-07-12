import Qt 4.7
//sample style file for a qml ui
/*
  color scheme
  Button sienna
  Rows   lightbrwn
  Grids  ltgrn
  Button Text white
  Title Text white

  */

QtObject {
    //main stage styles
    property int orbiterH: 480
    property int orbiterW: 640
    property color stageBG: "olivedrab"

    //textStyles

    property int title_size: 16
    property color tile_color: "white"

    property int t_but_size: 12
    property color t_but_color: "white"

    //title and other misc windo accents

    //button styling
    property int buttonH:60
    property int buttonW: 60
    property color button_system_color: "sienna"
    property color button_system_color_hover: "saddlebrown"

    property color button_action_color: "sienna"
    property color button_action_color_hover: "saddlebrown"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10



    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color bgColor: "olivedrab"

    //Notifications bar
    property int width: orbiterW - 5
    property color not_color: "sienna"
    property string not_pos: "anchors: parent.top"

    //special
    property color alert_bg: "sienna"
    property color alert_bg_text: "Orange"

    //advanced panel
    property color advanced_bg: "sienna"
    property color advanced_bg_text: "orange"

    //stage
    property color stage_bg: "burlywood"

}
