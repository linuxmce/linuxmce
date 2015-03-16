import QtQuick 2.3
pragma Singleton
//this is a qml singleton example to use the style object as a singleton, thus allowing one to override the style based on the selectors
Item{
    id:applicationStyle

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

    /* Application Colors */
    readonly property color appcolor_background:"#0088ff"
    readonly property color appcolor_background_light:"#80c4ff"
    readonly property color appcolor_background_medium:"#43acff"
    readonly property color appcolor_background_list:"#222222"
    readonly property color appcolor_foregroundColor:"darkgreen"
    readonly property color appcolor_navigationBackgroundColor:"blue"

    /*Application Buttons */
    readonly property int appButtonHeight:scaleY(4)
    readonly property int appButtonWidth:scaleX(6)
    readonly property string appbutton_confirm_color:appcolor_background
    readonly property color appbutton_cancel_color:"#f26c4f"
    readonly property color appbutton_disabled_color:"#464646"


    /* Application Fonts */
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"
    readonly property int appFontSize_list:scaleY(2)
    readonly property int appFontSize_header:scaleY(5)
    readonly property int appFontSize_description:scaleY(3)
    readonly property int appFontSize_title:scaleY(4)

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
