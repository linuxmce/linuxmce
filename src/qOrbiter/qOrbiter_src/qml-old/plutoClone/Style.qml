/*!
 *\class Style
 *\brief This component defines the qml style
 *
 *\ingroup desktop_noir
 *
 *This file is used for defining the style of the QML skin. Errors in this file will result in the QML engine falling back to the default skin.
 *The default skin is used as a testbed for functionality, and therefore considered to be the most up-to-date
 *
 */

import QtQuick 1.1

Item{
    id:stbStyle
    objectName: "STB"
    property alias style: stbStyle

    function scaleX(x){
    return x/100*appW
    }
    function scaleY(y){
    return y/100*appH
    }

    //skin description
    property string skincreator: "Langston Ball"
    property string skinname: "STB"
    property string skindir: "STB"
    property string skindescription: "Basic skin to mimic a standard Set-top box."
    property string skinversion: "1.0"
    property string skinvariation: "TV 16:9"
    property string maincolor: "Black"
    property string accentcolor: "white"

    /* Animation Properties */
    property int globalAnimationSpeed:750
    property int globalAnimationEasing:Easing.InOutQuad
    /* End Animation Properties */

    property int orbiterH: 600
    property int orbiterW: 800

    //main colors http://colorschemedesigner.com/#3JB1Tw0w0l7l7
    property color mainColor: "#1B5898"
    property color darkHighlightColor: "#FF6000"
    property color lightHighlightColor:"#F0C73C"

    property color lighttext: "slategrey"


    property int homescreenrowheight:100
    property int homescreenbuttonheight: 90
    property int homesreenbuttonwidth: 90

    //icon sizes
    property int iconHeight: 90
    property int iconWidth: 90
    property int  stdbuttonh: 90
    property int  stdbuttonw: 90

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
    property int notificationWidth: appW-5
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
