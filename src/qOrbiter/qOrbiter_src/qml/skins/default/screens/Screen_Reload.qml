import QtQuick 2.2
import "../components"
StyledScreen {
    anchors.fill: parent
    screen: qsTr("Regenerate Configuration")



    Panel{
        useHeader: false
        anchors.centerIn: parent

        Component.onCompleted: {
            manager.regenOrbiter()
        }

        Connections{
            target:manager
            onConfigurationChanged:{
                manager.currentScreen="Screen_1.qml"
            }
        }


        StyledText{
            anchors.centerIn: parent
            text:"Regenerating Configuration \n please be patient."
        }
    }



}
