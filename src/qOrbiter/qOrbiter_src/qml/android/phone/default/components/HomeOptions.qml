import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs


Row {
    id:advancedrow
    height: childrenRect.height
    width: childrenRect.width
    anchors.verticalCenter: parent.verticalCenter
    spacing: manager.b_orientation ? scaleX(10) : scaleX(12)
    
    ButtonSq{
        height: style.stdbuttonh
        width: style.stdbuttonw
        buttontext: qsTr("Power")
        imgSource: ""
        color:"black"
        onActivated: loadComponent("Power.qml")
    }
    
    ButtonSq{
        height: scaleY(10)
        width: scaleX(10)
        buttontext:manager.sPK_User
        imgSource: ""
        color:"black"
        onActivated:  loadComponent("UserSelector.qml")
    }
    
    ButtonSq{
        id:roombutton
        height: style.stdbuttonh
        width: style.stdbuttonw * 2
        imgSource: ""
        color:"black"
        buttontext:roomList.currentEA
        onActivated: loadComponent("RoomSelector.qml")
        
    }
    
    ButtonSq{
        height: style.stdbuttonh
        width: style.stdbuttonw
        imgSource: ""
        color:"black"
        buttontext: qsTr("Exit")
        MouseArea{
            anchors.fill: parent
            onClicked: closeOrbiter()
        }
    }
    
}
