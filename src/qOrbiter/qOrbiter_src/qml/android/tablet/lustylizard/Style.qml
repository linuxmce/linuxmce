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
        return x/100*appW
    }
    function scaleY(y){
        return y/100*appH
    }
    //skin description
    property string skincreator: "Jochen Roth (aka ochorocho)"
    property string skinname: "wip"
    property string skindir: "wip"
    property string skindescription: "Tablet skin"
    property string skinversion: "0.1"
    property string skinvariation: "Tablet 1280x800"
    property color maincolor: "#0D4392"
    property color accentcolor: "#EE873B"
    property color highlight1: "aliceblue"
    property color highlight2: "slategrey"
    property color alertcolor: "red"
    property color warncolor:"yellow"

    property color darkhighlight: "#628BC7"
    property color lighthighlight: "#3F76C7"
    //main stage styles
    property int orbiterH: appH
    property int orbiterW: appW
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
    property int  stdbuttonh: !b_orientation ? scaleY(10) : scaleY(10)
    property int  stdbuttonw: !b_orientation ? scaleX(10) :scaleX(10)
    property int widebuttonw: scaleX(10)
    property int widebuttonh: scaleY(10)
    property int mediabuttonH: scaleY(9)
    property int mediabuttonW: scaleX(5)
    property int avoptionbuttonh: scaleY(10)
    property int avoptionbuttonw: scaleX(6)


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

    //OCHOROCHO
    // DEFAULT BUTTON
    property int  btHomeDefaultH: !b_orientation ? scaleX(12) : scaleY(11)
    property int  btHomeDefaultW: !b_orientation ? scaleX(14) : scaleY(9.9)
    property int  btHomeDefaultSpace: !b_orientation ? scaleX(2.500) : scaleX(1.000)

    property int  bottomPanelH: !b_orientation ? scaleX(7) : scaleX(12)

}
