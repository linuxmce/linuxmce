// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0
import QtGraphicalEffects 1.0
import "noir/components"
Rectangle{
    id:splashPage
    height:appH
    width: appW
    color: "black"
    state: "starting"
    Component.onCompleted: state="waiting"
    onStateChanged: lateEffectTimer.restart()

    Timer{
        id:lateEffectTimer
        interval: 2000
       running:false
        onTriggered: {
            pop.start()
        }
    }

    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d| hh:mm ");
    }

    StyledText{
        id: txtDate
        text: getDate()
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        font.family: keyFont.name
        font.pixelSize: 28
        width: txtDate.paintedWidth
        anchors.right: splashPage.right
        anchors.top: splashPage.top

    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }


    FontLoader{
        id:keyFont
        name:"Sawasdee"
        source: "default/fonts/Sawasdee.ttf"
    }

    Rectangle{
        id:welcomeBox
        width: parent.width*.35
        height: parent.height
        color: "darkgrey"
        border.color: "white"
        border.width: 1
        opacity: .15

        Behavior on x{
            PropertyAnimation{
                duration: 2000
            }
        }
    }

    StyledText {
        id: welcomeLabel
        text: qsTr("Welcome to LinuxMCE")
        color: "white"
        font.family: "Sawasdee"
        anchors.horizontalCenter: welcomeBox.horizontalCenter
        y: welcomeBox.y + 20
        font.pixelSize: 36
        scale:1

        SequentialAnimation{
            id:pop
            PropertyAnimation{
                target: welcomeLabel
                property: "scale"
                to:1.5
                duration: 500
            }
            PropertyAnimation{
                target: welcomeLabel
                property: "scale"
                to:1
                duration: 500

            }
        }

    }

    StyledText {
        id: selectLabel
        text: qsTr("Please select from existing Qt Orbiters")
        font.family: keyFont.name
        color: "white"
        font.pixelSize: 20
        visible: false
        anchors.centerIn: welcomeBox
    }

    StyledText {
        id: createLabel
        text: qsTr("Or Create a new Qt Orbiter")
        font.family: keyFont.name
        color: "white"
        font.pixelSize: 20
        visible: selectLabel.visible
        anchors.top: selectLabel.bottom
        anchors.left: welcomeBox.left
    }

    Column{
        id:connectionColumn
        width: welcomeBox.width*.65
        height: welcomeBox.height/2
        opacity:0
        anchors.horizontalCenter: welcomeBox.horizontalCenter
        anchors.verticalCenter: welcomeBox.verticalCenter
        Behavior on opacity{

            PropertyAnimation{
                duration:1000
            }
        }


        Rectangle{
            id:hostBox
            width: parent.width*.65
            radius:10
            height: parent.height /3
            color: "transparent"
            StyledText {
                id: serverName
                text: qsTr("Connect To")
                font.family: keyFont.name
                font.pixelSize: 16
                color: "darkgrey"
            }
            TextInput {
                id: routerip
                text: srouterip
                font.pixelSize: 28
                font.family: keyFont.name
                //  onStyledTextChanged: setRouterIp(routerip.text)
                color: "white"
                font.bold: true
                onTextChanged: console.log(srouterip)
                anchors.top: serverName.bottom
                anchors.left: serverName.left

            }
        }

        Rectangle{
            id:deviceBox
            width: parent.width*.65
            radius:10
            height: parent.height /3
            color: "transparent"
            StyledText {
                id: deviceName
                text: qsTr("Device Number")
                font.family: keyFont.name
                font.pixelSize: 16
                color: "darkgrey"
            }
            TextInput {
                id: devicenumber
                width: scaleX(10)
                text: deviceid
                font.pixelSize: 28
                font.family: keyFont.name
                anchors.left: deviceName.left
                anchors.top:deviceName.bottom
                //  onStyledTextChanged: setRouterIp(routerip.text)
                color: "black"
                font.bold: true
                onTextChanged: console.log(deviceid)
            }
        }
        Rectangle{
            id:actions
            width: parent.width
            height: 150

            color:"transparent"

            Rectangle{
                id:confirmBox
                height: 40
                width: 80
                anchors.centerIn: parent
                StyledText{
                    id:confirmLabel
                    text:"Connect!"
                    font.family: keyFont.name
                    font.pixelSize: 20
                }

                color:confirmHit.focus ? "grey" : confirmHit.containsMouse ? "grey": "transparent"
                MouseArea{
                    id:confirmHit
                    hoverEnabled: true
                    anchors.fill: confirmBox
                }
            }
        }

    }

    ListView{
           id:existing_orbiters
           height: scaleY(10)
           width: parent.width-welcomeBox.width
           clip: true
           anchors.right: splashPage.right
           anchors.verticalCenter: splashPage.verticalCenter
           model:orbiterList
           visible: false
           orientation:ListView.Horizontal
           spacing:scaleX(5)
           delegate: Rectangle{
               id:existing_orbiter_delegate
               height: scaleY(15)
               width: scaleX(15)
               clip:true
               color: "transparent"
               border.color: "white"
               border.width: 1
               Rectangle{
                   id:transparency
                   anchors.fill: parent
                   color: "darkslategrey"
                   opacity: .15
               }

               Column
               {
                   height: childrenRect.height
                   width: childrenRect.width
                   anchors.centerIn: parent

                   StyledText {
                       id: orbiter_label
                       text: qsTr("Orbiter:\n")+ label
                       font.pixelSize: 12
                       font.family: keyFont.name
                       color: "white"
                       wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                   }
                   StyledText {
                       id: dev_num
                       text:qsTr("Device:")+ device
                       font.pixelSize: 12
                       font.family: keyFont.name
                       wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                   }
               }
               MouseArea {
                   anchors.fill: parent
                   onClicked: window.qmlSetupLmce(device, routerip.text)
               }
           }
       }

