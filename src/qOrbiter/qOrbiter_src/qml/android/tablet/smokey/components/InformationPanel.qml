import QtQuick 1.0
import "../components"
import "../../lib/handlers"

Item{
    id:info_panel
    width:parent.width
    anchors.left:parent.left
    anchors.bottom: parent.bottom
    Component.onCompleted:{ info_panel.state="retracted"; statusTimer.start() }

    Timer{
        id:statusTimer
        interval: 750
        triggeredOnStart: false
        onTriggered: {
            orbiter_status_text.opacity = 0;
        }

    }

    Rectangle{
        id:info_fill
        anchors.fill: info_panel
        color: "black"
        opacity: .65
    }
    
    StyledText{
        id:orbiter_status_text
        text:"LinuxMCE Orbiter "+manager.m_dwPK_Device + " connected."
        anchors.top: parent.top
        anchors.left: parent.left
        font.pixelSize:36
        font.bold: true
        color:"green"
        Behavior on opacity {

            SequentialAnimation{
                PropertyAnimation{
                    duration: 250
                }
                ScriptAction{
                    script: homeContent.source = "HomeScreenContent.qml"
                }
            }


        }
    }

    Loader{
        id:homeContent
        anchors.top: parent.top
        anchors.left: parent.left
        source:""
    }

    Clock{
        id:time_keeper
        anchors.top: orbiter_status_text.bottom
        anchors.left: orbiter_status_text.left
    }
    Column{
        id:user_info
        width: childrenRect.width
        height: parent.height
        anchors.right: parent.right
        StyledButton{
            buttonText.text: "User: "+manager.sPK_User
            hitArea.onReleased: info_panel.state="user"
        }
        StyledButton{
            id:location_info
            buttonText.text: "Location: "+roomList.currentRoom+"::"+roomList.currentEA
            hitArea.onReleased: info_panel.state="room"
        }
    }

    RoomSelector{
        visible: info_panel.state==="room"
    }

    StyledButton{
        id:close
        buttonText.text: "Close"
        hitArea.onReleased: info_panel.state="retracted"
        anchors.right: parent.right
    }

    
    states: [
        State {
            name: "retracted"
            PropertyChanges {
                target: info_panel
                height:scaleY(15)
            }
            PropertyChanges {
                target: time_keeper
                visible:true
            }
            PropertyChanges{
                target: orbiter_status_text
                visible:true
            }
            PropertyChanges{
                target:close
                visible:false
            }
            PropertyChanges{
                target:pageLoader
                visible:true
            }
            PropertyChanges{
                target:location_info
                visible:true
            }
            PropertyChanges{
                target:user_info
                visible:true
            }
            PropertyChanges {
                target: info_fill
                color:"black"
            }
        },
        State {
            name: "room"
            PropertyChanges {
                target: info_panel
                height:scaleY(92)
            }
            PropertyChanges {
                target: time_keeper
                visible:false
            }
            PropertyChanges {
                target: info_fill
                color:"green"
            }
            PropertyChanges{
                target: orbiter_status_text
                visible:false
            }
            PropertyChanges{
                target:close
                visible:true
            }
            PropertyChanges{
                target:pageLoader
                visible:false
            }
            PropertyChanges{
                target:location_info
                visible:false
            }
            PropertyChanges{
                target:user_info
                visible:false
            }
        },
        State {
            name: "user"
            PropertyChanges {
                target: info_panel
                height:scaleY(92)
            }
            PropertyChanges {
                target: time_keeper
                visible:false
            }
            PropertyChanges{
                target: orbiter_status_text
                visible:false
            }
            PropertyChanges{
                target:close
                visible:true
            }
            PropertyChanges{
                target:home_panel
                visible:false
            }
            PropertyChanges{
                target:location_info
                visible:false
            }
            PropertyChanges{
                target:user_info
                visible:false
            }
            PropertyChanges {
                target: info_fill
                color:"green"
            }
            PropertyChanges{
                target:pageLoader
                visible:false
            }

        },
        State {
            name: "hidden"
            //   when: screenfile !=="Screen_1.qml"
            PropertyChanges {
                target: time_keeper
                visible:false
            }
            PropertyChanges{
                target: orbiter_status_text
                visible:false
            }
            PropertyChanges {
                target: info_panel
                height:scaleY(0)
                visible:false
            }

            PropertyChanges{
                target:location_info
                visible:false
            }
            PropertyChanges{
                target:user_info
                visible:false
            }
            PropertyChanges{
                target:close
                visible:false
            }
            PropertyChanges{
                target:pageLoader
                visible:true
            }

        }
        
    ]
    transitions: [
        
        Transition {
            from: "retracted"
            to: "*"
            PropertyAnimation{
                target:info_panel
                properties:"height"
                duration:1000
                easing.type: Easing.OutBounce
            }
        },
        Transition {
            from: "*"
            to: "retracted"
            PropertyAnimation{
                target:info_panel
                properties:"height"
                duration:1000
                easing.type: Easing.OutElastic
            }
        }
    ]
}
