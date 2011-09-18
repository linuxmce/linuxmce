import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: style.orbiterW
    height: style.widebuttonh + 10
    color: style.darkhighlight

    Row{
        id:advancedrow
        height:childrenRect.height +5
        width: childrenRect.width
        anchors.centerIn: advanced_panel
        spacing:10
        clip: false

        ButtonSq {
            width: style.widebuttonw
            height: style.widebuttonh
            id: buttonsq1
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
            width: style.widebuttonw
            height: style.widebuttonh
            border.width: 2
            buttontext: "Power"
            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createPowerButton()
            }
        }


        ButtonSq {
            id: buttonsq3
            x: 125
            y: 1
            width: style.widebuttonw
            height: style.widebuttonh
            border.width: 2
            buttontext: "Sleeping Menu"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_29.qml")
            }
        }



        ButtonSq{
            id:roombutton
            width: style.widebuttonw
            height: style.widebuttonh
            buttontext: currentroom
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createRoomSelector()
            }
        }

        UserListComponent {id:user}

        ButtonSq{
            id:exit
            width: style.widebuttonw
            height: style.widebuttonh
            buttontext: "Exit Orbiter"
            MouseArea{
                anchors.fill: exit
                onClicked: closeOrbiter()
            }
        }

    }
}

