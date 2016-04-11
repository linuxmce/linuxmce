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
            height:Style.scaleY(15)

            buttonText: "Regen\nOrbiter"
            onActivated: manager.regenOrbiter(manager.iPK_Device)
        }

        LargeStyledButton{
            id:pending
            height:Style.scaleY(15)
            buttonText: "Pending\nTasks"

        }

        LargeStyledButton{
            id:networksettings
            height:Style.scaleY(15)
            buttonText: "Network\nSettings"

        }

        LargeStyledButton{
            id:avwizard
            height:Style.scaleY(15)
            buttonText: "Av\nWizard"

        }

        LargeStyledButton{
            id:regenorbiters
            height:Style.scaleY(15)
            buttonText: "Regen All\nOrbiters"

        }

        LargeStyledButton{
            id:quickreload
            height:Style.scaleY(15)
            buttonText: "Quick\n Reload"
            onActivated: manager.quickReload()

        }

        LargeStyledButton{
            id:changeStyle
            height:Style.scaleY(15)
            buttonText: "Change\nStyles"

        }
        LargeStyledButton{
            id:diagnostic
            height:Style.scaleY(15)
            buttonText: qsTr("Screen Information")
            onActivated: {
                qmlRoot.createPopup(screen_info)
            }

        }
        LargeStyledButton{
            id:refresh
            height:Style.scaleY(15)
            buttonText: qsTr("Reload Local QML")
            onActivated:{ manager.qmlReload()}
        }
        LargeStyledButton{
            id:ping
            height:Style.scaleY(15)
            buttonText: qsTr("Ping Test")
        }
        LargeStyledButton{
            id:resetDev
            height:Style.scaleY(15)
            buttonText: qsTr("Reset Device Id")
            onActivated:{
                manager.setDeviceNumber(-1)
                manager.writeConfig()
            }
        }
        LargeStyledButton{
            id:componentList
            height:Style.scaleY(15)
            buttonText: qsTr("Component List")
        }
        LargeStyledButton{
            id:settingsBtn
            height:Style.scaleY(15)
            buttonText: qsTr("Settings")
            onActivated: qmlRoot.createPopup(settingsComponent)
        }
        LargeStyledButton{
            id:exitBtn
            height:Style.scaleY(15)
            buttonText: qsTr("Exit QOrbiter")
            onActivated: manager.exitApp()
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

                    StyledText{
                        anchors.horizontalCenter: parent.horizontalCenter
                        text:screenInfo.primaryScreen.orientation
                    }
                }
            }
        }
    }
}
