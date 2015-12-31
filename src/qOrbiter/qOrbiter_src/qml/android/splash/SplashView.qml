// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    height: manager.appHeight
    width: manager.appWidth
    id:splashPage   
    signal setupStart(string x, string y)
    signal splashLoaded()
    focus:true

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
        font.family: myFont.name
        font.pointSize: headerTextSize
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

    ExistingOrbiters {
        id: existing_orbiters
    }

    NewOrbiterButton {
        id: newOrbiterButton
    }

    Text {
        id: loadingStatus
        text: "Status::" + manager.commandResponse
        anchors.topMargin: scaleY(15)
        font.pixelSize: infoTextSize
        font.family: myFont.name
        color: "white"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: connectionBox.horizontalCenter
    }
}




