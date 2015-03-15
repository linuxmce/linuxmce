/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.3
import "."
Item {
    id: splashLogic
    anchors.fill: parent
    function scaleX(w){
        return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

    Component.onCompleted: {
        console.log("Splash is loaded")
        splashLogic.state="connecting"
        window.qmlSetupLmce(window.deviceno, window.router)
        splashLogic.forceActiveFocus()
        state="connecting"
    }
    focus:true

    Keys.onTabPressed: manager.setDesiredOrientation(manager.isProfile ? Qt.LandscapeOrientation : Qt.PortraitOrientation)
    Keys.onPressed: {
        switch(event.key){
        case Qt.Key_F5 : manager.qmlReload(); break;
        default: break;
        }
    }


    function screenchange(screenname ){
        console.log(screenname)
    }

    Connections{
        target: window
        onShowList:{
            if(!window.hasOrbiters()){
                splashLogic.state="connected-nodevices"
            }
            else{
                splashLogic.state="connected-devices"
            }
        }
    }

    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
        source: "images/splash_bg.png"
        anchors.fill: parent
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .15
    }

    SplashView{
        id:splash_content
        anchors.fill: parent
    }

    property bool orbiterSetup:false
    property string router_ip: ""

    onOrbiterSetupChanged:{
        splashLogic.state="new-orbiter"
    }


    FontLoader{
        id:myFont
        name:"Sawasdee"
        //  source: "../../skins-common/fonts/Sawasdee.ttf"
    }

    states: [
        State {
            name: "connecting"
            PropertyChanges {
                //target: object

            }
        },State {
            name: "connected-nodevices"
            PropertyChanges {
                // target: object

            }
        }, State {
            name: "connected-devices"
            PropertyChanges {
                //  target: object

            }
        },
        State {
            name: "new-orbiter"
            PropertyChanges {
                // target: object

            }
        }

    ]
}
