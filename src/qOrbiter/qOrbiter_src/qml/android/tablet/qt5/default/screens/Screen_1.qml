import QtQuick 2.0
import "../components"
Item {
    height:manager.appHeight
    width:manager.appWidth

    Item{
        id:home_panel
        height: parent.height
        width:parent.width-content_panel.width
        anchors.left: parent.left
        anchors.top: parent.top
        Rectangle{
            id:home_panel_fill
            height: parent.height
            width: parent.width
            color:"antiquewhite"
            opacity: .15
        }
        StyledText{
            id:orbiter_status_text
            text:"LinuxMCE Orbiter "+manager.m_dwPK_Device + " connected."
            anchors.centerIn: parent
            font.pixelSize:44
            font.bold: true
            color:"green"
        }
        Clock{
            id:time_keeper
            anchors.top: orbiter_status_text.bottom
            anchors.left: orbiter_status_text.left

        }
    }

    Item{
        id:content_panel
        height: parent.height
        width:manager.b_orientation ? scaleX(15) : scaleX(14)
        anchors.right: parent.right
        anchors.top: parent.top
        Rectangle{
            anchors.fill: content_panel
            color: "grey"
            opacity: .35
        }

        Column{
            height: parent.height
            width: parent.width
            spacing:scaleY(2)
            ListView{
                id:scenarioList
                height:parent.height*.65
                width: parent.width
                spacing: scaleY(2)
                model:scenarios
                delegate: Item{
                    height: childrenRect.height
                    width: parent.width
                    StyledButton{
                        id:dummy
                        buttonText.text: name
                        buttonText.color: "antiquewhite"
                        width: parent.width -1

                    }
                }
            }

            StyledButton{
                buttonText.text:"Advanced"
               width: parent.width -1
            }


            StyledButton {
                id: exit_label
                width: parent.width -1
                buttonText.text: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.exitApp()
                }
            }
        }



    }
}
