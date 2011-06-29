import QtQuick 1.0

Rectangle {



    width: 800
    height: 50
    color: "#9e9d9d"
    id:advanced_panel

    ButtonSq{
        id:exit
        x: 732
        y: 0
        buttontext: "Exit Orbiter"
        buttontextcolor: style.button_system_color
        anchors.left: user.right
        anchors.leftMargin: 307
        height: 50
        width: 50

        MouseArea{
            anchors.fill: parent
            onClicked: close()
                }
             }

        Rectangle{
            id:user
            anchors.centerIn: parent
            height: 50
            width: 50
            color: "slategrey"

            Image {
                id: bg
                source: "../../../img/icons/personal.png"
                opacity: 5
                height:50
                smooth: false
                width: 50
                anchors.centerIn: parent

            }
            Text {
                id: user_text
                text: currentuser
                font.bold: true
                font.pointSize: 12
                color:"White"
                anchors.centerIn: parent

            }

        }

        ButtonSq {
            id: buttonsq1
            x: 1
            y: 1
            width: 60
            height: 50
            buttontextfontsize: 9
            border.width: 2
            buttontext: "Advanced"
        }

        ButtonSq {
            id: buttonsq2
            x: 63
            y: 1
            width: 60
            height: 50
            border.width: 2
            buttontext: "Power"
        }

        ButtonSq {
            id: buttonsq3
            x: 125
            y: 1
            width: 60
            height: 50
            border.width: 2
            buttontext: "Sleeping Menu"
        }

        ButtonSq {
            id: buttonsq4
            x: 425
            y: 0
            width: 60
            height: 50
            border.width: 2
            buttontextitalic: true
            buttontext: "room"
        }



}
