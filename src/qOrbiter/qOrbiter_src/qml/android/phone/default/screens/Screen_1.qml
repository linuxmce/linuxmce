import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:stage
    anchors.centerIn: parent
    signal swapStyle()
    width:manager.appWidth
    height:manager.appHeight
    color: "transparent"

    Image {
        id: bg
        source: b_orientation ?  "../img/homescreenP.png" : "../img/homescreenW.png"
        anchors.fill:parent
    }


/*
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
*/
    NowPlayingButton{
        id:fs_npButton
        anchors.top: spaceholder.bottom
        anchors.left: manager.b_orientation ? parent.horizontalCenter : parent.left
        anchors.leftMargin: manager.b_orientation ? (fs_npButton.width / 2) * -1 : scaleX(2)
    }

  /*
    Image {
        id: np_bg
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        height: scaleY(35)
        anchors.right: parent.right
        anchors.top: parent.top
        visible: dcenowplaying.b_mediaPlaying ? true : false
    }
    */

    DroidHomeSelector{
        id:home_selector;
        anchors.top: manager.b_orientation ? fs_npButton.bottom : spaceholder.bottom
        anchors.left: manager.b_orientation ? stage.left : fs_npButton.right
        anchors.leftMargin: manager.b_orientation ? scaleX(15) : scaleX(1)
    }
    ScenarioSelector{
        id:scenarioPopup
        currentModel: ""
        anchors.centerIn: parent
    }

/*
    EffectGaussianBlur{
        id: blur
        anchors.fill: bg
        divider: false
        dividerValue: 1
        opacity: 1
        radius: 0.75
        targetHeight: bg.height
        targetWidth: bg.width
        source: ShaderEffectSource { sourceItem: bg; hideSource: true }
        Component.onCompleted: console.log("blur loaded?")
    }

    EffectBillboard{
        id:billboard

    }
*/

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
            buttontext:manager.sPK_User
            MouseArea{
                anchors.fill: parent
                onClicked:  loadComponent("UserSelector.qml")
            }
        }

        ButtonSq{
            id:roombutton
            height: scaleY(5)
            width: scaleX(10)
            buttontext: roomList.currentEA
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


