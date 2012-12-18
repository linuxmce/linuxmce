import QtQuick 1.1


Rectangle {
    height: manager.appHeight
    width: manager.appWidth
   color: "black"
    Timer{
        id:wait
        interval: 2500
        onTriggered: {mainContent.source = "http://"+srouterip+"/lmce-admin/skins/android/Splash.qml"; }
        running:true
    }
    Rectangle{
        id:spinner
        height: rotation > 270 ? parent.height*.04 : parent.height *.10
        width: parent.width*.25
        color: rotation > 270 ? "darkgrey" : "white"
        opacity: .75
        anchors.top: loading.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        radius:10
        PropertyAnimation on rotation{
            from:0
            to:360
            duration:1500
            loops:Animation.Infinite
        }
    }

    Text {
        id: loading
        text: qsTr("Connecting to "+srouterip+", please be patient")
        anchors.centerIn: parent
        font.pixelSize: 16
        font.family: "Droid"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        opacity: .85
    }



        Text {
            id: dceMessages
            text: manager.dceResponse
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }


    Loader{
        id:mainContent
        height: manager.appHeight
        width: manager.appWidth
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1500}
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



}
