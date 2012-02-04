/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.0



Rectangle {
    id: rectangle1
    width: 480
    height: 800
    color: "slategrey"
    signal setupStart(string x, string y)
    signal splashLoaded()

    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/desktop_splash.png"
        anchors.fill: parent
    }
    Text {
        id: welcome
        text: qsTr("Welcome To LinuxMCE!")
        font.pixelSize: 14
        color: "darkgrey"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
    }

    Column{
        height: parent.height
        width: 150
        spacing: 2
        anchors.left: parent.left

        Text {
            id: connection_present
            text: qsTr("Connection")
            color: window.b_connectionPresent ? "transparent" : "red"
            font.pointSize: window.b_connectionPresent ? 12 : 12
        }

        Text {
            id: device
            text: qsTr("Device")
            color: window.b_devicePresent ? "transparent" : "red"
            font.pointSize: window.b_devicePresent ? 12 : 12
        }

        Text {
            id: configuration_file
            text: qsTr("Config")
            color: window.b_localConfigReady ? "red" : "green"
            font.pointSize: window.b_localConfigReady ? 12 : 12
        }

        Text {
            id: skin_index
            text: qsTr("Skins")
            font.pointSize: window.b_localConfigReady ? 12 : 12
           // opacity: 0
        }

        Text {
            id: orbiter_config
            text: qsTr("Orbiter Config")
            font.pointSize: window.b_localConfigReady ? 12 : 12
           // opacity: 0
        }

    }

    function screenchange(screenname )
    {
       pageLoader.source = "qrc:/desktop/"+screenname
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
        target:window
        onMessageChanged:loadingStatus.text = window.message
        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
    }

    Rectangle {
        width: parent.width
        height: 100
        opacity: 1
        color: "transparent"


    }
    Rectangle {
        id: rectangle2
      anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: rectangle1.width
        height: 57
        radius: 7
        anchors.centerIn: rectangle1
        border.width: 1
        border.color: "#000000"
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "transparent"
            }
            GradientStop {
                position: 1
                color: "#000000"
            }
        }
        opacity: 0.5
    }
    Text {
        id: connectionlabel
        text: qsTr("Set Connection Details")
        font.bold: true
        font.pixelSize: 12
        anchors.top: rectangle2.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{
        anchors.centerIn: rectangle2
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Text {
            text: qsTr("Host:")
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput {
            id: routerip
            width: 110
            text: srouterip
            font.pixelSize: 12
          //  onTextChanged: setRouterIp(routerip.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("Device:")
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12

        }
        TextInput {
            id: devicenumber
            width: 25
            text: deviceid
            font.pixelSize: 12
            //onTextChanged: setDeviceNo(devicenumber.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter

        }

      Rectangle {
            id: connectbutton
            height: 25
            width: 75
            color:"red"
            Text {
                id: name
                text: qsTr("Connect!")

                font.pixelSize: 12
            }

            radius:  5
            MouseArea{
                hoverEnabled: true
                onEntered: parent.color="green"
                onExited: parent.color="red"
                anchors.fill: parent
            onClicked: window.qmlSetupLmce(devicenumber.text, routerip.text)
            anchors.verticalCenter: parent.verticalCenter
            }
        }

      Rectangle {
            id: exitbutton
            height: 25
            width: 50

            Text {
                id: exitlabel
                text: qsTr("Exit")
                font.pixelSize: 16
            }

            radius:  4
            MouseArea{
                anchors.fill:parent
            onClicked: Qt.quit
            anchors.verticalCenter: parent.verticalCenter
            }

        }
    }
    Text {
        id: loadingStatus
        text: "Status " + window.message
        font.pointSize: 14
        color: "darkgrey"
        font.bold: true
        anchors.bottom: rectangle1.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
