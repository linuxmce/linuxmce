import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Item {
    id:advanced_panel
    width: manager.appWidth
    height: style.widebuttonh + scaleY(4)

    Rectangle{
        anchors.fill: parent
        color: "darkred"
        opacity: .35
        border.color: "white"
        border.width: 2

    }

//    Image {
//        id: panelbg
//        source: "../img/ui3/footer.png"
//        height: parent.height
//        width: parent.width
//        opacity: .85
//    }

    Row{
        id:advancedrow
        height:childrenRect.height +5
        width: childrenRect.width
        anchors.left: advanced_panel.left
        spacing:10
        clip: false

        AdvancedButton {          
            id: buttonsq1
            buttontext: "Advanced"
            buttonsqradius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_44.qml")
            }
        }

        AdvancedButton {
            id: buttonsq2                     
            buttontext: "Power"
            buttonsqradius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:componentLoader.source="Power.qml"
            }
        }


        AdvancedButton {
            id: buttonsq3
            buttontext: "Sleeping Menu"
            buttonsqradius: 5

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    gotoQScreen("Screen_29.qml")
                }
            }
        }


        AdvancedButton{
            id:roombutton           
            buttontext: roomList.currentEA
            buttonsqradius:5
            MouseArea{
                anchors.fill: parent
                onClicked:componentLoader.source="RoomSelector.qml"
            }
        }

        AdvancedButton{
            id:usersButton
            buttontext: manager.sPK_User
            MouseArea{
                anchors.fill: parent
                onClicked:componentLoader.source="UserListComponent.qml"
            }
        }

        AdvancedButton{
            id:exit           
            buttontext: "Exit Orbiter"
            buttonsqradius:5
            MouseArea{
                anchors.fill: exit
                onClicked: componentLoader.source="ExitConfirm.qml"
            }
        }
    }

    Clock{
        id:screen1time
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}

