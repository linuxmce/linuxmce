import QtQuick 2.2
import "."
import "../components"
Item {
    id: mediaSquare

    height: manager.isProfile ? appStyle.scaleY(15) : appStyle.scaleY(25)
    width:height
    Rectangle{
        opacity: .25
        anchors.fill: parent
        radius: height
        color:"black"
    }
    rotation: 45
    property int buttonSize:height*.45

    ImageButton{
        id:topLeft
        state:"round"
        rotation: -45
        height: buttonSize
        upImage: "../images/ps3_tri.png"
        anchors{
            left:parent.left
            top:parent.top
        }
        onActivated: manager.extraButtonPressed("triangle")
    }
    
    ImageButton{
        id:topRight
        height: buttonSize

        state: "round"
        upImage: "../images/ps3_o.png"
        anchors{
            right:parent.right
            top:parent.top
        }
        onActivated:manager.extraButtonPressed("circle")
    }
    
    ImageButton{
        id:bottomLeft
        height: buttonSize
        state: "round"
        upImage: "../images/ps3_sq.png"
        rotation: 45
        anchors{
            left:parent.left
            bottom:parent.bottom
        }
        onActivated:manager.extraButtonPressed("square")
    }


    
    ImageButton{
        id:bottomRight
        height: buttonSize
        state: "round"
        rotation: -45
        upImage:"../images/ps3_x.png"
        anchors{
            right:parent.right
            bottom:parent.bottom
        }
        onActivated: manager.extraButtonPressed("x")
    }

}
