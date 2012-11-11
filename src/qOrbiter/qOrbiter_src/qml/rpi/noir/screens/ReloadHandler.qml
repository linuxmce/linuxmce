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
        font.pointSize: scaleY(8)
        color: "black"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }
    WebView {
        id:web_regen
        anchors.top: reload_text.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
    }
    StyledText {
        id: reload_status
        text: dcemessage
        font.pointSize: scaleY(5)
        color: "black"
        font.bold: true
        anchors.top: web_regen.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
