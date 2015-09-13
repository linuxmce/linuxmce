import QtQuick 2.2
import "../components"
StyledScreen {
    screen: qsTr("Television EPG")
    Panel{
        headerTitle: screen

        EPGGrid {
            id: epgGrid
            anchors.top: parent.headerRect.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.right

        }

    }
}
