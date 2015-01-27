import QtQuick 2.2

/*!
 *\class Style
 *\brief This component defines the qml style
 *
 * \ingroup android_phone_default
 *
 *
 *This file is used for defining the style of the QML skin. Errors in this file will result in the QML engine falling back to the default skin.
 *The default skin is used as a testbed for functionality, and therefore considered to be the most up-to-date
 *
 */

QtObject{
    id:classicstyle
    objectName: "orbiter_style"
    //skin description
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "default"
    property string skindir: "default"
    property string skindescription: "Android phone skin, work in progress"
    property string skinversion: "1.0"
    property string skinvariation: "Android 320X480"
    property string maincolor: "black"
    property bool profile:!manager.isProfile
    property string commonQmlPath:"../../../../skins-common/qt5/default/"
    property variant customScreens: [ 1 ]

    //scale function
    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }

    //end scale function
    //-color definitions

    property color accentcolor: "orange"
    property color highlight1: "silver"
    property color highlight2: "orange"
    property color alertcolor: "red"
    property color darkColor:"black"
    property color warncolor:"yellow"

    property color darkhighlight: "darkgrey"
    property color lighthighlight: "green"
    //--end color definitions
    //main stage styles
    property int orbiterH:manager.appHeight
    property int orbiterW: manager.appWidth
    property int orbiterWp: 480
    property int orbiterHp: 320
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: scaleX(12)
    property int homesreenbuttonwidth: scaleY(12)
    property color homescreenscenariobuttoncolor: "aliceblue"
    property color homescreenfloorplanbuttoncolor: "lightsteelblue"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    property int homeselectorh: scaleY(15)
    property int homeselectorw: scaleY(15)


    //icon sizes
    property int iconHeight: manager.isProfile ? scaleY(17) :scaleY(10)
    property int iconWidth: manager.isProfile ? scaleX(25) :scaleY(12)
    property int  stdbuttonh: manager.isProfile ? scaleY(12) :scaleY(13)
    property int  stdbuttonw: manager.isProfile ? scaleY(12) :scaleY(13)
    property int buttonLabelText : manager.isProfile ? scaleY(3) : scaleY(2.5)

    //textStyles
    property string stylebuttonimage: ""
    property int title_size: scaleY(10)
    property color tile_color: "whitesmoke"

    property int t_but_size: scaleY(12)
    property color t_but_color: "darkgrey"

    //title and other misc windo accents
    property color titletextcolor: "black"
    //button styling
    property int buttonH: scaleY(12)
    property int buttonW: scaleY(12)

    property int avoptionbuttonh :scaleY(12)
    property int avoptionbuttonw :scaleY(12)

    property int mediabuttonw :scaleY(12)
    property int mediabuttonh :scaleY(12)

    property color button_system_color: "silver"
    property color button_system_color_hover: "darkgrey"

    property color button_action_color: "lightgrey"
    property color button_action_color_hover: "darkgrey"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: "lightsteelblue"
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonHeight + 10
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
    property int buttonHeight: scaleY(12) // minimum height to be able to hit it with a finger
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

    property double shadeOpacity:.65

    //--end color definitions

}
