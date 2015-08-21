/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.2

Item {
    id: splashLogic
   height: manager.appHeight
   width: manager.appWidth
    property int infoTextSize:24
    property int headerTextSize:42
    property int startButtonHeight:45
    property int startButtonWidth:100
    property bool orbiterSetup:false

   Component.onCompleted: console.log("splash loaded")


    onOrbiterSetupChanged:{  console.log(orbiterSetup) ;
        existing_orbiters.visible = false;
        orbiter_options.visible = true;
        newOrbiterOptionContainer.visible=true; orbiterWindow.showSetup()
    }
    onWidthChanged: console.log("detected size change")
    Image {
        id: splash
        fillMode: Image.PreserveAspectCrop
        anchors.centerIn: parent
        source: "lmcesplash.jpg"
        anchors.fill: parent
  }
    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
    }

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }

    Connections{
        target: orbiterWindow
        onShowExternal: {
            console.log("showing external ip box")
            ext_routerip.visible = true
        }
    }

    function screenchange(screenname )
    {
        pageLoader.source = screenname
        if (pageLoader.status == 1)
        {
            //manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
        }
        else
        {
            console.log("Command to change to:" + screenname + " failed!")

        }
    }


    Loader {
        id:pageLoader
        objectName: "loadbot"
        source: "SplashView.qml"
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
    }


}
