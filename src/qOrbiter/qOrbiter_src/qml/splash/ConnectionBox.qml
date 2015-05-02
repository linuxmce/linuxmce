import QtQuick 2.2
import org.linuxmce.screeninfo 1.0

Item{
    id: connectionBox
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenterOffset:10
    width: scaleX(65)
    height: scaleY(45)
    Rectangle {
        anchors.fill: connectionBox
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
        font.pixelSize: theme.appFontSize_header
        font.weight: Font.Light
        width:parent.width
         horizontalAlignment: Text.AlignHCenter
         verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color:theme.apptext_color_active
        anchors.top: connectionBox.top
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: myFont.name
    }

    Item{
        id:connectionVars
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: connectionlabel.bottom
        width: parent.width *.85
        height: parent.height *.65

        Text {
            id:host_label
            text: qsTr("Host:")
            font.pixelSize: theme.appFontSize_title
            color:theme.apptext_color_active
            font.family: myFont.name
            font.weight: Font.Light
            anchors.verticalCenter: parent.verticalCenter
            anchors. left:parent.left
        }

        Rectangle{
            anchors.fill: routerip
        }

        TextInput {
            id: routerip
            width: scaleX(17) * theme.dpRatio
            color: "#030000"
            clip: true
            text: window.router
            font.pixelSize:theme.appFontSize_list
            font.family: myFont.name
            anchors{
                top:host_label.bottom
                left: host_label.left
            }
        }

        TextInput {
            id: ext_routerip
            width: 80
            text: extip
            font.pixelSize: theme.appFontSize_list
            font.family: myFont.name
            //  onTextChanged: setRouterIp(routerip.text)
            color:theme.apptext_color_active
            anchors.verticalCenter: parent.verticalCenter
            visible: false
        }

        Text {
            id:device_label
            text: qsTr("Device:")
            color:theme.apptext_color_active
            font.pixelSize: theme.appFontSize_title
            font.family: myFont.name
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
        }
        TextInput {
            id: devicenumber
            width: scaleX(10)
            color:theme.apptext_color_active
            text: window.deviceno
            font.family: myFont.name
            font.pixelSize: theme.appFontSize_list
            anchors{
                right:device_label.right
                top:device_label.bottom
            }
        }
    }

    Row{
        id:goBox
        anchors.top: connectionVars.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width *.65
        spacing: scaleX(15)
        Rectangle {
            id: connectbutton
            height: theme.appButtonHeight*theme.dpRatio
            width: theme.appButtonWidth*theme.dpRatio
            radius:height
            color:theme.appbutton_confirm_color

            Text {
                id: name
                anchors.centerIn: parent

                text: qsTr("Go!")
                font.pixelSize: theme.appFontSize_list
                verticalAlignment: Text.AlignTop
                font.bold: true
                font.family: myFont.name
                color:theme.apptext_color_active
            }


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
            height: theme.appButtonHeight*screenInfo.primaryScreen.pixelRatio
            width: theme.appButtonWidth*screenInfo.primaryScreen.pixelRatio
            radius: height
            color: "red"

            Text {
                id: exitlabel
                anchors.centerIn: parent
                color:theme.apptext_color_active

                text: qsTr("Exit!")
                font.pixelSize: theme.appFontSize_list
                font.family: myFont.name
            }

            MouseArea{
                anchors.fill: parent
                onClicked: manager.closeOrbiter()

            }
        }
    }
    states: [
        State {
            name: "showing"
            PropertyChanges {
                target: connectionBox
                opacity:1
                scale:1
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: connectionBox
                opacity:0
                scale:0
            }
        }
    ]
}
