import QtQuick 1.1

Item {
    width: manager.appWidth
    height: manager.appHeight

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65

    }
    MouseArea{
        anchors.fill: parent
      onReleased: {}
    }

    Item{
        height: parent.height/2
        width: parent.width /2
        anchors.centerIn: parent
        Rectangle{
            anchors.fill: parent
            color:"darkred"
            opacity: .65
            radius:5
            border.color: "white"
            border.width: 1
        }
        StyledText{
            id:confirmLabel
            text: qsTr("Confirm Exit?")
            fontSize: listViewText
            anchors.top:parent.top
            color:"white"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row{
            id:btnRow
            height: childrenRect.height
            width: childrenRect.width
            anchors.centerIn: parent
            spacing:scaleX(2)
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
