/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.1

Item {
    id: splashLogic

    height: appH
    width: appW


    property bool orbiterSetup:false
    property string router_ip: ""

    onOrbiterSetupChanged:{  console.log(orbiterSetup) ; existing_orbiters.visible = false; orbiter_options.visible = true; newOrbiterOptionContainer.visible=true; window.showSetup()}
    onWidthChanged: console.log("detected size change")

    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: "default/img/icons/backgrounds/bedroom.png"
        anchors.fill: parent
    }
    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../skins-common/fonts/Sawasdee.ttf"
    }

    function scaleX(x){
        return x/100*appW
    }
    function scaleY(y){
        return y/100*appH
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

    //    Connections{
    //        target:window
    //        onMessageChanged:loadingStatus.text = window.message
    //        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
    //    }
}
