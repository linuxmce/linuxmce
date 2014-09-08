// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
//games? screen. not mame but game systems that are connected like ps3, etc
import QtQuick 1.1
import "../components"
MediaPlaybackTemplate {
    id:games

    StyledText{
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        fontSize: 50
        text:qsTr("Game System")
        color:"white"
    }


}
