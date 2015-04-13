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

import QtQuick 2.2

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

    property bool useOpacity:false
    property double appOpacity: .45

    //main colors http://colorschemedesigner.com/#3JB1Tw0w0l7l7
    property color mainColor: "black"
    property color darkHighlightColor: "darkgreen"
    property color lightHighlightColor:"yellow"

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
    property color title_color: "white"
    property color sub_title_color:"green"

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

    //New style //
    /* Overall */
    property double bgLightOpacity:.35
    property double bgHeavyOpacity:.65
    property double bgContentOpacity:.50

    /* Headers, containers and lists */
    property color contentBgColor:"darkgrey"
    property int headerFontSize:14
    property color headerBgColor:"green"
    property int listItemHeight:scaleY(8)
    property color listItemBgColor:"green"
    property color listItemActiveBgColor: "white"
    property color listItemPressedBgColor: "limegreen"
    property color listItemTextInactiveColor:"white"
    property color listItemTextActiveColor:"black"
    property Gradient bgHeaderGradient: Gradient{
        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.65;color:"black"}
    }
    property Gradient bgContentGradient: Gradient{

        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.75;color:"black"}
    }

    /* Buttons and their junk */
    property int buttonHeight:scaleY(4)
    property int  buttonWidth: scaleX(12)
    property int buttonLabelSize:scaleY(2)
    property color buttonColor:"darkgreen"
    property color buttonPressedColor:"white"
    property color buttonBorderColor:"white"
    property int buttonBorderWidth:1
    property Gradient buttonGradient: Gradient{
        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.65;color:"black"}
    }


    /* Animation Speeds */
    property int quickAnimation:350
    property int mediumAnimation:500
    property int longAnimation:750
    property int animationEasing:Easing.OutBack


    //end scale function
    //-color definitions

    property color accentcolor: "green"
    property color highlight1: "silver"
    property color highlight2: "orange"
    property color alertcolor: "red"
    property color warncolor:"yellow"
    property color darkColor:"black"
    property double shadeOpacity:.65

    property color darkhighlight: "darkgrey"
    property color lighthighlight: "green"
    //--end color definitions
}
