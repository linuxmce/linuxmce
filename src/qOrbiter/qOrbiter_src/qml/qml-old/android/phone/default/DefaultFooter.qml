import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs
import DceScreenSaver 1.0
import AudioVisual 1.0
//phone - default
Row{
    id:ftr
    height: manager.isProfile ? scaleY(8) : scaleY(12)
    anchors{
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .75
    }
    StyledButton{
        buttonText: manager.sPK_User
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
        }
        onActivated: {loadComponent("UserSelector.qml");}
        
    }
    
    Clock{
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}
