
import QtQuick 1.0
import "../components"
import "../../../../skins-common/lib/handlers"
StyledScreen{
    id:sleepingAlamsScreen
    Component.onCompleted: {
        forceActiveFocus()
    }
Panel{
    anchors.fill: parent
    headerTitle: "Sleeping Alarms for this room: "+manager.currentRoom
    ListView{
        id:alarmlist
        anchors.fill: parent
        model: manager.getDataGridModel("sleepingAlarms", 29, String(manager.getlocation()))
        anchors.centerIn: parent
        anchors.margins: 10
	anchors.topMargin: 50
        delegate:Item{
            height: scaleY(15)
            anchors{
                right:parent.right
                left:parent.left

            }
            anchors.margins: 2
            Rectangle{
                color:appstyle.darkhighlight
                border.color: "black"
                border.width: 2
                anchors.fill: parent
                anchors.margins: 10
            }
            Row{
                id:delegatalayoutrow
                anchors.centerIn: parent
                width: parent.width
                anchors{
                    top:parent.top
                    left:parent.left
                    right:parent.right
                    bottom:parent.bottom
                }
                spacing: 2

                Rectangle{
                    id:indicator
                    height: scaleY(15)
                    width: parent.width *.25
                    color: status ?"green":  "red"
                    border.color: "black"
                    border.width: 2

                    StyledText {
                        id: statelabel
                        text: status === true ? qsTr("enabled") : qsTr("disabled")
                        anchors.centerIn: parent
                    }

                    AlarmToggleHandler{
                    }
                }
                Column{
		    anchors.verticalCenter: parent.verticalCenter
                    StyledText {
                        id: alarmname
                        text:qsTr("Name: ") + name
                    }
                    StyledText {
                        id: alarmtimeLabel
                        text:qsTr("Alarm Set For: ") + alarmtime
                    }
                    StyledText {
                        id: daysactive
                        text: qsTr("Active on: ")+ active
                    }
                    StyledText {
                        id: countdown
                        text: qsTr("Time Left: ") +remaining
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        forceActiveFocus()
    }
}


}
