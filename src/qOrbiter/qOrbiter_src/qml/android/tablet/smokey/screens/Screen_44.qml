import QtQuick 1.1
import "../components"

Item {
    anchors.fill: parent

    onVisibleChanged: {
            if(visible){
                androidSystem.updateBuildInformation()
                androidSystem.updateExternalStorageLocation()
            }
        }

    Flow{
        anchors.centerIn: parent
        anchors.fill: parent
        spacing: 25

        StyledButton{
            buttonText: qsTr("Regen this Orbiter")
            onActivated: manager.regenOrbiter(manager.m_dwPK_Device)
        }

        StyledButton{
            buttonText: qsTr("Quick Reload")
            onActivated: manager.quickReload()
        }

        StyledButton{
            buttonText: qsTr("Change Styles")
            onActivated: loadComponent("SkinSelector.qml")
        }
    }
}
