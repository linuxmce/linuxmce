import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0
import "../."
StyledScreen {

    focus:true
    screen:qsTr("Pending Tasks")
    Panel{
        headerTitle: screen
        GenericListModel {
            id: pendingTasks
            label:qsTr("Current pending tasks")
            anchors.fill: parent
            anchors.topMargin: Style.scaleY(10)

            model: manager.getDataGridModel("ptasks", DataGrids.Pending_Tasks)

            delegate: Item {
                width: scaleX(90)
                height: scaleY(10)

                StyledText {
                    id: taskLabel
                    text: value
                    color: "white"
                    fontSize: 32
                }
            }

        }
    }
}
