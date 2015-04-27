import QtQuick 2.2
import "../components"
StyledScreen{
    screen:qsTr("Live TV")
    keepHeader: false
    BroadcastMediaBase {
        id:livetv_screen

    }
}


