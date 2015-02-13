import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen {
    keepHeader: true
    focus:true
    screen:qsTr("Pending Tasks")

    ListView {
        id: pendingTasks
        anchors.fill: parent
        anchors.topMargin: scaleY(10)

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
