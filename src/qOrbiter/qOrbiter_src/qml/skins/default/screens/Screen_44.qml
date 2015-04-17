import QtQuick 2.2
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
                qmlRoot.createPopup(screen_info)
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
        StyledButton{
            id:resetDev
            state: "large-fixed"
            buttonText: qsTr("Reset Device Id")
            onActivated:{
                manager.setDeviceNumber(-1)
                manager.writeConfig()
            }
        }
        StyledButton{
            id:exitBtn
           state: "large-fixed"
            buttonText: qsTr("Exit Application")
            onActivated:{
             manager.exitApp()
            }
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

    Component{
        id:screen_info
        GenericPopup{
            title:qsTr("Screen Information")
            content:Item{
                Column{
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing:10
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Screen Name: %1").arg(screenInfo.primaryScreen.screenName)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Current Orientation: %1").arg(screenInfo.primaryScreen.orientation)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Native Orientation: %1").arg(screenInfo.primaryScreen.nativeOrientation)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Logical DPI: %1").arg(screenInfo.primaryScreen.logicalDpi)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Physical DPI: %1").arg(screenInfo.primaryScreen.physicalDpi)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 mm").arg(screenInfo.primaryScreen.heightMM)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 mm").arg(screenInfo.primaryScreen.widthMM)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Diagonal Size: %1 (in) Screen Size:: %2").arg(screenInfo.primaryScreen.diagonalInches).arg(screenInfo.primaryScreen.stringDeviceSize)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 (px)").arg(screenInfo.primaryScreen.width)
                    }
                    StyledText{
                        isBold: true
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 (px)").arg(screenInfo.primaryScreen.height)
                    }
                }
            }
        }
    }
}
