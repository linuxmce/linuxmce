import QtQuick 2.2

Item{
    id:powerControl
    property int btWidth: scaleX(40)
    property int btHeight: scaleY(20)
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
        height: btHeight
        spacing:scaleX(10)
        anchors{
            bottom: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        anchors.bottomMargin: scaleY(5)

        StyledButton{
            buttonText: qsTr("Display On")
            height: btHeight
            width: btWidth
            onActivated: manager.toggleDisplay(1)

        }

        StyledButton{
            buttonText: qsTr("Display Off")
            height: btHeight
            width: btWidth
            onActivated: manager.toggleDisplay(0)

        }
    }
    Row{
        anchors{
            top:btnRow.bottom
            left:btnRow.left
            right:btnRow.right
        }
        anchors.topMargin: scaleY(5)
        spacing:scaleX(10)
        height: btnRow.height
        StyledButton{
            buttonText: qsTr("Turn Off MD")
            height: btHeight
            width: btWidth
            //            handler:PowerToggleHandler{mode:0}
            //            useHandler: true
        }
        StyledButton{
            buttonText: qsTr("Turn On MD")
            height: btHeight
            width: btWidth
            //            handler:PowerToggleHandler{mode:0}
            //            useHandler: true
        }
    }
}
