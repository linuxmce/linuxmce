import QtQuick 1.0
import "../components"


    Rectangle {
        id:stage
anchors.centerIn: parent
        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
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
                buttontext: qsTr("Location")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createListComponent("RoomSelector.qml", stage)
                }

            }

            ButtonSq{
                height: 45
                width: 45
                buttontext: qsTr("User")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  MyJs.createListComponent("UserSelector.qml", stage)
                }

            }
        }

    }


