// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2


Item {
  anchors.fill: parent
    id:splashPage

    signal setupStart(string x, string y)
    signal splashLoaded()


    Connections{
        target: orbiterWindow
        onShowList:{

            if(existing_orbiters.count === 0){
                screenChange("NewOrbiterSetup.qml")
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
        font.pointSize: 42
        anchors.top: parent.top
        anchors.topMargin: scaleY(5)
        anchors.left: parent.left
        font.bold: false
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
        font.family:"Roboto"
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

}




