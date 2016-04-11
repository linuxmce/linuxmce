// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:mediaHeader
    width: appW
    height: appH *.06
    color: "transparent"
    Image {
        id: hdrbg
        source: "../img/thin_bar.png"
        anchors.fill: parent
    }

    anchors.top: parent.top

    StyledText {
        id: nowPlayingText
        text: dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
        color: "white"
        font.pixelSize: 16

        anchors.centerIn: mediaHeader
    }

    StyledText {
        id: mediaClock
        text:dceTimecode.qsCurrentTime+ " :: " + dceTimecode.qsTotalTime

        color: "white"
        font.pixelSize: 14
        anchors.right: mediaHeader.right
        anchors.verticalCenter: mediaHeader.verticalCenter
    }
}
