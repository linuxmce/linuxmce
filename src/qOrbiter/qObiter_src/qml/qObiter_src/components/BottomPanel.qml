import QtQuick 1.0

Rectangle {



    width: 800
    height: 50
    color: "#9e9d9d"
    id:advanced_panel

    ButtonSq{
        id:exit
        x: 655
        y: 0
        buttontext: "Exit Orbiter"
        buttontextcolor: style.button_system_color
        anchors.left: user.right
        anchors.leftMargin: 331
        height: 50
        width: 50

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


           UserListComponent {id:user ;x: 258;y: 0}


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

        RoomSelector{id: intalledrooms ;x: 425;y: 0 ;height: 50 }



}