Connections{
    target: window
    onShowList:state = "orbiterselect"
}

Row{
    id:indicatorContainer
    width: splashPage.width - welcomeBox.width
    height: scaleY(8)
    anchors.bottom: splashPage.bottom
    anchors.right: splashPage.right

    Rectangle{
        id:connectionIndicator
        width: scaleX(10)
        height: scaleY(8)
        color: "transparent"
        Image {
            id: connectionImage
            source: "default/img/icons/jabber_protocol.png"
            anchors.fill: parent
            visible:window.b_connectionPresent ? true:false
        }
        StyledText {
            id: connectionStatus
            text: qsTr("Connection")
            font.family: keyFont.name
            color: window.b_connectionPresent ? "white" : "red"
        }
    }

    Rectangle{
        id:deviceIndicator
        width: scaleX(10)
        height: scaleY(10)
        color: "transparent"
        Image {
            id: deviceImage
            source: "default/img/icons/harddrive2.png"
            anchors.fill: parent
            visible: window.b_devicePresent ? true :false
        }
        StyledText {
            id: deviceStatus
            text: qsTr("Device")
            font.family: keyFont.name
            color: window.b_devicePresent ? "white" : "red"
        }
    }

    Rectangle{
        id:configIndicator
        width: scaleX(10)
        height: scaleY(10)
        color: "transparent"
        Image {
            id: configImage
            source: "default/img/icons/package_editors.png"
            anchors.fill: parent
            visible: window.b_devicePresent ? true :false
        }
        StyledText {
            id: configStatus
            text: qsTr("Local Config")
            font.family: keyFont.name
            color: window.b_localConfigReady ? "white" : "red"
        }
    }
}


    states: [
        State {
            name: "starting"
            PropertyChanges {
                target: welcomeBox
                x:parent.width *-1
            }
        },
        State {
            name: "orbiterselect"

            PropertyChanges {
                target: existing_orbiters
                visible:true
            }
            PropertyChanges {
                target: connectionColumn
                opacity:0
            }
            PropertyChanges {
                target: selectLabel
                visible:true
            }
        },
        State {
            name: "waiting"
            PropertyChanges {
                target: welcomeBox
                x:0
            }
            PropertyChanges {
                target: connectionColumn
                opacity:1
            }
            PropertyChanges {
                target: lateEffectTimer
                running:true
            }
        },
        State {
            name: "loading"
            PropertyChanges {
                target: welcomeBox
                x:parent.width *-1
            }
        }
    ]

}

//Rectangle {
//    height:manager.appHeight
//    width:manager.appWidth
//    id:splashPage
//    color: "black"
//    signal setupStart(string x, string y)
//    signal splashLoaded()

