// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0
import QtGraphicalEffects 1.0
Rectangle{
    id:splashPage
    height: appH
    width: appW
    color: "black"
    state: "starting"
    Component.onCompleted: state="waiting"
    property alias myFont: welcomeLabel.font

    Image {
        id: myBackground
        source: "default/img/icons/orbiterbg.png"
    }

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

    Text{
        id: txtDate
        text: getDate()
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        font.family: myFont
        font.pointSize: 28
        width: txtDate.paintedWidth
        anchors.right: splashPage.right
        anchors.bottom: splashPage.bottom

    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }


    FontLoader{
        id:keyFont
        name:"Sawasdee"
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

    Text {
        id: welcomeLabel
        text: qsTr("Welcome to LinuxMCE")
        color: "white"
        font.family: "Sawasdee"
        anchors.horizontalCenter: welcomeBox.horizontalCenter
        y: welcomeBox.y + 20
        font.pointSize: 18
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
        DropShadow{
            source: welcomeLabel
            radius: 5
            horizontalOffset: 5
            verticalOffset: 10
            samples:8
            spread: 3
        }
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
            Text {
                id: serverName
                text: qsTr("Connect To")
                font.family: myFont
                font.pointSize: 16
                color: "darkgrey"
            }
            TextInput {
                id: routerip
                text: srouterip
                font.pointSize: 28
                font.family: myFont
                //  onTextChanged: setRouterIp(routerip.text)
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
            Text {
                id: deviceName
                text: qsTr("Device Number")
                font.family: myFont
                font.pointSize: 16
                color: "darkgrey"
            }
            TextInput {
                id: devicenumber
                width: scaleX(10)
                text: deviceid
                font.pointSize: 28
                font.family: myFont
                anchors.left: deviceName.left
                anchors.top:deviceName.bottom
                //  onTextChanged: setRouterIp(routerip.text)
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
                Text{
                    id:confirmLabel
                    text:"Connect!"
                    font.family: myfont
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
           height: scaleY(35)
           width: parent.width
           clip: true
           anchors.centerIn: parent
           model:orbiterList
           visible: false
           orientation:ListView.Horizontal
           spacing:scaleX(10)
           delegate: Rectangle{
               id:existing_orbiter_delegate
               height: scaleY(10)
               width: existing_orbiters.width / 4
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
                       font.pointSize: 12
                       font.family: myFont
                       wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                   }
                   Text {
                       id: dev_num
                       text:qsTr("Device:")+ device
                       font.pointSize: 12
                       font.italic: true
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
                target: welcomeBox
                x:splashPage.width - welcomeBox.width
            }
            PropertyChanges {
                target: existing_orbiters
                visible:true
            }
            PropertyChanges {
                target: connectionColumn
                opacity:0

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
//    height:appH
//    width:appW
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

//            Text {
//                id: connection_label
//                text: qsTr("Connection")
//                color: window.b_connectionPresent ? "green" : "red"
//                font.pointSize: window.b_connectionPresent ? 32: 30
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

//            Text {
//                id: device_Label
//                text: qsTr("Device")
//                color: window.b_devicePresent ? "green" : "red"
//                font.pointSize: window.b_devicePresent ? 32 : 30
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

//            Text {
//                id: config_label
//                text: qsTr("Config")
//                color: window.b_localConfigReady ? "green" : "red"
//                font.pointSize: window.b_localConfigReady ? 32 : 30
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
            color: "transparent"
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
//        width: appW*.75
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
//    Text {
//        id: connectionlabel
//        text: qsTr("Set Connection Details")
//        font.pointSize: 36
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
//        Text {
//            text: qsTr("Host:")
//            font.pointSize: 28
//            font.family: "Droid Sans"
//            font.bold: true
//        }

//        TextInput {
//            id: routerip
//            width: 80
//            text: srouterip
//            font.pointSize: 28
//            font.family: "Droid Sans"
//            //  onTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true
//            onTextChanged: console.log(srouterip)

//        }

//        TextInput {
//            id: ext_routerip
//            width: 80
//            text: extip
//            font.pointSize: 10
//            font.family: "Droid Sans"
//            //  onTextChanged: setRouterIp(routerip.text)
//            color: "grey"

//            visible: false
//        }

//        Text {
//            text: qsTr("Device:")
//            font.pointSize: 28
//            font.family: "Droid Sans"
//            //  onTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true


//        }
//        TextInput {
//            id: devicenumber
//            width: scaleX(10)
//            text: deviceid
//            font.pointSize: 28
//            font.family: "Droid Sans"
//            //  onTextChanged: setRouterIp(routerip.text)
//            color: "black"
//            font.bold: true
//            onTextChanged: console.log(deviceid)
//        }

//        Rectangle {
//            id: connectbutton
//            height: scaleY(5)
//            width: scaleX(10)
//            color:"red"

////            anchors.left: devicenumber.right
////            anchors.leftMargin: scaleX(5)
//            Text {
//                id: name
//                anchors.centerIn: parent
//                anchors.fill: parent
//                text: qsTr("Go!")
//                font.pointSize: 28
//                font.family: "Droid Sans"
//                //  onTextChanged: setRouterIp(routerip.text)
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
//            Text {
//                id: exitlabel
//                anchors.centerIn: parent
//                anchors.fill: parent
//                text: qsTr("Exit")
//                font.pointSize: 11
//            }

//            radius:  4
//            MouseArea{
//                onClicked: closeOrbiter()
//                anchors.verticalCenter: parent.verticalCenter
//            }

//        }
//    }
//    Text {
//        id: loadingStatus
//        text: "Status " + manager.commandResponse
//        anchors.topMargin: scaleY(15)
//        font.pointSize: 14
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

//                Text {
//                    id: orbiter_label
//                    text: qsTr("Orbiter:")+ label
//                    font.pointSize: 12
//                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//                }
//                Text {
//                    id: dev_num
//                    text:qsTr("Device:")+ device
//                    font.pointSize: 12
//                    font.italic: true
//                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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

//        Text {
//            id: newOrbiterLabel
//            text: qsTr("Create New Orbiter?")
//            font.pointSize: 15
//            width: parent.width
//            anchors.centerIn: parent
//            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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




