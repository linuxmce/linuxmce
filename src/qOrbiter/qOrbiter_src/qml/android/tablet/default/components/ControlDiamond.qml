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
        buttonLabel: ""
        anchors.centerIn: parent
        onActivated: manager.pauseMedia()
        Image {
            id: pauseImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/playpause_dn.png" : "../images/playpause_up.png"
            height: 75
            width: 75
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:prev
        height:75
        buttonLabel: ""
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("-1")
        Image {
            id: prevImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/prev_dn.png" : "../images/prev_up.png"
            height: 50
            width: 50
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:next
        height:75
        buttonLabel: ""
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("+1")
        Image {
            id: nextImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/next_dn.png" : "../images/next_up.png"
            height: 50
            width: 50
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:ff
        height: 75
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        onActivated: manager.setPlaybackSpeed(+2)
        Image {
            id: ffImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/ff_dn.png" : "../images/ff_up.png"
            height: 50
            width: 50
            smooth: true
            anchors.centerIn: parent
        }

    }
    ControlButton{
        id:rw
        height: 75
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        onActivated: manager.setPlaybackSpeed(-2)
        Image {
            id: rwImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/rw_dn.png" : "../images/rw_up.png"
            height: 50
            width: 50
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:stop
        height: 60
        buttonLabel: ""
        onActivated: manager.stopMedia()
        anchors{
            right: parent.right
            bottom: parent.bottom
        }
        Image {
            id: stopImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/stop_dn.png" : "../images/stop_up.png"
            height: 35
            width: 35
            opacity: parent.opacity
            smooth: true
            anchors.centerIn: parent
        }
    }


}
