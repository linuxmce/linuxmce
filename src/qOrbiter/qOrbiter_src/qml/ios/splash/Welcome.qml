import QtQuick 2.2

Item {
    id:welcome
    height: manager.appHeight
    width: manager.appWidth
    Component.onCompleted: forceActiveFocus()
    focus:true

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Rectangle{
        anchors.fill: parent
        opacity:.25
        gradient: Gradient{
            GradientStop{position: 0.0; color: "transparent"}
            GradientStop{position: .45; color:"white"}
        }
    }


    Keys.onReleased: {

        switch(event.key){
        case Qt.Key_Back:
            Qt.quit()
            break;
        case Qt.Key_MediaPrevious:
            Qt.quit()
            break;
        default:
            console.log(event.key)
            break;

        }
        event.accepted=true
    }

    Timer{
        id:wait
        interval: 3500
        onTriggered: {mainContent.source = "http://"+orbiterWindow.router+"/lmce-admin/skins/ios/Splash.qml"; }
        running:false
        Component.onCompleted:
        {
            console.log("Wait timer checking")
            forceActiveFocus()
            wait.start()
        }
    }

    Text {
        id: loading
        text: qsTr("Connecting to "+orbiterWindow.router+", please be patient \n" )
        anchors.top: parent.top
        font.pixelSize: 28
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        font.weight: Font.Light
    }

    Text {
        id: splashtxt
        text: qsTr("LinuxMCE for your "  )
        anchors.bottom: parent.bottom
        font.pixelSize:36
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "green"
        anchors.horizontalCenter: parent.horizontalCenter
        font.weight: Font.Light
    }

    Item{
        id:fil
        anchors.centerIn: parent
        height: 45
        width: 400

        Rectangle{

            anchors.fill: parent
            color:"green"
            border.color: "white"
            border.width: 1
            radius:5
            opacity:switched ? 1 : 0
            property bool switched:false

            Timer{
                id:sw
                running:true
                interval:800

                onTriggered: {
                    parent.switched = !parent.switched
                }
                repeat: true
            }

            Behavior on opacity{
                PropertyAnimation{
                    duration: 750
                }
            }
        }
    }

    Loader{
        id:mainContent
        height: manager.appHeight
        width: manager.appWidth
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1500}
        onStatusChanged: if(mainContent.status === Loader.Error){
                             loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server at "+orbiterWindow.router+" Please ensure you can reach your core. \n I will continue trying. \n"+ mainContent.sourceComponent.errorString())
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
