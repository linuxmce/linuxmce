// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:genericlist
    height:scaleY(80)
    width:manager.appWidth -10
    property int itemH:manager.appHeight /3 - exit_button.height
    property int itemW:genericlist.width / 2
    anchors{
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }
    Rectangle{
        id:exit_button
        height: scaleY(12)

        anchors{
            left:parent.left
            right:parent.right
            top:parent.top
        }
        color: "darkgreen"

        StyledText {
            id: exit
            text: qsTr("Exit")
            font.pixelSize: manager.isProfile ? scaleY(3) :scaleY(5)
            anchors.centerIn: parent
            color: "white"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:loadComponent("NullComponent.qml")
        }
    }


    GridView{
        id: genericview
        anchors{
            top:exit_button.bottom
            bottom: parent.bottom
            left:parent.left
            right:parent.right
        }
        model: userList
        cellHeight:itemH
        cellWidth: itemW
        interactive: true
        clip:true
        delegate:
            Item{
            id:generic_item
            height:  itemH- 15
            width: itemW -15

            StyledButton{
                anchors.fill: parent
                buttonText:username
                onActivated: {
                    setCurrentUser(index+1)
                    loadComponent("NullComponent.qml")
                }
            }
        }
    }
}


