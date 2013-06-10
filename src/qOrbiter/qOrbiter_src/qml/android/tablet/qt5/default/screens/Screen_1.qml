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
            anchors.left: nav_row.left
            anchors.top: nav_row.bottom

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
