import QtQuick 1.1
import "../components"
MediaPlaybackTemplate {
    id:screen_fiddy_fo
    height: scaleY(100)
    width: scaleX(100)
    Component.onCompleted: {
        forceActiveFocus()
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }

}
