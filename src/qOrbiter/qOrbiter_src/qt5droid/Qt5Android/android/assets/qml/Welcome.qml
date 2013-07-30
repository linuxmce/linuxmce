import QtQuick 2.0

Rectangle {
    id:welcome
    height: manager.appHeight
    width: manager.appWidth
    color: wait.running ? "darkyellow": "darkgreen"

    Timer{
        id:wait
        interval: 3500
        onTriggered: {mainContent.source = "http://"+window.router+"/lmce-admin/skins/android/qt5/Splash.qml"; }
        running:false
        Component.onCompleted: wait.start()
    }
    Rectangle{
        id:spinner
        height:  parent.height *.10
        width: parent.width*.25
        color: "green"  //rotation > 270 ? "darkgrey" : "white"
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
        text: qsTr("Connecting to "+window.router+", please be patient \n" )
        anchors.centerIn: parent
        font.pixelSize: 16
        font.family: "Droid"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        opacity: .85
    }

    Loader{
        id:mainContent
        height: manager.appHeight
        width: manager.appWidth
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1500}
        onStatusChanged: if(mainContent.status === Loader.Error){
                             loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server at "+window.router+" Please ensure you can reach your core. \n I will continue trying. \n"+ mainContent.sourceComponent.errorString())
                             console.log(mainContent.sourceComponent.errorString())
                             wait.restart()
                         }
        //                         else if (mainContent.status != Loader.Loading){
        //                             loading.text = "Loading, please wait \n" + progress +"% \n"+sourceComponent.errorString()

        //                         }
                         else  if (mainContent.status === Loader.Ready){
                             mainContent.opacity = .01
                             loading.visible= false
                             loading.text = "Content Loaded, one moment"
                         }

    }



}
