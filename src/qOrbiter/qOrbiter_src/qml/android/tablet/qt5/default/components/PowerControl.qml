import QtQuick 2.0
import "../../../../../skins-common/lib/handlers"

GenericContainer{
    id:powerControl
    headerLabel: "Power Control"
    fullscreen: true
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
            label: qsTr("Display On")
            height: 120
            width: 200
            onActivated: manager.toggleDisplay(1)

        }

        StyledButton{
            label: qsTr("Display Off")
            height: 120
            width: 200
            onActivated: manager.toggleDisplay(0)

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
            label: qsTr("Turn Off MD")
            height: 120
            width: 200
            //            handler:PowerToggleHandler{mode:0}
            //            useHandler: true
        }
        StyledButton{
            label: qsTr("Turn On MD")
            height: 120
            width: 200
            //            handler:PowerToggleHandler{mode:0}
            //            useHandler: true
        }
    }
}
