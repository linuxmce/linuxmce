import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs
Item{
    id:optionPanel
    width: parent.width
    height: parent.height
    property int buttonH:optionPanel.height / advancedrow.children.length -10
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .85
    }

    opacity: showOptions ? 1 : 0

    Behavior on opacity{
        PropertyAnimation{
            duration:350
        }
    }

    Flow {
        id:advancedrow
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        spacing:5

        StyledButton{
            id:leader
            height: buttonH
            anchors{
            left:parent.left
            right:parent.right
            leftMargin: 15
            rightMargin: 15
            }

            buttonText.text: qsTr("Power")
            onActivated: {loadComponent("Power.qml");showOptions = !showOptions}
        }

        StyledButton{
            width: leader.width
            buttonText.text:manager.sPK_User
            height: buttonH
            anchors{
            left:parent.left
            right:parent.right
            leftMargin: 15
            rightMargin: 15
            }
            onActivated:  {loadComponent("UserSelector.qml");showOptions = !showOptions}
        }

        StyledButton{
            id:roombutton
            height: buttonH
            anchors{
            left:parent.left
            right:parent.right
            leftMargin: 15
            rightMargin: 15
            }
            buttonText.text:roomList.currentEA
            onActivated: {
                loadComponent("RoomSelector.qml")
                showOptions = !showOptions
            }

        }

        StyledButton{
            height: buttonH
            anchors{
            left:parent.left
            right:parent.right
            leftMargin: 15
            rightMargin: 15
            }
            buttonText.text: qsTr("Exit")
            onActivated:  {closeOrbiter(); showOptions = !showOptions }
        }

    }
}
