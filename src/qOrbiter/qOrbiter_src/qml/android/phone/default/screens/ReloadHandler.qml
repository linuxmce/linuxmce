// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtWebKit 1.0
import "../components"
Rectangle {
    width: appW
    height: appH
    color:"transparent"
    HomeButton{id:home_but; anchors.left: parent.left; anchors.top: parent.top
    Text {
        id: reload_label
        text: qsTr("Router is Reloading")
        font.pixelSize: scaleY(4)
        color:"orange"
        anchors.centerIn: parent
    }

    WebView {
        anchors.top: reload_label.bottom
        height: parent.height - 200
        width: parent.width - 100
        url: "http://192.168.80.1/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device


    }
}
}
