/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.2

import "."

Item {
    id: splashLogic
    anchors.fill: parent

    Style{
        id:theme
    }

    Component{
        id:splashViewComp
        SplashView{

        }
    }

    Component{
        id:newOrbiter
        SetupNewOrbiter{

        }
    }

    Component{
        id:loadingViewComp
        LoadingView{}
    }

    function scaleX(w){
        return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

    Component.onCompleted: {
        console.log("Splash is loaded")
        orbiterWindow.qmlSetupLmce(manager.iPK_Device, manager.currentRouter)
        splashLogic.forceActiveFocus()
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
        splashLogic.state="new-orbiter"
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
        sourceComponent: splashViewComp
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
            when: manager.iPK_Device !=-1 && orbiterWindow.b_connectionPresent
            PropertyChanges {
                target:splash_content
                // sourceComponent:splashViewComp
            }
        },
        State {
            when:!orbiterWindow.b_connectionPresent
            name: "disconnected"
            PropertyChanges {
                target:splash_content
                // sourceComponent:splashViewComp
            }
        },
        State {
            name: "new-orbiter"
            PropertyChanges {
                target: splash_content
                sourceComponent:newOrbiter
            }
        },

        State {
            name: "no-connection"
            when:!orbiterWindow.b_connectionPresent
            PropertyChanges {
                target: splash_content
                sourceComponent:splashViewComp
            }
        }

    ]
}
