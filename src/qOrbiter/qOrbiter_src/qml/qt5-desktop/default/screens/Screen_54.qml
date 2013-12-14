import QtQuick 2.0
import "../components"
MediaPlaybackTemplate {
    id:screen_fiddy_fo

    Component.onCompleted: {
        forceActiveFocus()
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }

}
