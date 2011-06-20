import QtQuick 1.0

Rectangle {
    property alias rowlabel: rowheader.buttontext

    radius: 1
    border.color: "#000000"
    height: 80
    width: parent.width
    color: "orange"
    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5


        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Media"

            }

        ButtonSq {
            id: now_playing
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            }
        ButtonSq {
            id: mythTV
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "MythTV"


            }

        ButtonSq {
            id: videos
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Videos"


            }
        ButtonSq {
            id: audio
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Audio"

            }
    }
}
