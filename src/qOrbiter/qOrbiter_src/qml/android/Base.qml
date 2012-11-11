import QtQuick 1.1
import com.nokia.android 1.1
import Qt.labs.shaders 1.0

Rectangle {
   height: appH
   width: appW
   color: "darkslategrey"
    Timer{
        id:wait
        interval: 2500
        onTriggered: {mainContent.source = "http://"+srouterip+"/lmce-admin/skins/android/Splash.qml"; }
        running:true
    }
    Label {
        id: loading
        text: qsTr("Connecting to "+srouterip+", please be patient")
        anchors.centerIn: parent
        font.pixelSize: 16
        font.family: "Droid"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    }

    Loader{
        id:mainContent
        height: appH
        width: appW
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1000}
        onStatusChanged: if(mainContent.status != Loader.Ready){

                             loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server at "+srouterip+" Please ensure you can reach your core \n I will continue trying. \n"+ mainContent.Error)
                             wait.restart()
                         }
                         else {
                             mainContent.opacity = .01
                             loading.visible= false
                             loading.text = "Content Loaded, one moment"
                         }
    }

    Text {
        id: dceMessages
        text: manager.dceResponse
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
