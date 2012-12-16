import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: manager.appWidth
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
            buttontext: "Power"
            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createStageComponent("Power.qml" , stage)
            }
        }


        AdvancedButton {
            id: buttonsq3
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
            buttontext: currentroom
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createRoomSelector()
            }
        }

        UserListComponent {id:user}

        AdvancedButton{
            id:exit           
            buttontext: "Exit Orbiter"
            MouseArea{
                anchors.fill: exit
                onClicked: manager.closeOrbiter()
            }
        }

    }
}

