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
            text: qsTr("Router Disconnect, in the meantime, we will attempt to refresh scenarios")
            width: scaleX(85)
            anchors.centerIn: parent
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: scaleY(3)
            color:"orange"
        }

        WebView {
            anchors.top: reload_label.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height - 200
            width: parent.width - 100
            url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
        }
    }
}
