import QtQuick 1.1
import "../components"

StyledScreen {
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
            onActivated: manager.regenOrbiter(manager.iPK_Device)
        }

        StyledButton{
            buttonText: qsTr("Quick Reload")
            onActivated: manager.quickReload()
        }

        StyledButton{
            buttonText: qsTr("Change Styles")
            onActivated: loadComponent("SkinSelector.qml")
        }
        StyledButton{
            buttonText: qsTr("Refresh Skin")
            onActivated: manager.clearSkinCache()
        }
    }
}
