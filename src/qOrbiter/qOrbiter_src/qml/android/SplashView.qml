// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.android 1.1

Rectangle {
    height:appH
    width:appW
    color: "slategrey"
    signal setupStart(string x, string y)
    signal splashLoaded()

    Connections{
        target: window
        onShowList:existing_orbiters.visible = true
        onPageChanged:screenchange(qmlPage)
    }
    Image {
        id: splash
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/desktop_splash.png"
        anchors.fill: parent
    }

    Row{

        height: childrenRect.height
        width: scaleX(85)
        spacing: scaleX(10)

        Rectangle{

            id:connection_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"

            Text {
                id: connection_label
                text: qsTr("Connection")
                color: window.b_connectionPresent ? "green" : "red"
                font.pixelSize: window.b_connectionPresent ? 14 : 12
            }
        }

        Rectangle{
            id:device_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: device_icon
                source: ""
            }

            Text {
                id: device_Label
                text: qsTr("Device")
                color: window.b_devicePresent ? "green" : "red"
                font.pixelSize: window.b_devicePresent ? 14 : 12
            }
        }
/*
        Rectangle{
            id:config_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: config_icon
                source: ""
            }

            Text {
                id: config_label
                text: qsTr("Config")
                color: window.b_localConfigReady ? "green" : "red"
                font.pixelSize: window.b_localConfigReady ? 14 : 12
            }
        }

        Rectangle{
            id:skin_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: skin_icon
                source: ""
            }

            Text {
                id: skin_label
                text: qsTr("Skins")
                color: window.b_skinIndexReady ? "green" : "red"
                font.pixelSize: window.b_skinIndexReady ? 14 : 12
            }
        }

        Rectangle{
            id:skindata_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: skinData_icon
                source: ""
            }

            Text {
                id: skin_data_label
                text: qsTr("Orbiter Ready")
                color: window.b_orbiterConfigReady ? "green" : "red"
                font.pixelSize: window.b_orbiterConfigReady ? 14 : 12
            }
        }
*/

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
        anchors.centerIn: parent
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
                onClicked: ext_routerip.visible ? window.qmlSetupLmce(devicenumber.text, ext_routerip.text) : window.qmlSetupLmce(devicenumber.text, routerip.text)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: exitbutton
            height: scaleY(5)
            width: scaleX(6)
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
        text: "Status " + dcerouter.commandResponse
        anchors.topMargin: scaleY(15)
        font.pixelSize: 14
        font.family: "Droid Sans"
        color: "white"
        anchors.top: rectangle2.bottom
        anchors.horizontalCenter: rectangle2.horizontalCenter
    }

    ListView{
        id:existing_orbiters
        height: scaleY(35)
        width: scaleX(55)
        clip: true
        anchors.centerIn: rectangle2
        model:orbiterList
        visible: false
        opacity: existing_orbiters.visible
        delegate: Rectangle{
            id:existing_orbiter_delegate
            height: scaleY(5)
            width: existing_orbiters.width
            color: "slategrey"
            border.color: "white"
            border.width: 1
            Column
            {
                height: childrenRect.height
                width: childrenRect.width
                anchors.centerIn: parent

                Text {
                    id: orbiter_label
                    text: qsTr("Orbiter:")+ label
                    font.pixelSize: 12
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Text {
                    id: dev_num
                    text:qsTr("Device:")+ i_device_number
                    font.pixelSize: 12
                    font.italic: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: window.qmlSetupLmce(i_device_number, routerip.text)
            }
        }
    }

    Rectangle{
        id:newOrbiterButton
        height: scaleY(10)
        width:scaleX(55)
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

        Text {
            id: newOrbiterLabel
            text: qsTr("Create New Orbiter?")
            font.pixelSize: 15
            width: parent.width
            anchors.centerIn: parent
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        visible: existing_orbiters.visible
        anchors.bottom: existing_orbiters.top
        anchors.horizontalCenter: existing_orbiters.horizontalCenter
        MouseArea{
            anchors.fill: parent
            onClicked:screenchange("SetupNewOrbiter.qml")
        }
    }

}




