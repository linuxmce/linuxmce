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

    readonly property int transition_animationTime:350
    readonly property int transition_accentTime:500
    readonly property int transitionFadeTime:350

    readonly property color appcolor_background:"#0088ff"
    readonly property color appcolor_background_light:"#80c4ff"
    readonly property color appcolor_background_medium:"#43acff"
    readonly property color appcolor_background_list:"#222222"
    readonly property color appcolor_foregroundColor:"darkgreen"
    readonly property color appcolor_navigationBackgroundColor:"blue"

    readonly property string appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)
    readonly property string appbutton_confirm_color:appcolor_background
    readonly property color appbutton_cancel_color:"#f26c4f"
    readonly property color appbutton_disabled_color:"#464646"
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"

    readonly property int appFontSize_list:manager.isProfile ? scaleY(1.5) : scaleY(2)
    readonly property int appFontSize_header:manager.isProfile ? scaleY(4) : scaleY(4.5)
    readonly property int appFontSize_description:manager.isProfile ? scaleY(2) : scaleY(2.5)
    readonly property int appFontSize_title:manager.isProfile ? scaleY(2.5) : scaleY(3)

    readonly property int listViewWidth_large:scaleX(45)
    readonly property int listViewWidth_medium:scaleX(20)
    readonly property int listViewWidth_small:scaleX(15)

    readonly property int appButtonHeight:manager.isProfile ? scaleY(4) : scaleY(6)
    readonly property int appButtonWidth:manager.isProfile ? scaleX(6) : scaleX(4)

}
