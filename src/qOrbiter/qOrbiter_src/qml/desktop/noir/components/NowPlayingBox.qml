import QtQuick 1.1

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
            id: nowplayingboxtext2


           anchors.bottom: parent.bottom
            wrapMode: "NoWrap"
            text: dcenowplaying.qs_subTitle
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
            font.pixelSize: scaleY(1)
            font.bold: true
        }
    }

    Image {
        id: image1
        x: 125
        y: 101
        width: 100
        height: 100
        source: "image://datagridimg/"+dcenowplaying.nowplayingimage
    }

    Rectangle {
        id: rectangle2
        x: 0
        y: 262
        width: 350
        height: 88
        color: "#ffffff"

        Text {
            id: nowplayingboxtext

           anchors.fill: parent
           anchors.top: parent.top
            wrapMode: "NoWrap"
            text: dcenowplaying.qs_mainTitle
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: scaleY(2)
        }


    }
}
