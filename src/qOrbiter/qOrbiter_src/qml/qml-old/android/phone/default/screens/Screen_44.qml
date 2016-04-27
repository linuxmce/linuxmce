import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import org.linuxmce.enums 1.0
/*Android - phone - defaut */
StyledScreen {
    id: advancedscreen
    Item{
        id:content
        anchors.fill:parent
        state: "reg"
        Flow {
            id: flow1
            anchors.centerIn: parent
            height: scaleY(85)
            width: scaleX(95)
            spacing: 10

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

        Rectangle{
            id:diagPanel
            anchors.fill: parent
            color:"black"

            onVisibleChanged: {
                if(visible){
                    androidSystem.updateBuildInformation()
                    androidSystem.updateExternalStorageLocation()
                }
            }

            StyledButton{
                anchors.top:parent.top
                anchors.left: parent.left
                buttonText: qsTr("Back")
                MouseArea{
                    anchors.fill: parent
                    onClicked: content.state="reg"
                }
            }
            Column{
                height: parent.height*.75
                width: parent.width*.85
                anchors.bottom: parent.bottom


                StyledText{
                    text: "Api Level:"+ androidSystem.apiLevel
                    font.pixelSize: 26
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: androidSystem.redStandard
                }

                StyledText{
                    text: "Storage Location:"+ androidSystem.externalStorageLocation
                    font.pixelSize: 26
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: androidSystem.blueStandard
                }

                StyledText{
                    text: "Media Mount Status:"+ androidSystem.mountStatus
                    font.pixelSize: 26
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: androidSystem.getOrangeStandard()
                }

                StyledText{
                    text: "Made By:"+ androidSystem.deviceManufacturer
                    font.pixelSize: 26
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "white"
                }

                StyledText{
                    text: "Devicename:"+ androidSystem.deviceName
                    font.pixelSize: 26
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: androidSystem.purpleStandard
                }

            }
        }

        states: [
            State {
                name: "reg"
                PropertyChanges {
                    target: flow1
                    visible:true
                }
                PropertyChanges {
                    target: diagPanel
                    visible:false
                }
            },
            State{
                name:"diagnostic"
                PropertyChanges{
                    target:flow1
                    visible:false
                }
                PropertyChanges {
                    target: diagPanel
                    visible:true
                }

            }

        ]
    }

}
