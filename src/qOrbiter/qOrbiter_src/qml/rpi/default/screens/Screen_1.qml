import QtQuick 2.2
import "../components"
Item {
    id:screen1
    height: manager.appHeight
    width: manager.appWidth

    Component.onCompleted: {
        manager.setBoundStatus(true)
        if(dcenowplaying.b_mediaPlaying )
        {
            console.log("going to screen "+ dcenowplaying.qs_screen)
            manager.setCurrentScreen(dcenowplaying.qs_screen)
        }
    }

    Connections{
        target: dcenowplaying
        onScreenTypeChanged:manager.setCurrentScreen(dcenowplaying.qs_screen)
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }

}
