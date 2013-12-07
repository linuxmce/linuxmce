import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(15)
    height: (buttons.count)*localButtonH
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }

    Rectangle{
        anchors.fill: btnDisplay
        color: "green"
        opacity: .85
    }

    VisualItemModel {
        id:buttons

        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
//            anchors{
//                left:parent.left
//                right:parent.right
//                leftMargin: 15
//                rightMargin: 15
//            }
            useBorder: false
            buttonText: qsTr("Exit")
            onActivated:  {closeOrbiter(); showOptions = !showOptions }
        }

        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
//            anchors{
//                left:parent.left
//                right:parent.right
//                leftMargin: 15
//                rightMargin: 15
//            }
            useBorder: false
            buttonText: qsTr("Sleeping Alarms")
            onActivated:  {manager.gotoQScreen("Screen_29.qml"); showOptions = !showOptions }
        }

    }

    ListView{
        id:btnDisplay
        model:buttons
        height:advancedrow.height

    }



}
