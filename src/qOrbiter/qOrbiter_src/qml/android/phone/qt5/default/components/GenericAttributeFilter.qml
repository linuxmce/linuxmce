import QtQuick 2.0


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
    Item{
        id:heading
        anchors.top: generic_filter_view.top
        width: filterlist.width
        height: scaleY(8)
        Rectangle{
            anchors.fill: parent
            color:androidSystem.orangeStandard
        }

        StyledText{
            text: "Click to Close"
            color:"black"
            font.pixelSize: 18
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
        delegate:Item{
        height:delegateHeight
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
                        color: status ? "green" : "red"

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


