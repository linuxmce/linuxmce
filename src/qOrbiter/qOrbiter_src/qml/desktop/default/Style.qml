import QtQuick 1.1

//sample style file for a qml ui
/*
  color scheme
  Button sienna
  Rows   lightbrwn
  Grids  ltgrn
  Button Text white
  Title Text white
  */

/*!
 *\class Style \name Default Style
 *\brief This component defines the qml style
 *\ingroup qml_desktop_default
 *
 *This file is used for defining the style of the QML skin. Errors in this file will result in the QML engine falling back to the default skin.
 *The default skin is used as a testbed for functionality, and therefore considered to be the most up-to-date
 *
 */

/**@name Desktop Default
  * This relates to the Desktop platform, default skin.
  */

///@{
QtObject{
    id:defaultStlye
    objectName: "orbiter_style"

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }
    //skin description
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "Default"
    property string skindir: "default"
    property string skindescription: "Pluto Classic Skin"
    property string skinversion: "1.0"
    property string skinvariation: "Desktop 800x600"
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
    property int  stdbuttonh: scaleY(13)
    property int  stdbuttonw: scaleX(8)
    property int widebuttonw: scaleX(10)
    property int widebuttonh: scaleY(10)
    property int mediabuttonH: scaleY(9)
    property int mediabuttonW: scaleX(5)
    property int avoptionbuttonh: scaleY(10)
    property int avoptionbuttonw: scaleX(6)


    //textStyles
    property string stylebuttonimage: ""
    property int title_size:scaleX(.20)
    property color title_color: "whitesmoke"
    property int  stdbuttontext:scaleX(4)
    property color lighttext: "whitesmoke"
    property color darktext: "slategrey"

    property int t_but_size: 16
    property color t_but_color: highlight2

    //title and other misc windo accents
    property color titletextcolor: "black"
    //button styling
    property int buttonH:9
    property int buttonW: 9
    property color button_system_color: "#3F76C7"
    property color button_system_color_hover: "#22416F"

    property color button_action_color: "lightsteelblue"
    property color button_action_color_hover: "lightblue"


    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: accentcolor
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
    property color rowbgColor: "#22416F"

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
    property color stage_bg: "#22416F"
    property color bgcolor: "#0D4392"

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
///@}
