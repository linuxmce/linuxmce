import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../../lib/effects"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: manager.appWidth - scaleX(1)
    height: style.widebuttonh + scaleY(4)
    anchors.horizontalCenter: parent.horizontalCenter
    color:"transparent"
    border.color: "black"
    border.width: 3

    DropShadow{
        id:headerDrop
        sourceItem:panelbg
        height: panelbg.height
        width: panelbg.width
        distance:2
        color:"black"
    }
    Image {
        id: panelbg
        source: manager.imagePath+"ui3/bluish_fill.png"
        height: parent.height
        width: parent.width
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
                onClicked:MyJs.createStageComponent("Power.qml" , stage)
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
                onClicked: {
                    gotoQScreen("Screen_29.qml")

                }
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

       // UserListComponent {id:user}

        AdvancedButton{
            id:exit
            width: style.widebuttonw
            height: style.widebuttonh
            buttontext: "Exit Orbiter"
            MouseArea{
                anchors.fill: exit
                onClicked: manager.closeOrbiter()
            }
        }

    }
}

