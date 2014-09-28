import QtQuick 1.0
import com.nokia.android 1.1

Item {
    id:rect
    width: manager.appWidth
    height: manager.appHeight
    anchors.centerIn: parent
    clip: true
    property variant activeModel

    onActiveModelChanged: console.log("activeModel changed")
    visible: activeModel !=="NULL" ? true : false

    onVisibleChanged: {
        if(visible)
          manager.setGridStatus(false)
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .85
    }


    ListView{
        id:genericAttributeView
        height: scaleY(65)
        width: scaleX(55)
        model: activeModel
        spacing:scaleY(1)
        anchors.top: heading.bottom
        anchors.horizontalCenter: heading.horizontalCenter

        delegate:ListItem{
            height:scaleY(15)
            width:parent.width

            Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "white"
                border.width: 1
                radius:5
            }
            Row{
                anchors.fill: parent
                StyledText{
                    text: name

                    color: "white"
                    width: parent.width *.75
                }

                CheckBox{
                    onClicked: activeModel.setSelectionStatus(name)
                    checked: status
                    height:parent.height /2
                    width:height
                }
            }
        }
    }
    ListHeading{
        id:heading
        anchors.top: rect.top
        anchors.horizontalCenter: parent.horizontalCenter
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
}


