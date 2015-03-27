import QtQuick 2.3
import "../"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item{
    id:header
    height: Style.appNavigation_panelHeight
    state:uiOn ? "open" : "closed"
    anchors{
        left:layout.left
        right:layout.right
    }
    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_light
        opacity: Style.appHeader_opacity
        
    }
    StyledText{
        id:orbiterId
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
            leftMargin:15
        }
        text:qsTr("Orbiter %1").arg(manager.iPK_Device)
    }

    StyledButton{
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
        buttonText: qsTr("Home")
        onActivated: manager.setCurrentScreen("Screen_1.qml")
    }
    
    states: [
        State {
            name: "open"
            AnchorChanges{
                target:header
                anchors.bottom: undefined
                anchors.top: layout.top
            }
        },
        State {
            name: "closed"
            AnchorChanges{
                target:header
                anchors.top: undefined
                anchors.bottom: layout.top
            }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: Style.transition_accentTime
            }
        }
    ]
}
