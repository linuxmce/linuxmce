/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.2

Item {
    id: splashLogic
    height: 720
    width: 1280

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Component.onCompleted: {
        pageLoader.source= "SplashView.qml"
    }

    property bool orbiterSetup:false
    property string router_ip: ""

    onOrbiterSetupChanged:{
        console.log(orbiterSetup);
        existing_orbiters.visible = false;
        orbiter_options.visible = true;
        newOrbiterOptionContainer.visible=true;
        orbiterWindow.showSetup()
    }

    onWidthChanged: console.log("detected size change")
    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
        source: "../noir/img/icons/bedroom.jpg"
        anchors.fill: parent
    }
    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
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
      //  source:
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
