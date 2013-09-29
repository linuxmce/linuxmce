import QtQuick 1.1
import "../components"
Item {
    id:alarms_screen

    anchors.fill: parent
    focus:true
    Component.onCompleted: {
        forceActiveFocus()
        manager.getSleepingAlarms()
    }

    StyledText{
        id:alarms_label
        text:qsTr("Sleeping Alarms")
        fontSize: 32
    }
}
