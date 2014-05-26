import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

StyledScreen {
    id: advancedscreen
    Item{
        id:content
        anchors.fill:parent


        state: "reg"



        Flow {
            id: flow1
            anchors.centerIn: parent
            height: scaleY(75)
            width: scaleX(85)
            spacing: 10

            StyledButton{
                id:regenorbiter

                buttonText: "Regen \nOrbiter"
                onActivated:  regenOrbiter(manager.m_dwPK_Device)
            }

            StyledButton{
                id:pending

                buttonText: "Pending\n Tasks"

            }

            StyledButton{
                id:networksettings

                buttonText: "Network \nSettings"

            }

            StyledButton{
                id:avwizard

                buttonText: "Av\n wizard"

            }

            StyledButton{
                id:regenorbiters

                buttonText: "Regen \n All Orbiters"

            }

            StyledButton{
                id:quickreload

                buttonText: "Quick \n Reload"
                onActivated: manager.quickReload()

            }

            StyledButton{
                id:changeStyle

                buttonText: "Change\n Styles"

            }
            StyledButton{
                id:diagnostic

                buttonText: "Diagnostics"
                onActivated: content.state="diag"

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
