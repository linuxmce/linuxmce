import QtQuick 1.1
import "../components"

Item{
    id:controlDiamond
    height: manager.appHeight *.45
    width: manager.appHeight *.45
    anchors.right: parent.right
    anchors.rightMargin: scaleX(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(5)
    
    ControlButton {
        id:play
        buttonLabel: qsTr("Play/Pause")
        anchors.centerIn: parent
        onActivated: manager.pauseMedia()
    }

    ControlButton{
        id:prev
        height:75
        buttonLabel: qsTr("Prev")
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("-1")
    }

    ControlButton{
        id:next
        height:75
        buttonLabel: qsTr("Next")
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("+1")
    }

    ControlButton{
        id:ff
        height: 75
        buttonLabel: qsTr("FF")
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        onActivated: manager.setPlaybackSpeed(+2)
    }
    ControlButton{
        id:rw
        height: 75
        buttonLabel: qsTr("RW")
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        onActivated: manager.setPlaybackSpeed(-2)
    }

    ControlButton{
        id:stop
        height: 60
        buttonLabel: qsTr("Stop")
        onActivated: manager.stopMedia()
        anchors{
            right: parent.right
            bottom: parent.bottom
        }
    }


}
