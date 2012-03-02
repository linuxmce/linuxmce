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

    Text{
        id:spaceholder
        text:manager.dceResponse
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.top: stage.top
        anchors.topMargin: scaleY(1)
        width: scaleX(75)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "silver"

    }
    Column{
        id:maindisplay
        anchors.top: spaceholder.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: childrenRect.height
        width: scaleX(90)
        spacing: scaleY(1)
        NowPlayingButton{ width: parent.width}
        DroidHomeSelector{id:home_selector; width: parent.width}

    }

    Image {
        id: bottomBg
        source: "../img/lowerbkg.png"
        height: advancedrow.height + scaleY(1)
        width: advancedrow.width + scaleX(1)
        anchors.bottom: stage.bottom
        anchors.horizontalCenter: stage.horizontalCenter

    }
    Row
    {
        id:advancedrow
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: bottomBg
        spacing: scaleX(4)

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
            id:roombutton
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontext: currentroom
            Image {
                id: centerbottom
                source: "../img/grid.png"
                anchors.fill: roombutton
            }
            MouseArea{
                anchors.fill: parent
                onClicked:  MyJs.createStageComponent("RoomSelector.qml", stage)
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


