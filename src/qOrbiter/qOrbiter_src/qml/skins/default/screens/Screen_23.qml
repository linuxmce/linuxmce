import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen {
    anchors.fill: parent
    id:intercom
    screen:"Intercom"

    Panel{
        id: intercomPanel
        property int intercomStep: 0
        property int fromDevice: 0

        headerTitle: "Intercom"
        anchors.centerIn: parent

        content: Rectangle {
            id: phoneList
            border.color : "white"
            border.width : 1
            color: "black"
            opacity: .85
            radius: 2
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: appStyle.scaleY(5)
            anchors.bottomMargin: appStyle.scaleY(5)

            StyledText {
                id: helpText
                anchors.top: parent.top
                text: intercomPanel.intercomStep == 0 ? qsTr('Choose the extension you would like to call:') :
                                                 qsTr('Choose the device you would like to call from:')
                anchors.margins: 8
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TelecomDevices {
                id: telecomDevices
                anchors.top: helpText.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                showCallToDevices: true
                showClose: false

                onSelected: {
                    if (intercomPanel.intercomStep == 0) {
                        // switch to call from device list
                        intercomPanel.fromDevice = selectedDevice
                        intercomPanel.intercomStep = 1
                        showCallToDevices = false
                    } else if (intercomPanel.intercomStep == 1) {
                        // make call
                        manager.makeCall(currentuser, '', intercomPanel.fromDevice, selectedDevice)
                    }
                }
            }
        }
    }
}
