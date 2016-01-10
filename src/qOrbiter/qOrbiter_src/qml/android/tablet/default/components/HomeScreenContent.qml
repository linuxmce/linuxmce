import QtQuick 1.1

Item{
    id:homeScreenItems
    height: info_panel.height / 2
    width: info_panel.width *.55

    Row{
        height: parent.height
        width: parent.width
        spacing:scaleX(2)       
        StyledButton{
            id:location_info
            buttonText: roomList.currentRoom//+"::"+roomList.currentEA
            hitArea.onReleased: info_panel.state="room"
        }
        StyledButton{
            buttonText: "Sleeping Menu"
            onActivated: manager.setCurrentScreen("Screen_29.qml")
        }

    }

}
