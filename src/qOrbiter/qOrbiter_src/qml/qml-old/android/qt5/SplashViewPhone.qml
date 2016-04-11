// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2


Item {
    height:manager.appHeight
    width:manager.appWidth
    id:splashPage

    signal setupStart(string x, string y)
    signal splashLoaded()

    Component.onCompleted: {
        if(existing_orbiters.count!==0){
            existing_orbiters.state="showing"
            newOrbiterButton.state="showing"
            connectionBox.state="hidden"
        }
    }

    Connections{
        target: orbiterWindow
        onShowList:{
            console.log("Showing list")
            if(existing_orbiters.count === 0){
                screenChange("NewOrbiterSetup.qml")
            }
            else{

            }
        }
        onPageChanged:screenchange(qmlPage)
    }
    Text {
        id: welcome
        text: qsTr("LinuxMCE")
        font.pointSize: 42
        anchors.top: parent.top
        anchors.topMargin: scaleY(5)
        font.weight: Font.Light
        anchors.horizontalCenter: parent.horizontalCenter

    }

    StatusRow {
        id: statusRow
        anchors.top: parent.top
    }

    Connections{
        target:orbiterWindow
        onMessageChanged:loadingStatus.text = orbiterWindow.message
        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
    }

    Rectangle {
        width: parent.width
        height: scaleY(20)
        opacity: 1
        color: "transparent"

    }
    ConnectionBox {
        id: connectionBox
    }


    Text {
        id: loadingStatus
        text: "Status " + manager.commandResponse
        anchors.topMargin: scaleY(15)
        font.pixelSize: 14
        font.family:"Roboto"
        color: "white"
        anchors.top: connectionBox.bottom
        anchors.horizontalCenter: connectionBox.horizontalCenter
    }

    ExistingOrbiters {
        id: existing_orbiters
    }

//    NewOrbiterButton {
//        id: newOrbiterButton
//    }

}




