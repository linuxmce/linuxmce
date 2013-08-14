import QtQuick 1.1


Rectangle {
    anchors.fill: parent
    color: "black"
    Timer{
        id:wait
        interval: 3500
        onTriggered: {mainContent.source = "http://"+window.router+"/lmce-admin/skins/android/Splash.qml"; }
        running:true
    }

    Rectangle{
        id:fil
        anchors.centerIn: parent
        height: parent.height/2
        width: parent.width/2
        color:"black"
        border.color: "white"
        border.width: 1
        Text {
            id: loading
            text: qsTr("Connecting to "+window.router+", please be patient \n" )
            anchors.centerIn: parent
            font.pixelSize: 16
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: "white"
            opacity: .85
        }

    }

//    Row{
//        anchors.bottom: parent.bottom
//        width: parent.width/2
//        height: childrenRect.height
//        Text{
//            text: "Api Level:"+ android.apiLevel
//            font.pixelSize: 16
//            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//            color: "white"
//            opacity: .85
//        }

//        Text{
//            text: "Api Level:"+ android.externalStorageLocation
//            font.pixelSize: 16
//            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//            color: "white"
//            opacity: .85
//        }
//    }



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
