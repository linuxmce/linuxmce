import QtQuick 1.1
import com.nokia.android 1.1
import Qt.labs.shaders 1.0

Rectangle {
    anchors.fill: parent
    color: "slategrey"



    Timer{
        id:wait
        interval: 5000
        onTriggered: {mainContent.source = "http://"+srouterip+"/lmce-admin/skins/android/Splash.qml"; }
        running:true
    }
    Label {
        id: loading
        text: qsTr("Loading, please be patient")
        anchors.centerIn: parent
        font.pixelSize: 16
        font.family: "Droid"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    }

    Loader{
        id:mainContent
        anchors.fill: parent
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1000}
        onStatusChanged: if(mainContent.status != Loader.Ready){
                             mainContent.visible= false
                             mainContent.source = ""
                             loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server. Please ensure you can reach your core")
                         }
                         else {
                             mainContent.opacity = .01
                             loading.visible= false
                         }
    }

}
