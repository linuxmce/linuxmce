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

    readonly property string appcolor_background:"#0088ff"
    readonly property string appcolor_background_light:"#80c4ff"
    readonly property string appcolor_background_medium:"#43acff"
    readonly property string appcolor_background_list:"#222222"
    readonly property string appcolor_foregroundColor:"darkgreen"
    readonly property string appcolor_navigationBackgroundColor:"blue"

    readonly property string appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)
    readonly property string appbutton_confirm_color:appcolor_background
    readonly property string appbutton_cancel_color:"#f26c4f"
    readonly property string appbutton_disabled_color:"#464646"
    readonly property string apptext_color_active:"white"
    readonly property string apptext_color_inactive:"grey"

}
