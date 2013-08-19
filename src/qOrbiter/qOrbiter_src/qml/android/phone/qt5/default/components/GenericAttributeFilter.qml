import QtQuick 2.0
import com.nokia.android 1.1

Rectangle {
    id:generic_filter_view
    width: filterlist.width
    height: heading.height + filterlist.height
    color: "black"
    anchors.centerIn: parent
    clip: true
    property variant currentModel
    property int modelCount: filterlist.count
    property int delegateHeight:scaleY(10)
    state:"inactive"

    onCurrentModelChanged: currentModel !== "" ? state="active": state="inactive"

    ListView{
        id:filterlist
        height: scaleY(65)
        width:scaleX(55)
        model:currentModel
        ListHeading{
            id:heading
            anchors.top: generic_filter_view.top
            width: filterlist.width
            height: scaleY(8)
            Label{
                text: "Click to Close"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: generic_filter_view.currentModel=""
            }
        }

        delegate:ListItem{
        height:delegateHeight
            Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "orange"
                border.width: 1
                Row{
                    anchors.fill: parent
                    ListItemText{
                        text: name
                        color: "white"
                        font.family: "Droid Sans"
                        width:parent.width *.75
                        font.pointSize:scaleY(2)
                    }

                    CheckBox{
                        onClicked:filterlist.model.setSelectionStatus(name)
                        checked: status
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "active"
            PropertyChanges {
                target: generic_filter_view
                visible:true

            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: generic_filter_view
                visible:false
            }

        }
    ]
}


