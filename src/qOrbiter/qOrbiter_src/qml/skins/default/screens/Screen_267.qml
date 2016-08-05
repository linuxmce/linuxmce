import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen{
    id:userstatus
    screen: "Active Calls"
    Panel{
        headerTitle: screen

        GenericListModel {
            id: activeCalls
            anchors.left: parent.left
            anchors.top: parent.headerRect.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.horizontalCenter
            dataGrid: DataGrids.Active_Channels
            dataGridLabel: "activeChannels"
            delegate: StyledButton{
                anchors.left: parent.left
                anchors.right: parent.right
                buttonText: description
                onActivated:   {
                    activeUsersOnChannel.model = manager.getDataGridModel("activeUsersOnChannel", DataGrids.Active_Users_On_Channel, value)
                }
            }
        }

        GenericListModel {
            id: activeUsersOnChannel
            anchors.left: parent.horizontalCenter
            anchors.top: parent.headerRect.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            delegate: StyledButton{
                anchors.left: parent.left
                anchors.right: parent.right
                buttonText: description
                onActivated:   {
                }
            }
        }

    }

    Timer {
        interval: 2000
        repeat: true
        running: true
        onTriggered: activeCalls.refresh()
    }
}
