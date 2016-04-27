/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.0



Rectangle {
    id: rectangle1
    width: manager.appWidth
    height: manager.appHeight
    onHeightChanged: updateOrientation()
    color: "slategrey"
    signal setupStart(string x, string y)
    signal splashLoaded()

    function updateOrientation(){
        rectangle1.height =manager.appHeight
        rectangle1.width = manager.appWidth
    }

    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectCrop
        source: "../default/img/background.png"
        anchors.fill: parent
    }

    Column{
        x: 9
        y: 169
        height: parent.height
        width: 150
        spacing: 2

        Text {
            id: connection_present
            text: qsTr("Connection")
            color: orbiterWindow.b_connectionPresent ? "transparent" : "red"
            font.pointSize: orbiterWindow.b_connectionPresent ? 14 : 12
        }

        Text {
            id: device
            text: qsTr("Device")
            color: orbiterWindow.b_devicePresent ? "transparent" : "red"
            font.pointSize: orbiterWindow.b_devicePresent ? 14 : 12
        }

        Text {
            id: configuration_file
            text: qsTr("Config")
            color: orbiterWindow.b_localConfigReady ? "red" : "green"
            font.pointSize: orbiterWindow.b_localConfigReady ? 14 : 12
        }

        Text {
            id: skin_index
            text: qsTr("Skins")
           // opacity: 0
        }

        Text {
            id: orbiter_config
            text: qsTr("Orbiter Config")
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
        target:orbiterWindow
        onMessageChanged:loadingStatus.text = orbiterWindow.message
        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
    }

    Rectangle {
        width: parent.width
        height: 100
        opacity: 1
        color: "transparent"
        Text {
            id: welcome
            text: qsTr("Welcome To LinuxMCE!")
            font.pointSize: 18
            font.italic: true
            font.family: "Droid Sans"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

    }
    Rectangle {
        id: rectangle2
        x: -24
        y: 87
      anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: 314
        height: 83
        radius: 7
        anchors.verticalCenterOffset: -124
        anchors.horizontalCenterOffset: 1
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
        font.pixelSize: 12
        font.bold: true
        anchors.top: rectangle2.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{
        anchors.centerIn: rectangle2
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Text {
            text: qsTr("Host:")
            font.pixelSize: 11
            font.family: "Droid Sans"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput {
            id: routerip
            width: 80
            text: srouterip
            font.pointSize: 10
            font.family: "Droid Sans"
          //  onTextChanged: setRouterIp(routerip.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("Device:")
            font.pixelSize: 11
            font.family: "Droid Sans"
            anchors.verticalCenter: parent.verticalCenter
        }
        TextInput {
            id: devicenumber
            width: 25
            text: deviceid
            font.family: "Droid Sans"
            font.pixelSize: 12
            //onTextChanged: setDeviceNo(devicenumber.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter

        }

      Rectangle {
            id: connectbutton
            height: 25
            width: 41
            color:"red"
            Text {
                id: name
                x: 10
                y: 6
                text: qsTr("Go!")
                font.pixelSize: 12
                verticalAlignment: Text.AlignTop
                font.bold: true
            }

            radius:  5
            MouseArea{
                hoverEnabled: true
                onEntered: parent.color="green"
                onExited: parent.color="red"
                anchors.fill: parent
            onClicked: orbiterWindow.qmlSetupLmce(devicenumber.text, routerip.text)
            anchors.verticalCenter: parent.verticalCenter
            }
        }

      Rectangle {
            id: exitbutton
            height: 25
            width: 45

            Text {
                id: exitlabel
                x: 13
                y: 6
                text: qsTr("Exit")
                font.pixelSize: 11
            }

            radius:  4
            MouseArea{
            onClicked: closeOrbiter()
            anchors.verticalCenter: parent.verticalCenter
            }

        }
    }
    Text {
        id: loadingStatus
        x: 33
        y: 423
        text: "Status " + orbiterWindow.message
        font.pixelSize: 14
        font.family: "Droid Sans"
        color: "white"
        anchors.top: rectangle2.bottom
    }
}
