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
    Connections{
        target: dcenowplaying
        onImageChanged:np_bg.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Image {
        id: np_bg
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.fill: parent
        visible: dcenowplaying.b_mediaPlaying ? true : false
        opacity: .25
    }


    Text{
        id:spaceholder
        text:manager.dceResponse
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.top: stage.top
        anchors.topMargin: scaleY(1)
        width: scaleX(99)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "silver"
    }

    NowPlayingButton{
        id:fs_npButton
        anchors.top: spaceholder.bottom
        anchors.left: manager.b_orientation ? parent.horizontalCenter : parent.left
        anchors.leftMargin: manager.b_orientation ? (fs_npButton.width / 2) * -1 : scaleX(2)
    }

    DroidHomeSelector{
        id:home_selector;
        anchors.top: manager.b_orientation ? fs_npButton.bottom : spaceholder.bottom
        anchors.left: manager.b_orientation ? stage.left : fs_npButton.right
        anchors.leftMargin: manager.b_orientation ? scaleX(15) : scaleX(1)
    }



    Image {
        id: bottomBg
        source: "../img/lowerbkg.png"
        height: advancedrow.height + scaleY(4)
        width: advancedrow.width + scaleX(4)
        anchors.bottom: stage.bottom
        anchors.horizontalCenter: stage.horizontalCenter

    }
    Row
    {
        id:advancedrow
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: bottomBg
        spacing: manager.b_orientation ? scaleX(10) : scaleX(12)

        ButtonSq{
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontext: qsTr("Power")
            MouseArea{
                anchors.fill: parent
                onClicked: loadComponent("Power.qml")
            }

        }

        ButtonSq{
            height: scaleY(10)
            width: scaleX(10)
            buttontext: currentuser
            MouseArea{
                anchors.fill: parent
                onClicked:  loadComponent("UserSelector.qml")
            }
        }

        ButtonSq{
            id:roombutton
            height: scaleY(5)
            width: scaleX(10)
            buttontext: currentroom
            MouseArea{
                anchors.fill: parent
                onClicked:  loadComponent("RoomSelector.qml")
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


