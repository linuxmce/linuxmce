import QtQuick 2.0
import "../components"
import "../../../lib/handlers"
Item {
    height:manager.appHeight
    width:manager.appWidth
    Component.onCompleted: current_header_model=scenarios
        Item{
            id:home_panel
            height: parent.height - info_panel.height - nav_row.height
            width:parent.width
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: scaleY(8)

            GridView{
                id:scenario_display
                height:parent.height *.75
                width: parent.width *.75
                anchors.centerIn: parent
                model:current_scenario_model
                cellWidth:scaleX(20)
                cellHeight:scaleY(12)
                delegate: StyledButton{
                    buttonText.text: title
                    textSize: 38
                    hitArea.onReleased: manager.execGrp(params)
                }

            }
        }

        Item{
            id:info_panel           
            width:parent.width
            anchors.left:parent.left
            anchors.bottom: parent.bottom
            Component.onCompleted: info_panel.state="retracted"


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
            StyledButton{
                id:close
                buttonText.text: "Close"
                hitArea.onReleased: info_panel.state="retracted"
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
                        target:home_panel
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
    }
