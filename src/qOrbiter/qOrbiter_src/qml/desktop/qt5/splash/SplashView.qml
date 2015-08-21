// import QtQuick 2.2
 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2


Rectangle {
    height:manager.appHeight
    width:manager.appWidth
    id:splashPage
    color: "transparent"
    signal setupStart(string x, string y)
    signal splashLoaded()

    Connections{
        target: orbiterWindow
        onShowList:{

            if(existing_orbiters.count === 0){
                screenchange("SetupNewOrbiter.qml")
            }
            else{
                existing_orbiters.state="showing"
                newOrbiterButton.state="showing"
                connectionBox.state="hidden"
            }
        }
        onPageChanged:screenchange(qmlPage)
    }
    Text {
        id: welcome
        text: qsTr("LinuxMCE")
        font.family: myFont.name
        font.pointSize: 42
        anchors.top: parent.top
        anchors.topMargin: scaleY(5)
        anchors.horizontalCenter: parent.horizontalCenter
        color:"white"
    }


    StatusRow {
        id: statusRow
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
        font.family: myFont.name
        color: "white"
        anchors.top: connectionBox.bottom
        anchors.horizontalCenter: connectionBox.horizontalCenter
    }

    ExistingOrbiters {
        id: existing_orbiters
    }

    NewOrbiterButton {
        id: newOrbiterButton
    }

    FirstRunOptions {
        id: firstRunOptions
        visible:manager.first_run
        anchors.bottom: parent.bottom
    }

}




