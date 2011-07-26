import QtQuick 1.0

Rectangle {


    width: 800
    height: 75
    color: "#9e9d9d"
    id:advanced_panel

    Row{
        id:advancedrow
        height:childrenRect.height
        anchors.centerIn: parent
        width: parent.width
        spacing:10
        clip: false

        ButtonSq {
            id: buttonsq1
            x: 1
            y: 1
            width: 60
            height: 60
            buttontextfontsize: 9
            border.width: 2
            buttontext: "Advanced"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_44.qml")
            }
        }

        ButtonSq {
            id: buttonsq2
            x: 63
            y: 1
            width: 60
            height: 60
            border.width: 2
            buttontext: "Power"
            radius: 5
        }

        ButtonSq {
            id: buttonsq3
            x: 125
            y: 1
            width: 60
            height: 60
            border.width: 2
            buttontext: "Sleeping Menu"
            radius: 5
        }



    RoomSelector{id: installedrooms }
    UserListComponent {id:user}

    ButtonSq{
        id:exit
        buttontext: "Exit Orbiter"
        buttontextcolor: style.button_system_color

        height: 60
        width: 60

        MouseArea{
            x: 0
            y: 0
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 0
            anchors.fill: exit
            onClicked: close()
                }
             }

    }
}
