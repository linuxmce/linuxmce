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
            font.pixelSize: scaleY(6)
            font.family: "Droid Sans"
            anchors.bottom: advancedrow.top
            color: "silver"
        }
        Row
        {
            id:advancedrow
            height: childrenRect.height
            width: childrenRect.width
            anchors.horizontalCenter: stage.horizontalCenter
            anchors.bottom: stage.bottom
            spacing: scaleX(5)
            ButtonSq{
                id:roombutton
                height: style.stdbuttonh
                width: style.stdbuttonw
                buttontext: currentroom
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


