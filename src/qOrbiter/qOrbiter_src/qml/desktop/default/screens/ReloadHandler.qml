// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtWebKit 1.0
import "../components"
Rectangle {
    id:router_reloading
    width: manager.appWidth
    height: manager.appHeight
    color: "transparent"
    Rectangle{
        anchors.fill: parent
        color: "lightgrey"
        opacity: .25
    }


    StyledText {
        id: reload_text
        text: qsTr("Router is Reloading, Please be patient")
        font.pixelSize: scaleY(8)
        color: "black"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Timer{
        id:webHideTimer
        interval: 5000
        running:true
        onTriggered: fade.start()
    }

    WebView {
        id:web_regen
        anchors.centerIn: parent
        height: parent.height - 200
        width: parent.width - 100
        url: "http://"+srouterip+"/lmce-admin/qOrbiterGenerator.php?d="+iPK_Device

        PropertyAnimation on opacity {
            id:fade
            duration:2000
            from: 1
            to:0
        }
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
    Rectangle{
        id:spinner
        height: 75
        width: 75
        color: "grey"
        radius:10
        anchors.right: parent.right
        PropertyAnimation on rotation {
            id:spinnin
            from:0
            to:360
            duration: 1000
            loops: Animation.Infinite
        }
    }
    Timer{
        id:continueTimer
        interval: 5000
        running: true
        onTriggered: manager.qmlSetupLmce(iPK_Device,srouterip)
    }

}
