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


    QtObject{
        id:crystalShades
        objectName: "orbiter_style"
        //skin description
        property string skincreator: "Raymond Bemelmans (aka RayBe)"
        property string skinname: "CrystalShades"
        property string skindir: "crystalshades"
        property string skindescription: "Floorplan Skin"
        property string skinversion: "0.1"
        property string skinvariation: "Desktop 720p"
        property string maincolor: "aliceblue"
        property string accentcolor: "lightsteelblue"
        property color highlight1: "aliceblue"
        property color darkhighlight: "#628BC7"
        property color lighthighlight: "#3F76C7"

    //main stage styles
    property int orbiterH: 720*.9
    property int orbiterW: 1280*.9
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
