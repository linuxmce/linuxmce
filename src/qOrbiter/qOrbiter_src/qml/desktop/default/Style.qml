import QtQuick 1.0

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
    property string skinname: "Default"
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

}
