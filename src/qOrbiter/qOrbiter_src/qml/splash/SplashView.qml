// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.3
import "."
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
        id:topContainer
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        height:parent.height *.08

        Rectangle{
            id:welcome_container
            color:Style.appcolor_background
            anchors.fill: parent
            opacity: .15
        }

        Text {
            id: welcome
            text: qsTr(" Welcome to LinuxMCE")
            //font.family: myFont.name
            font.pointSize: Style.appFontSize_header
            anchors.left:parent.left
            anchors.horizontalCenter: parent.horizontalCenter
            color:"white"
        }
        Text {
            id: welcome_select
            text: qsTr("Please Choose an orbiter")
            //font.family: myFont.name
            font.pointSize: Style.appFontSize_description
            //font.weight: Font.Light
            anchors.left:welcome.right
            color:"white"
        }
        Text {
            id: ui_name
            text: "DefaultUI"
            //font.family: myFont.name
            font.pointSize: Style.appFontSize_description
            font.weight: Font.Light
            anchors.left:parent.right
            color:"white"
        }
    }

//    StatusRow {
//        id: statusRow
//    }


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


//    Text {
//        id: loadingStatus
//        text: "Status " + manager.commandResponse
//        font.pointSize: Style.appFontSize_description
//        color: "white"
//        anchors.top: topContainer.bottom
//        anchors.horizontalCenter: connectionBox.horizontalCenter
//    }

    ExistingOrbiters {
        id:existing_orbiters
    }

    NewOrbiterButton {
        id:newOrbiterButton
    }

}




