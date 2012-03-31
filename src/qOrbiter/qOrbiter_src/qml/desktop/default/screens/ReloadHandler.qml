// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtWebKit 1.0

Rectangle {
    id:router_reloading
    width: appW
    height: appH
    color: "transparent"


    Text {
        id: reload_text
        text: qsTr("Router is Reloading")
        font.pixelSize: scaleY(4)
        color: "black"
        font.bold: true
    }
    WebView {
        anchors.top: reload_label.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
    }
}
