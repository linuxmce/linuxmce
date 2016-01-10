import QtQuick 2.2
import "../components"
import "../."
import org.linuxmce.grids 1.0

StyledScreen {
    id:alarms_screen
    screen:qsTr("Sleeping Menu")
    keepHeader: true
    focus:true

    Panel{
        id:alarms_info
        headerTitle: screen
        anchors.centerIn: parent
        content: Item{
            anchors.fill: parent
            GenericListModel{
                id:alarm_model
                // model: manager.getDataGridModel("sleepingAlarms", DataGrids.Alarms_In_Room, String(manager.getlocation()))
                dataGrid: DataGrids.Alarms_In_Room
                dataGridLabel: "sleepingAlarms"
                dataGridOptions: String(manager.getlocation())
                label:qsTr("(%1) Alarms in Room").arg(alarm_model.modelCount)
                height: parent.height
                width: parent.width
                delegate: Item{
                    width: parent.width
                    height: Style.listViewItemHeight
                    Rectangle{
                        anchors.fill: parent
                        color: /*handler.pressed ? "yellow" :*/ status ? "green" : "red"
                        opacity: .75
                        radius:5
                        border.color: "white"
                        border.width: 1
                    }

                    Column{
                        height: parent.height
                        width: parent.width/2
                        StyledText {
                            id: alarmname
                            text:name
                            color:"white"
                            fontSize:Style.appFontSize_list
                        }
                        StyledText {
                            id: alarmtimeLabel
                            text:qsTr("Alarm Set For: ") + alarmtime
                            color:"white"
                            fontSize:Style.appFontSize_list
                        }
                        StyledText {
                            id: daysactive
                            text: qsTr("Active on: ")+ active
                            color:"white"
                            fontSize:Style.appFontSize_list
                        }
                    }

                    StyledText {
                        id: activeStatus
                        text: status ? qsTr("Enabled") : "Disabled"
                        color:"white"
                        fontSize: Style.appFontSize_title
                        anchors.right: parent.right
                        anchors.rightMargin: Style.scaleX(2)
                        anchors.top: parent.top
                    }

                    StyledText {
                        id: countdown
                        text: qsTr("Time Left:\n ") +remaining
                        color:"white"
                        fontSize:Style.appFontSize_list
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            alarm_model.model.setData(index, "status", !status)
                            alarm_model.refresh()
                        }
                    }


                }
            }
        }
    }

    Component.onCompleted: {
        forceActiveFocus()
    }



}
