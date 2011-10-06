import QtQuick 1.0

Rectangle {
    width: 350
    height: 350
    color: style.advanced_bg

    Rectangle {
        id: rectangle1
        x: 0
        y: 0
        width: 350
        height: 38
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#ffffff"
            }

            GradientStop {
                position: 1
                color: "#3878a0"
            }
        }

        Text {
            id: nowplayingboxtext
            x: 8
            y: 5
            width: 50
            height: 20
            wrapMode: "NoWrap"
            text: dcenowplaying.qs_mainTitle
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Text {
            id: text1
            x: 255
            y: 12
            text: dcenowplaying.qs_playbackSpeed
            font.pixelSize: 12
        }
    }

    Image {
        id: image1
        x: 125
        y: 101
        width: 100
        height: 100
        source: ""
    }

    Rectangle {
        id: rectangle2
        x: 0
        y: 262
        width: 350
        height: 88
        color: "#ffffff"
    }
}
