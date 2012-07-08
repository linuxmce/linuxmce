// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
// this is for android platform specifically

import QtQuick 1.1

Rectangle {
    id: rectangle1
    height: appH
    width: appW
    focus: true
    onScaleChanged: console.log("scale changed")

    Connections{
        target: window
        onOrientationChanged:updateRez()
    }

    Connections{
        target:window
        onDeviceChanged: pageLoader.source ? ":/main/SetupNewOrbiter.qml": "Splash.qml"
    }

    Connections{
        target: window
        onShowList:existing_orbiters.visible = true
        onShowExternal: ext_routerip.visible = true
    }

    function scaleX(x){
        return x/100*appW
    }
    function scaleY(y){
        return y/100*appH
    }

    function updateRez()
    {
        rectangle1.height = appH
        rectangle2.width = appW
    }

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

    Row{

        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleX(2)

        Text {
            id: connection_present
            text: qsTr("Connection")
            color: window.b_connectionPresent ? "green" : "red"
            font.pixelSize: window.b_connectionPresent ?  scaleY(3) : scaleY(4)
        }

        Text {
            id: device
            text: qsTr("Device")
            color: window.b_devicePresent ? "green" : "red"
            font.pixelSize: window.b_devicePresent ?  scaleY(3) : scaleY(4)
        }

        Text {
            id: configuration_file
            text: qsTr(" Local Config")
            color: window.b_localConfigReady ? "green" : "red"
            font.pixelSize: window.b_localConfigReady ?  scaleY(3) : scaleY(4)
        }

        Text {
            id: skins
            text: qsTr("Skins Location")
            color: window.b_localConfigReady ? "green" : "red"
            font.pixelSize: window.b_skinDataReady ?  scaleY(3) : scaleY(4)
        }

        Text {
            id: skin_index
            text: qsTr("Skins Loaded")
            color: window.b_localConfigReady ? "green" : "red"
            font.pixelSize: window.b_skinIndexReady ?  scaleY(3) : scaleY(4)
        }

        Text {
            id: orbiter_config
            text: qsTr("Orbiter Config")
            color: window.b_localConfigReady ? "green" : "red"
            font.pixelSize: window.b_orbiterConfigReady ?  scaleY(3) : scaleY(4)

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


    Text {
        id: welcome
        text: qsTr("Welcome To LinuxMCE!")
        font.pixelSize: scaleY(4)
        font.italic: true
        font.family: "Droid Sans"
        anchors.bottom: rectangle2.top
        anchors.bottomMargin: scaleY(5)
        anchors.horizontalCenter: parent.horizontalCenter

    }
    MouseArea{
        anchors.fill: parent
        onPressed: parent.focus = true
    }

    Rectangle {
        id: rectangle2

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset:10
        width: scaleX(100)
        height: scaleY(20)
        radius: 7
        anchors.horizontalCenterOffset: 1
        anchors.centerIn: rectangle1
        border.width: 1
        border.color: "green"
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
        font.pixelSize: scaleY(4)
        font.bold: false
        anchors.top: rectangle2.top
        anchors.horizontalCenter: parent.horizontalCenter

    }



    Text {
        text: qsTr("Host")
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.bottom: routerip.top
        anchors.horizontalCenter: routerip.horizontalCenter
    }

    TextInput {
        id: routerip
        width: scaleX(25)
        text: srouterip
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        //  onTextChanged: setRouterIp(routerip.text)
        fillColor: "grey"
        anchors.verticalCenter: rectangle2.verticalCenter
        anchors.verticalCenterOffset: scaleY(2)
        anchors.left: rectangle2.left
        anchors.leftMargin: scaleX(20)
        onFocusChanged: routerip.focus ? routerip.opacity = 1 : routerip.opacity = .5


    }

    Text {
        text: qsTr("Device:")
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.bottom: devicenumber.top
        anchors.horizontalCenter: devicenumber.horizontalCenter
    }
    TextInput {
        id: devicenumber
        width: scaleX(10)
        text: deviceid
        font.family: "Droid Sans"
        font.pixelSize: scaleY(3)
        //onTextChanged: setDeviceNo(devicenumber.text)
        fillColor: "grey"
        focus: false
        anchors.verticalCenter: routerip.verticalCenter
        anchors.left: routerip.right
        anchors.leftMargin: scaleX(10)
        onFocusChanged: devicenumber.focus ? devicenumber.opacity = 1 : devicenumber.opacity = .5
    }

    Rectangle {
        id: connectbutton
        height: scaleY(5)
        width: scaleX(10)
        color:"red"

        anchors.left: devicenumber.right
        anchors.leftMargin: scaleX(5)
        anchors.verticalCenter: devicenumber.verticalCenter
        Text {
            id: name
            anchors.centerIn: parent
            anchors.fill: parent
            text: qsTr("Go!")
            font.pixelSize: scaleY(3)
            verticalAlignment: Text.AlignTop
            font.bold: true
        }

        radius:  5
        MouseArea{
            hoverEnabled: true
            onEntered: parent.color="green"
            onExited: parent.color="red"
            anchors.fill: parent
            onClicked: window.qmlSetupLmce(devicenumber.text, routerip.text)
            anchors.verticalCenter: devicenumber.verticalCenter
        }
    }

    Rectangle {
        id: exitbutton
        height: scaleY(5)
        width: scaleX(10)
        anchors.left: connectbutton.right
        anchors.leftMargin: scaleX(5)
        anchors.verticalCenter: devicenumber.verticalCenter
        radius:  4

        Text {
            id: exitlabel
            anchors.centerIn: parent

            text: qsTr("Exit")
            font.pixelSize: scaleY(3)
        }
        MouseArea{
            anchors.fill: parent
            onClicked:console.log("exit")

        }
    }

    Text {
        id: loadingStatus

        text: "Status " + window.message
        anchors.topMargin: -17
        font.pixelSize: scaleY(5)
        font.family: "Droid Sans"
        color: "white"
        anchors.bottom: rectangle2.bottom
        anchors.horizontalCenter: rectangle2.horizontalCenter
    }

    ListView{
        id:existing_orbiters
        height: scaleY(65)
        width: scaleX(75)
        anchors.centerIn: parent
        model:orbiterList
        visible: false
        delegate: Rectangle{
            id:existing_orbiter_delegate
            height: scaleY(20)
            width: scaleX(70)
            color: "slategrey"
            border.color: "white"
            border.width: 1
            Text {
                id: orbiter_label
                text: label
                font.pixelSize: scaleY(2)
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
            Text {
                id: dev_num
                anchors.left: orbiter_label.right
                text:qsTr("Device:")+ i_device_number
                 font.pixelSize: scaleY(2)
                 wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            MouseArea {
                anchors.fill: parent
                onClicked: window.qmlSetupLmce(i_device_number, routerip.text)
            }
        }
    }
}
