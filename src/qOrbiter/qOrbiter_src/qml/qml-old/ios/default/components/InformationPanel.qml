import QtQuick 2.2

Item{
    id:info_panel
    property bool restore:false
    anchors{
        top:undefined
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }

    MouseArea{
        anchors.fill: parent
        onClicked: uiOn=!uiOn
    }

    Component.onCompleted:{
        info_panel.state="retracted";
    }

    Rectangle{
        id:info_fill
        anchors.fill: info_panel
        color: "black"
        opacity: .65
    }
    
    StyledText{
        id:orbiter_status_text
        text:"LinuxMCE Orbiter: "+manager.iPK_Device
        anchors{
            right:parent.right
            bottom:parent.bottom
        }

        font.pixelSize:fontSize
        font.bold: true
        color:"green"
        Behavior on opacity {

            SequentialAnimation{
                PropertyAnimation{
                    duration: 250
                }
            }
        }
    }

    Loader{
        id:homeContent
        anchors.top: parent.top
        anchors.left: parent.left
        source:"HomeScreenContent.qml"
        visible:(info_panel.state === "hidden" || info_panel.state==="retracted" )
    }

    Clock{
        id:time_keeper
       anchors{
           verticalCenter: parent.verticalCenter
         //  horizontalCenterOffset: scaleX(-5)
           horizontalCenter: parent.horizontalCenter
       }
    }

    Row{
        id:user_info
        width: childrenRect.width
        height: parent.height
        anchors.right: parent.right
        anchors.rightMargin: scaleX(2)
        spacing: scaleY(2)
        StyledButton{
            buttonText: "Power"
            onActivated: info_panel.state="power"
        }
        StyledButton{
            buttonText: manager.sPK_User
            hitArea.onReleased: info_panel.state="user"
        }

    }

    RoomSelector{
        visible: info_panel.state==="room"
    }
    UserSelector{
        visible: info_panel.state==="user"
    }

    PowerControl {
        id: powerControl
        visible: info_panel.state==="power"
    }

    StyledButton{
        id:close
        buttonText: "Close"
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
                opacity:1
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
                opacity:0
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
            PropertyChanges{
                target:homeContent
                source:""
            }
            PropertyChanges {
                target: time_keeper
                visible:false
            }
            PropertyChanges{
                target: orbiter_status_text
                opacity:0
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
            name: "power"
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
                opacity:0
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
            name: "hidden"
             when: manager.currentScreen !=="Screen_1.qml" || !uiOn
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
                duration:skinstyle.animation_medium
                easing.type: skinstyle.animation_easing
            }
        },
        Transition {
            from: "*"
            to: "retracted"
            PropertyAnimation{
                target:info_panel
                properties:"height"
                duration:skinstyle.animation_medium
                easing.type: skinstyle.animation_easing
            }


        }
    ]
}
