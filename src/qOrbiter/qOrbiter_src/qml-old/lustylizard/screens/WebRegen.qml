import QtQuick 1.0
import QtWebKit 1.0
import "../components"

Rectangle {
    height: appH
    width: appW
    color: "slategrey"

    Timer{
        triggeredOnStart: false
        interval: 1500
        running: true
        onTriggered: manager.reloadHandler()
    }

   HomeButton{anchors.top: parent.top; anchors.left: parent.left}

    WebView{
        anchors.centerIn: parent
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+manager.currentRouter+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device
    }

}




