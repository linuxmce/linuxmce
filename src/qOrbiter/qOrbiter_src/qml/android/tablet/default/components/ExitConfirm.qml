import QtQuick 1.1

Item {
    width: manager.appWidth
    height: manager.appHeight

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
    }

    Item{
        height: parent.height/2
        width: parent.width /2
        anchors.centerIn: parent
        Rectangle{
            anchors.fill: parent
            color:"darkred"
            opacity: .65
        }
        StyledText{
            id:confirmLabel
            text: qsTr("Confirm Exit?")
            fontSize: listViewText
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row{
            id:btnRow
            height: childrenRect.height
            width: childrenRect.width
            anchors.centerIn: parent
            AdvancedButton{
                id:confirm
                buttontext: qsTr("Exit")
                onBtnPress: manager.closeOrbiter()
            }

            AdvancedButton{
                id:cancel
                buttontext: qsTr("Cancel")
                onBtnPress: componentLoader.source=""
            }

        }

    }
}
