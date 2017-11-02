import QtQuick 2.2
import "../../components"
import "../"
StyledScreen {
    id:advancedScreen
    onActiveFocusChanged: flow.forceActiveFocus()
    Flow{
        id:flow
        anchors.centerIn: parent
        anchors.fill: parent
        spacing: 25
        focus:true
        property int focusIndex:-1

        Keys.onLeftPressed:{ if(focusIndex==-1) return; focusIndex--; console.log(focusIndex)}
        Keys.onRightPressed:{ focusIndex++; console.log(focusIndex)}
        onFocusIndexChanged: {
            if(focusIndex!=-1){
                console.log("activating child "+ focusIndex)
                flow.children[focusIndex].forceActiveFocus()
            }
        }
        onActiveFocusChanged: console.log("flow has focus")

        LargeStyledButton{            id:regenorbiter
            height:appStyle.scaleY(15)

            buttonText: "Regen\nOrbiter"
            onActivated: manager.regenOrbiter(manager.iPK_Device)
        }

        LargeStyledButton{
            id:pending
            height:appStyle.scaleY(15)
            buttonText: "Pending\nTasks"

        }

        LargeStyledButton{
            id:networksettings
            height:appStyle.scaleY(15)
            buttonText: "Network\nSettings"

        }

        LargeStyledButton{
            id:avwizard
            height:appStyle.scaleY(15)
            buttonText: "Av\nWizard"

        }

        LargeStyledButton{
            id:regenorbiters
            height:appStyle.scaleY(15)
            buttonText: "Regen All\nOrbiters"

        }

        LargeStyledButton{
            id:quickreload
            height:appStyle.scaleY(15)
            buttonText: "Quick\n Reload"
            onActivated: manager.quickReload()

        }

        LargeStyledButton{
            id:changeStyle
            height:appStyle.scaleY(15)
            buttonText: "Change\nStyles"

        }
        LargeStyledButton{
            id:diagnostic
            height:appStyle.scaleY(15)
            buttonText: qsTr("Screen Information")
            onActivated: {
                qmlRoot.createPopup(screen_info)
            }

        }
        LargeStyledButton{
            id:refresh
            height:appStyle.scaleY(15)
            buttonText: qsTr("Reload Local QML")
            onActivated:{ manager.qmlReload()}
        }
        LargeStyledButton{
            id:ping
            height:appStyle.scaleY(15)
            buttonText: qsTr("Ping Test")
        }
        LargeStyledButton{
            id:resetDev
            height:appStyle.scaleY(15)
            buttonText: qsTr("Reset Device Id")
            onActivated:{
                manager.setDeviceNumber(-1)
                manager.writeConfig()
            }
        }
        LargeStyledButton{
            id:componentList
            height:appStyle.scaleY(15)
            buttonText: qsTr("Component List")
        }
        LargeStyledButton{
            id:settingsBtn
            height:appStyle.scaleY(15)
            buttonText: qsTr("Settings")
            onActivated: qmlRoot.createPopup(settingsComponent)
        }
        LargeStyledButton{
            id:exitBtn
            height:appStyle.scaleY(15)
            buttonText: qsTr("Exit QOrbiter")
            onActivated: manager.closeOrbiter()
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

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Screen Name: %1").arg(screenInfo.primaryScreen.screenName)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Current Orientation: %1").arg(screenInfo.primaryScreen.orientation)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Native Orientation: %1").arg(screenInfo.primaryScreen.nativeOrientation)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Logical DPI: %1").arg(screenInfo.primaryScreen.logicalDpi)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Physical DPI: %1").arg(screenInfo.primaryScreen.physicalDpi)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 mm").arg(screenInfo.primaryScreen.heightMM)
                    }
                    StyledText{

                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 mm").arg(screenInfo.primaryScreen.widthMM)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Diagonal Size: %1 (in) Screen Size:: %2").arg(screenInfo.primaryScreen.diagonalInches).arg(screenInfo.primaryScreen.stringDeviceSize)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Pixel Density Rating: %1 ").arg(screenInfo.primaryScreen.pixelDensityString)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Width: %1 (px)").arg(screenInfo.primaryScreen.width)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Height: %1 (px)").arg(screenInfo.primaryScreen.height)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Pixel Ratio: %1").arg(screenInfo.primaryScreen.pixelRatio)
                    }

                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Version %1").arg(orbiterVersion)
                    }
                    StyledText{
                        fontSize: appStyle.appFontSize_list
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:qsTr("Compile Date %1").arg(buildDate)
                    }
                }
            }
        }
    }
}
