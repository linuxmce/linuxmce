import QtQuick 2.2
import "../components"
Item{
    id:optionPanel
    width: showOptions ? parent.width : 0
    height: showOptions ? parent.height : 0
    opacity: showOptions ? 1 : 0
    onWidthChanged: console.log("show iptions")
    Rectangle{
        anchors.fill: parent
        color: "black"
    }



    Behavior on opacity{
        PropertyAnimation{
            duration:350
        }
    }

    Column {
        id:advancedrow
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: parent
        spacing: manager.isProfile ? scaleX(10) : scaleX(12)

        ButtonSq{
            id:leader
            height: style.stdbuttonh
            width: optionPanel.width*.85
            buttontext: qsTr("Power")
            buttontextcolor: "White"
            buttontextfontsize: scaleY(4)
            imgSource: ""
            color:"orange"
            activatedColor:androidSystem.blueStandard
            onActivated: {loadComponent("Power.qml");showOptions = !showOptions}
        }

        ButtonSq{
            height: scaleY(10)
            width: leader.width
            buttontext:manager.sPK_User
            buttontextcolor: "White"
            buttontextfontsize: scaleY(4)
            imgSource: ""
            color:"orange"
            onActivated:  {loadComponent("UserSelector.qml");showOptions = !showOptions}
        }

        ButtonSq{
            id:roombutton
            height: style.stdbuttonh
            width: leader.width
            imgSource: ""
            color:"orange"
            buttontextcolor: "White"
            buttontextfontsize: scaleY(4)
            buttontext:roomList.currentEA
            onActivated: {
                loadComponent("RoomSelector.qml")
                showOptions = !showOptions
            }

        }

        ButtonSq{
            height: style.stdbuttonh
            width: leader.width
            imgSource: ""
            color:"orange"
            buttontextcolor: "White"
            buttontextfontsize: scaleY(4)
            buttontext: qsTr("Exit")
            onActivated:  {closeOrbiter(); showOptions = !showOptions }
        }
    }
}
