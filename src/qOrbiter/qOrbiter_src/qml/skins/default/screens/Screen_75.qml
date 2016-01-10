import QtQuick 2.2
import "../components"
StyledScreen {
    screen:qsTr("Pick Room for Device")
    Panel{
        headerTitle: screen
        GenericListModel{
            id:rooms
            anchors.fill: parent
            label:qsTr("Which Room is this device in?")
        }

    }
}
