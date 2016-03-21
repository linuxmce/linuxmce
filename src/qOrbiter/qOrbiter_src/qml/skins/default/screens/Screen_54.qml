import QtQuick 2.2
import "../components"
StyledScreen {
    screen: qsTr("Stored Audio Playback")
    onScreenOpen: {
        manager.clearAllDataGrid()

    }
    GenericPlaybackScreen{
        id:screen_54
        anchors.fill: parent
    }

}