//    Connections{
//        target: manager
//        onShowList:{
//            existing_orbiters.visible = true
//            if(existing_orbiters.count === 0){
//                screenChange("NewOrbiterSetup.qml")
//            }
//        }
//        onPageChanged:screenchange(qmlPage)
//    }
//    Image {
//        id: splash
//        anchors.centerIn: splashPage
//        fillMode: Image.PreserveAspectFit
//        source: "qrc:/img/desktop_splash.png"
//        anchors.fill: parent
//    }

//    Row{

//        height: childrenRect.height
//        width: scaleX(95)
//        spacing: scaleX(15)

//        Rectangle{

//            id:connection_indicator
//            height: scaleX(5)
//            width: scaleX(5)
//            color: "transparent"

//            StyledText {
//                id: connection_label
//                text: qsTr("Connection")
//                color: window.b_connectionPresent ? "green" : "red"
//                font.pixelSize: window.b_connectionPresent ? 32: 30
//            }
//        }

//        Rectangle{
//            id:device_indicator
//            height: scaleX(5)
//            width: scaleX(5)
//            color: "transparent"
//            Image {
//                id: device_icon
//                source: ""
//            }

//            StyledText {
//                id: device_Label
//                text: qsTr("Device")
//                color: window.b_devicePresent ? "green" : "red"
//                font.pixelSize: window.b_devicePresent ? 32 : 30
//            }
//        }

//        Rectangle{
//            id:config_indicator
//            height: scaleX(5)
//            width: scaleX(5)
//            color: "transparent"
//            Image {
//                id: config_icon
//                source: ""
//            }

//            StyledText {
//                id: config_label
//                text: qsTr("Config")
//                color: window.b_localConfigReady ? "green" : "red"
//                font.pixelSize: window.b_localConfigReady ? 32 : 30
//            }
//        }
/*
        Rectangle{
            id:skin_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: skin_icon
                source: ""
            }

            StyledText {
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

            StyledText {
                id: skin_data_label
                text: qsTr("Orbiter Ready")
                color: window.b_orbiterConfigReady ? "green" : "red"
                font.pixelSize: window.b_orbiterConfigReady ? 14 : 12
            }
        }
*/

//    }

//    Connections{
//        target:window
//        onMessageChanged:loadingStatus.text = window.message
//        //onStatusChanged: screenchange("SetupNewOrbiter.qml")
//    }

//    Rectangle {
//        width: parent.width
//        height: scaleY(20)
//        opacity: 1
//        color: "transparent"

//    }
//    Rectangle {
//        id: rectangle2

//        anchors.verticalCenter: parent.verticalCenter
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.verticalCenterOffset:10
//        width: manager.appWidth*.75
//        height: scaleY(20)
//        radius: 7
//        anchors.horizontalCenterOffset: 1
//        anchors.centerIn: splashPage
//        border.width: 1
//        border.color: "#000000"
//        gradient: Gradient {
//            GradientStop {
//                position: 0
//                color: "transparent"
//            }
//            GradientStop {
//                position: 1
//                color: "#000000"
//            }
//        }
//        opacity: 0.5
//    }
//    StyledText {
//        id: connectionlabel
//        text: qsTr("Set Connection Details")
//        font.pixelSize: 36
//        font.bold: false
//        anchors.top: rectangle2.top
//        anchors.horizontalCenter: parent.horizontalCenter
//        color:"white"
//    }

//    Row{
//        anchors.centerIn: rectangle2
//        anchors.verticalCenter: parent.verticalCenter
//        spacing: 150
//        width: rectangle2.width
//        StyledText {
//            text: qsTr("Host:")
//            font.pixelSize: 28
//            font.family: "Droid Sans"
//            font.bold: true
//        }

//        StyledTextInput {
//            id: routerip
//            width: 80
//            text: srouterip
//            font.pixelSize: 28
//            font.family: "Droid Sans"
//            //  onStyledTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true
//            onStyledTextChanged: console.log(srouterip)

//        }

//        StyledTextInput {
//            id: ext_routerip
//            width: 80
//            text: extip
//            font.pixelSize: 10
//            font.family: "Droid Sans"
//            //  onStyledTextChanged: setRouterIp(routerip.text)
//            color: "grey"

//            visible: false
//        }

//        StyledText {
//            text: qsTr("Device:")
//            font.pixelSize: 28
//            font.family: "Droid Sans"
//            //  onStyledTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true


