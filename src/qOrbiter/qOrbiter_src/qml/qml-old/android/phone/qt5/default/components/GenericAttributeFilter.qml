import QtQuick 2.2


Item {
    id:generic_filter_view
    width: manager.appWidth
    height: manager.appHeight
    clip: true
    property variant currentModel
    property int modelCount: filterlist.count
    property int delegateHeight:scaleY(10)
    state:"inactive"
    onCurrentModelChanged: currentModel !== "" ? state="active": state="inactive"

    Rectangle{
        anchors.fill: parent
        color: "black"
        MouseArea{
            anchors.fill: parent
        }
    }

    Item{
        id:heading
        anchors.top: generic_filter_view.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: filterlist.width
        height: scaleY(8)
        Rectangle{
            anchors.fill: parent
            color:androidSystem.orangeStandard
        }

        StyledText{
            text: "Click to Close"
            color:"black"
            font.pixelSize: 22
        }
        MouseArea{
            anchors.fill: parent
            onClicked: generic_filter_view.currentModel=""
        }
    }
    ListView{
        id:filterlist
        height: scaleY(65)
        width:scaleX(55)
        model:currentModel
        anchors.top: heading.bottom
        anchors.left: heading.left
        delegate:Item{
            height:scaleY(15)
            width: scaleX(55)
            Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "orange"
                border.width: 1
                Row{
                    anchors.fill: parent
                    StyledText{
                        text: name
                        color: "white"
                        width:parent.width *.75
                        font.pointSize:scaleY(4)
                    }

                    Rectangle{
                        height: 45
                        width: 45
                        radius:5
                        color: status ? "green" : "red"
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        MouseArea{
                            anchors.fill: parent
                            onClicked:filterlist.model.setSelectionStatus(name)

                        }
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


