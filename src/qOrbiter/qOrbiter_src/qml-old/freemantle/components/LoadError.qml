import QtQuick 1.0
import QtWebKit 1.0
import "../components"

Rectangle {
    width: style.orbiterW
    height: style.orbiterH

   HomeButton{anchors.top: parent.top; anchors.left: parent.left}

    WebView{
        anchors.centerIn: parent
        height: parent.height - 200
        width: parent.width - 100
        url: "http://dcerouter/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device

    }
}




