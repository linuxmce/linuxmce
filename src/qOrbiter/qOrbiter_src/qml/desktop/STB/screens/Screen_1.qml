import QtQuick 1.1
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
            manager.gotoQScreen(dcenowplaying.qs_screen)
        }
    }

    Connections{
        target: dcenowplaying
        onScreenTypeChanged:manager.gotoQScreen(dcenowplaying.qs_screen)
    }



    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }

    StyledText{
        anchors.centerIn: parent
        text:"Screen 1"
        font.pixelSize: 32
        color:"white"
    }

}
