import QtQuick 2.0

//sample style file for a qml ui
/*
  color scheme
  Button sienna
  Rows   lightbrwn
  Grids  ltgrn
  Button StyledText white
  Title StyledText white

  */


Item{
    id:classicstyle
    objectName: "noir"
    property alias style: classicstyle
    function scaleX(x){
    return x/100*manager.appWidth
    }
    function scaleY(y){
    return y/100*manager.appHeight
    }

    //skin description
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "Noir"
    property string skindir: "noir"
    property string skindescription: "Dark Skin with glossy highlights"
    property string skinversion: "1.0"
    property string skinvariation: "TV 16:9"
    property string maincolor: "Black"
    property string accentcolor: "white"
    //main stage styles
    property int orbiterH: 600
    property int orbiterW: 800
    property color stageBg: "black"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: 90
    property int homesreenbuttonwidth: 90
    property color homescreenscenariobuttoncolor: "darkgrey"
    property color homescreenfloorplanbuttoncolor: "lightgrey"
    property color homeScreenNowPlayingColorInActive: "black"
    property color homeScreenNowPlayingColorActive: "grey"
    property color highlight1: "white"
    property color lighttext: "slategrey"
    property color darkhighlight: "black"
    //icon sizes
    property int iconHeight: 90
    property int iconWidth: 90
    property int  stdbuttonh: scaleY(8)
    property int  stdbuttonw: scaleX(8)
    property int  mediabuttonH: scaleY(8)
    property int  mediabuttonW: scaleX(8)
    property int  avoptionbuttonh:scaleY(8)
    property int  avoptionbuttonw:scaleX(8)

    //textStyles

    property int title_size: 18
    property color tile_color: "white"

    property int t_but_size: 16
    property color t_but_color: "darkgrey"

    //title and other misc windo accents
    property color titletextcolor: "darkgrey"

    //icons
    property string stylebuttonimage: "../../../img/icons/noir/sqtransp.png"

    //button styling
    property int buttonRegFont: scaleX(5)
    property int buttonH:95
    property int buttonW: 95
    property color button_system_color: "darkgrey"
    property color button_system_color_hover: "lightblue"

    property color button_action_color: "maroon"
    property color button_action_color_hover: "lightblue"

    property string bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: "maroon"
    property color arrow_text_nav: "white"

    //row container styling
    property int rowWidth: orbiterW
    property int rowH: buttonH
    property color rowbgColor: "darkgrey"

    //Notifications bar
    property int notificationWidth: manager.appWidth-5
    property color not_color: "darkgrey"
    property string not_pos: "anchors: parent.top"
    property Gradient generalGradient: Gradient {
        id:bg
        GradientStop {position: 0.0; color: "#636363"}
        GradientStop {position: 0.39; color: "#323233"}
        GradientStop {position: 1.0; color: "#333333"}
    }

    //special
    property color alert_bg: "black"
    property color alert_bg_text: "white"

    //advanced panel
    property color advanced_bg: "black"
    property color advanced_bg_text: "white"

    //stage
    property color stage_bg: "darkgrey"
    property color bgcolor: "black"

}
