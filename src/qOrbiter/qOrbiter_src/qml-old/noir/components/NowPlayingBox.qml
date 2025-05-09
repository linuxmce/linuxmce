import QtQuick 2.0

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

        StyledText {
            id: nowplayingboxtext2


           anchors.bottom: parent.bottom
            wrapMode: "NoWrap"
            text: dcenowplaying.qs_subTitle
            font.bold: true
            smooth: true
            horizontalAlignment: StyledText.AlignHCenter
            font.pointSize: 12
        }

        StyledText {
            id: text1
            x: 255
            y: 12
            text: dcenowplaying.qs_playbackSpeed
            font.pointSize: scaleY(1)
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

        StyledText {
            id: nowplayingboxtext

           anchors.fill: parent
           anchors.top: parent.top
            wrapMode: "NoWrap"
            text: dcenowplaying.qs_mainTitle
            font.bold: true
            smooth: true
            horizontalAlignment: StyledText.AlignHCenter
            font.pointSize: scaleY(2)
        }


    }
}
