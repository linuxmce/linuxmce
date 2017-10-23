import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen{
    id:callfromfavorites

    Panel{
        id: myPanel
        headerTitle: "Call From Favorites/recent calls."
        property string numberToCall: ''

        GenericListModel {
            id: recentCalls
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.horizontalCenter
            anchors.topMargin: myPanel.panelHeaderHeight+2
            anchors.rightMargin: 2

            label: 'Recent calls'
            model: manager.getDataGridModel("recentCalls", DataGrids.Recent_Calls)
            delegate: StyledButton {
                anchors.left: parent.left
                anchors.right: parent.right
                buttonText: description
                textSize: appStyle.appFontSize_list
                onActivated:   {
                    myPanel.numberToCall = value
                    telecomDevices.visible = true
                }
            }

        }
        GenericListModel {
            id: favoriteCalls
            anchors.top: parent.top
            anchors.left: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.topMargin: myPanel.panelHeaderHeight+2
            anchors.leftMargin: 2

            label: 'Favorites'
            model: manager.getDataGridModel("speedDial", DataGrids.Speed_Dial)
            delegate: StyledButton {
                anchors.left: parent.left
                anchors.right: parent.right
                buttonText: description
                textSize: appStyle.appFontSize_list
                onActivated: {
                    myPanel.numberToCall = value
                    telecomDevices.visible = true
                }
            }
        }

        TelecomDevices {
            id: telecomDevices
            visible: false
            anchors.fill: parent
            showCallToDevices: false
            onSelected: {
                visible = false
                manager.makeCall(currentuser, myPanel.numberToCall, selectedDevice, '')
            }
        }
    }

}
