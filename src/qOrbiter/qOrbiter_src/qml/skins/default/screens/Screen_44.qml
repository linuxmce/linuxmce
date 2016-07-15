import QtQuick 2.2
import org.linuxmce.screens 1.0

import "../components"
StyledScreen {
    id:advancedScreen
    screen: qsTr("Advanced Options")
    Panel{
        anchors.centerIn: parent
        headerTitle: screen
        useHeader: true

        content:  Flickable{
            anchors.fill: parent
            contentWidth: parent.width
            contentHeight:parent.width
            clip:true
            Flow{
                id:flow
                anchors.centerIn: parent
                anchors.fill: parent
                spacing: 25
                Component.onCompleted: forceActiveFocus()

                StyledButton{
                    id:regenorbiter

                    buttonText: "Regen\nOrbiter"
                    onActivated:  manager.currentScreen="Screen_Reload.qml"
                }

                StyledButton{
                    id:pending
                    buttonText: "Pending\nTasks"
                    onActivated: manager.currentScreen=Screens.PendingTasks

                }

                StyledButton{
                    id:networksettings

                    buttonText: "Network\nSettings"

                }

                StyledButton{
                    id:avwizard

                    buttonText: "Av\nWizard"

                }

                StyledButton{
                    id:regenorbiters

                    buttonText: "Regen All\nOrbiters"

                }

                StyledButton{
                    id:quickreload

                    buttonText: "Quick\n Reload"
                    onActivated: manager.quickReload()

                }

                StyledButton{
                    id:changeStyle

                    buttonText: "Change\nStyles"

                }
                StyledButton{
                    id:diagnostic

                    buttonText: qsTr("Screen Information")
                    onActivated: {
                        qmlRoot.createPopup(screen_info)
                    }

                }
                StyledButton{
                    id:refresh

                    buttonText: qsTr("Reload Local QML")
                    onActivated:{ manager.qmlReload()}
                }
                StyledButton{
                    id:ping

                    buttonText: qsTr("Ping Test")
                }
                StyledButton{
                    id:componentList

                    buttonText: qsTr("Component List")
                }
                StyledButton{
                    id:settingsBtn

                    buttonText: qsTr("Settings")
                    onActivated: qmlRoot.createPopup(settingsComponent)
                }
                StyledButton{
                    id:resetDev

                    buttonText: qsTr("Reset Device Id")
                    onActivated:{
                        manager.setDeviceNumber(-1)
                        manager.writeConfig()
                    }
                }
                StyledButton{
                    buttonText: qsTr("Clear Settings")
                    onActivated:{
                       settings.destroySettingsData()
                    }
                }

                StyledButton{
                    id:exitBtn

                    buttonText: qsTr("Exit Application")
                    onActivated:{
                        manager.closeOrbiter()
                    }
                }
            }
        }
    }


    Component{
        id: settingsComponent
        GenericPopup {
            title:qsTr("Settings")
            content: GenericDialog{
                title: qsTr("Application Settings")
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

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Screen Name: %1").arg(screenInfo.primaryScreen.screenName)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Current Orientation: %1").arg(screenInfo.primaryScreen.orientation)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Native Orientation: %1").arg(screenInfo.primaryScreen.nativeOrientation)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Logical DPI: %1").arg(screenInfo.primaryScreen.logicalDpi)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Physical DPI: %1").arg(screenInfo.primaryScreen.physicalDpi)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 mm").arg(screenInfo.primaryScreen.heightMM)
                    }
                    StyledText{

                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 mm").arg(screenInfo.primaryScreen.widthMM)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Diagonal Size: %1 (in) Screen Size:: %2").arg(screenInfo.primaryScreen.diagonalInches).arg(screenInfo.primaryScreen.stringDeviceSize)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Pixel Density Rating: %1 ").arg(screenInfo.primaryScreen.pixelDensityString)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 (px)").arg(screenInfo.primaryScreen.width)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 (px)").arg(screenInfo.primaryScreen.height)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Pixel Ratio: %1").arg(screenInfo.primaryScreen.pixelRatio)
                    }

                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Version %1").arg(orbiterVersion)
                    }
                    StyledText{
                        fontSize: Style.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Compile Date %1").arg(buildDate)
                    }
                }
            }
        }
    }
}
