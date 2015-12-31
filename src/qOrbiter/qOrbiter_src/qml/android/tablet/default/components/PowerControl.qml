import QtQuick 1.1
import "../../../../skins-common/lib/handlers"


Item{
    id:powerControl
    anchors{
        top:parent.top
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }
    
    StyledText{
        anchors.bottom: btnRow.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Power Control")
        fontSize: scaleY(9)
    }

    Row{
        id:btnRow
        height:140
        spacing:scaleX(10)
        width: scaleX(50)
        anchors{
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        anchors.rightMargin: 50
        anchors.leftMargin: 50

        StyledButton{
            buttonText: qsTr("Display On")
            height: 120
            width: 200
            handler:PowerToggleHandler{mode:1}
            useHandler: true
        }

        StyledButton{
            buttonText: qsTr("Display Off")
            height: 120
            width: 200
            handler:PowerToggleHandler{mode:0}
            useHandler: true
        }
    }
    Row{
        anchors{
            top:btnRow.bottom
            left:btnRow.left
            right:btnRow.right
        }
        spacing:scaleX(10)
        height: 140
        StyledButton{
            buttonText: qsTr("Turn Off MD")
            height: 120
            width: 200
//            handler:PowerToggleHandler{mode:0}
//            useHandler: true
        }
        StyledButton{
            buttonText: qsTr("Turn On MD")
            height: 120
            width: 200
//            handler:PowerToggleHandler{mode:0}
//            useHandler: true
        }
    }
}
