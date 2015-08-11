import QtQuick 2.2
import "../components"
StyledScreen {
    anchors.fill: parent

    Panel{
        useHeader: false
        anchors.centerIn: parent
        Component.onCompleted: {

        }


        StyledText{
            anchors.centerIn: parent
            text:"Router is Reloading \n please be patient."
        }
    }



}
