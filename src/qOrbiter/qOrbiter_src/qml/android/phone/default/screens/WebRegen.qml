import QtQuick 1.0
import QtWebKit 1.0

Rectangle {
    width: appW
    height: appH

    Text {
        id: label
        text: qsTr("Regenerating your Configuration data...")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        font.bold: true
        font.pixelSize: scaleY(3)
    }

    WebView {
        anchors.top: label.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
    }

}
