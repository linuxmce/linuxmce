import QtQuick 1.0

Rectangle {
    width: np_box.width
    height: np_box.height
    id: now_playing

    visible: dcenowplaying.b_mediaPlaying ? true : false
    color:"transparent"

    Image {
        id: np_box
        source: "../img/nowplayingbox.png"
    }
    Image {
        id: nowplayingimage
        height: style.orbiterh
        fillMode: Image.PreserveAspectCrop
        width: style.orbiterw
        anchors.fill: np_box
        source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

    }
    Text {
        id: np_label
        text: qsTr("NowPlaying")
        font.pixelSize: 12
        anchors.top: np_box.top
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: 12
        anchors.top: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        width: np_box.width - 40
        anchors.horizontalCenter: np_box.horizontalCenter
    }

    MouseArea{
        anchors.fill: parent
        onClicked:screenchange(dcenowplaying.qs_screen)
    }

    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: 12
        anchors.bottom: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: np_box.horizontalCenter
        color: "white"

    }


}
