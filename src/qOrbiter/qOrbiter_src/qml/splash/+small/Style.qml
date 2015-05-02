import QtQuick 2.2
pragma Singleton
//this is a qml singleton example to use the style object as a singleton, thus allowing one to override the style based on the selectors
Item{
    id:smallSplashStyle
    property string description:"Small UI Style Definition for small devices."

    function scaleX(w){
         return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

    /* Transitions */
    readonly property int transition_animationTime:350
    readonly property int transition_accentTime:500
    readonly property int transitionFadeTime:350

    /*Application colors */
    readonly property color appcolor_background:"#0088ff"
    readonly property color appcolor_background_light: "#80c4ff"
    readonly property color appcolor_background_medium:"#43acff"
    readonly property color appcolor_background_list:"#222222"
    readonly property color appcolor_foregroundColor:"darkgreen"
    readonly property color appcolor_navigationBackgroundColor:"blue"

    /* Application Buttons */
    readonly property int appButtonHeight:manager.isProfile ? scaleY(4) : scaleY(6)
    readonly property int appButtonWidth:manager.isProfile ? scaleX(6) : scaleX(4)
     readonly property string appbutton_confirm_color:appcolor_background
    readonly property color appbutton_cancel_color:"#f26c4f"
    readonly property color appbutton_disabled_color:"#464646"


    /* Application fonts */
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"
    readonly property int appFontSize_list:14*screenInfo.primaryScreen.pixelRatio
    readonly property int appFontSize_header:20*screenInfo.primaryScreen.pixelRatio
    readonly property int appFontSize_description:16*screenInfo.primaryScreen.pixelRatio
    readonly property int appFontSize_title:22*screenInfo.primaryScreen.pixelRatio

    property int fontSize_small:14*screenInfo.primaryScreen.pixelRatio
    property int fontSize_medium:16*screenInfo.primaryScreen.pixelRatio
    property int fontSize_large:22*screenInfo.primaryScreen.pixelRatio

    property int fontSize_listItem:14*screenInfo.primaryScreen.pixelRatio
    property int fontSize_listTitle:20*screenInfo.primaryScreen.pixelRatio
    property int fontSize_listAccent:12*screenInfo.primaryScreen.pixelRatio


    /* Application Navigation bars */
    readonly property int appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)
    readonly property int appNavigation_panelHeight:manager.isProfile ? scaleY(9) : scaleY(16)

    /* Application Menu Panels */
    readonly property int appPanelHeight:manager.isProfile ? scaleY(12) :scaleY(14)
    readonly property int appMenuPanel_height:scaleY(90)
    readonly property int appMenuPanel_widthSmall:scaleX(15)
    readonly property int appMenuPanel_widthMedium:scaleX(45)
    readonly property int appMenuPanel_widthLarge:scaleX(85)

    /* Application list views */
    readonly property int listViewWidth_large:scaleX(45)
    readonly property int listViewWidth_medium:scaleX(20)
    readonly property int listViewWidth_small:scaleX(15)
    readonly property int listViewItemHeight:manager.isProfile ? scaleY(10) : scaleY(12)
    readonly property int listViewMargin:manager.isProfile ? scaleX(5) : scaleX(1.5)
}
