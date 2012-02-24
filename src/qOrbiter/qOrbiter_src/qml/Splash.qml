/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.1



Rectangle {
    id: rectangle1
    height:720
    width:1280
    onWidthChanged: console.log("detected size change")

    function scaleX(x){
        return x/100*appH
    }
    function scaleY(y){
        return y/100*appW
    }

    Connections{
        target: window
        onShowList:existing_orbiters.visible = true
        onShowExternal: ext_routerip.visible = true
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

    Column{

        height: childrenRect.height
        width: scaleX(40)
        spacing: 2

        Text {
            id: connection_present
            text: qsTr("Connection")
            color: window.b_connectionPresent ? "transparent" : "red"
            font.pointSize: window.b_connectionPresent ? 14 : 12
        }

        Text {
            id: device
            text: qsTr("Device")
            color: window.b_devicePresent ? "transparent" : "red"
            font.pointSize: window.b_devicePresent ? 14 : 12
        }

        Text {
            id: configuration_file
            text: qsTr("Config")
            color: window.b_localConfigReady ? "red" : "green"
            font.pointSize: window.b_localConfigReady ? 14 : 12
        }

        Text {
            id: skin_index
            text: qsTr("Skins")
            opacity: 0
        }

        Text {
            id: orbiter_config
            text: qsTr("Orbiter Config")
            opacity: 0
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
        height: scaleY(20)
        opacity: 1
        color: "transparent"

    }
    Rectangle {
        id: rectangle2

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset:10
        width: scaleX(99)
        height: scaleY(20)
        radius: 7
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
        font.pixelSize: 11
        font.bold: false
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
            font.pixelSize: 10
            font.family: "Droid Sans"
            //  onTextChanged: setRouterIp(routerip.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput {
            id: ext_routerip
            width: 80
            text: extip
            font.pixelSize: 10
            font.family: "Droid Sans"
            //  onTextChanged: setRouterIp(routerip.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter
            visible: false
        }

        Text {
            text: qsTr("Device:")
            font.pixelSize: 11
            font.family: "Droid Sans"
            anchors.verticalCenter: parent.verticalCenter
        }
        TextInput {
            id: devicenumber
            width: scaleX(10)
            text: deviceid
            font.family: "Droid Sans"
            font.pixelSize: 12
            //onTextChanged: setDeviceNo(devicenumber.text)
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter

        }

        Rectangle {
            id: connectbutton
            height: scaleY(5)
            width: scaleX(10)
            color:"red"

            anchors.left: devicenumber.right
            anchors.leftMargin: scaleX(5)
            Text {
                id: name
                anchors.centerIn: parent
                anchors.fill: parent
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
                onClicked: window.qmlSetupLmce(devicenumber.text, routerip.text)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: exitbutton
            height: scaleY(5)
            width: scaleX(10)
            anchors.left: connectbutton.right
            anchors.leftMargin: scaleX(5)
            Text {
                id: exitlabel
                anchors.centerIn: parent
                anchors.fill: parent
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

        text: "Status " + window.message
        anchors.topMargin: -17
        font.pixelSize: 14
        font.family: "Droid Sans"
        color: "white"
        anchors.bottom: rectangle2.bottom
        anchors.horizontalCenter: rectangle2.horizontalCenter
    }

    ListView{
        id:existing_orbiters
        height: scaleY(16)
        width: scaleX(55)
        clip: true
        anchors.centerIn: rectangle2
        model:orbiterList
        visible: false
        delegate: Rectangle{
            id:existing_orbiter_delegate
            height: scaleY(10)
            width: existing_orbiters.width
            color: "slategrey"
            border.color: "white"
            border.width: 1
            Column
            {
                height: childrenRect.height
                width: existing_orbiters.width

                Text {
                    id: orbiter_label
                    text: qsTr("Orbiter:")+ label
                    font.pixelSize: scaleY(1.75)
                    width: existing_orbiters.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Text {
                    id: dev_num
                    width: existing_orbiters.width
                    text:qsTr("Device:")+ i_device_number
                    font.pixelSize: scaleY(2)
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }



            MouseArea {
                anchors.fill: parent
                onClicked: window.qmlSetupLmce(i_device_number, routerip.text)
            }
        }
    }
}
