import QtQuick 1.1
import "../components"
StyledScreen {
    anchors.fill: parent

    Panel{
        useHeader: false
        anchors.centerIn: parent
        StyledText{
            anchors.centerIn: parent
            text:"Router is Reloading \n please be patient."
        }
    }



}
