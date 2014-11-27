import QtQuick 1.1
import "../components"

Item{
    id:controlDiamond
    height:manager.isProfile ? parent.height *.20 : manager.appHeight *.40
    width: height
    anchors.right: parent.right
    anchors.rightMargin: scaleX(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(2)
   property int outsideButtonHeight: play.height*.65


    ControlButton{
        id:prev
       height: outsideButtonHeight
        buttonLabel: ""
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("-1")
        Image {
            id: prevImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/prev_dn.png" : "../images/prev_up.png"
            anchors.fill: parent
            anchors.margins:5
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:next
        height: outsideButtonHeight
        buttonLabel: ""
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("+1")
        Image {
            id: nextImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/next_dn.png" : "../images/next_up.png"
            anchors.fill: parent
            anchors.margins:5
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:ff
        height: outsideButtonHeight
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        onActivated: manager.setPlaybackSpeed(+2)
        Image {
            id: ffImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/ff_dn.png" : "../images/ff_up.png"
            anchors.fill: parent
            anchors.margins:5
            smooth: true
            anchors.centerIn: parent
        }

    }
    ControlButton{
        id:rw
        height:outsideButtonHeight
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        onActivated: manager.setPlaybackSpeed(-2)
        Image {
            id: rwImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/rw_dn.png" : "../images/rw_up.png"
            anchors.fill: parent
            anchors.margins:5
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:stop
        height: play.height/2
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
            anchors.fill: parent
            anchors.margins:10
            opacity: parent.opacity
            smooth: true
            anchors.centerIn: parent
        }
    }
    ControlButton {
        id:play
        height:manager.isProfile ? parent.height *.45: parent.height*.35
        buttonLabel: ""
        anchors.centerIn: parent
        onActivated: manager.pauseMedia()
        Image {
            id: pauseImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? "../images/playpause_dn.png" : "../images/playpause_up.png"
            height: parent.height*.75
            width: height
            smooth: true
            anchors.centerIn: parent
        }
    }

}
