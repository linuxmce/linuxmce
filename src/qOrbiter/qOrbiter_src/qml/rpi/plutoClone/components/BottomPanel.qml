import QtQuick 2.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: manager.appWidth
    height: skinStyle.widebuttonh + scaleY(4)
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
            width: skinStyle.widebuttonw
            height: skinStyle.widebuttonh
            id: buttonsq1
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
            width: skinStyle.widebuttonw
            height: skinStyle.widebuttonh
            buttontext: "Power"
            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createStageComponent("Power.qml" , stage)
            }
        }


        AdvancedButton {
            id: buttonsq3
            x: 125
            y: 1
            width: skinStyle.widebuttonw
            height: skinStyle.widebuttonh
            buttontext: "Sleeping Menu"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    setCurrentScreen("Screen_29.qml")

                }
            }
        }


        AdvancedButton{
            id:roombutton
            width: skinStyle.widebuttonw
            height: skinStyle.widebuttonh
            buttontext: currentroom
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createRoomSelector()
            }
        }

        UserListComponent {id:user}

        AdvancedButton{
            id:exit
            width: skinStyle.widebuttonw
            height: skinStyle.widebuttonh
            buttontext: "Exit Orbiter"
            MouseArea{
                anchors.fill: exit
                onClicked: manager.closeOrbiter()
            }
        }

    }
}

