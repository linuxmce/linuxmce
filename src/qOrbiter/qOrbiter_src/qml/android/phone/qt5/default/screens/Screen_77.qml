// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
//games? screen. not mame but game systems that are connected like ps3, etc
import QtQuick 2.2
import "../components"
Rectangle {

    // property alias synText:
    id: games_system_remote
    height: manager.appHeight
    width: manager.appWidth
    state: ""
    color: "transparent"

    NowPlayingBox{id:np_box
        anchors.top: games_system_remote.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
    }

    HaControls{id:automation_controls; anchors.top: home.bottom; anchors.left:home.left}

    AudioRemote {
        id: audioremote1
        anchors.bottom: games_system_remote.bottom
    }
    HomeButton{id:home; anchors.leftMargin: 5; anchors.topMargin: 5;anchors.left: games_system_remote.left; anchors.top:games_system_remote.top}





    ButtonSq {
        id: homebutton
        anchors.top: parent.top
        anchors.left: parent.left
        visible: false
        buttontext: "home"
        MouseArea{
            anchors.fill:parent
            onClicked: games_system_remote.state=""
        }
    }

}
