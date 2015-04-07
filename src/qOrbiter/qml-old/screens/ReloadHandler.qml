// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0
import QtWebKit 1.0

Rectangle {
    id:router_reloading
    width: manager.appWidth
    height: manager.appHeight
    color: "slategrey"


    StyledText {
        id: reload_text
        text: qsTr("Router is Reloading, Please be patient")
        font.pixelSize: scaleY(8)
        color: "black"
        font.bold: false
        anchors.horizontalCenter: parent.horizontalCenter
    }
    WebView {
        id:web_regen
        anchors.centerIn: parent
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
    }
    StyledText {
        id: reload_status
        text: dcemessage
        font.pixelSize: scaleY(5)
        color: "black"
        font.bold: true
        anchors.top: web_regen.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
