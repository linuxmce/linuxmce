import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0
import "../."
StyledScreen {

    focus:true
    screen:qsTr("Pending Tasks")
    Panel{
        headerTitle: screen
        Timer{
            running: true
            interval: 1500
            repeat:true
            onTriggered:{console.log("triggered!"); pendingTasks.refresh() }

        }

        GenericListModel {
            id: pendingTasks
            label:qsTr("Current pending tasks")
            anchors.fill: parent
            anchors.topMargin: Style.scaleY(10)
            dataGrid: DataGrids.Pending_Tasks
            dataGridLabel: "ptasks_"
            model: manager.getDataGridModel("ptasks", DataGrids.Pending_Tasks)

            delegate: Item {
                width: Style.scaleX(90)
                height: Style.scaleY(10)
                Column{
                    anchors.fill: parent
                    StyledText {
                        id: tasktitle
                        text: description
                        color: "white"
                        fontSize: Style.appFontSize_list
                    }
                    StyledText {
                        id: taskLabel
                        text: value
                        color: "white"
                         fontSize: Style.appFontSize_list
                    }
                }

            }

        }
    }
}
