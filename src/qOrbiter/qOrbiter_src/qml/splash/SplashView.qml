// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Item {
   anchors.fill: parent
    id:splashPage

    signal setupStart(string x, string y)
    signal splashLoaded()

    Connections{
        target: window
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

    Item{
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        height:parent.height *.08

        Rectangle{
            id:welcome_container
            color:"black"
            anchors.fill: parent
        }

        Text {
            id: welcome
            text: qsTr(" Welcome to LinuxMCE")
            //font.family: myFont.name
            font.pointSize: 42
            anchors.top: parent.top
            anchors.left:parent.left
            anchors.margins: scaleX(5)
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
        }
        Text {
            id: welcome_select
            text: qsTr("Please Choose an orbiter")
            //font.family: myFont.name
            font.pointSize: 42
            font.weight: Font.Light
            anchors.top: welcome.bottom
            anchors.left:parent.left
            anchors.margins: scaleX(5)
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
        }
    }

    StatusRow {
        id: statusRow
    }


    Connections{
        target:window
        onMessageChanged:loadingStatus.text = window.message
        onStatusChanged: screenchange("SetupNewOrbiter.qml")
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
        font.pointSize: 14
        font.family: myFont.name
        color: "white"
        anchors.top: connectionBox.bottom
        anchors.horizontalCenter: connectionBox.horizontalCenter
    }

    ExistingOrbiters {
        id:existing_orbiters
    }

    NewOrbiterButton {
        id:newOrbiterButton
    }

}




