import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen {
    id:alarms_screen
    screen:qsTr("Sleeping Alarms")
    keepHeader: true
    focus:true

    Component.onCompleted: {
        forceActiveFocus()
    }

    ListView{
        id:alarmlist
        height: scaleY(70)
        width: scaleX(98)
        model: manager.getDataGridModel("sleepingAlarms", DataGrids.Alarms_In_Room, String(manager.getlocation()))

        orientation: ListView.Vertical
        anchors.centerIn: parent
        spacing:scaleY(2)
        delegate:
            Item{
            anchors.left: parent.left
            anchors.right: parent.right

            height: 100

            Rectangle{
                anchors.fill: parent
                color:/*handler.pressed ? "yellow" :*/ status ? "green" : "red"
                opacity: .75
                radius:5
                border.color: "white"
                border.width: 1
            }

            Column{
                height: parent.height
                width: childrenRect.width
                StyledText {
                    id: alarmname
                    text:name
                    color:"white"
                    fontSize: 25
                }
                StyledText {
                    id: alarmtimeLabel
                    text:qsTr("Alarm Set For: ") + alarmtime
                    color:"white"
                    fontSize: 16
                }
                StyledText {
                    id: daysactive
                    text: qsTr("Active on: ")+ active
                    color:"white"
                    fontSize: 16
                }
            }

            StyledText {
                id: activeStatus
                text: status ? qsTr("Enabled") : "Disabled"
                color:"white"
                fontSize: 25
                anchors.right: parent.right
                anchors.rightMargin: scaleX(2)
                anchors.top: parent.top
            }

            StyledText {
                id: countdown
                text: qsTr("Time Left:\n ") +remaining
                color:"white"
                fontSize: 16
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    alarmlist.model.setData(index, "status", !status)

                }
            }


        }

    }
}
