import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item{
    id:arrows
    visible:false
    height: manager.isProfile ? Style.scaleY(27) :Style.scaleY(50)
    width: height
    Rectangle{
        id:centerOk

        anchors.centerIn: parent
        height: parent.height/3
        width: height
        radius: height
        color:ok_ms.pressed? Style.appcolor_background_medium : Style.appbutton_confirm_color
        MouseArea{
            id:ok_ms
            anchors.fill: parent
            onClicked: manager.moveDirection(5)
        }
    }
    

    DirectionButton{
       id:left
       rotation: 90
       width: centerOk.height
       onActivated: manager.moveDirection(3)
       anchors{
           verticalCenter: parent.verticalCenter
           left:parent.left
       }
    }

    DirectionButton{
        id:top
        width: centerOk.height
        anchors{
            horizontalCenter: parent.horizontalCenter
            top:parent.top
        }
        rotation:180
        onActivated: manager.moveDirection(1)
    }

    DirectionButton{
        id:right
        width: centerOk.height
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
        rotation:270
        onActivated: manager.moveDirection(4)
    }

    DirectionButton{
        id:down
        width: centerOk.height
        anchors{
            bottom:parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        onActivated: manager.moveDirection(2)
    }


}
