// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:configurationPanel
    width: scaleX(55)
    height: scaleY(45)
    color: "lightgrey"
    radius:5
    state:"hidden"
    Rectangle{
        id:mask
        height: manager.appHeight
        width: manager.appWidth
        color: "lightgrey"
        opacity:.26
        anchors.centerIn: parent
        MouseArea{
            anchors.fill: mask
        }
    }

    Rectangle{
        id:close
        height: 50
        width: 50
        color: "grey"
        anchors.left: parent.left
        Text {
            id: exlbl
            text: qsTr("Close")
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked: configurationPanel.state="hidden"
        }

    }

    anchors.verticalCenter: advancedscreen.verticalCenter
    AnchorAnimation{
        duration: 2000
        easing.type: Easing.InBounce
        easing.amplitude: 1
    }

    Row{
        height: childrenRect.height
        width: parent.width
        spacing: 10
        anchors.top: parent.top
        anchors.topMargin: scaleY(15)
        ConfigItemBox{
            id:debugMode
            headerText: "Enable Debug mode?"
            useToggle: true
            toggleValue:manager.debugMode
            onConfigSwitchChanged: manager.debugMode = !manager.debugMode
        }


    }




    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: configurationPanel
                x: scaleX(-65)
                visible:false
            }

            AnchorChanges{
                target:configurationPanel
                anchors.horizontalCenter: undefined
                anchors.verticalCenter: undefined
            }
        },
        State {
            name: "showing"
            PropertyChanges {
                target: configurationPanel
                visible:true

            }
            AnchorChanges{
                target:configurationPanel
                anchors.horizontalCenter: advancedscreen.horizontalCenter
                anchors.verticalCenter: advancedscreen.verticalCenter
            }
        }
    ]

}
