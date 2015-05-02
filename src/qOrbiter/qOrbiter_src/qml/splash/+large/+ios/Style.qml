import QtQuick 2.2

Item{
    id:iosSplashStyle

    function scaleX(w){
        return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

      readonly property double dpRatio:screenInfo.primaryScreen.pixelRatio

    /* Transitions */
    readonly property int transition_animationTime:350
    readonly property int transition_accentTime:500
    readonly property int transitionFadeTime:350

    /* Application Colors */
    readonly property color appcolor_background:"#0088ff"
    readonly property color appcolor_background_light:"#80c4ff"
    readonly property color appcolor_background_medium:"#43acff"
    readonly property color appcolor_background_list:"#222222"
    readonly property color appcolor_foregroundColor:"darkgreen"
    readonly property color appcolor_navigationBackgroundColor:"blue"

    /*Application Buttons */
    readonly property string appbutton_confirm_color:appcolor_background
    readonly property color appbutton_cancel_color:"#f26c4f"
    readonly property color appbutton_disabled_color:"#464646"
    readonly property int appButtonHeight:scaleY(4)
    readonly property int appButtonWidth:scaleX(6)

    /* Application Fonts */
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"
    readonly property int appFontSize_list:12*dpRatio
    readonly property int appFontSize_header:18*dpRatio
    readonly property int appFontSize_description:14*dpRatio
    readonly property int appFontSize_title:16*dpRatio

    property int fontSize_small:appFontSize_list
    property int fontSize_medium:appFontSize_description
    property int fontSize_large:appFontSize_title

    property int fontSize_listItem:14*dpRatio
    property int fontSize_listTitle:16*dpRatio
    property int fontSize_listAccent:12*dpRatio

    /* Application Navigation Bars */
    readonly property string appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)
    readonly property string appNavigation_panelHeight:manager.isProfile ? scaleY(9) : scaleY(16)

    /* Application Menu Panels */
    readonly property int appMenuPanel_height:scaleY(90)
    readonly property int appMenuPanel_widthSmall:scaleX(25)
    readonly property int appMenuPanel_widthMedium:scaleX(45)
    readonly property int appMenuPanel_widthLarge:scaleX(85)

    /* Application List Views */
    readonly property int listViewWidth_large:scaleX(55)
    readonly property int listViewWidth_medium:scaleX(20)
    readonly property int listViewWidth_small:scaleX(15)
    readonly property int listViewItemHeight:manager.isProfile ? scaleY(12) : scaleY(15)
    readonly property int listViewMargin:manager.isProfile ? scaleX(5) : scaleX(2.5)

}
