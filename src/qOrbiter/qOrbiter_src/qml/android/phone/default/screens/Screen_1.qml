import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:stage
    anchors.centerIn: parent
    signal swapStyle()
    width: appW
    height: appH
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
            height: scaleX(11)
            width: scaleX(11)
            buttontext: qsTr("Location")
            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createStageComponent("RoomSelector.qml", stage)
            }

        }

        ButtonSq{
            height: scaleX(11)
            width: scaleX(11)
            buttontext: currentuser
            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createStageComponent("UserSelector.qml", stage)
            }

        }
    }

}


