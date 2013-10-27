import QtQuick 1.1
Item{
    id:usersComponent

    anchors{
        top:parent.top
        bottom:parent.bottom
        left:parent.left
        right:parent.right
    }

    StyledText{
        id:select_label
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text:qsTr("Select User")
    }

    GridView{
        id:userListGrid
        anchors{
          top:select_label.bottom
          left:usersComponent.left
          right:usersComponent.right
          bottom:usersComponent.bottom
        }
        model:userList
        cellHeight:scaleY(17)
        cellWidth:scaleX(19)

        delegate:Item{
            height: scaleY(16)
            width: scaleX(18)
            StyledButton{
                height: scaleY(16)
                width: scaleX(18)
                buttonText: username
                hitArea.onReleased: {
                    manager.setCurrentUser(index+1)
                    info_panel.state="retracted"
                }
            }
        }
    }

}
