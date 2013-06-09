import QtQuick 2.0
import "../components"
Item {
    height:manager.appHeight
    width:manager.appWidth

    Rectangle{
        anchors.fill: nav_row
        color: "black"
        opacity: .65
    }

    Row{
        id:nav_row
        height: scaleY(8)
        width:parent.width
        spacing:scaleY(2)
        ListView{
            id:scenarioList
            height:scaleY(7)
            width:  (scaleX(10)*5)
            spacing: scaleY(2)
            model:scenarios
            clip:true
            orientation:ListView.Horizontal
            anchors.verticalCenter: parent.verticalCenter
            delegate: Item{
                height: childrenRect.height
                width: childrenRect.width
                StyledButton{
                    id:dummy
                    buttonText.text: name
                    buttonText.color: "antiquewhite"
                    hitArea.onReleased: {
                        if(modelName==="currentRoomLights")
                            scenario_display.model = currentRoomLights
                        else if(modelName==="currentRoomMedia")
                            scenario_display.model = currentRoomMedia
                        else if(modelName==="currentRoomClimate")
                            scenario_display.model = currentRoomClimate
                        else if(modelName==="currentRoomTelecom")
                            scenario_display.model=currentRoomTelecom
                        else if(modelName==="currentRoomSecurity")
                            scenario_display.model = currentRoomSecurity
                    }
                }
            }
        }
            StyledButton{
                buttonText.text:"Advanced"
            }
            StyledButton {
                id: exit_label
                buttonText.text: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.exitApp()
                }
            }
        }


        Item{
            id:home_panel
            height: parent.height - info_panel.height - nav_row.height
            width:parent.width
            anchors.left: nav_row.left
            anchors.top: nav_row.bottom

            GridView{
                id:scenario_display
                height:parent.height *.75
                width: parent.width *.75
                anchors.centerIn: parent
                model:currentRoomTelecom
                cellWidth:scaleX(20)
                cellHeight:scaleY(12)
                delegate: StyledButton{
                    buttonText.text: title
                    textSize: 38
                }

            }
        }

        Item{
            id:info_panel
            height: scaleY(15)
            width:parent.width
            anchors.left:parent.left
            anchors.bottom: parent.bottom
            Rectangle{
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
                }
                StyledButton{
                    buttonText.text: "Location: "+roomList.currentRoom+"::"+roomList.currentEA
                }
            }
        }
    }