//        }
//        StyledTextInput {
//            id: devicenumber
//            width: scaleX(10)
//            text: deviceid
//            font.pixelSize: 28
//            font.family: "Droid Sans"
//            //  onStyledTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true
//            onStyledTextChanged: console.log(deviceid)
//        }

//        Rectangle {
//            id: connectbutton
//            height: scaleY(5)
//            width: scaleX(10)
//            color:"red"

////            anchors.left: devicenumber.right
////            anchors.leftMargin: scaleX(5)
//            StyledText {
//                id: name
//                anchors.centerIn: parent
//                anchors.fill: parent
//                text: qsTr("Go!")
//                font.pixelSize: 28
//                font.family: "Droid Sans"
//                //  onStyledTextChanged: setRouterIp(routerip.text)
//                color: "black"
//                font.bold: true

//            }

//            radius:  5
//            MouseArea{
//                hoverEnabled: true
//                onEntered: parent.color="green"
//                onExited: parent.color="red"
//                anchors.fill: parent
//                onClicked: ext_routerip.visible ? window.qmlSetupLmce(devicenumber.text, ext_routerip.text) : window.qmlSetupLmce(devicenumber.text, routerip.text)
//                anchors.verticalCenter: parent.verticalCenter
//            }
//        }

//        Rectangle {
//            id: exitbutton
//            height: scaleY(5)
//            width: scaleX(6)
////            anchors.left: connectbutton.right
////            anchors.leftMargin: scaleX(5)
//            StyledText {
//                id: exitlabel
//                anchors.centerIn: parent
//                anchors.fill: parent
//                text: qsTr("Exit")
//                font.pixelSize: 11
//            }

//            radius:  4
//            MouseArea{
//                onClicked: closeOrbiter()
//                anchors.verticalCenter: parent.verticalCenter
//            }

//        }
//    }
//    StyledText {
//        id: loadingStatus
//        text: "Status " + manager.commandResponse
//        anchors.topMargin: scaleY(15)
//        font.pixelSize: 14
//        font.family: "Droid Sans"
//        color: "white"
//        anchors.top: rectangle2.bottom
//        anchors.horizontalCenter: rectangle2.horizontalCenter
//    }


//    ListView{
//        id:existing_orbiters
//        height: scaleY(35)
//        width: scaleX(55)
//        clip: true
//        anchors.centerIn: rectangle2
//        model:orbiterList
//        visible: false
//        orientation:ListView.Horizontal
//        delegate: Rectangle{
//            id:existing_orbiter_delegate
//            height: scaleY(5)
//            width: existing_orbiters.width
//            color: "slategrey"
//            border.color: "white"
//            border.width: 1
//            Column
//            {
//                height: childrenRect.height
//                width: childrenRect.width
//                anchors.centerIn: parent

//                StyledText {
//                    id: orbiter_label
//                    text: qsTr("Orbiter:")+ label
//                    font.pixelSize: 12
//                    wrapMode: StyledText.WrapAtWordBoundaryOrAnywhere
//                }
//                StyledText {
//                    id: dev_num
//                    text:qsTr("Device:")+ device
//                    font.pixelSize: 12
//                    font.italic: true
//                    wrapMode: StyledText.WrapAtWordBoundaryOrAnywhere
//                }
//            }
//            MouseArea {
//                anchors.fill: parent
//                onClicked: window.qmlSetupLmce(device, routerip.text)
//            }
//        }
//    }

//    Rectangle{
//        id:newOrbiterButton
//        height: scaleY(10)
//        width:scaleX(55)
//        gradient: Gradient {
//            GradientStop {
//                position: 0
//                color: "transparent"
//            }
//            GradientStop {
//                position: 1
//                color: "#000000"
//            }
//        }

//        StyledText {
//            id: newOrbiterLabel
//            text: qsTr("Create New Orbiter?")
//            font.pixelSize: 15
//            width: parent.width
//            anchors.centerIn: parent
//            wrapMode: StyledText.WrapAtWordBoundaryOrAnywhere
//        }
//        visible: existing_orbiters.visible
//        anchors.bottom: existing_orbiters.top
//        anchors.horizontalCenter: existing_orbiters.horizontalCenter
//        MouseArea{
//            anchors.fill: parent
//            onClicked:screenchange("SetupNewOrbiter.qml")
//        }
//    }

//}




