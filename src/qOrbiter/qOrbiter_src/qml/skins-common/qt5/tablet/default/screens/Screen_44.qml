import QtQuick 2.2
import "../components"
import org.linuxmce.settings 1.0
import org.linuxmce.enums 1.0
/*Android - tablet - defaut */
StyledScreen {
    anchors.fill: parent
    id:advancedScreen
    Flickable{
        anchors.centerIn: parent
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight:advancedFlick.height
        Flow{
            id:advancedFlick
            width: parent.width

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
                onActivated:  manager.gotoQScreen("Screen_82.qml")
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
                id:mediaPlayerState
                state: "large-fixed"
                buttonText: dcePlayer.mediaPlayerConnected ? "Media Player\nConnected":"Enable\nMedia Player"
                onActivated: {
                    if(!dcePlayer.mediaPlayerConnected){
                        dcePlayer.restart()
                    }
                }
            }

            StyledButton{
                id:resetId
                state:"large-fixed"
                buttonText: "Reset Device Id"
                onActivated: {
                    manager.setDeviceNumber(-1)
                    manager.writeConfig()
                    manager.exitApp()
                }
            }
            StyledButton{
                id:dpi
                state:"large-fixed"
                buttonText: "Physical DPI::"+screenInfo.primaryScreen.physicalDpi+"\n"+"Logical DPI::"+screenInfo.primaryScreen.logicalDpi
                onActivated: {
                    manager.setDeviceNumber(-1)
                    manager.writeConfig()
                    manager.exitApp()
                }
            }
            StyledButton{
                id:internalExternal
                property bool settingsValue:settings.getOption(SettingsType.Settings_UI,SettingsKey.Setting_Ui_NetworkLoading)
                state:"large-fixed"
                buttonText: settingsValue ? "Use Network Skins" : "Use Local Skins"
                onActivated: {
                    settings.setOption( Number(SettingsType.Settings_UI),Number(SettingsKey.Setting_Ui_NetworkLoading), !settingsValue)
                    settingsValue = !settingsValue
                }
            }

        }
    }


}
