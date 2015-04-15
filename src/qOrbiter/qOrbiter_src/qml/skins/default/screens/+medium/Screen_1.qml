import QtQuick 2.2
import "../."
import "../../components"

import org.linuxmce.screens 1.0
StyledScreen {
    id:screen

    StyledText{
        anchors.centerIn: parent
        fontSize: 72
        text:"LinuxMCE Tablet UI"
        color:"white"
    }
    Column{
        id:scenario_list
        spacing:5
        visible: uiOn
        width:Style.listViewWidth_medium
        height: childrenRect.height
        anchors.verticalCenter: parent.verticalCenter

        StyledButton{
            buttonText: qsTr("Lights")

            onActivated: qmlRoot.createPopup(roomSelector)
        }
        StyledButton{
            buttonText: qsTr("Media")

            onActivated: qmlRoot.createPopup(roomSelector)
        }
        StyledButton{
            buttonText: qsTr("Climate")

            onActivated: qmlRoot.createPopup(roomSelector)
        }
        StyledButton{
            buttonText: qsTr("Telecom")

            onActivated: qmlRoot.createPopup(roomSelector)
        }
        StyledButton{
            buttonText: qsTr("Security")

            onActivated: qmlRoot.createPopup(roomSelector)
        }
    }

}
