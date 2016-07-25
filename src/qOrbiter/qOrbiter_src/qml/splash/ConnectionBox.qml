import QtQuick 2.2
import org.linuxmce.screeninfo 1.0
import org.linuxmce.settings 1.0

Item{
    id: connectionBox
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenterOffset:10
    width: scaleX(85)
    height: scaleY(45)
    opacity:0
    Rectangle {
        anchors.fill: connectionBox
        radius: 7
        anchors.horizontalCenterOffset: 1
        anchors.centerIn: parent
        border.width: 1
        border.color: "#000000"
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "transparent"
            }
            GradientStop {
                position: 1
                color: "#000000"
            }
        }
        opacity: 0.5
    }
    Text {
        id: connectionlabel
        text: qsTr("Set Connection Details for Device %1").arg(settings.getOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_Device_ID))
        font.pointSize: theme.appFontSize_header
        font.weight: Font.Light
        width:parent.width
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color:theme.apptext_color_active
        anchors.top: connectionBox.top
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: myFont.name
    }
    Column{
        id:connetion_layout
        anchors{
            left:parent.left
            right:parent.right
            top:connectionlabel.bottom
            bottom:exitbutton.top
        }
        ConnectionElement {
            height: parent.height/2
            id: connectionVars
            location: qsTr("Home")
            onValueSet: {
                settings.setOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_Hostname, router)
            }
        }
        ConnectionElement{
            height: parent.height/2
            id:connection_away
            location: qsTr("Away")
            router: settings.getOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_ExternalHostname)
            onValueSet: {
                 settings.setOption(SettingsType.Settings_Network, SettingsKey.Setting_Network_ExternalHostname, router)
            }
        }

    }

//        Rectangle {
//            id: connectbutton
//            height: theme.appButtonHeight*theme.dpRatio
//            width: theme.appButtonWidth*theme.dpRatio
//            radius:height
//            color:theme.appbutton_confirm_color

//            Text {
//                id: name
//                anchors.centerIn: parent

//                text: qsTr("Go!")
//                font.pixelSize: theme.appFontSize_list
//                verticalAlignment: Text.AlignTop
//                font.bold: true
//                font.family: myFont.name
//                color:theme.apptext_color_active
//            }


//            MouseArea{
//                hoverEnabled: true
//                onEntered: parent.color="green"
//                onExited: parent.color="red"
//                anchors.fill: parent
//                onClicked: { orbiterWindow.qmlSetupLmce(orbiterWindow.deviceno, routerip.text) }
//                anchors.verticalCenter: parent.verticalCenter
//            }
//        }

        Rectangle {
            id: exitbutton
            height: parent.height*.25
            width: theme.appButtonWidth
            radius: 5
            anchors{
                horizontalCenter: parent.horizontalCenter
                bottom:parent.bottom
                bottomMargin: 5
            }

            color: "red"

            Text {
                id: exitlabel
                anchors.centerIn: parent
                color:theme.apptext_color_active
                text: qsTr("Exit!")
                font.pointSize: theme.appFontSize_list
            }

            MouseArea{
                anchors.fill: parent
                onClicked: manager.closeOrbiter()

            }
        }

    states: [
        State {
            name: "showing"

            PropertyChanges {
                target: connectionBox
                opacity:1
                scale:1
            }
        },
        State {
            name: "hidden"
            when:orbiterList.count!==0 || orbiterWindow.b_connectionPresent
            PropertyChanges {
                target: connectionBox
                opacity:0
                scale:0
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            PropertyAnimation{
                duration: 350
            }
        }
    ]
}
