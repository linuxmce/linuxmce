import QtQuick 1.1
/*!
 *\class Style
 *\brief This component defines the qml style for the Basic QML skin
 *
 *\ingroup desktop_basic
 *
 *This file is used for defining the style of the QML skin. Errors in this file will result in the QML engine falling back to the default skin.
 *The default skin is used as a testbed for functionality, and therefore considered to be the most up-to-date
 *
 */


QtObject{
    id:plutoHomeStyle
    objectName: "orbiter_style"
    //skin description
    property string skincreator: "PlutoHome transformed by Peer Oliver Schmidt"
    property string skinname: "Basic UI1"
    property string skindir: "basic"
    property string skindescription: "Transformation of LinuxMCE UI1 Basic To QML"
    property string skinversion: "0.1"
    property string skinvariation: "Desktop 1080p"
    property string maincolor: "aliceblue"
    property string accentcolor: "lightsteelblue"
    property color highlight1: "aliceblue"
    property color darkhighlight: "#628BC7"
    property color lighthighlight: "#3F76C7"

    //main stage styles
    property int orbiterH: 1080*.9
    property int orbiterW: 1920*.9
    property color stageBG: "slategray"
    property int homescreenrowheight:100
    property int homescreenbuttonheight: 90
    property int homesreenbuttonwidth: 90
    property color homescreenscenariobuttoncolor: "aliceblue"
    property color homescreenfloorplanbuttoncolor: "lightsteelblue"
    property color homeScreenNowPlayingColorInActive: "indianred"
    property color homeScreenNowPlayingColorActive: "lightcyan"

    //icon sizes
    property int iconHeight: 90
    property int iconWidth: 90
    property int  stdbuttonh: 90
    property int  stdbuttonw: 90

    //textStyles
    property string stylebuttonimage: ""
    property int title_size: 18
    property color tile_color: "whitesmoke"

    property int t_but_size: 16
    property color t_but_color: "darkgrey"

    property color lighttext: "grey" // added by RayBe

    //title and other misc windo accents
    property color titletextcolor: "black"
    //button styling
    property int buttonH:95
    property int buttonW: 95
    property color button_system_color: "lightsteelblue"
    property color button_system_color_hover: "lightblue"

    property color button_action_color: "lightsteelblue"
    property color button_action_color_hover: "lightblue"

    property variant bgImg: "null"
    property string b_title: "null"
    property int but_smooth: 10

    //navigations
    property color arrow_color_nav: "lightsteelblue"
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5
    property int rowH: buttonH + 10
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

}
