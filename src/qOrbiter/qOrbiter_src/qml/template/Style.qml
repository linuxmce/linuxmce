import QtQuick 1.0


/*
 This is a sample style file for a LinuxMCE Skin. In this file, you can define all the properties you need to give you skin style.
 Things you can list here include:
 *Default Heights
 *Global Functions (see scaleX() and scaleY()
 *Default Colors

Please read this document so you can get a feel to what extent you can predefine the styles.

NOTE: the following, as listed in the skin must always be there. you are free to assign properties in between.

QtObject{
    id:style
    objectName: "orbiter_style"


  */


//---------------do not change-------------------------//
QtObject{
    id:style
    objectName: "orbiter_style"
//---------------end do not change---------------------//


//-------------function definitions -------------------//
    /*
      These two functions are responsible for always scaling whatever we ask to a particular size.
      The two variables, appW and appH are always updated with the width and height respectively of the qml orbiterWindow
      Since we know this, we can bind equations to it that will scale thing to %80 of the width or height and so on.
      Its even used for text scaling as shown below.
      */

    function scaleX(x){
        return x/100*appW
    }
    function scaleY(y){
        return y/100*appH
    }
//-------------end function definitions----------------//

    /*
      You Must fill out the below section!!!
      */
    property string skincreator: "Langston Ball (aka golgoj4)"
    property string skinname: "Pluto Classic"
    property string skindir: "default"                     //if different from the skin name. otherwise, the name is fine.
    property string skindescription: "Pluto Classic Skin"
    property string skinversion: "1.0"
    property string skinvariation: "Desktop 800x600"

    //-----------end required----------------------------//


    //begin skin style definitions//

    property color maincolor: "#0D4392"    //note the colors can be web colors, or named SVG colors, or rgb
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
    property int  stdbuttonh: scaleY(13)            //note the use of the scaling function to set properly proportioned buttons.
    property int  stdbuttonw: scaleX(8)
    property int widebuttonw: scaleX(10)
    property int widebuttonh: scaleY(10)
    property int mediabuttonH: scaleY(9)
    property int mediabuttonW: scaleX(5)
    property int avoptionbuttonh: scaleY(10)
    property int avoptionbuttonw: scaleX(6)


    //textStyles
    property string stylebuttonimage: ""
    property int title_size: 20* scaleX(appW)           //note the text is also scaled by this function
    property color title_color: "whitesmoke"
    property int  stdbuttontext: 18 * scaleX(appW)
    property color lighttext: "whitesmoke"
    property color darktext: "slategrey"

    property int t_but_size: 16                        //ot it can be hardcoded
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
    property color arrow_color_nav: accentcolor                 //you can also link properties together so that entire color schemes can change with a few changes
    property color arrow_text_nav: "black"


    //row container styling
    property int rowWidth: orbiterW - 5                         //you can do evaluations in addition to getting function results
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
