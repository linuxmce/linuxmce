import QtQuick 1.1
//phone - default
Item{
    id:ftrItem
    height: profile ? scaleY(14) : scaleY(10)
    onStateChanged: console.log("Footer State::"+state)

    anchors{
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }

    Rectangle{
        anchors.fill: parent
        color:appstyle.headerBgColor
        opacity: appstyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: appstyle.bgHeaderGradient
        opacity: appstyle.bgHeavyOpacity
    }

    Row{

        height: manager.isProfile ? scaleY(8) : scaleY(6)
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

    }

    states: [
        State {
            name: "closed"
            when:manager.currentScreen!=="Screen_1.qml" && uiOn

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
            when:manager.currentScreen=="Screen_1.qml" && uiOn
            AnchorChanges{
                target: ftrItem
                anchors{
                    top:undefined
                    bottom:parent.bottom
                }
            }
        },
        State {
            name: "screen_saver"
            when:!uiOn
            AnchorChanges{
                target: ftrItem
                anchors{
                    bottom:undefined
                    top:qmlroot.bottom
                }
            }
            AnchorChanges{
                target: pageLoader
                anchors{

                    right:qmlroot.left
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:appstyle.animation_quick
                easing:appstyle.animation_easing
            }
        }
    ]


}
