import QtQuick 2.3
import "../components"
StyledScreen {
    id:advancedScreen

    Flow{
        anchors.centerIn: parent
        anchors.fill: parent
        spacing: 25
        Component.onCompleted: forceActiveFocus()

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
            buttonText: qsTr("Screen Information")
            onActivated: {
                qmlRoot.createPopup(settingsComponent)
            }

        }
        StyledButton{
            id:refresh
            state: "large-fixed"
            buttonText: qsTr("Reload Local QML")
            onActivated:{ manager.qmlReload()}
        }
        StyledButton{
            id:ping
            state: "large-fixed"
            buttonText: qsTr("Ping Test")
        }
        StyledButton{
            id:componentList
            state: "large-fixed"
            buttonText: qsTr("Component List")
        }
        StyledButton{
            id:settingsBtn
            state: "large-fixed"
            buttonText: qsTr("Settings")
            onActivated: qmlRoot.createPopup(settingsComponent)
        }
    }

    Component{
        id: settingsComponent
        GenericPopup {
            title:qsTr("Settings")
            content: GenericDialog{
                title: "Settings"
                dialogContent:
                    Component{
                    SettingsOptions {
                        id: settingsOption
                    }
                }
            }
        }
    }

    Component{
        id:compList
        GenericPopup{
            title:qsTr("Components Sample")
        }
    }
}
