// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

import "../components"

Rectangle {
    id:router_reloading
    width: manager.manager.appWidth
    height: manager.manager.appHeight
    color: "transparent"

    Text {
        id: reload_text
        text: qsTr("Router is Reloading, Please be patient")
        font.pixelSize: scaleY(8)
        color: "black"
        font.bold: true
        style:Text.Sunken
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: reload_status
        text: dcemessage
        font.pixelSize: scaleY(5)
        color: "black"
        font.bold: true
        anchors.centerIn: parent
    }

    Timer{
        id:continueTimer
        interval: 5000
        running: true
        onTriggered: manager.qmlSetupLmce(iPK_Device,srouterip)
    }

}
