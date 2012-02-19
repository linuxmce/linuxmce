import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


    Rectangle {
        id:stage
        anchors.centerIn: parent
        signal swapStyle()
        height: appH
        width: appW
        color: "transparent"

NowPlayingBox{anchors.horizontalCenter: parent.horizontalCenter}

        Column{
            id:maindisplay
            anchors.centerIn: parent
            height: childrenRect.height
            width: scaleX(100)

            DroidHomeSelector{anchors.horizontalCenter: parent.horizontalCenter}

        }
        Text{
            id:spaceholder
            text:dcemessage
            font.pixelSize: scaleY(3)
            font.family: "Droid Sans"
            anchors.bottom: advancedrow.top
            color: "silver"

        }
        Image {
            id: bottomBg
            source: "../img/lowerbkg.png"
            height: scaleY(10)
            width: scaleX(55)
            anchors.bottom: stage.bottom
            anchors.horizontalCenter: stage.horizontalCenter

        }
        Row
        {
            id:advancedrow
            height: bottomBg.height
            width: bottomBg.width
           anchors.centerIn: bottomBg
            spacing: scaleX(4)
            ButtonSq{
                id:roombutton
                height: style.stdbuttonh
                width: style.stdbuttonw
                buttontext: currentRoom
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createStageComponent("RoomSelector.qml", stage)
                }



            }

            ButtonSq{
                height: style.stdbuttonh
                width: style.stdbuttonw
                buttontext: currentuser
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createStageComponent("UserSelector.qml", stage)
                }

            }

            ButtonSq{
                height: style.stdbuttonh
                width: style.stdbuttonw
                buttontext: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: closeOrbiter()
                }

            }
        }

    }


