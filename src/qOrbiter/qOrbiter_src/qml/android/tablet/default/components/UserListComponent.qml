import QtQuick 1.0
import com.nokia.android 1.1
Item{
    id:userlist_root
    height:manager.appHeight
    width: manager.appWidth

    Rectangle{
        id:mask
        anchors.fill: parent
        color:"black"
        opacity: .90
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    StyledText{
        anchors.top: parent.top
        text: qsTr("Please Select User")
        fontSize: scaleY(7)
        color: "aliceblue"
        style: Text.Sunken
    }

    ListView{
        id: userListview
        width: manager.appWidth
        height: scaleY(45)
        visible: true
        model: userList
        anchors.centerIn: parent
        orientation: ListView.Horizontal
        interactive: true
        delegate:  ListItem {
            height:scaleY(35)
            width:scaleX(20)

            Rectangle{
                height:scaleY(5)
                width: parent.width
                color: style.not_color
                clip:true
            }
            StyledText {
                id: usernamelabel
                text: username
                anchors.centerIn: parent
                font.bold: true
                color:"white"
                fontSize: scaleY(4)
            }
            onClicked:{setCurrentUser(index+1);componentLoader.source=""}
        }
    }
}

