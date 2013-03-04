import QtQuick 1.0
import com.nokia.android 1.1

Rectangle {
    id:rect
    width: childrenRect.width
    height: heading.height + genericAttributeView.height
    color: "transparent"
    anchors.centerIn: parent
    clip: true
    property variant activeModel
    onActiveModelChanged: console.log("activeModel changed")
    visible: activeModel !=="NULL" ? true : false
    ListHeading{
        id:heading
        anchors.top: rect.top
        width: genericAttributeView.width
        height: scaleY(8)
        Label{
            text: "Click to Close"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: activeModel="NULL"
        }
    }
    ListView{
        id:genericAttributeView
        height: scaleY(65)
        width: scaleX(35)
        model: activeModel
        anchors.top: heading.bottom


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
                        onClicked: activeModel.setSelectionStatus(name)
                        checked: status
                    }
                }
            }
        }
    }
}


