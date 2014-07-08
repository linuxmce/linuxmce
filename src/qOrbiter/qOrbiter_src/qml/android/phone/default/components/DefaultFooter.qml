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
        color:appStyle.headerBgColor
        opacity: appStyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: appStyle.bgHeaderGradient
        opacity: appStyle.bgHeavyOpacity
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


            AnchorChanges{
                target: ftrItem
                anchors{
                    bottom:undefined
                    top:parent.bottom
                }
            }
        },
        State {
            name: "open"
            when:manager.currentScreen="Screen_1.qml"


            AnchorChanges{
                target: ftrItem
                anchors{
                    top:undefined
                    bottom:parent.bottom
                }
            }

        }
    ]


}
