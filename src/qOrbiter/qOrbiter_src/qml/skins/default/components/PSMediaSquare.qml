import QtQuick 2.2
import "."
import "../components"
Item {
    id: mediaSquare

    height: manager.isProfile ? Style.scaleY(20) : Style.scaleY(35)
    width:height
    Rectangle{
        opacity: .25
        anchors.fill: parent
    }

    StyledButton{
        id:topLeft
        state:"round"
        height: parent.height*.25

        buttonText:"\&#8420; "
        txtObj.color: "green"

        txtObj.textFormat: Text.RichText
        fontSize: 24
        anchors{
            left:parent.left
            top:parent.top
        }
        onActivated: manager.extraButtonPressed("triangle")
    }
    
    StyledButton{
        id:topRight
        height: parent.height*.25

        state: "round"
        buttonText: "\&#8413;"
        txtObj.color: "red"
        txtObj.textFormat: Text.RichText
        fontSize: 24
        anchors{
            right:parent.right
            top:parent.top
        }
        onActivated:manager.extraButtonPressed("circle")
    }
    
    StyledButton{
        id:bottomLeft
        height: 75
        state: "round"
        buttonText: "\&#8414;"
        txtObj.color: "pink"
        fontSize: 24

        txtObj.textFormat: Text.RichText
        anchors{
            left:parent.left
            bottom:parent.bottom
        }
        onActivated:manager.extraButtonPressed("square")
    }
    
    StyledButton{
        id:bottomRight
        height: parent.height*.25
        state: "round"
        buttonText: "\&#88;"
        txtObj.color: "skyblue"

        fontSize: 24
        txtObj.textFormat: Text.RichText
        anchors{
            right:parent.right
            bottom:parent.bottom
        }
        onActivated: manager.extraButtonPressed("x")
    }

}
