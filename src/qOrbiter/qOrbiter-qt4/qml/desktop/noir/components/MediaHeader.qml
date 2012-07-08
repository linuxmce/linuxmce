// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:mediaHeader
    width: appW
    height: appH *.06
    //color: "transparent"
    gradient:style.generalGradient

    anchors.top: parent.top

    Text {
        id: nowPlayingText
        text: dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
        color: "white"
        font.pixelSize: 16
        font.family: "Nimbus Sans L"
        anchors.centerIn: mediaHeader
    }

    Text {
        id: mediaClock
        text:dceTimecode.qsCurrentTime+ " :: " + dceTimecode.qsTotalTime
        font.family: "Nimbus Sans L"
        color: "white"
        font.pixelSize: 14
        anchors.right: mediaHeader.right
        anchors.verticalCenter: mediaHeader.verticalCenter
    }
}
