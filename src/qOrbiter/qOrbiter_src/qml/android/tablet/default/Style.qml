import QtQuick 1.0
/*!
 *\class Style
 *\brief This component defines the qml style
 *
 * \ingroup android_tablet_default
 *
 *This file is used for defining the style of the QML skin. Errors in this file will result in the QML engine falling back to the default skin.
 *The default skin is used as a testbed for functionality, and therefore considered to be the most up-to-date
 *
 */
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
    id:style
    objectName: "orbiter_style"

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }
    //skin description
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "Pluto Classic"
    property string skindir: "default"
    property string skindescription: "Pluto Classic Skin"
    property string skinversion: "1.0"
    property string skinvariation: "Desktop 800x600"
    property color maincolor: "#0D4392"
    property color accentcolor: "#EE873B"
    property color highlight1: "aliceblue"
    property color highlight2: "slategrey"
    property color alertcolor: "red"
    property color warncolor:"yellow"

    property color darkhighlight: "#628BC7"
    property color lighthighlight: "#3F76C7"
    //main stage styles
    property int orbiterH: manager.appHeight
    property int orbiterW: manager.appWidth
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
    property int  stdbuttonh: !b_orientation ? scaleY(13) : scaleY(9)
    property int  stdbuttonw: !b_orientation ? scaleX(10) :scaleX(17)
    property int widebuttonw: scaleX(10)
    property int widebuttonh: scaleY(10)
    property int mediabuttonH:!b_orientation ? scaleY(16) : scaleY(20)
    property int mediabuttonW: !b_orientation ? scaleX(20) :scaleX(20)
    property int avoptionbuttonh: !b_orientation ? scaleY(13) : scaleY(12)
    property int avoptionbuttonw: !b_orientation ? scaleX(8) :scaleX(11)


    //textStyles
    property string stylebuttonimage: ""
    property int title_size: 20* scaleX(orbiterW)
    property color title_color: "whitesmoke"
    property int  stdbuttontext: 18 * scaleX(orbiterW)
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

}
