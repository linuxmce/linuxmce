// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:np_box
    width: scaleX(80)
    height: scaleY(35)
    color: "transparent"
    border.color: "silver"



    Text {
        id: np_label
        text: qsTr("NowPlaying")
        font.pixelSize: scaleY(3)
        anchors.top: np_box.top
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: scaleY(3)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "silver"
        width: np_box.width
        anchors.left: np_label.right
    }


    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: 12
        anchors.bottom: np_box.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked:manager.gotoQScreen(dcenowplaying.qs_screen)
    }
}
