import QtQuick 2.2

import org.linuxmce.enums 1.0
import "../."
Item {
    id: mediaSquare
    height: manager.isProfile ? appStyle.scaleY(33) : appStyle.scaleY(50)
    width:manager.isProfile ? appStyle.scaleX(50) : appStyle.scaleX(33)

    anchors{
        verticalCenter: parent.verticalCenter
        left:parent.left
    }

    ImageButton {
        id: up
        height: 100
        width: 100
        upImage: "../images/arrow.png"
        downImage: "../images/arrow.png"
        onActivated: manager.moveDirection(1)
        anchors.horizontalCenter: centerBtntn.horizontalCenter
        anchors.bottom: centerBtntn.top
        imgRotation: 180
    }

    ImageButton {
        id: dn
        height: 100
        width: 100
        upImage: "../images/arrow.png"
        downImage: "../images/arrow.png"
        anchors.horizontalCenter: centerBtntn.horizontalCenter
        anchors.top: centerBtntn.bottom
        onActivated: manager.moveDirection(2)
    }
    ImageButton {
        id: left
        height: 100
        width: 100
        upImage: "../images/arrow.png"
        downImage: "../images/arrow.png"
        anchors.verticalCenter: centerBtntn.verticalCenter
        anchors.right: centerBtntn.left
        onActivated: manager.moveDirection(3)
        imgRotation: 90
    }

    ImageButton {
        id: right
        height: 100
        width: 100
        upImage: "../images/arrow.png"
        downImage: "../images/arrow.png"
        anchors.verticalCenter: centerBtntn.verticalCenter
        anchors.left: centerBtntn.right
        onActivated: manager.moveDirection(4)
        imgRotation: 270
    }

    StyledButton{
        id:centerBtntn
        height: parent.height*.40
        state: "round"
        buttonText: "Enter"
        anchors.centerIn: parent
        onActivated: manager.moveDirection(5)
    }
}
