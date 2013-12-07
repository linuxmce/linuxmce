import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs
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
        useBorder: false
        buttonText: qsTr("Sleeping Alarms")
        onActivated: {loadComponent("Power.qml");showOptions = !showOptions}
    }
    
    //        StyledButton{
    //            width: leader.width
    //            buttonText:manager.sPK_User
    //            height: buttonH
    //            anchors{
    //            left:parent.left
    //            right:parent.right
    //            leftMargin: 15
    //            rightMargin: 15
    //            }
    //            onActivated:  {loadComponent("UserSelector.qml");showOptions = !showOptions}
    //        }
    
    //        StyledButton{
    //            id:roombutton
    //            height: buttonH
    //            anchors{
    //            left:parent.left
    //            right:parent.right
    //            leftMargin: 15
    //            rightMargin: 15
    //            }
    //            buttonText:roomList.currentEA
    //            onActivated: {
    //                loadComponent("RoomSelector.qml")
    //                showOptions = !showOptions
    //            }
    
    //        }
    
    StyledButton{
        height: buttonH
        anchors{
            left:parent.left
            right:parent.right
            leftMargin: 15
            rightMargin: 15
        }
        useBorder: false
        buttonText: qsTr("Exit")
        onActivated:  {closeOrbiter(); showOptions = !showOptions }
    }
    
}
