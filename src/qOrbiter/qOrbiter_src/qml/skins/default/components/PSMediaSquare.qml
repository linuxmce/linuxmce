import QtQuick 2.2
import "."
import "../components"
Item {
    id: mediaSquare
    height: manager.isProfile ? Style.scaleY(33) : Style.scaleY(50)
    width:manager.isProfile ? Style.scaleX(50) : Style.scaleX(33)
    Rectangle{
        opacity: .25
        anchors.fill: parent
    }

    
    StyledButton{
        id:topLeft
        state:"round"
        height: parent.height*.25
        buttonText: "triangle"
        anchors{
            left:parent.left
            top:parent.top
        }
        onActivated: manager.extraButtonPressed(buttonText)
    }
    
    StyledButton{
        id:topRight
        height: parent.height*.25
        state: "round"
        buttonText: "circle"
        anchors{
            right:parent.right
            top:parent.top
        }
        onActivated:manager.extraButtonPressed(buttonText)
    }
    
    StyledButton{
        id:bottomLeft
        height: 75
        state: "round"
        buttonText: "square"
        anchors{
            left:parent.left
            bottom:parent.bottom
        }
        onActivated:manager.extraButtonPressed(buttonText)
    }
    
    StyledButton{
        id:bottomRight
        height: parent.height*.25
        state: "round"
        buttonText: "x"
        anchors{
            right:parent.right
            bottom:parent.bottom
        }
        onActivated: manager.extraButtonPressed(buttonText)
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
