/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.2
import "."

Item {
    id: splashLogic
    height: manager.appHeight
    width: manager.appWidth

    Style{
        id:theme
    }
    function scaleX(w){
        return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

    Component.onCompleted: {
        console.log("Splash is loaded")
        splashLogic.state="connecting"
        window.qmlSetupLmce(manager.iPK_Device, manager.m_ipAddress)
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

    Loader{
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
            when: manager.iPK_Device !==-1

            PropertyChanges {
               target:splash_content
               source:"LoadingView.qml"
            }
        },State {
             when:manager.iPK_Device==-1 && orbiterList.length===0
            name: "connected-nodevices"
            PropertyChanges {
               target: splash_content
               source:"LoadingView.qml"
            }

        }, State {
            when:manager.iPK_Device==-1
            name: "connected-devices"
            PropertyChanges {
               target: splash_content
             source:"SplashView.qml"
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
