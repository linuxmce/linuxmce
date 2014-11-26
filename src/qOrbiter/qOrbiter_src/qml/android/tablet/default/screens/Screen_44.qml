import QtQuick 1.1
import "../components"
import org.linuxmce.enums 1.0
/*Android - tablet - defaut */
StyledScreen {
    anchors.fill: parent
    id:advancedScreen
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
            id:regenorbiter
            state: "large-fixed"
            buttonText: "Regen\nOrbiter"
            onActivated:  regenOrbiter(manager.iPK_Device)
        }

        StyledButton{
            id:pending
            state: "large-fixed"
            buttonText: "Pending\nTasks"

        }

        StyledButton{
            id:networksettings
            state: "large-fixed"
            buttonText: "Network\nSettings"

        }

        StyledButton{
            id:avwizard
            state: "large-fixed"
            buttonText: "Av\nWizard"

        }

        StyledButton{
            id:regenorbiters
            state: "large-fixed"
            buttonText: "Regen All\nOrbiters"

        }

        StyledButton{
            id:quickreload
            state: "large-fixed"
            buttonText: "Quick\n Reload"
            onActivated: manager.quickReload()

        }

        StyledButton{
            id:changeStyle
            state: "large-fixed"
            buttonText: "Change\nStyles"

        }
        StyledButton{
            id:diagnostic
            state: "large-fixed"
            buttonText: "Diagnostics"
            onActivated: content.state="diag"

        }
        StyledButton{
            id:refresh
            state: "large-fixed"
            buttonText: "Clear\nCache"
            onActivated:{ manager.clearSkinCache(); manager.goBacktoQScreen()}
        }
        StyledButton{
            id:ping
            state: "large-fixed"
            buttonText: "Ping\nTest"
        }
        StyledButton{
            id:switchProfile
            state: "large-fixed"
            buttonText: manager.hostDevice==HostDevices.ANDROID_PHONE ? "Switch to\n Tablet skins" : "Switch to\n Phone Skins"
            onActivated: {
                manager.toggleSkinType()
            }
        }

    }
}
