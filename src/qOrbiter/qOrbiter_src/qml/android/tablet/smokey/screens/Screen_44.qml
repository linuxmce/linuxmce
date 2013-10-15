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
            buttonText.text: qsTr("Regen this Orbiter")
            onActivated: manager.regenOrbiter(manager.m_dwPK_Device)
        }

        StyledButton{
            buttonText.text: qsTr("Quick Reload")
            onActivated: manager.quickReload()
        }

        StyledButton{
            buttonText.text: qsTr("Change Styles")
            onActivated: loadComponent("SkinSelector.qml")
        }


        StyledButton{
            buttonText.text: buttonText.text === "Media Test" ? "Stop Media" : qsTr("Media Test")
            onActivated:{
                if(buttonText.text !=="Media Test"){
                    androidSystem.stopMedia(); buttonText.text = "Media Test"
                }else
                {
                    androidSystem.playMedia("http://www.virginmegastore.me/Library/Music/CD_001214/Tracks/Track1.mp3");
                    buttonText.text = "Stop Media"
                }
            }
        }

    }
}
