/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.0

Item {
    id: splashLogic
    height:appH
    width:appW
    property bool orbiterSetup:false
    Component.onCompleted: screenchange("SplashView.qml")

    onOrbiterSetupChanged:{  console.log(orbiterSetup) ; existing_orbiters.visible = false; orbiter_options.visible = true; newOrbiterOptionContainer.visible=true; orbiterWindow.showSetup()}
    onWidthChanged: console.log("detected size change")

    function scaleX(x){
        return x/100*appH
    }
    function scaleY(y){
        return y/100*appW
    }

    Connections{
        target: orbiterWindow
        onShowList:existing_orbiters.visible = true      
        onPageChanged:screenchange(qmlPage)

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
