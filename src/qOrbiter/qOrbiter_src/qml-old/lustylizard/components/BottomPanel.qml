import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:advanced_panel
    width: appW
    height: style.widebuttonh
    color: "#000000"
    opacity: 0.800
    transformOrigin: Item.Bottom
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 0

    Rectangle {
        id:currplaybg
        width: scaleX(40)
        color: "#000000"
        opacity: 0
        transformOrigin: Item.Bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        height: parent.height
    }

    ButtonSq {
        id: now_playing
        anchors.top: parent.top
        visible: dcenowplaying.b_mediaPlaying ? true : false
        width: currplaybg.width
        height: parent.height
        buttontext: dcenowplaying.qs_mainTitle +" " + dceTimecode.qsCurrentTime
        anchors.leftMargin: scaleX(2)
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter


        MouseArea{
            anchors.fill: parent
            onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
            onEntered: {
                currplaybg.opacity = 0.8
            }

            onExited: {
                currplaybg.opacity = 0
            }
        }
    }

    ButtonNotify {
        id: notification
        height: parent.height - scaleY(1.4)
        width: parent.height
        text: "i"
        anchors.right: parent.right
        anchors.rightMargin: scaleX(2)
        anchors.verticalCenter: parent.verticalCenter
    }

        /*
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
*/

/*        UserListComponent {id:user}

        AdvancedButton{
            id:exit           
            buttontext: "Exit Orbiter"
            MouseArea{
                anchors.fill: exit
                onClicked: manager.closeOrbiter()
            }
        }
*/
}

