// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import "../noir/components"
Item {
//    anchors{
//        top:parent.top
//        left:parent.left
//        right:parent.right
//    }

//    id:splashPage

//    signal setupStart(string x, string y)
//    signal splashLoaded()

//    Connections{
//        target: orbiterWindow
//        onShowList:{

//            if(existing_orbiters.count === 0){
//                screenChange("NewOrbiterSetup.qml")
//            }
//            else{
//                existing_orbiters.state="showing"
//                newOrbiterButton.state="showing"
//                connectionBox.state="hidden"
//            }
//        }
//        onPageChanged:screenchange(qmlPage)
//    }

//    StatusRow {
//        id: statusRow
//    }

//    Text {
//        id: welcome
//        text: qsTr("LinuxMCE\n"+manager.appWidth+" x "+manager.appHeight)
//        font.family: myFont.name
//        font.pointSize: 42
//        anchors.top: statusRow.bottom
//        anchors.topMargin: scaleY(5)
//        anchors.horizontalCenter: parent.horizontalCenter
//        color:"white"
//    }

//    Connections{
//        target:orbiterWindow
//        onMessageChanged:loadingStatus.text = orbiterWindow.message
//        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
//    }


//    ConnectionBox {
//        id: connectionBox
//    }


//    Text {
//        id: loadingStatus
//        text: "Status " + manager.commandResponse
//        anchors.topMargin: scaleY(15)
//        font.pixelSize: 14
//        font.family: myFont.name
//        color: "white"
//        anchors.top: connectionBox.bottom
//        anchors.horizontalCenter: connectionBox.horizontalCenter
//    }

//    ExistingOrbiters {
//        id: existing_orbiters
//    }

//    NewOrbiterButton {
//        id: newOrbiterButton
//    }

}




