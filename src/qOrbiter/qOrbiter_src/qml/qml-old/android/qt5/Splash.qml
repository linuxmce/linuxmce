/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.2

Rectangle {
    id: splashLogic
    height:manager.appHeight
    width:manager.appWidth
    property bool orbiterSetup:false
    color:"black"

    onOrbiterSetupChanged:{
        console.log(orbiterSetup) ;
        existing_orbiters.visible = false;
        orbiter_options.visible = true;
        newOrbiterOptionContainer.visible=true; orbiterWindow.showSetup()
    }
    onWidthChanged: console.log("detected size change")
    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
        source: "../tablet/default/img/icons/backgrounds/bedroom.png"
        anchors.fill: parent
  }

    FontLoader{
        id:myFont
        name:"Roboto"
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
        height:parent.height
        width: parent.width
        objectName: "loadbot"
        source:manager.isPhone===2 ? "SplashView.qml" : "SplashViewPhone.qml"
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
    }

    Connections{
        target:orbiterWindow
        onMessageChanged:loadingStatus.text = orbiterWindow.message
        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
    }
}
