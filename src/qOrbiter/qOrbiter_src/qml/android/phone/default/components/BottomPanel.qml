import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: 320
    height: 50
    color:"transparent"

    Image {
        id: panelbg
        source: "../images/footer.png"
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
                onClicked:setCurrentScreen("Screen_44.qml")
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
                onClicked:MyJs.createPowerButton()
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
                onClicked:setCurrentScreen("Screen_29.qml")
            }
        }



        AdvancedButton{
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

        AdvancedButton{
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

