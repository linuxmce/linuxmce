import QtQuick 2.2
import "../components"
import "../."
import org.linuxmce.grids 1.0

StyledScreen {
    id:alarms_screen
    screen:qsTr("Sleeping Alarms")
    keepHeader: true
    focus:true

    Panel{
        id:alarms_info
        headerTitle: screen
        content: Item{
            anchors.fill: parent
            GenericListModel{
                model: manager.getDataGridModel("sleepingAlarms", DataGrids.Alarms_In_Room, String(manager.getlocation()))
//                dataGrid: DataGrids.Alarms_In_Room
//                dataGridLabel: "sleepingAlarms"
//                dataGridOptions: String(manager.getlocation())
                label:"Alarms in Room"

                width: parent.width
                delegate: Item{
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
                            alarmlist.model.setData(index, "status", !status)

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
