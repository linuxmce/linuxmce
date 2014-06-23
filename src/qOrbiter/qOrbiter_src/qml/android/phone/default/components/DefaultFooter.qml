import QtQuick 1.1
//phone - default
Item{
    id:ftrItem
    height: manager.b_orientation ? scaleY(16) : scaleY(12)
    anchors{
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }
    Rectangle{
        anchors.fill: parent
        color:skinStyle.headerBgColor
        opacity: skinStyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: skinStyle.bgHeaderGradient
        opacity: skinStyle.bgHeavyOpacity
    }

    Row{
        id:ftr
        height: manager.b_orientation ? scaleY(8) : scaleY(6)
        spacing: scaleX(5)
        anchors{
            left:parent.left
            right:parent.right
            verticalCenter: parent.verticalCenter
        }

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
        StyledText{
           text:"Current Router Address:"+manager.m_ipAddress
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
