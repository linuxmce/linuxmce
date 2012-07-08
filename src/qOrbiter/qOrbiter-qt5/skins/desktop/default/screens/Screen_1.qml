import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


    Rectangle {
        id:stage
        anchors.centerIn: parent
        signal swapStyle()
       anchors.fill: parent
        color: "transparent"



        Column{
            id:maindisplay
            anchors.centerIn: parent
            height: childrenRect.height
            width: scaleX(100)
            NowPlayingBox{anchors.horizontalCenter: parent.horizontalCenter}
            DroidHomeSelector{anchors.horizontalCenter: parent.horizontalCenter}

        }
        Row
        {
            anchors.top: maindisplay.bottom
            spacing: 20
            ButtonSq{
                height: 45
                width: 45
                buttontext: currentroom
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createStageComponent("RoomSelector.qml", stage)
                }

            }

            ButtonSq{
                height: 45
                width: 45
                buttontext: currentuser
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createStageComponent("UserSelector.qml", stage)
                }

            }
        }

    }


