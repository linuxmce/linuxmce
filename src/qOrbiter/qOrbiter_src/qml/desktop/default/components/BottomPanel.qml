import QtQuick 1.1


import "../js/ComponentLoader.js" as MyJs


Item {
    id:advanced_panel
    width: manager.appWidth - scaleX(1)
    height: skinStyle.widebuttonh + scaleY(4)  


    Image {
        id: panelbg
        source: manager.imagePath+"ui3/bluish_fill.png"
        height: parent.height
        width: parent.width
        opacity: .25
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
            buttontext: roomList.currentRoom+"\n"+roomList.currentEA
            MouseArea{
                anchors.fill: parent
                onClicked:componentLoader.source ="../components/RoomSelector.qml"
            }
        }

       // UserListComponent {id:user}

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

