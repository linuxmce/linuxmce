import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: appW
    height: scaleY(15)
    color:"darkgrey"

    /*
    Image {
        id: panelbg
        source: "../img/ui3/footer.png"
        height: parent.height
        width: parent.width
        opacity: .85
    }
*/
    Row{
        id:advancedrow
        height:childrenRect.height +5
        width: childrenRect.width
        anchors.centerIn: advanced_panel
        spacing:scaleX(2)
        clip: false

        AdvancedButton {
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
                    setCurrentScreen("Screen_29.qml")

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

        AdvancedButton {
            id: createuserlist
            buttontext: sPK_User
            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    MyJs.createStageComponent("UserListComponent.qml", stage)
                }
            }
        }


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

