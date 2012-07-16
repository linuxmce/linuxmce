/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.0



Rectangle {
    id: splashPage
    height:appH
    width:appW
    property bool orbiterSetup:false
    onOrbiterSetupChanged:{  console.log(orbiterSetup) ; existing_orbiters.visible = false; orbiter_options.visible = true, window.showSetup()}
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
        onShowExternal: {
            console.log("showing external ip box")
            ext_routerip.visible = true
        }

    }

    Connections{
        target:dcerouter
        onDeviceIdChanged:console.log(dcerouter.m_dwPK_Device)
    }

    color: "slategrey"
    signal setupStart(string x, string y)
    signal splashLoaded()

    Image {
        id: splash
        anchors.centerIn: splashPage
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/desktop_splash.png"
        anchors.fill: parent
    }

    Column{

        height: childrenRect.height
        width: scaleX(40)
        spacing: 2

        Rectangle{

            id:connection_indicator
            height: scaleX(5)
            width: scaleX(5)

            Image {
                id: connection_icon
                source: ""
            }

            Text {
                id: connection_label
                text: qsTr("Connection")
                color: window.b_connectionPresent ? "green" : "red"
                font.pointSize: window.b_connectionPresent ? 14 : 12
            }
        }

        Rectangle{
            id:device_indicator
            height: scaleX(5)
            width: scaleX(5)

            Image {
                id: device_icon
                source: ""
            }

            Text {
                id: device_Label
                text: qsTr("Device")
                color: window.b_devicePresent ? "green" : "red"
                font.pointSize: window.b_devicePresent ? 14 : 12
            }
        }

        Rectangle{
            id:config_indicator
            height: scaleX(5)
            width: scaleX(5)

            Image {
                id: config_icon
                source: ""
            }

            Text {
                id: config_label
                text: qsTr("Config")
                color: window.b_localConfigReady ? "green" : "red"
                font.pointSize: window.b_localConfigReady ? 14 : 12
            }
        }

        Rectangle{
            id:skin_indicator
            height: scaleX(5)
            width: scaleX(5)

            Image {
                id: skin_icon
                source: ""
            }

            Text {
                id: skin_label
                text: qsTr("Skins")
                color: window.b_skinIndexReady ? "green" : "red"
                font.pointSize: window.b_skinIndexReady ? 14 : 12
            }
        }

        Rectangle{
            id:skindata_indicator
            height: scaleX(5)
            width: scaleX(5)

            Image {
                id: skinData_icon
                source: ""
            }

            Text {
                id: skin_data_label
                text: qsTr("Orbiter Ready")
                color: window.b_orbiterConfigReady ? "green" : "red"
                font.pointSize: window.b_orbiterConfigReady ? 14 : 12
            }
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
        anchors.centerIn: splashPage
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
        anchors.topMargin: scaleY(15)
        font.pixelSize: 14
        font.family: "Droid Sans"
        color: "white"
        anchors.top: rectangle2.bottom
        anchors.horizontalCenter: rectangle2.horizontalCenter
    }

    ListView{
        id:existing_orbiters
        height: scaleY(16)
        width: scaleX(55)
        clip: true
        anchors.centerIn: rectangle2
        model:orbiterList
        visible: orbiterSetup ===true ? false:true
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

    Rectangle{
        id:newOrbiterButton
        height: scaleY(10)
        width:scaleX(55)
        color: "slategrey"
        Text {
            id: newOrbiterLabel
            text: qsTr("Create New Orbiter?")
            font.pixelSize: scaleY(3)
            width: parent.width
            anchors.centerIn: parent
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        visible:true // window.newOrbiter ? true : false
        anchors.bottom: existing_orbiters.top
        anchors.horizontalCenter: existing_orbiters.horizontalCenter
        MouseArea{
            anchors.fill: parent
            onClicked: orbiterSetup=true
        }
    }

    Row{
        id:orbiter_options
        spacing:1
        anchors.centerIn: parent
        height: scaleY(35)
        width: scaleX(70)
        visible: false
        z:5
        Rectangle{
            id:goButton
            height: scaleY(15)
            width: scaleX(15)
            Text {
                id: goLabel
                text: qsTr("Go")
            }
            MouseArea{
                anchors.fill: goButton
                onClicked: {console.log("Going");
                    window.setupNewOrbiter(users_options.currentIndex+1, rooms_options.currentIndex+1,1,1,1,1);
                    orbiter_options.visible = false

                }
            }
        }

        Rectangle{
            id:usersBlock
            height: scaleY(20)
            width: scaleX(15)
            color: "lightgrey"
            Text {
                id: usersLabel
                text: qsTr("Select User")
            }
            clip:true

            ListView{
                id:users_options
                height: scaleY(20)
                width: scaleX(15)
                model:users
                spacing:5
                anchors.top: usersLabel.bottom
                delegate: Rectangle{
                    height: scaleY(5)
                    width: scaleX(15)
                    color: users.currentIndex ? "green" : "slategrey"
                    Text {
                        text:dataTitle
                        anchors.centerIn: parent
                        color: "black"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{console.log(dataTitle); users.currentIndex=index}
                    }
                }
            }
        }

        Rectangle{
            id:roomsBlock
            height: scaleY(20)
            width: scaleX(15)
            color: "lightgrey"
            Text {
                id: roomsLabel
                text: qsTr("Select Room")
            }
            clip:true

            ListView{
                id:rooms_options
                height: scaleY(20)
                width: scaleX(15)
                model:rooms
                spacing:5
                anchors.top: roomsLabel.bottom
                delegate: Rectangle{
                    height: scaleY(5)
                    width: scaleX(15)
                    color: rooms.currentIndex ? "green" : "slategrey"
                    Text {
                        text:dataTitle
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {console.log(dataTitle);
                            rooms.currentIndex = index
                        }
                    }
                }
            }
        }
/*
        Rectangle{
            id:skinsBlock
            height: scaleY(20)
            width: scaleX(15)
            color: "slategrey"
            Text {
                id: skinsLabel
                text: qsTr("Select Skin")
            }
            clip:true

            ListView{
                id:skins_options
                height: scaleY(20)
                width: scaleX(15)
                model:dummyModel
                anchors.top: skinsLabel.bottom
                delegate: Rectangle{
                    height: scaleY(5)
                    width: scaleX(15)
                    color: "lightgrey"
                    Text {
                        text: optionTitle
                        anchors.centerIn: parent
                    }
                }
            }
        }

        Rectangle{
            id:langBlock
            height: scaleY(20)
            width: scaleX(15)
            color: "slategrey"
            Text {
                id: langLabel
                text: qsTr("Select Lang")
            }
            clip:true

            ListView{
                id:lang_options
                height: scaleY(20)
                width: scaleX(15)
                model:dummyModel
                anchors.top: langLabel.bottom
                delegate: Rectangle{
                    height: scaleY(5)
                    width: scaleX(15)
                    color: "lightgrey"
                    Text {
                        text: optionTitle
                        anchors.centerIn: parent
                    }
                }
            }
        }
        */

    }

    ListModel{
        id:dummyModel
        ListElement{
            optionTitle:"foo"
            optionValue:1
        }
    }

}
