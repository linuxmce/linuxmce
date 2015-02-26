/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 2.3
Item {
    id: splashLogic

    width: manager.appWidth
    height: manager.appHeight
    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
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

    Component.onCompleted: {
       // pageLoader.source= "SplashView.qml"
    }

    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
        source: "../images/login-bg.png"
        anchors.fill: parent
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .55
    }
    SplashView{
        anchors.fill: parent
    }

    Loader {
        id:pageLoader
        anchors.fill: parent
        objectName: "loadbot"
        //  source:
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
    }


    property bool orbiterSetup:false
    property string router_ip: ""

    onOrbiterSetupChanged:{
        console.log(orbiterSetup);
       // existing_orbiters.visible = false;
       // orbiter_options.visible = true;
       // newOrbiterOptionContainer.visible=true;
        window.showSetup()
    }

    onWidthChanged: console.log("detected size change")

    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
    }

//    Connections{
//        target:window
//        onMessageChanged:loadingStatus.text = window.message
//        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
//    }
}
