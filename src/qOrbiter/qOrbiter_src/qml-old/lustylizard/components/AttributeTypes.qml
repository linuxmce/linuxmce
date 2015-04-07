import QtQuick 1.0
import com.nokia.android 1.1

Rectangle {
    id:rect
    width: childrenRect.width
    height: heading.height + genrelist.height
    color: "transparent"
    anchors.centerIn: parent
//    border.color: style.button_system_color
//    border.width: 1
    clip: true




    ListView{
        id:genrelist
        height: scaleY(65)
        width: scaleX(35)
        ListHeading{
            id:heading
            anchors.top: rect.top
            width: genrelist.width
            height: scaleY(8)
            Label{
                text: "Click to Close"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: rect.destroy()
            }
        }
        model: attribfilter
        //            cellHeight: 150
        //            cellWidth: 150

        delegate:ListItem{

            Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "yellow"
                border.width: 1
                Row{
                    anchors.fill: parent
                    ListItemText{
                        text: name
                        color: "white"
                        font.family: "Droid Sans"
                    }

                    CheckBox{
                        onClicked: attribfilter.setSelectionStatus(name)
                        checked: status
                    }
                }
            }
        }
    }
}


