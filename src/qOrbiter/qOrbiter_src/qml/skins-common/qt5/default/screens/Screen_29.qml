import QtQuick 2.2
import "../components"

Item {
    id:alarms_screen
    height: pageLoader.height
    width:manager.appWidth
    focus:true
    Component.onCompleted: {
        forceActiveFocus()
    }

    StyledText{
        id:alarms_label
        text:qsTr("Sleeping Alarms")
        fontSize: 32
    }

    ListView{
        id:alarmlist
        height: scaleY(70)
        width: scaleX(75)
        model: manager.getDataGridModel("sleepingAlarms", 29, String(manager.getlocation()))

        orientation: ListView.Vertical
        anchors.centerIn: parent
        spacing:scaleY(5)
        delegate:
            Item{
            height: scaleY(25)
            width: scaleX(75)

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
                    fontSize: 42
                }
                StyledText {
                    id: alarmtimeLabel
                    text:qsTr("Alarm Set For: ") + alarmtime
                    color:"white"
                    fontSize: 32
                }
                StyledText {
                    id: daysactive
                    text: qsTr("Active on: ")+ active
                    color:"white"
                    fontSize: 22
                }
            }

            StyledText {
                id: activeStatus
                text: status ? qsTr("Enabled") : "Disabled"
                color:"white"
                fontSize: 42
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            StyledText {
                id: countdown
                text: qsTr("Time Left:\n ") +remaining
                color:"white"
                fontSize: 42
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
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
