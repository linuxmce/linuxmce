import QtQuick 1.0
import Qt.labs.shaders 1.0


import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle {
    id: gradientheader
    width:parent.width
    height: childrenRect.height
    color:"transparent"
    anchors.top: parent.top

    Image {
        id: headerimage
        source: "../img/icons/header.png"
        height:parent.height
        width:parent.width
        opacity: .75
    }
    
    Text {
        id: headertext
        height:scaleY(2)
        text:qsTr("Speed: ") + dceTimecode.playbackSpeed
        font.family: "Droid Sans"
        font.pixelSize: scaleY(2)
        color: "aliceblue"
    }
    
    Text {
        id: timecode
        height:scaleY(2)
        text: dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
        font.family: "Droid Sans"
        font.pixelSize: scaleY(1) *2.15
        anchors.bottom:parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
        color:"aliceblue"
    }
}
