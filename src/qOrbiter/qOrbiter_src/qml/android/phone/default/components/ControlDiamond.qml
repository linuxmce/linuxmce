import QtQuick 1.1
import "../components"

Item{
    id:controlDiamond
    height: manager.isProfile ? manager.appHeight *.33 : manager.appHeight *.45
    width:height
    anchors.right: parent.right
    anchors.rightMargin: scaleX(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(5)
    property string imgRoot:"../../../tablet/default/images/"
    
    ControlButton {
        id:play
        buttonLabel: ""
         height: manager.isProfile ? 75 : 65
        anchors.centerIn: parent
        onActivated: manager.pauseMedia()
        Image {
            id: pauseImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? imgRoot+"playpause_dn.png" : imgRoot+"playpause_up.png"
            height: parent.height*.65
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:prev
        height: manager.isProfile ? 75 : 65
        buttonLabel: ""
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("-1")
        Image {
            id: prevImg
            source:  parent.pressed ? imgRoot+"prev_dn.png" : imgRoot+"prev_up.png"
            height: parent.height*.65
            width: height
            fillMode: Image.PreserveAspectFit
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:next
        height: manager.isProfile ? 75 : 65
        buttonLabel: ""
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onActivated: manager.newTrack("+1")
        Image {
            id: nextImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? imgRoot+"next_dn.png" : imgRoot+"next_up.png"
            height: parent.height*.65
            smooth: true
            anchors.centerIn: parent
        }
    }

    ControlButton{
        id:ff
        height: manager.isProfile ? 75 : 65
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        onActivated: manager.setPlaybackSpeed(+2)
        Image {
            id: ffImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? imgRoot+"ff_dn.png" : imgRoot+"ff_up.png"
            height: parent.height*.65
            smooth: true
            anchors.centerIn: parent
        }

    }
    ControlButton{
        id:rw
        height: manager.isProfile ? 75 : 65
        buttonLabel: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        onActivated: manager.setPlaybackSpeed(-2)
        Image {
            id: rwImg
            fillMode: Image.PreserveAspectFit
            source:  parent.pressed ? imgRoot+"rw_dn.png" : imgRoot+"rw_up.png"
            height:parent.height*.65
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
            source:  parent.pressed ? imgRoot+"stop_dn.png" : imgRoot+"stop_up.png"
            height: parent.height*.65
            opacity: parent.opacity
            smooth: true
            anchors.centerIn: parent
        }
    }


}
