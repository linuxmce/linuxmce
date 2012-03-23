import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: style.orbiterW
    height: style.widebuttonh + scaleY(4)
    color:"transparent"

    Image {
        id: panelbg
        source: "../img/ui3/footer.png"
        height: parent.height
        width: parent.width
        opacity: .85
    }

    Row{
        id:advancedrow
        height:childrenRect.height +5
        width: childrenRect.width
        anchors.centerIn: panelbg
        spacing:10
        clip: false

        AdvancedButton {
            width: style.widebuttonw
            height: style.widebuttonh
            id: buttonsq1
            buttontextfontsize: 9

            buttontext: "Advanced"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_44.qml")
            }
        }

        AdvancedButton {
            id: buttonsq2
            x: 63
            y: 1
            width: style.widebuttonw
            height: style.widebuttonh

            buttontext: "Power"
            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:loadComponent("Power.qml")
            }
        }


        AdvancedButton {
            id: buttonsq3
            x: 125
            y: 1
            width: style.widebuttonw
            height: style.widebuttonh

            buttontext: "Sleeping Menu"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_29.qml")
            }
        }



        AdvancedButton{
            id:roombutton
            width: style.widebuttonw
            height: style.widebuttonh
            buttontext: currentroom
            MouseArea{
                anchors.fill: parent
                onClicked:loadComponent("RoomSelector.qml")
            }
        }

        UserListComponent {id:user}

        AdvancedButton{
            id:exit
            width: style.widebuttonw
            height: style.widebuttonh
            buttontext: qsTr("Exit Orbiter")

            MouseArea{
                anchors.fill: exit
                onClicked: closeOrbiter()
            }
        }

    }
}

