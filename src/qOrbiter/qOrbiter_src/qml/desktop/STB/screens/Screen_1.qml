import QtQuick 1.1
import "../components"
StyledScreen {
    id:screen1


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



}
