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

    readonly property string appcolor_background:"grey"
    readonly property string appcolor_foregroundColor:"darkgreen"
    readonly property string appcolor_navigationBackgroundColor:"blue"

    readonly property string appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)


}
