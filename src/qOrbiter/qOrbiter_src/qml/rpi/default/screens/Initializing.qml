import QtQuick 2.0

Rectangle {
    width: 640
    height: 480

    StyledText {
        id: text1
        x: 197
        y: 93
        width: 246
        height: 39
        text: qsTr("LinuxMCE is Initializing")
        font.family: "Pothana2000"
        font.bold: false
        font.pixelSize: 22
    }

    StyledText {
        id: text2
        x: 103
        y: 183
        text: qsTr("DCE Connected")
        font.pixelSize: 12
    }

    StyledText {
        id: text3
        x: 103
        y: 213
        text: qsTr("Skin Data")
        font.pixelSize: 12
    }

    StyledText {
        id: text4
        x: 103
        y: 243
        text: qsTr("Orbiter Data")
        font.pixelSize: 12
    }
}
