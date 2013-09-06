import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs


Item {
    height: manager.appHeight
    width:  manager.appWidth
    id: advancedscreen
    state: "reg"
    HomeButton{}

    Flow {
        id: flow1
        anchors.centerIn: parent
        height: scaleY(75)
        width: scaleX(85)
        spacing: 10

        ButtonSq{
            id:regenorbiter
            height: style.buttonH
            width: style.buttonW
            buttontext: "Regen Orbiter"
            radius: 5

            MouseArea{
                anchors.fill:parent
                z:5
                onClicked: regenOrbiter(manager.m_dwPK_Device)
            }
        }

        ButtonSq{
            id:pending
            height: style.buttonH
            width: style.buttonW
            buttontext: "Pending Tasks"
            radius: 5
        }

        ButtonSq{
            id:networksettings
            height: style.buttonH
            width: style.buttonW
            buttontext: "Network Settings"
            radius: 5
        }

        ButtonSq{
            id:avwizard
            height: style.buttonH
            width: style.buttonW
            buttontext: "Av wizard"
            radius: 5
        }

        ButtonSq{
            id:regenorbiters
            height: style.buttonH
            width: style.buttonW
            buttontext: "Regen All Orbiters"
            radius: 5
        }

        ButtonSq{
            id:quickreload
            height: style.buttonH
            width: style.buttonW
            buttontext: "Quick Reload"
            radius: 5
            MouseArea{
                anchors.fill: parent
                z:10
                onClicked: manager.quickReload()
            }
        }

        ButtonSq{
            id:changeStyle
            height: style.buttonH
            width: style.buttonW
            buttontext: "Change Styles"
            radius: 5

            MouseArea{
                anchors.fill: parent
               // onClicked: MyJs.createComponentObjects("SkinSelector.qml")
            }
        }
        ButtonSq{
            id:diagnostic
            height: style.buttonH
            width: style.buttonW
            buttontext: "Diagnostics"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:advancedscreen.state="diag"
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

        ButtonSq{
            anchors.top:parent.top
            anchors.left: parent.left
            buttontext: qsTr("Back")
            MouseArea{
                anchors.fill: parent
                onClicked: advancedscreen.state="reg"
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
