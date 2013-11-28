import QtQuick 1.1
//phone - default
Item{
    id:ftrItem
    height: manager.b_orientation ? scaleY(8) : scaleY(12)
    anchors{
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .75
    }

    Row{
        id:ftr
        anchors.fill: parent
        spacing: scaleX(5)
        StyledButton{
            buttonText: manager.sPK_User
            onActivated: {loadComponent("UserSelector.qml");}


        }

        StyledButton{
            buttonText: roomList.currentRoom
            onActivated: {  loadComponent("RoomSelector.qml")    }
        }
        StyledButton{
            buttonText: "Power"
            onActivated: {loadComponent("Power.qml")}
        }
    }

    states: [
        State {
            name: "closed"
            when:manager.currentScreen!=="Screen_1.qml"
            PropertyChanges {
                target: ftrItem
                 height: manager.b_orientation ? scaleY(0) : scaleY(0)
            }
        },
        State {
            name: "open"
            when:manager.currentScreen="Screen_1.qml"
            PropertyChanges {
                target: ftrItem
                 height: manager.b_orientation ? scaleY(8) : scaleY(12)
            }
        }
    ]


}
